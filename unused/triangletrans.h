#ifndef REMESHER_TRIANGLE_TRANS_HEADER
#define REMESHER_TRIANGLE_TRANS_HEADER

#include "defines.h"
#include "matrix4.h"
#include "vec3.h"

REMESHER_NAMESPACE_BEGIN

/*
 * This class calculates a transformation matrix for triangles
 * using rotation and translation so that after transformation
 * vertex "a" lies in the origin, and the normal of the triangle points
 * towards the negative x-axis. So, vertex "b" and "c" lies on the yz-plane.
 */
class TriangleTrans : public Matrix4f
{
  public:
    TriangleTrans (void);
    TriangleTrans (Vec3f const& a, Vec3f const& b, Vec3f const& c);
    void calculate (Vec3f const& a, Vec3f const& b, Vec3f const& c);
};

/* ---------------------------------------------------------------- */

inline
TriangleTrans::TriangleTrans (void)
{
}

inline
TriangleTrans::TriangleTrans (Vec3f const& a,
    Vec3f const& b, Vec3f const& c)
{
  this->calculate(a, b, c);
}

REMESHER_NAMESPACE_END

#endif /* REMESHER_TRIANGLE_TRANS_HEADER */
