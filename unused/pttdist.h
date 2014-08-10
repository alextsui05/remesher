#ifndef REMESHER_PTT_DIST_HEADER
#define REMESHER_PTT_DIST_HEADER

#include "defines.h"
#include "vec2.h"
#include "vec3.h"
#include "matrix4.h"
#include "triangletrans.h"

REMESHER_NAMESPACE_BEGIN

enum TriangleRegion
{
  TRI_REGION_INSIDE,
  TRI_REGION_EDGE_1,
  TRI_REGION_EDGE_2,
  TRI_REGION_EDGE_3,
  TRI_REGION_VERTEX_A,
  TRI_REGION_VERTEX_B,
  TRI_REGION_VERTEX_C
};

/*
 * PTT dist -- Point-to-triangle distance
 *
 * This class calculates the squared distance (ommiting the square root
 * for optimization purposes) from a point to a triangle.
 *
 * The code is optimized for processing many points (with get_squared_dist())
 * for one triangle set with set_triangle().
 */
class PTTDist
{
  private:
    /* Transformaton matrix to transform a triangle to the yz-plane. */
    TriangleTrans trans;
    /* Edge points for the new triangle in 2D. */
    Vec2f a, b, c;
    /* Edge equation gardient information. */
    Vec2f e1m, e2m, e3m, e45m, e67m, e89m;
    /* This is the region of the clothest point on the triangle for
     * the last vertex given to get_squared_dist(...). */
    TriangleRegion region;

    /* ceq -- calculate edge equation
     *
     * The following function calculates the edge equation value
     * of v for a line passing through p with gardient m[1] / m[0]. */
    float ceq (Vec2f const& p, Vec2f const& m, Vec2f const& v);

  public:
    PTTDist (void);

    /* Calculates a new transformation matrix for fast
     * point-to-triangle distance calculations. */
    void set_triangle (Vec3f const& _a, Vec3f const& _b, Vec3f const& _c);

    /* Returns the squared distance from "v" to the triangle.
     * Also sets the region for the clothest point on the triangle.
     * Retrieve this with get_last_region(). */
    float get_squared_dist (Vec3f const& v);

    /* Calculates the shortest distance from "v" to a straight line
     * given by a position "p" and a direction "r". */
    float squared_dist_to_edge (Vec3f const& v, Vec3f const& p, Vec3f const& r);

    /* Calculates the shortest point from "v" on a straight line
     * given by a position "p" and a direction "r". */
    Vec3f nearest_vert_on_edge (Vec3f const& v, Vec3f const& p, Vec3f const& r);

    /* Calculates the region for a transformed and projected vertex "v".
     * "v" must be transformed using "trans" and projected on the
     * yz-plane (ignoring the x-coordinate, "y" and "z" swapped). */
    TriangleRegion get_region_for (Vec2f const& v);

    /* Returns the clothest region for the last point given
     * to get_squared_dist(). */
    TriangleRegion get_last_region (void) const;

    /* Returns the transformation matrix used to transform the triangle
     * so that "a" is in the orgin, and "b" and "c" are
     * somewhere in the yz-plane. */
    Matrix4f const& get_transformation (void) const;
};

/* ---------------------------------------------------------------- */

inline float
PTTDist::ceq (Vec2f const& p, Vec2f const& m, Vec2f const& v)
{
  return (v[0] - p[0]) * m[1] - (v[1] - p[1]) * m[0];
}

inline TriangleRegion
PTTDist::get_last_region (void) const
{
  return this->region;
}

inline Matrix4f const&
PTTDist::get_transformation (void) const
{
  return this->trans;
}

REMESHER_NAMESPACE_END

#endif /* REMESHER_PTT_DIST_HEADER */
