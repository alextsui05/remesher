#ifndef GL_SKY_BOX_HEADER
#define GL_SKY_BOX_HEADER

#include "libremesh/refptr.h"
#include "texture.h"
#include "gldrawable.h"

class GLSkyBox;
typedef RefPtr<GLSkyBox> GLSkyBoxPtr;

class GLSkyBox : public GLDrawable
{
  private:
    Texture tex[6];

  protected:
    GLSkyBox (void);

    void glinit_impl (void);
    void draw_impl (void);

  public:
    static GLSkyBoxPtr create (void);
};

/* ---------------------------------------------------------------- */

inline GLSkyBoxPtr
GLSkyBox::create (void)
{
  return GLSkyBoxPtr(new GLSkyBox);
}

#endif /* GL_SKY_BOX_HEADER */
