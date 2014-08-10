#ifndef CURVATURE_EXTRACT_HEADER
#define CURVATURE_EXTRACT_HEADER

#include <vector>

#include "refptr.h"
#include "defines.h"
#include "algorithm.h"
#include "trianglemesh.h"
#include "vertexneighbours.h"

REMESHER_NAMESPACE_BEGIN

struct CurvatureFieldConf
{
  /* Alpha and beta describes how gauss and abs. mean curvature
   * values are weighted. Alpha and beta are greater zero and sum to
   * unity. Typical values are alpha = beta = 0.5f. */
  float alpha;
  float beta;

  /* The exponent for the contrast function, which is basically a gamma
   * function. An exponent of 1.0f disables the contrast function. */
  float contrast_exp;

  /* Laplacian smoothing settings. Smooth factor describes the intensity
   * of smoothing, typically around 0.5f. Smoothing is executed iteration
   * times, causing in values being distributed over more edges. */
  float smooth_factor;
  std::size_t smooth_iter;

  CurvatureFieldConf (void);
};

/* ---------------------------------------------------------------- */

/* A class to store the gaussian and absolute mean curvature.
 * The valid flag is set the vertex does not belong to a valid facette.
 */
struct VertexCurvature
{
  bool valid;
  float gaussian;
  float abs_mean;
  float density;

  VertexCurvature (void);
  VertexCurvature (float gaussian, float abs_mean);
};

/* ---------------------------------------------------------------- */

/* The curvature field calculates a curvature value for each vertex.
 * If the vertex neighbours are supplied, this data structure is
 * used. Otherwise vertex neighbours are calculated.
 */
class CurvatureField;
typedef RefPtr<CurvatureField> CurvatureFieldPtr;

class CurvatureField : public Algorithm, public std::vector<VertexCurvature>
{
  private:
    CurvatureFieldConf config;
    VertexNeighboursPtr vn;
    TriangleMeshPtr mesh;

  protected:
    CurvatureField (void);

    void* run (void);

    void calculate_curvature (void);
    void calculate_density (float alpha, float beta);
    void smooth_density_field (int iterations, float lambda);
    void apply_contrast_function (float exponent);

  public:
    static CurvatureFieldPtr create (void);

    void set_config (CurvatureFieldConf const& config);
    void set_mesh (TriangleMeshPtr mesh);
    void set_vertex_neighbours (VertexNeighboursPtr vn);
};

/* ================================================================ */

inline
CurvatureFieldConf::CurvatureFieldConf (void)
{
  this->alpha = 0.5f;
  this->beta = 0.5f;
  this->contrast_exp = 1.0f;
  this->smooth_factor = 0.5f;
  this->smooth_iter = 0;
}

inline
VertexCurvature::VertexCurvature (void)
{
  this->valid = false;
  this->gaussian = 0.0f;
  this->abs_mean = 0.0f;
}

inline
VertexCurvature::VertexCurvature (float gaussian, float abs_mean)
{
  this->valid = true;
  this->gaussian = gaussian;
  this->abs_mean = abs_mean;
}

inline CurvatureFieldPtr
CurvatureField::create (void)
{
  return CurvatureFieldPtr(new CurvatureField());
}

inline void
CurvatureField::set_config (CurvatureFieldConf const& config)
{
  this->config = config;
}

inline void
CurvatureField::set_mesh (TriangleMeshPtr mesh)
{
  this->mesh = mesh;
}

inline void
CurvatureField::set_vertex_neighbours (VertexNeighboursPtr vn)
{
  this->vn = vn;
}

REMESHER_NAMESPACE_END

#endif /* CURVATURE_EXTRACT_HEADER */
