#ifndef GL_TEST_SCENE_HEADER
#define GL_TEST_SCENE_HEADER

#include <vector>

#include "libremesh/vec3.h"
#include "libremesh/plane.h"
#include "gldrawable.h"

class GLTestScene;
typedef RefPtr<GLTestScene> GLTestScenePtr;

class GLTestScene : public GLDrawable
{
  private:
    std::vector<Remesher::Vec3f> points;
    Remesher::Plane3f plane;

  protected:
    GLTestScene (void);
    void glinit_impl (void);
    void draw_impl (void);

  public:
    static GLTestScenePtr create (void);
};

/* ---------------------------------------------------------------- */

inline GLTestScenePtr
GLTestScene::create (void)
{
  return GLTestScenePtr(new GLTestScene);
}

#endif /* GL_TEST_SCENE_HEADER */
