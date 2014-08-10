#include <iostream>
#include "vertexneighbours.h"

REMESHER_NAMESPACE_BEGIN

VertexNeighbours::VertexNeighbours (TriangleMeshPtr mesh)
{
  /* Create some short hands. */
  MeshVertexList const& vertices = mesh->get_vertices();
  MeshFaceList const& faces = mesh->get_faces();

  this->clear();
  this->resize(vertices.size());

  std::size_t fsize = faces.size() / 3;
  for (std::size_t i = 0; i < fsize; ++i)
  {
    this->at(faces[i * 3 + 0]).push_back(i);
    this->at(faces[i * 3 + 1]).push_back(i);
    this->at(faces[i * 3 + 2]).push_back(i);
  }

  /* The data structure now offers a list of adjacent triangles for each
   * vertex. These triangles are in no specific order. A new list
   * is created that corresponds to v1, ..., vn in counter-clockwise
   * order around v. The first triangle is then v, v1, v2, the second
   * is v, v2, v3, and so on.
   */
  for (std::size_t i = 0; i < this->size(); ++i)
    this->build_vertex_list(mesh, i);
}

/* ---------------------------------------------------------------- */

void
VertexNeighbours::build_vertex_list (TriangleMeshPtr mesh, std::size_t vertex)
{
  /* Backup the list of adjacent triangles. */
  std::vector<int> adj = this->at(vertex);

  /* The list of adj. triangles is replaced with adj. vertices. */
  this->at(vertex).clear();

  /* Degree of the vertex must be at least 3. */
  if (adj.size() < 3)
  {
    std::cout << "Exiting: Triangle valence too small" << std::endl;
    return;
  }

  /* Create a shorthand for the faces. */
  MeshFaceList const& faces = mesh->get_faces();

  /* Debugging dump. */
  #if 0
  std::cout << "Debugging: Common vertex: " << vertex << std::endl;
  for (unsigned int i = 0; i < adj.size(); ++i)
  {
    std::cout << "Triangle " << i << " (" << adj[i] << "): ";
    for (unsigned int j = 0; j < 3; ++j)
      std::cout << faces[adj[i] * 3 + j] << " ";
    std::cout << std::endl;
  }
  #endif

  /* Start with some triangle, determine starting and common vertex. */
  std::size_t fidx = adj.back() * 3;
  int starting = -1;
  int common = -1;
  for (unsigned int i = 0; i < 3; ++i)
    if (faces[fidx + i] == vertex)
    {
      starting = faces[fidx + (i + 1) % 3];
      common = faces[fidx + (i + 2) % 3];
      break;
    }

  /* Check if starting and common vertex are valid. */
  if (starting < 0 || common < 0)
  {
    /* The triangle does not belong to the vertex. Should not happen. */
    std::cout << "Exiting: Triangle does not belong" << std::endl;
    return;
  }

  /* Build a new list of vertices around the current. */
  this->at(vertex).push_back(starting);
  adj.pop_back();

  while (common != starting)
  {
    /* If there is no more adjacent triangle and we're not
     * around the vertex yet, it's a border vertex. */
    if (adj.empty())
    {
      std::cout << "Exiting: Not yet around and no more triangles" << std::endl;
      this->at(vertex).clear();
      return;
    }

    /* Find the adjacent triangle for the common vertex. Also, find
     * the next common vertex and store it. If there is no result,
     * there is a topologic problem. */
    int next_common = -1;
    unsigned int adj_idx = 0;

    for (; next_common < 0 && adj_idx < adj.size(); ++adj_idx)
      for (unsigned int j = 0; next_common < 0 && j < 3; ++j)
        if (faces[adj[adj_idx] * 3 + j] == (unsigned int)common)
          next_common = faces[adj[adj_idx--] * 3 + (j + 1) % 3];

    if (next_common < 0)
    {
      std::cout << "Exiting: No triangle with common vertex" << std::endl;
      this->at(vertex).clear();
      return;
    }

    /* Check if we found a new common vertex. Abort otherwise. */
    if (next_common == common)
    {
      std::cout << "Exiting: No new common vertex" << std::endl;
      this->at(vertex).clear();
      return;
    }

    /* Remove the current triangle and append a new vertex. */
    adj.erase(adj.begin() + adj_idx);
    this->at(vertex).push_back(common);
    common = next_common;
  }

  /* The new common vertex is the starting vertex and we're done.
   * A new list with ordered vertices has been created. If there
   * are more adjacent triangles left, it's probably a non-2-manifold. */
  if (!adj.empty())
  {
    std::cout << "Exiting: Non 2-manifold vertex detected" << std::endl;
    this->at(vertex).clear();
    return;
  }
}

REMESHER_NAMESPACE_END
