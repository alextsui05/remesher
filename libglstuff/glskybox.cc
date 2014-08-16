#include <iostream>
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


#include "fonttexture.h"
#include "glskybox.h"

GLSkyBox::GLSkyBox (void)
{
  this->mmask = MODELVIEW_ROTATE;
  this->ppref = PROJECTION_FOV_43;
}

/* ---------------------------------------------------------------- */

void
GLSkyBox::glinit_impl (void)
{
  std::cout << "Loading skybox images..." << std::flush;
  for (std::size_t i = 0; i < 6; ++i)
  {
    std::string filename;
    switch (i)
    {
      case 0: filename = "zneg.png"; break;
      case 1: filename = "xpos.png"; break;
      case 2: filename = "zpos.png"; break;
      case 3: filename = "xneg.png"; break;
      case 4: filename = "ypos.png"; break;
      case 5: filename = "yneg.png"; break;
    }
    this->tex[i].load("../skybox/images/" + filename);

  }
  std::cout << " done." << std::endl;
}

/* ---------------------------------------------------------------- */

void
GLSkyBox::draw_impl (void)
{
  // Store the current matrix
  glPushMatrix();

  // Enable/Disable features
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);

  // Just in case we set all vertices to white.
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  // Render the front quad
  glBindTexture(GL_TEXTURE_2D, this->tex[0].glid);
  glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(  1.0f, -1.0f, -1.0f );
      glTexCoord2f(1, 0); glVertex3f( -1.0f, -1.0f, -1.0f );
      glTexCoord2f(1, 1); glVertex3f( -1.0f,  1.0f, -1.0f );
      glTexCoord2f(0, 1); glVertex3f(  1.0f,  1.0f, -1.0f );
  glEnd();

  // Render the left quad
  glBindTexture(GL_TEXTURE_2D, this->tex[1].glid);
  glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(  1.0f, -1.0f,  1.0f );
      glTexCoord2f(1, 0); glVertex3f(  1.0f, -1.0f, -1.0f );
      glTexCoord2f(1, 1); glVertex3f(  1.0f,  1.0f, -1.0f );
      glTexCoord2f(0, 1); glVertex3f(  1.0f,  1.0f,  1.0f );
  glEnd();

  // Render the back quad
  glBindTexture(GL_TEXTURE_2D, this->tex[2].glid);
  glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( -1.0f, -1.0f,  1.0f );
      glTexCoord2f(1, 0); glVertex3f(  1.0f, -1.0f,  1.0f );
      glTexCoord2f(1, 1); glVertex3f(  1.0f,  1.0f,  1.0f );
      glTexCoord2f(0, 1); glVertex3f( -1.0f,  1.0f,  1.0f );

  glEnd();

  // Render the right quad
  glBindTexture(GL_TEXTURE_2D, this->tex[3].glid);
  glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( -1.0f, -1.0f, -1.0f );
      glTexCoord2f(1, 0); glVertex3f( -1.0f, -1.0f,  1.0f );
      glTexCoord2f(1, 1); glVertex3f( -1.0f,  1.0f,  1.0f );
      glTexCoord2f(0, 1); glVertex3f( -1.0f,  1.0f, -1.0f );
  glEnd();

  // Render the top quad
  glBindTexture(GL_TEXTURE_2D, this->tex[4].glid);
  glBegin(GL_QUADS);
      glTexCoord2f(0, 1); glVertex3f( -1.0f,  1.0f, -1.0f );
      glTexCoord2f(0, 0); glVertex3f( -1.0f,  1.0f,  1.0f );
      glTexCoord2f(1, 0); glVertex3f(  1.0f,  1.0f,  1.0f );
      glTexCoord2f(1, 1); glVertex3f(  1.0f,  1.0f, -1.0f );
  glEnd();

  // Render the bottom quad
  glBindTexture(GL_TEXTURE_2D, this->tex[5].glid);
  glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f( -1.0f, -1.0f, -1.0f );
      glTexCoord2f(0, 1); glVertex3f( -1.0f, -1.0f,  1.0f );
      glTexCoord2f(1, 1); glVertex3f(  1.0f, -1.0f,  1.0f );
      glTexCoord2f(1, 0); glVertex3f(  1.0f, -1.0f, -1.0f );
  glEnd();

  // Restore enable bits and matrix
  glPopAttrib();
  glPopMatrix();
}
