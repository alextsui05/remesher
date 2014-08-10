#include <iostream>

#include "featureextract.h"

REMESHER_NAMESPACE_BEGIN

EdgeMeshPtr
FeatureExtract::from_mesh (TriangleMeshPtr mesh, float threshold)
{
  std::cout << "Start extracting features" << std::endl;

  EdgeMeshPtr edgemesh = EdgeMesh::create();
  EdgeList& edges = edgemesh->get_edges();

  MeshVertexList& fnormals = mesh->get_face_normals();
  MeshFaceList& faces = mesh->get_faces();
  unsigned int num_faces = faces.size() / 3;

  for (unsigned int i = 0; i < num_faces; ++i)
  {
    for (unsigned int j = 0; j < 3; ++j)
    {
      unsigned int idx1[2];
      idx1[0] = faces[i * 3 + j];
      idx1[1] = faces[i * 3 + ((j + 1) % 3)];

      if (idx1[0] > idx1[1])
      {
        unsigned int tmp(idx1[0]);
        idx1[0] = idx1[1];
        idx1[1] = tmp;
      }

      /* Seek for the same edge in subsequent triangles. */
      bool found = false;
      for (unsigned int k = i + 1; k < num_faces; ++k)
      {
        for (unsigned int l = 0; l < 3; ++l)
        {
          unsigned int idx2[2];
          idx2[0] = faces[k * 3 + l];
          idx2[1] = faces[k * 3 + ((l + 1) % 3)];

          if (idx2[0] > idx2[1])
          {
            unsigned int tmp(idx2[0]);
            idx2[0] = idx2[1];
            idx2[1] = tmp;
          }

          /* Valid edge found. */
          if (idx1[0] == idx2[0] && idx1[1] == idx2[1])
          {
            Vec3f& n1 = fnormals[i];
            Vec3f& n2 = fnormals[k];
            float angle = n1.scalar(n2);
            if (angle < threshold)
              edges.push_back(std::make_pair(idx1[0], idx1[1]));
            found = true;
          }
        }
      }

      /* The current edge seems to be a border. */
      if (!found)
      {
        /* TODO */
      }
    }
  }

  std::cout << "Done extracting features" << std::endl;

  return edgemesh;
}

REMESHER_NAMESPACE_END
