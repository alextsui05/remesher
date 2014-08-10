#include <iostream>

#include "exception.h"
#include "pttdist.h"

REMESHER_NAMESPACE_BEGIN

PTTDist::PTTDist (void)
{
}

/* ---------------------------------------------------------------- */

void
PTTDist::set_triangle (Vec3f const& _a, Vec3f const& _b, Vec3f const& _c)
{
  /* Create the transformation matrix. */
  this->trans.calculate(_a, _b, _c);

  /* Transform the triangle vertices the the yz-plane. */
  Vec3f new_a = this->trans * _a;
  Vec3f new_b = this->trans * _b;
  Vec3f new_c = this->trans * _c;

  /* Generate 2D-coordinates for fast processing. */
  this->a[0] = new_a[2]; /* Near 0.0f, with numeric instability. */
  this->a[1] = new_a[1]; /* Same here. */
  this->b[0] = new_b[2];
  this->b[1] = new_b[1];
  this->c[0] = new_c[2];
  this->c[1] = new_c[1];

  #if REMESHER_NAN_CHECKS
  /* Detects NAN. May happen for zero-area triangles. */
  if (std::isnan(this->a[0]))
  {
    std::cout << "Detected NAN value!" << std::endl;
    std::cout << "A: " << _a << std::endl;
    std::cout << "B: " << _b << std::endl;
    std::cout << "C: " << _c << std::endl;
    std::cout << "New A: " << new_a << std::endl;
    std::cout << "New B: " << new_b << std::endl;
    std::cout << "New C: " << new_c << std::endl;
    std::cout << "Trans: " << this->trans << std::endl;
  }
  #endif

  /* Generate gradient information for the edges of the triangle. */
  this->e1m = this->b - this->a;
  this->e2m = this->c - this->b;
  this->e3m = this->a - this->c;
  this->e45m = Vec2f(this->e1m[1], -this->e1m[0]);
  this->e67m = Vec2f(this->e2m[1], -this->e2m[0]);
  this->e89m = Vec2f(this->e3m[1], -this->e3m[0]);

  #if 0
  /* Some code that detects numeric instability. */
  if (this->a[0] != 0.0f || this->a[1] != 0.0f)
  {
    std::cout << "Numerical instability detected!" << std::endl;
    std::cout << "  3D vertex A: " << _a << std::endl;
    std::cout << "  3D vertex B: " << _b << std::endl;
    std::cout << "  3D vertex C: " << _c << std::endl;
    std::cout << "  2D vertex A: " << this->a << " expected (0,0)" << std::endl;
    std::cout << "  2D vertex B: " << this->b << " expected (x,0)" << std::endl;
    std::cout << "  2D vertex C: " << this->c << std::endl;
  }
  #endif
}

/* ---------------------------------------------------------------- */

float
PTTDist::get_squared_dist (Vec3f const& v)
{
  /* Transformed vertex for fast distance calculation. */
  Vec3f new_v(this->trans * v);
  /* Projected point on the yz-plane. */
  Vec2f v_proj(new_v[2], new_v[1]);
  /* Determine region that is clothest. */
  this->region = this->get_region_for(v_proj);

  #if 0
  std::cout << "Transrolated point: " << new_v << ", "
      << "Projected point: " << v_proj << ", "
      << "Region is: " << this->region << std::endl;
  #endif

  switch (this->region)
  {
    case TRI_REGION_INSIDE:
      return new_v[0] * new_v[0];
    case TRI_REGION_EDGE_1:
      return squared_dist_to_edge(new_v, Vec3f(0.0f, this->a[1], this->a[0]),
          Vec3f(0.0f, this->b[1] - this->a[1], this->b[0] - this->a[0]));
    case TRI_REGION_EDGE_2:
      return squared_dist_to_edge(new_v, Vec3f(0.0f, this->b[1], this->b[0]),
          Vec3f(0.0f, this->c[1] - this->b[1], this->c[0] - this->b[0]));
    case TRI_REGION_EDGE_3:
      return squared_dist_to_edge(new_v, Vec3f(0.0f, this->c[1], this->c[0]),
          Vec3f(0.0f, this->a[1] - this->c[1], this->a[0] - this->c[0]));
    case TRI_REGION_VERTEX_A: /* Vertex A is (0,0) */
      return new_v[0] * new_v[0] + new_v[1] * new_v[1] + new_v[2] * new_v[2];
    case TRI_REGION_VERTEX_B: /* Vertex B is (x, y) */
      return new_v[0] * new_v[0]
          + (new_v[1] - this->b[1]) * (new_v[1] - this->b[1])
          + (new_v[2] - this->b[0]) * (new_v[2] - this->b[0]);
    case TRI_REGION_VERTEX_C: /* Vertex C is (x, y) */
      return new_v[0] * new_v[0]
          + (new_v[1] - this->c[1]) * (new_v[1] - this->c[1])
          + (new_v[2] - this->c[0]) * (new_v[2] - this->c[0]);
  }
  throw std::exception();
}

/* ---------------------------------------------------------------- */

TriangleRegion
PTTDist::get_region_for (Vec2f const& v)
{
  float ceq4 = this->ceq(this->a, this->e45m, v);
  float ceq9 = this->ceq(this->a, this->e89m, v);

  if (ceq4 >= 0.0f && ceq9 <= 0.0f)
    return TRI_REGION_VERTEX_A;

  float ceq5 = this->ceq(this->b, this->e45m, v);
  float ceq6 = this->ceq(this->b, this->e67m, v);

  if (ceq5 <= 0.0f && ceq6 >= 0.0f)
    return TRI_REGION_VERTEX_B;

  float ceq7 = this->ceq(this->c, this->e67m, v);
  float ceq8 = this->ceq(this->c, this->e89m, v);

  if (ceq7 <= 0.0f && ceq8 >= 0.0f)
    return TRI_REGION_VERTEX_C;

  float ceq1 = this->ceq(this->a, this->e1m, v);
  float ceq2 = this->ceq(this->b, this->e2m, v);
  float ceq3 = this->ceq(this->c, this->e3m, v);

  if (ceq1 <= 0.0f && ceq2 <= 0.0f && ceq3 <= 0.0f)
    return TRI_REGION_INSIDE;

  if (ceq1 > 0.0f && ceq4 < 0.0f && ceq5 > 0.0f)
    return TRI_REGION_EDGE_1;

  if (ceq2 > 0.0f && ceq6 < 0.0f && ceq7 > 0.0f)
    return TRI_REGION_EDGE_2;

  if (ceq3 > 0.0f && ceq8 < 0.0f && ceq9 > 0.0f)
    return TRI_REGION_EDGE_3;

  /* Can this ever happen?
   * Yes, if NAN or other strange things are returned.
   */
  std::cout << "Error: Could not determine region, debug follows" << std::endl;
  std::cout << "  ceq1: " << ceq1 << std::endl;
  std::cout << "  ceq2: " << ceq2 << std::endl;
  std::cout << "  ceq3: " << ceq3 << std::endl;
  std::cout << "  ceq4: " << ceq4 << std::endl;
  std::cout << "  ceq5: " << ceq5 << std::endl;
  std::cout << "  ceq6: " << ceq6 << std::endl;
  std::cout << "  ceq7: " << ceq7 << std::endl;
  std::cout << "  ceq8: " << ceq8 << std::endl;
  std::cout << "  ceq9: " << ceq9 << std::endl;

  std::cout << "  2D A: " << this->a << std::endl;
  std::cout << "  2D B: " << this->b << std::endl;
  std::cout << "  2D C: " << this->c << std::endl;

  std::cout << "  Returing REGION_VERTEX_A" << std::endl;

  return TRI_REGION_VERTEX_A;
  //throw Exception("Invalid region detected");
}

/* ---------------------------------------------------------------- */

Vec3f
PTTDist::nearest_vert_on_edge (Vec3f const& v, Vec3f const& p, Vec3f const& r)
{
  /* This solves lambda in the following equation:
   * r * (p + lambda * r) = r * v
   * "p + lambda * r" is the equation for the straigt line.
   */
  float lambda = (r[0] * (v[0] - p[0]) + r[1] * (v[1] - p[1])
      + r[2] * (v[2] - p[2])) / (r[0] * r[0] + r[1] * r[1] + r[2] * r[2]);

  if (lambda < 0.0f)
  {
    std::cout << "Warning: lambda < 0.0f: " << lambda << std::endl;
  }
  if (lambda > 1.0f)
  {
    std::cout << "Warning: lambda > 1.0f: +" << lambda << std::endl;
  }

  //if (lambda > 10.0f)
  //  throw std::string("out");
  //if (lambda < -10.0f)
  //  throw std::string("out");

  /* This is the nearest point on the edge. */
  return p + r * lambda;
}

/* ---------------------------------------------------------------- */

float
PTTDist::squared_dist_to_edge (Vec3f const& v, Vec3f const& p, Vec3f const& r)
{
  /* "d" is the vector between "v" and "x"
   * (x is the nearest point on the edge p + labmda * r). */
  Vec3f d = v - this->nearest_vert_on_edge(v, p, r);

  //std::cout << "Nearest point to v vector is " << d << " using "
  //    << "p = " << p << ", r = " << r << std::endl;

  /* Return the squared length of "d". */
  return d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
}

REMESHER_NAMESPACE_END
