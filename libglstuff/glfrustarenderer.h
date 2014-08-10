#ifndef GL_FRUSTA_RENDERER_HEADER
#define GL_FRUSTA_RENDERER_HEADER

#include <vector>

#include "libremesh/vec3.h"
#include "libremesh/refptr.h"
#include "gldrawable.h"

struct GLFrustaParams
{
    Remesher::Vec3f pos;
    Remesher::Vec3f lookat;
    Remesher::Vec3f upvec;
};

/* ---------------------------------------------------------------- */

class GLFrustaRenderer;
typedef RefPtr<GLFrustaRenderer> GLFrustaRendererPtr;

class GLFrustaRenderer : public GLDrawable
{
  private:
    std::vector<GLFrustaParams> frusta;

  protected:
    void draw_impl (void);

  public:
    static GLFrustaRendererPtr create (void);
    void init (std::string const& bundle_file);
};

/* ---------------------------------------------------------------- */

inline GLFrustaRendererPtr
GLFrustaRenderer::create (void)
{
  GLFrustaRendererPtr ret(new GLFrustaRenderer);
  ret->init("/gris/scratch/mvs_datasets/rus_kapelle3-100623/bundle/synth_0.out");
  return ret;
}

#endif /* GL_FRUSTA_RENDERER_HEADER */
