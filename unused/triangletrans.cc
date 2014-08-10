#include <cmath>

#include "exception.h"
#include "triangletrans.h"

REMESHER_NAMESPACE_BEGIN

void
TriangleTrans::calculate (Vec3f const& a,
    Vec3f const& b, Vec3f const& c)
{
  /* Normal for this triangle. */
  Vec3f n((b - a).cross(c - a).norm());

  /* Detects zero-surface triangles. n[i] is NAN then. */
  if (std::isnan(n[0]) || std::isnan(n[1]) || std::isnan(n[2]))
    throw Exception("NAN values in triangle normal");

  Matrix4f m1;
  /* Check if the the triangle is already aligned (that is, if
   * the normal of the triangle and the yz-plane have same direction).
   * If it is, rotation vector can not be calcuated.
   * Handle this case separately and more efficient. */
  if (n[1] == 0.0f && n[2] == 0.0f)
  {
    if (n[0] > 0.0f)
    {
      /* Alignment need to be fixed with a 180 degree
       * rotation around the y axis. */
      m1[0] = -1.0f; m1[10] = -1.0f;
      m1[3] = a[0];  m1[11] = a[2];
    }
    else
    {
      /* Alignment need not to be fixed. */
      m1[0] = 1.0f;  m1[10] = 1.0f;
      m1[3] = -a[0]; m1[11] = -a[2];
    }

                   m1[1]  = 0.0f; m1[2]  = 0.0f;
    m1[4]  = 0.0f; m1[5]  = 1.0f; m1[6]  = 0.0f; m1[7]  = -a[1];
    m1[8]  = 0.0f; m1[9]  = 0.0f;
    m1[12] = 0.0f; m1[13] = 0.0f; m1[14] = 0.0f; m1[15] = 1.0f;
  }
  else
  {
    /* This is the rotation vector for the first rotation.
     * Cross product of n and (-1, 0, 0), normalized.
     */
    Vec3f v = Vec3f(0.0f, -n[2], n[1]).norm();

    /* cos(a) = n * (-1, 0, 0) / (|n| * |(-1, 0, 0)|)
     *        = -n[0] (n is already normalized)
     */
    float cos_alpha = -n[0];
    float sin_alpha = std::sin(std::acos(cos_alpha));

    /* Pre-computations for the matrix. */
    float one_m_ca = 1.0f - cos_alpha;
    float v0_sa = v[0] * sin_alpha;
    float v1_sa = v[1] * sin_alpha;
    float v2_sa = v[2] * sin_alpha;
    float v0_omca = v[0] * one_m_ca;
    float v1_omca = v[1] * one_m_ca;
    float v2_omca = v[2] * one_m_ca;

    /* The first rotation matrix translates the triangle so that "a" is in
     * the center. Then it rotates the triangle into the yz-plane. */
    m1[0] = cos_alpha + v[0] * v0_omca;
    m1[1] = v[0] * v1_omca - v2_sa;
    m1[2] = v[0] * v2_omca + v1_sa;
    m1[3] = m1[0] * -a[0] - m1[1] * a[1] - m1[2] * a[2];

    m1[4] = v[1] * v0_omca + v2_sa;
    m1[5] = cos_alpha + v[1] * v1_omca;
    m1[6] = v[1] * v2_omca - v0_sa;
    m1[7] = m1[4] * -a[0] - m1[5] * a[1] - m1[6] * a[2];

    m1[8] = v[2] * v0_omca - v1_sa;
    m1[9] = v[2] * v1_omca + v0_sa;
    m1[10] = cos_alpha + v[2] * v2_omca;
    m1[11] = m1[8] * -a[0] - m1[9] * a[1] - m1[10] * a[2];

    m1[12] = 0.0f;
    m1[13] = 0.0f;
    m1[14] = 0.0f;
    m1[15] = 1.0f;
  }

  #if 0 /* No need for the second rotation. */
  /* The second rotation rotates vertex b onto the z axis. */
  Vec3f b2 = m1 * b;
  float beta = atan2(b2[1], b2[2]);
  //float cos_beta = b2[2] / b2.length();
  //float sin_beta = sin(acos(cos_beta));
  float cos_beta = cos(beta);
  float sin_beta = sin(beta);

  Matrix4f m2;
  m2[0]  = 1.0f;  m2[1] = 0.0f;      m2[2]  = 0.0f;     m2[3]  = 0.0f;
  m2[4]  = 0.0f;  m2[5] = cos_beta;  m2[6]  = -sin_beta; m2[7]  = 0.0f;
  m2[8]  = 0.0f;  m2[9] = sin_beta; m2[10] = cos_beta; m2[11] = 0.0f;
  m2[12] = 0.0f;  m2[13] = 0.0f;     m2[14] = 0.0f;     m2[15] = 1.0f;

  Matrix4f final = m2 * m1;

  #endif

  for (unsigned int i = 0; i < 16; ++i)
    this->m[i] = m1[i];
}

REMESHER_NAMESPACE_END
