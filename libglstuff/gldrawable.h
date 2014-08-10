#ifndef GL_DRAWABLE_HEADER
#define GL_DRAWABLE_HEADER

#include "libremesh/refptr.h"
#include "cameracontrol.h"

class GLDrawable;
typedef RefPtr<GLDrawable> GLDrawablePtr;

/* A base class for drawable elements for GtkOpenGLScene */
class GLDrawable
{
  protected:
    bool visible;
    bool initialized;
    ModelviewMask mmask;
    ProjectionPref ppref;

    GLDrawable (void);

    /* Hotspot. All drawing goes here, this is called each frame. */
    virtual void draw_impl (void) = 0;

    /* Hotspot. GL initialization code goes here. This is because in
     * the constructor, the GL context is not available. */
    virtual void glinit_impl (void);

  public:
    virtual ~GLDrawable (void);

    void draw (void);
    void set_visible (bool value);
    bool get_visible (void) const;

    /* Rendering preferences of the drawable. */
    void set_modelview (ModelviewMask mmask);
    ModelviewMask get_modelview (void) const;

    void set_projection (ProjectionPref ppref);
    ProjectionPref get_projection (void) const;
};

/* ---------------------------------------------------------------- */

inline
GLDrawable::GLDrawable (void)
  : visible(true), initialized(false),
    mmask(MODELVIEW_ALL), ppref(PROJECTION_FOV_22)
{
}

inline
GLDrawable::~GLDrawable (void)
{
}

inline void
GLDrawable::draw (void)
{
  if (!visible)
    return;

  if (!this->initialized)
  {
    this->glinit_impl();
    this->initialized = true;
  }

  this->draw_impl();
}

inline void
GLDrawable::set_visible (bool value)
{
  this->visible = value;
}

inline bool
GLDrawable::get_visible (void) const
{
  return this->visible;
}

inline void
GLDrawable::set_modelview (ModelviewMask mmask)
{
  this->mmask = mmask;
}

inline ModelviewMask
GLDrawable::get_modelview (void) const
{
  return this->mmask;
}

inline void
GLDrawable::set_projection (ProjectionPref ppref)
{
  this->ppref = ppref;
}

inline ProjectionPref
GLDrawable::get_projection (void) const
{
  return this->ppref;
}

inline void
GLDrawable::glinit_impl (void)
{
}

#endif /* GL_DRAWABLE_HEADER */
