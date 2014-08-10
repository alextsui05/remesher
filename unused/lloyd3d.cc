#include <iostream>
#include <cstdlib>

#include "exception.h"
#include "microdelaunay.h"
#include "delaunayflips.h"
#include "elapsedtimer.h"
#include "lloyd3d.h"

REMESHER_NAMESPACE_BEGIN

void
Lloyd3d::start_lloyd_relaxation (void)
{
  if (this->config.iterations == 0)
    return;

  ElapsedTimer t;

  /* Relocation is handled by separate code to ensure reuse. */
  this->reloc.set_data(this->rmesh, this->rvinfo);

  /* Allocate memory for the new references during each iteration. */
  this->reflist_new = this->reflist->create_copy();
  for (std::size_t iter = 0; iter < this->config.iterations; ++iter)
  {
    std::cout << "Starting Lloyd-Iteration " << iter << std::endl;

    {
      DelaunayFlips df(this->emesh, this->evinfo);
      df.set_feature_edges(this->features);
      df.flip_edges();
    }

#if REMESHER_PARALLELIZATION
    this->handle_all_vertices_parallel();
#else
    this->handle_all_vertices();
#endif

    /* After each iteration, the new reference list is applied. */
    for (std::size_t i = 0; i < this->reflist->size(); ++i)
      this->reflist[i] = this->reflist_new[i];
  }

  /* Algorithm is finished. Drop working data. */
  this->reflist_new.reset();

  /* Restore the delaunay property for the last time. */
  DelaunayFlips df(this->emesh, this->evinfo);
  df.set_feature_edges(this->features);
  df.flip_edges();

  std::cout << "Done after " << this->config.iterations
      << " Lloyd iterations, took " << t.get_elapsed() << "ms" << std::endl;

  return;
}

/* ---------------------------------------------------------------- */

void
Lloyd3d::handle_all_vertices (void)
{
  std::size_t vamount = this->emesh->get_vertices().size();
  this->handle_vertices(0, vamount);
}

/* ---------------------------------------------------------------- */

void
Lloyd3d::handle_all_vertices_parallel (void)
{
  /* If there are too few vertices, don't use threads.
   * We want at least 100 vertices per thread. */
  std::size_t vamount = this->emesh->get_vertices().size();
  if (vamount < REMESHER_LLOYD_THREADS * 100)
  {
    this->handle_all_vertices();
    return;
  }

  /* Create a bunch of threads and start them. */
  std::vector<LloydThread*> threads;
  threads.resize(REMESHER_RELAXATION_THREADS);
  float vpt = (float)vamount / (float)REMESHER_RELAXATION_THREADS;
  for (std::size_t t = 0; t < REMESHER_RELAXATION_THREADS; ++t)
  {
    std::size_t start = (std::size_t)((float)t * vpt);
    std::size_t end = (std::size_t)((float)(t + 1) * vpt);
    if (t + 1 == REMESHER_RELAXATION_THREADS)
      end = vamount;
    std::cout << "Launching thread " << t << " with bounds ["
        << start << ", " << end << "[" << std::endl;
    threads[t] = new LloydThread(this, start, end);
    threads[t]->pt_create();
  }

  /* Join the threads. */
  for (std::size_t t = 0; t < REMESHER_RELAXATION_THREADS; ++t)
  {
    threads[t]->pt_join();
    delete threads[t];
    std::cout << "Thread " << t << " destroyed" << std::endl;
  }
}

/* ---------------------------------------------------------------- */

void
Lloyd3d::handle_vertices (std::size_t start, std::size_t end)
{
  #if 0
  /* Randomize vertex processing order with a index map. */
  typedef std::vector<std::size_t> RndMap;
  RndMap rnd;
  rnd.resize(this->evinfo.size());
  for (std::size_t i = 0; i < rnd.size(); ++i)
    rnd[i] = i;
  for (std::size_t i = 0; i < rnd.size(); ++i)
  {
    std::size_t rnd_idx = ::rand() % rnd.size();
    if (rnd_idx != i)
      std::swap(rnd[i], rnd[rnd_idx]);
  }
  #endif

  for (std::size_t i = start; i < end; ++i)
  {
    switch (this->evinfo[i].vclass)
    {
      case VERTEX_CLASS_SIMPLE:
        try
        { this->handle_vertex(i); }
        catch (Exception& e)
        {
          float p = (float)(i - start) / (float)(end - start) * 100.0f;
          std::cout << "Lloyd3d (" << (int)p << "%): " << e << std::endl;
        }
        break;

      default:
        //std::cout << "Lloyd3d: Vertex is not simple!" << std::endl;
        break;
    }
  }
}

/* ---------------------------------------------------------------- */

void
Lloyd3d::handle_vertex (std::size_t index)
{
  MeshVertexList const& verts = this->emesh->get_vertices();
  MeshFaceList const& faces = this->emesh->get_faces();

  /* Get adjacent faces and vertices for the vertex. */
  VertexInfo::FaceList const& fl = this->evinfo[index].adj_faces;
  VertexInfo::VertexList vl;
  this->evinfo->get_adjacent_vertices(index, vl);

  /* Check if we have features set. We will not move any feature
   * vertices to keep the original shape of the mesh. */
  if (!this->features->empty()
      && this->features->count_edges_for_vertex(index, vl) > 0)
    return;

  /* Build a 2D micro patch to calculate centroid. */
  MicroDelaunay md;
  md.set_center_vertex(verts[index]);
  for (std::size_t i = 0; i < vl.size(); ++i)
    md.append_adjacent_vertex(verts[vl[i]]);
  md.calculate_patch();

  /* If density information is available, we aim
   * at calculating a weighted centroid. */
  if (!this->rdens->empty())
  {
    bool valid_density = true;
    {
      VertexRef const& ref = this->reflist[index];
      VertexDensity vd = this->rdens->get_density(ref.face, ref.bary);
      md.set_center_density(vd.density);
      valid_density = valid_density && vd.valid;
    }
    for (std::size_t i = 0; i < vl.size(); ++i)
    {
      VertexRef const& ref = this->reflist[vl[i]];
      VertexDensity vd = this->rdens->get_density(ref.face, ref.bary);
      md.append_adjacent_density(vd.density);
      valid_density = valid_density && vd.valid;
    }
    if (!valid_density)
      md.clear_adjacent_density();
  }

  /* If we have constrained delaunay edges, pass them to allow
   * the Vornoi cell to be clipped against the edges. */
  if (!this->features->empty())
  {
    for (std::size_t i = 0; i < vl.size(); ++i)
    {
      std::size_t ip1 = (i + 1) % vl.size();
      if (this->features->is_feature_edge(vl[i], vl[ip1]))
        md.set_feature_edge(i);
    }
  }

  {
    /* Get centroid of voronoi cell. */
    Vec2f centroid = md.get_voronoi_centroid();
    //std::cout << "Centroid: " << centroid << std::endl;

    /* Get face and barycentric coordinates regarding evolving mesh. */
    std::size_t adj_face_idx;
    Vec2f bary;
    md.get_face_and_bary(centroid, adj_face_idx, bary);
    std::size_t new_face = fl[adj_face_idx];

    /* Shift barycentric coordinate from MicroPatch. */
    for (int i = 0; i < 3; ++i)
      if (faces[new_face * 3 + i] == index)
      {
        Vec3f tmp_bary(bary[0], bary[1], 1.0f - bary[0] - bary[1]);
        bary[0] = tmp_bary[(3 - i) % 3];
        bary[1] = tmp_bary[(4 - i) % 3];
      }

    /* Get vertex references for the containing face to the referece mesh. */
    VertexRef const& ref1 = this->reflist[faces[new_face * 3 + 0]];
    VertexRef const& ref2 = this->reflist[faces[new_face * 3 + 1]];
    VertexRef const& ref3 = this->reflist[faces[new_face * 3 + 2]];

    //std::cout << "Relocating vertex " << index << " to face " << new_face
    //    << " with " << bary << std::endl;

    VertexRef new_ref = this->reloc.relocate(ref1, ref2, ref3, bary);

    //std::cout << "Relocation results in face " << new_ref.face
    //    << " with " << new_ref.bary << std::endl;
    Triangle3f tri(this->rmesh, new_ref.face);

    /* Update point and reference. */
    this->emesh->get_vertices()[index] = tri.get_point_for_bary(new_ref.bary);
    this->reflist_new[index] = new_ref;
  }
}

/* ================================================================ */

LloydThread::LloydThread (Lloyd3d* lloyd, std::size_t start, std::size_t end)
  : lloyd(lloyd), start(start), end(end)
{
}

/* ---------------------------------------------------------------- */

void*
LloydThread::run (void)
{
  this->lloyd->handle_vertices(this->start, this->end);
  return 0;
}

REMESHER_NAMESPACE_END
