#ifndef CAMERA_CONTROL_HEADER
#define CAMERA_CONTROL_HEADER

#include "libremesh/refptr.h"

enum MouseButtonMask
{
  MOUSE_BUTTON_1 = 1 << 8,
  MOUSE_BUTTON_2 = 1 << 9,
  MOUSE_BUTTON_3 = 1 << 10,
  MOUSE_BUTTON_4 = 1 << 11,
  MOUSE_BUTTON_5 = 1 << 12
};

/* ---------------------------------------------------------------- */

enum ModelviewMask
{
  MODELVIEW_TRANSLATE = 1 << 0,
  MODELVIEW_ROTATE = 1 << 1,
  MODELVIEW_SCALE = 1 << 2,
  MODELVIEW_ALL = MODELVIEW_TRANSLATE | MODELVIEW_ROTATE | MODELVIEW_SCALE
};

/* ---------------------------------------------------------------- */

enum ProjectionPref
{
  PROJECTION_FOV_22,
  PROJECTION_FOV_43,
  PROJECTION_FOV_62
};

/* ---------------------------------------------------------------- */

struct CameraInfo
{
  /* Dimension of the viewport. */
  int width, height;
  /* Clipping planes. */
  float znear, zfar;
  /* Dimension of the near plane. */
  float left, right, bottom, top;

  CameraInfo (void);
};

/* ---------------------------------------------------------------- */

class CameraControl;
typedef RefPtr<CameraControl> CameraControlPtr;

class CameraControl
{
  protected:
    CameraInfo info;

  protected:
    CameraControl (void);

  public:
    /* With and height are automatically set. */
    virtual ~CameraControl (void) {}
    void set_camera_info (CameraInfo const& info);

    /* Overwrite these. */
    virtual void set_mouse_move (MouseButtonMask mask, int x, int y) = 0;
    virtual void set_mouse_down (int button, int x, int y) = 0;
    virtual void set_mouse_up (int button, int x, int y) = 0;
    virtual void reset_view (void) = 0;

    /* Use these for scene setup. */
    virtual void setup_projection (ProjectionPref ppref = PROJECTION_FOV_22);
    virtual void apply_projection (void);
    virtual void apply_modelview (ModelviewMask mmask = MODELVIEW_ALL) = 0;

    void print_debug (void);
};

/* ---------------------------------------------------------------- */

inline
CameraInfo::CameraInfo (void)
{
  this->width = 0;
  this->height = 0;
  this->znear = 0.0f;
  this->zfar = 0.0f;
  this->left = 0.0f;
  this->right = 0.0f;
  this->bottom = 0.0f;
  this->top = 0.0f;
}

inline
CameraControl::CameraControl (void)
{
}

inline void
CameraControl::set_camera_info (CameraInfo const& info)
{
  this->info = info;
}

#endif /* CAMERA_CONTROL_HEADER */
