#include <iostream>
#include <GL/gl.h>

#include "camerarotate.h"

CameraRotate::CameraRotate (void)
{
    this->reset_view();
}

/* ---------------------------------------------------------------- */

void
CameraRotate::reset_view (void)
{
    this->scene_upvec = Remesher::Vec3f(0.0f, 0.0f, -1.0f);
    this->cur_rad = 0.0f;
    this->cur_frame = 200;
}

/* ---------------------------------------------------------------- */

void
CameraRotate::next_frame (void)
{
    this->cur_frame += 1;
    this->cur_rad = RAD_PER_FRAME * (float)this->cur_frame;
    while (this->cur_rad > 2.0f * MY_PI)
        this->cur_rad -= 2.0f * MY_PI;
    std::cout << "Current frame: " << this->cur_frame << std::endl;
}

/* ---------------------------------------------------------------- */

void
CameraRotate::apply_modelview (ModelviewMask /*mask*/)
{
    float& v1 = this->scene_upvec[0];
    float& v2 = this->scene_upvec[1];
    float& v3 = this->scene_upvec[2];

    float omcosa = 1.0f - std::cos(this->cur_rad);
    float sina = std::sin(this->cur_rad);
    float cosa = std::cos(this->cur_rad);

    float mat[16] =
    {
        cosa + v1 * v1 * omcosa,
        v2 * v1 * omcosa + v3 * sina,
        v3 * v1 * omcosa - v2 * sina,
        0.0f,

        v1 * v2 * omcosa - v3 * sina,
        cosa + v2 * v2 * omcosa,
        v3 * v2 * omcosa + v1 * sina,
        0.0f,

        v1 * v3 * omcosa + v2 * sina,
        v2 * v3 * omcosa - v1 * sina,
        cosa + v3 * v3 * omcosa,
        0.0f,

        0.0f, 0.0f, 0.0f, 1.0f
    };

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(-75.0f, 1.0f, 0.0f, 0.0f);
    glMultMatrixf(mat);

#if 0
    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);

        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);

        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
#endif

    glTranslatef(0.0f, -0.25f, -0.05f);
}

/* ---------------------------------------------------------------- */

void
CameraRotate::set_mouse_move (MouseButtonMask, int, int)
{
}

void 
CameraRotate::set_mouse_down (int, int, int)
{
}

void 
CameraRotate::set_mouse_up (int, int, int)
{
}

