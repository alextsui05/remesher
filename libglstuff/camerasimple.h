#ifndef CAMERA_SIMPLE_HEADER
#define CAMERA_SIMPLE_HEADER

#include "cameracontrol.h"

#define CAM_SIMPLE_Z_DIST 3.0f

class CameraSimple : public CameraControl
{
  private:
    int motion_begin_x;
    int motion_begin_y;
    int motion_dist_x;
    int motion_dist_y;

    int click_scale_y;
    float click_scale;
    float scale;

  protected:
    CameraSimple (void);

  public:
    static CameraControlPtr create (void);

    void set_mouse_move (MouseButtonMask mask, int x, int y);
    void set_mouse_down (int button, int x, int y);
    void set_mouse_up (int button, int x, int y);
    void reset_view (void);

    void apply_modelview (ModelviewMask mmask = MODELVIEW_ALL);
};

/* ---------------------------------------------------------------- */

inline CameraControlPtr
CameraSimple::create (void)
{
  return CameraControlPtr(new CameraSimple);
}

#endif /* CAMERA_SIMPLE_HEADER */
