#include <cmath>
#include <iostream>

#include "curvaturefield.h"

REMESHER_NAMESPACE_BEGIN

CurvatureField::CurvatureField (void)
{
}

/* ---------------------------------------------------------------- */

void*
CurvatureField::run (void)
{
  /* Check if there is a valid mesh. */
  if (this->mesh.get() == 0 || this->mesh->get_vertices().size() == 0)
    return 0;

  /* Create connectivity information if it's not already there. */
  if (this->vn.get() == 0 || this->vn->size() == 0)
    this->vn = VertexNeighbours::create(this->mesh);

  /* Calculate the curvature values for each vertex. */
  this->calculate_curvature();

  /* Calculate the density value for each vertex. The density value
   * is a combination of the absolute gaussian curvature and the
   * absolute mean curvature, clamped to [0.0f, 1000.0f] and
   * scaled to range [0.0f, 1.0f]. */
  this->calculate_density(this->config.alpha, this->config.beta);

  /* Apply a contrast function. An exponent of 1.0f has no effect
   * and the contrast function is skipped in that case. */
  if (this->config.contrast_exp != 1.0f)
    this->apply_contrast_function(this->config.contrast_exp);

  /* Do laplacian smoothing. If either iterations is zero or the smooth
   * factor is zero, smoothing has no effect and is skipped. */
  if (this->config.smooth_iter != 0 && this->config.smooth_factor != 0.0f)
    this->smooth_density_field(this->config.smooth_iter,
        this->config.smooth_factor);

  return 0;
}

/* ---------------------------------------------------------------- */

void
CurvatureField::calculate_curvature (void)
{
  std::cout << "Building curvaure information." << std::endl;

  /* Create some short hands. */
  MeshVertexList const& vertices = this->mesh->get_vertices();
  //MeshFaceList const& faces = mesh->get_faces();
  //MeshNormalList const& fnormals = mesh->get_face_normals();

  /* Calculate curvatures for each vertex. */
  this->clear();
  this->reserve(this->vn->size());
  for (unsigned int i = 0; i < this->vn->size(); ++i)
  {
    std::vector<int>& vi = this->vn->at(i);

    if (vi.empty())
    {
      /* Push back an uninitialized (invalid) vertex curvature value. */
      //std::cout << "Invalid vertex found. Curvature not set!" << std::endl;
      this->push_back(VertexCurvature());
      continue;
    }

    /* Start computation by iterating over the triangles. This computation
     * determines the face areas and integral gaussian and mean curvature. */
    float face_areas = 0;
    float int_gaussian = 2.0f * (float)MY_PI;
    float int_abs_mean = 0.0f;

    /* Three edges with its length and two normals are maintained. */
    Vec3f e1;
    Vec3f e2 = vertices[vi[0]] - vertices[i];
    Vec3f e3 = vertices[vi[1]] - vertices[i];
    float len_e1;
    float len_e2 = e2.length();
    Vec3f n12;
    Vec3f n23 = e2.cross(e3);
    n23.norm_self();

    for (unsigned int j = 0; j < vi.size(); ++j)
    {
      /* Update edges, length and normals. */
      e1 = e2;
      e2 = e3;
      e3 = vertices[vi[(j + 2) % vi.size()]] - vertices[i];

      len_e1 = len_e2;
      len_e2 = e2.length();
      float len_e12 = (e2 - e1).length();

      n12 = n23;
      n23 = e2.cross(e3);
      n23.norm_self();

      /* Compute area of the triangle. */
      float p = (len_e1 + len_e2 + len_e12) / 2.0f;
      face_areas += ::sqrtf(p * (p - len_e1) * (p - len_e2) * (p - len_e12));

      /* Compute integral gaussian curvaure. */
      float angle = ::acosf(e1.scalar(e2) / (len_e1 * len_e2));
      int_gaussian -= angle;

      /* Compute integral absolute mean curvature. */
      float normal_scalar = n12.scalar(n23);
      normal_scalar = MY_MIN(1.0f, normal_scalar);
      int_abs_mean += 0.25f * len_e2 * ::fabsf(::acosf(normal_scalar));

      /* Check for NAN values. This is mainly to detect problems. */
      if (int_abs_mean != int_abs_mean)
      {
        std::cout << "NAN in iteration " << j << std::endl;
        std::cout << "Scalar: " << n12.scalar(n23) << std::endl;
        std::cout << "Acos: " << ::acosf(n12.scalar(n23)) << std::endl;
      }
    }

    /* Compute gaussian and absolute mean corvature. The barycentric area
     * of the triangles is used, therefore one third of the area is used. */
    float gaussian = 3 * int_gaussian / face_areas;
    float abs_mean = 3 * int_abs_mean / face_areas;

    if (abs_mean != abs_mean)
      std::cout << "Warning: Abs mean curvature is NAN" << std::endl;

    this->push_back(VertexCurvature(gaussian, abs_mean));
  }

  std::cout << "Done calculating curvature values." << std::endl;
}

/* ---------------------------------------------------------------- */

void
CurvatureField::calculate_density (float alpha, float beta)
{
  for (unsigned int i = 0; i < this->size(); ++i)
  {
    if (!this->at(i).valid)
      continue;

    VertexCurvature& vc = this->at(i);
    float gaussian_part = alpha * (float)::fabs(vc.gaussian);
    float abs_mean_part = beta * vc.abs_mean * vc.abs_mean;
    float curvature = gaussian_part + abs_mean_part;

    if (curvature < 0.0f)
      curvature = 0.0f;

    if (curvature > 1000.0f)
      curvature = 1000.0f;

    this->at(i).density = curvature / 1000.0f;
  }
}

/* ---------------------------------------------------------------- */

void
CurvatureField::smooth_density_field (int iterations, float lambda)
{
  if (iterations <= 0)
    return;

  for (unsigned int iter = 0; iter < (unsigned int)iterations; ++iter)
  {
    /* Backup density information. */
    std::vector<float> density;
    density.resize(this->size());
    for (unsigned int i = 0; i < this->size(); ++i)
      density[i] = this->at(i).density;

    /* Calculate new density. */
    for (unsigned int i = 0; i < this->size(); ++i)
    {
      if (!this->at(i).valid)
        continue;

      float density_sum = 0.0f;
      std::vector<int>& neighbours = this->vn->at(i);
      unsigned int valid_neighbours = 0;
      for (unsigned int v = 0; v < neighbours.size(); ++v)
      {
        if (!this->at(neighbours[v]).valid)
          continue;
        density_sum += density[neighbours[v]] - density[i];
        valid_neighbours += 1;
      }

      if (valid_neighbours > 0)
      {
        this->at(i).density = density[i] + lambda * density_sum / (float)valid_neighbours;
        if (this->at(i).density < 0.0f)
          this->at(i).density = 0.0f;
        if (this->at(i).density > 1.0f)
          this->at(i).density = 1.0f;
      }
    }
  }
}

/* ---------------------------------------------------------------- */

void
CurvatureField::apply_contrast_function (float exponent)
{
  for (unsigned int i = 0; i < this->size(); ++i)
  {
    this->at(i).density = ::powf(this->at(i).density, exponent);
  }
}

REMESHER_NAMESPACE_END
