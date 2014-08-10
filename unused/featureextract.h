#ifndef REMESHER_FEATURE_EXTRACT_HEADER
#define REMESHER_FEATURE_EXTRACT_HEADER

#include "defines.h"
#include "trianglemesh.h"
#include "edgemesh.h"

REMESHER_NAMESPACE_BEGIN

class FeatureExtract
{
  public:
    static EdgeMeshPtr from_mesh (TriangleMeshPtr mesh, float threshold);
};

REMESHER_NAMESPACE_END

#endif /* REMESHER_FEATURE_EXTRACT_HEADER */
