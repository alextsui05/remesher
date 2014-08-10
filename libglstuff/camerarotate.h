#ifndef CAMERA_ROTATE_HEADER
#define CAMERA_ROTATE_HEADER

#include "libremesh/vec3.h"
#include "cameracontrol.h"

#define RAD_PER_FRAME ((float)MY_PI / 180.0f)

class CameraRotate : public CameraControl
{
private:
    Remesher::Vec3f scene_upvec;

    int cur_frame;
    float cur_rad;

public:
    CameraRotate (void);
    static CameraControlPtr create (void);

    void set_mouse_move (MouseButtonMask mask, int x, int y);
    void set_mouse_down (int button, int x, int y);
    void set_mouse_up (int button, int x, int y);
    void reset_view (void);
    void apply_modelview (ModelviewMask mask);

    void next_frame (void);
};

/* ---------------------------------------------------------------- */

inline CameraControlPtr
CameraRotate::create (void)
{
  return CameraControlPtr(new CameraRotate);
}

#endif /* CAMERA_ROTATE_HEADER */
