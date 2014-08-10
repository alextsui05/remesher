#include <iostream>
#include <vector>
#include <GL/gl.h>

#include "gldefines.h"
#include "texture.h"

#if LIBSOIL_TEXTURE_LOADING
# include <SOIL/SOIL.h>
#endif

Texture::Texture (void)
{
}

/* ---------------------------------------------------------------- */

void
Texture::load (std::string const& filename)
{
#if LIBSOIL_TEXTURE_LOADING
  this->glid = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO,
      SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

  if (this->glid == 0)
    std::cout << "Error loading image: " << SOIL_last_result() << std::endl;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#else
  this->test_texture();
  std::cout << "Texture loading is disabled because libSOIL is disabled."
      << std::endl;
#endif
}

/* ---------------------------------------------------------------- */

void
Texture::test_texture (void)
{
  #define TEX_SIZE 8
  #define CHECK_SIZE 1

  std::vector<GLubyte> bitmap;
  bitmap.resize(TEX_SIZE * TEX_SIZE * 3);

  for (int i = 0; i < TEX_SIZE; ++i)
    for (int j = 0; j < TEX_SIZE; ++j)
    {
      //GLubyte c = (((((i & CHECK_SIZE) == 0) ^ ((j & CHECK_SIZE))) == 0)) * 255;
      GLubyte c = (GLubyte)(((i / CHECK_SIZE + j / CHECK_SIZE) % 2 == 0) * 255);
      bitmap[(i * TEX_SIZE + j) * 3 + 0] = c;
      bitmap[(i * TEX_SIZE + j) * 3 + 1] = c;
      bitmap[(i * TEX_SIZE + j) * 3 + 2] = c;
    }

  //glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &this->glid);
  glBindTexture(GL_TEXTURE_2D, this->glid);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_SIZE, TEX_SIZE, 0,
      GL_RGB, GL_UNSIGNED_BYTE, &bitmap[0]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
