#ifndef CAMERA_TRACKBALL_HEADER
#define CAMERA_TRACKBALL_HEADER

#include "libremesh/vec3.h"
#include "cameracontrol.h"

#define CAM_TRACKBALL_Z_DIST 3.0f

class CameraTrackball : public CameraControl
{
  private:
    Remesher::Vec3f click_vec;
    Remesher::Vec3f click_axis;
    float click_angle;
    int click_scale_y;
    float click_scale;

    Remesher::Vec3f move_vec;
    Remesher::Vec3f rot_axis;
    float rot_angle;
    float scale;

  protected:
    CameraTrackball (void);

    Remesher::Vec3f get_pointer_vector (float x, float y);
    Remesher::Vec3f get_new_center (int x, int y);

  public:
    static CameraControlPtr create (void);

    void set_mouse_move (MouseButtonMask mask, int x, int y);
    void set_mouse_down (int button, int x, int y);
    void set_mouse_up (int button, int x, int y);
    void reset_view (void);

    void set_center (Remesher::Vec3f const& center);

    void apply_modelview (ModelviewMask mmask = MODELVIEW_ALL);
};

/* ---------------------------------------------------------------- */

inline CameraControlPtr
CameraTrackball::create (void)
{
  return CameraControlPtr(new CameraTrackball);
}

#endif /* CAMERA_TRACKBALL_HEADER */
