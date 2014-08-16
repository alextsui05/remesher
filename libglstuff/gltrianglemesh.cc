#include <iostream>
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "libremesh/vertexinfo.h"
#include "libremesh/triangle3.h"

#include "gltrianglemesh.h"

void
GLTriangleMesh::draw_impl (void)
{
  if (this->mesh.get() == 0)
    return;

  /* Shade the mesh with polygon offset enabled. */
  glPolygonOffset(2.0f, -2.0f);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_LIGHTING);

  glDepthFunc(GL_LEQUAL);

  switch (this->shading)
  {
    default:
    case MESH_SHADING_NONE:
      break;

    case MESH_SHADING_FACE:
      glCallList(this->dlist_id);
      break;

    case MESH_SHADING_VERTEX:
      glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      glColor4fv(&this->shade_color[0]);
      this->draw_with_vertex_normals();
      glDisable(GL_COLOR_MATERIAL);
      break;
  };

  glDisable(GL_POLYGON_OFFSET_FILL);

  /* Render decorations on the mesh. */
  glPolygonOffset(1.0f, -1.0f);
  glEnable(GL_POLYGON_OFFSET_LINE);
  glEnable(GL_BLEND);
  glDisable(GL_LIGHTING);

  glColor4fv(&this->deco_color[0]);
  if (this->decoration & MESH_DECO_POINTS)
  {
    this->draw_points(true);
  }

  glColor4fv(&this->deco_color[0]);
  if (this->decoration & MESH_DECO_LINES)
  {
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
    this->draw_with_vertex_normals(false);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
  }

  glColor4fv(&this->deco_color[0]);
  if (this->decoration & MESH_DECO_VORONOI)
  {
    this->draw_voronoi_cells();
  }

  if (this->decoration & MESH_DECO_VNORMALS)
  {
    this->draw_vertex_normals();
  }

  if (this->decoration & MESH_DECO_FNORMALS)
  {
    this->draw_face_normals();
  }

  if (this->decoration & MESH_DECO_FACE_ORDER)
  {
    this->draw_face_connections();
  }

  if (this->decoration & MESH_DECO_REGULARITY)
  {
    glPointSize(4.0f);
    this->draw_vertex_regularity();
  }

  glDisable(GL_BLEND);
  glDisable(GL_POLYGON_OFFSET_FILL);
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::set_mesh (Remesher::TriangleMeshPtr mesh)
{
  this->mesh = mesh;
  this->update_mesh();
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::update_mesh (void)
{
  this->regen_face_shading();
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::draw_points (bool allow_color)
{
  Remesher::MeshVertexList const& vertices = this->mesh->get_vertices();
  Remesher::MeshVertexColorList const& colors = this->mesh->get_vertex_colors();

#if 0
  std::cout << "Drawing points with immediate mode" << std::endl;
  glBegin(GL_POINTS);
  for (std::size_t i = 0; i < vertices.size(); ++i)
  {
    glVertex3fv(&vertices[i][0]);
    if (colors.size() == vertices.size())
      glColor3ub(colors[i][0], colors[i][1],colors[i][2]);
  }
  glEnd();
#endif  

  if (vertices.size() < 100000)
    glPointSize(2.0f);
  else
    glPointSize(1.0f);

  if (allow_color && this->color_array.size() == vertices.size())
  {
    glColorPointer(3, GL_FLOAT, 0, &this->color_array[0]);
    glEnableClientState(GL_COLOR_ARRAY);
  }
  else if (allow_color && colors.size() == vertices.size())
  {
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, &colors[0]);
    glEnableClientState(GL_COLOR_ARRAY);
  }

  glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size());
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::draw_with_vertex_normals (bool allow_color)
{
  Remesher::MeshVertexList const& vertices = this->mesh->get_vertices();
  Remesher::MeshFaceList const& faces = this->mesh->get_faces();
  Remesher::MeshNormalList const& vnormals = this->mesh->get_vertex_normals();
  //Remesher::MeshVertexColorList const& colors = this->mesh->get_vertex_colors();

  if (allow_color && this->color_array.size() == vertices.size())
  {
    glColorPointer(3, GL_FLOAT, 0, &this->color_array[0]);
    glEnableClientState(GL_COLOR_ARRAY);
  }
#if 0
  else if (allow_color && colors.size() == vertices.size())
  {
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, &colors[0]);
    glEnableClientState(GL_COLOR_ARRAY);
  }
#endif

  glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
  glNormalPointer(GL_FLOAT, 0, &vnormals[0]);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glDrawElements(GL_TRIANGLES, (GLsizei)faces.size(),
      GL_UNSIGNED_INT, &faces[0]);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::draw_with_face_normals (void)
{
  Remesher::MeshVertexList const& vertices = this->mesh->get_vertices();
  Remesher::MeshFaceList const& faces = this->mesh->get_faces();
  Remesher::MeshNormalList& fnormals = this->mesh->get_face_normals();
  //Remesher::MeshVertexColorList const& colors = this->mesh->get_vertex_colors();
  std::size_t size = faces.size() / 3;

  glBegin(GL_TRIANGLES);
  if (this->color_array.size() == vertices.size())
  {
    for (std::size_t i = 0; i < size; ++i)
    {
      glNormal3fv(&fnormals[i][0]);
      glColor3fv(&this->color_array[faces[i * 3 + 0]][0]);
      glVertex3fv(&vertices[faces[i * 3 + 0]][0]);
      glColor3fv(&this->color_array[faces[i * 3 + 1]][0]);
      glVertex3fv(&vertices[faces[i * 3 + 1]][0]);
      glColor3fv(&this->color_array[faces[i * 3 + 2]][0]);
      glVertex3fv(&vertices[faces[i * 3 + 2]][0]);
    }
  }
#if 0
  else if (colors.size() == vertices.size())
  {
    for (std::size_t i = 0; i < size; ++i)
    {
      glNormal3fv(&fnormals[i][0]);
      glColor3ubv(&colors[faces[i * 3 + 0]][0]);
      glVertex3fv(&vertices[faces[i * 3 + 0]][0]);
      glColor3ubv(&colors[faces[i * 3 + 1]][0]);
      glVertex3fv(&vertices[faces[i * 3 + 1]][0]);
      glColor3ubv(&colors[faces[i * 3 + 2]][0]);
      glVertex3fv(&vertices[faces[i * 3 + 2]][0]);
    }
  }
#endif
  else
  {
    for (std::size_t i = 0; i < size; ++i)
    {
      glNormal3fv(&fnormals[i][0]);
      glVertex3fv(&vertices[faces[i * 3 + 0]][0]);
      glVertex3fv(&vertices[faces[i * 3 + 1]][0]);
      glVertex3fv(&vertices[faces[i * 3 + 2]][0]);
    }
  }
  glEnd();
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::regen_face_shading (void)
{
  if (this->mesh.get() == 0)
    return;

  /* Clear and regenerate display list for flat shading. */
  if (this->dlist_id != 0)
  {
    glDeleteLists(this->dlist_id, 1);
    this->dlist_id = 0;
  }

  this->dlist_id = glGenLists(1);
  glNewList(this->dlist_id, GL_COMPILE);

  /* Render flat shading into the display list. */
  glEnable(GL_LIGHTING);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glColor4fv(&this->shade_color[0]);
  this->draw_with_face_normals();
  glDisable(GL_COLOR_MATERIAL);

  glEndList();
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::draw_vertex_normals (void)
{
  if (this->mesh.get() == 0)
    return;

  Remesher::MeshVertexList const& vertices = this->mesh->get_vertices();
  Remesher::MeshNormalList const& vnormals = this->mesh->get_vertex_normals();

  /* Draw vertex normals. */
  glBegin(GL_LINES);
  for (std::size_t i = 0; i < vertices.size(); ++i)
  {
    glVertex3fv(&vertices[i][0]);
    glVertex3fv(&(vertices[i] + vnormals[i] * 0.01f)[0]);
  }
  glEnd();
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::draw_face_normals (void)
{
  if (this->mesh.get() == 0)
    return;

  Remesher::MeshVertexList const& verts = this->mesh->get_vertices();
  Remesher::MeshFaceList const& faces = this->mesh->get_faces();
  Remesher::MeshNormalList const& fnormals = this->mesh->get_face_normals();

  glBegin(GL_LINES);
  std::size_t face_amount = faces.size() / 3;
  for (std::size_t i = 0; i < face_amount; ++i)
  {
    Remesher::Vec3f const& a = verts[faces[i * 3 + 0]];
    Remesher::Vec3f const& b = verts[faces[i * 3 + 1]];
    Remesher::Vec3f const& c = verts[faces[i * 3 + 2]];
    Remesher::Vec3f center = (a + b + c) / 3.0f;
    glVertex3fv(&center[0]);
    glVertex3fv(&(center + fnormals[i] * 0.01f)[0]);
  }
  glEnd();
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::draw_voronoi_cells (void)
{
  /* Draw the Voronoi cells. */
  Remesher::MeshFaceList const& faces = this->mesh->get_faces();
  Remesher::MeshVertexList const& verts = this->mesh->get_vertices();
  Remesher::VertexInfoListPtr vinfo
      = Remesher::VertexInfoList::create(this->mesh);

  glLineWidth(1.0f);
  glBegin(GL_LINES);
  for (std::size_t i = 0; i < vinfo->size(); ++i)
  {
    Remesher::VertexInfo::FaceList const& adj = vinfo[i].adj_faces;
    for (std::size_t j = 0; j < adj.size(); ++j)
    {
      std::size_t jp1 = (j + 1) % adj.size();

      //if (adj[j] < adj[jp1])
      //  continue;

      Remesher::Triangle3f tri1(verts[faces[adj[j] * 3 + 0]],
          verts[faces[adj[j] * 3 + 1]], verts[faces[adj[j] * 3 + 2]]);
      Remesher::Triangle3f tri2(verts[faces[adj[jp1] * 3 + 0]],
          verts[faces[adj[jp1] * 3 + 1]], verts[faces[adj[jp1] * 3 + 2]]);
      Remesher::Vec3f cc1 = tri1.get_circumcenter();
      Remesher::Vec3f cc2 = tri2.get_circumcenter();

      glVertex3fv(cc1.array());

      /* Calculate the intersection to avoid line hiding. */
      {
        /* Find vertices between the two faces. */
        std::size_t v1idx = MAX_SIZE_T;
        std::size_t v2idx = MAX_SIZE_T;
        for (std::size_t k = 0; k < 3; ++k)
          for (std::size_t l = 0; l < 3; ++l)
            if (faces[adj[j] * 3 + k] == faces[adj[jp1] * 3 + l]
                && faces[adj[j] * 3 + (k + 1) % 3]
                == faces[adj[jp1] * 3 + (l + 2) % 3])
            {
              v1idx = faces[adj[j] * 3 + k];
              v2idx = faces[adj[j] * 3 + (k + 1) % 3];
            }

        /* Calculate "intersection" edge and Voronoi line. */
        if (v1idx != MAX_SIZE_T && v2idx != MAX_SIZE_T)
        {
          Remesher::Vec3f const& xp = cc1;
          Remesher::Vec3f xd = cc2 - cc1;
          Remesher::Vec3f yp = verts[v1idx];
          Remesher::Vec3f yd = verts[v2idx] - yp;

          float s = ((xd * xd) * 4.0f * (yp * yd - yd * xp)
              + 4.0f * (xd * xp - xd * yp) * (xd * yd))
              / ((xd * yd) * (xd * yd) * 4.0f
              - 4.0f * (xd * xd) * (yd * yd));

          if (s > 0.0f && s < 1.0f)
          {
            Remesher::Vec3f p = yp + yd * s;
            glVertex3fv(p.array());
            glVertex3fv(p.array());
          }
        }
      }

      glVertex3fv(cc2.array());
    }
  }
  glEnd();
  glDisable(GL_BLEND);
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::draw_face_connections (void)
{
  glColor4f(0.2f, 0.2f, 1.0f, 1.0f);
  Remesher::MeshFaceList const& faces = this->mesh->get_faces();
  Remesher::MeshVertexList const& verts = this->mesh->get_vertices();
  for (std::size_t i = 0; i < faces.size() / 3 - 1; ++i)
  {
    Remesher::Vec3f a = verts[faces[i * 3 + 0]];
    Remesher::Vec3f b = verts[faces[i * 3 + 1]];
    Remesher::Vec3f c = verts[faces[i * 3 + 2]];

    Remesher::Vec3f d = verts[faces[(i + 1) * 3 + 0]];
    Remesher::Vec3f e = verts[faces[(i + 1) * 3 + 1]];
    Remesher::Vec3f f = verts[faces[(i + 1) * 3 + 2]];

    Remesher::Vec3f u = (a + b + c) / 3.0f;
    Remesher::Vec3f v = (d + e + f) / 3.0f;

    glBegin(GL_LINES);
      glVertex3fv(&u[0]);
      glVertex3fv(&v[0]);
    glEnd();
  }
}

/* ---------------------------------------------------------------- */

void
GLTriangleMesh::draw_vertex_regularity (void)
{
  Remesher::MeshVertexList const& verts = this->mesh->get_vertices();
  Remesher::VertexInfoListPtr vinfo
      = Remesher::VertexInfoList::create(this->mesh);

  std::size_t high_cnt = 0;
  std::size_t low_cnt = 0;
  std::size_t reg_cnt = 0;

  glBegin(GL_POINTS);
  for (std::size_t i = 0; i < verts.size(); ++i)
  {
    std::size_t deg = vinfo[i].adj_faces.size();
    bool high_deg = false;
    bool low_deg = false;

    switch (vinfo[i].vclass)
    {
      case Remesher::VERTEX_CLASS_SIMPLE:
        if (deg > 6)
          high_deg = true;
        else if (deg < 6)
          low_deg = true;
        break;

      case Remesher::VERTEX_CLASS_BORDER:
        if (deg > 3)
          high_deg = true;
        else if (deg < 3)
          low_deg = true;
        break;

      default:
        break;
    }

    if (high_deg)
    {
      glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3fv(&verts[i][0]);
      high_cnt += 1;
    }
    else if (low_deg)
    {
      glColor3f(0.0f, 0.0f, 0.0f);
      glVertex3fv(&verts[i][0]);
      low_cnt += 1;
    }
    else
    {
      reg_cnt += 1;
    }
  }
  glEnd();

  //std::cout << "Total vertices: " << verts.size()
  //    << ", high degree: " << high_cnt << ", low degree: " << low_cnt
  //    << ", regular: " << reg_cnt << std::endl;
}
