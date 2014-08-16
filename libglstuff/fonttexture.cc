#include <iostream>
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "libremesh/exception.h"
#include "consolefont.h"
#include "fonttexture.h"

#define BG_COLOR 0, 0, 0, 64
#define FG_COLOR 255, 255, 255, 255

FontTexture::FontTexture (void)
{
}

/* ---------------------------------------------------------------- */

FontTexture::~FontTexture (void)
{
  glDeleteTextures(1, &this->tex_id);
}

/* ---------------------------------------------------------------- */

void
FontTexture::bind (void) const
{
  glBindTexture(GL_TEXTURE_2D, this->tex_id);
}

/* ---------------------------------------------------------------- */

void
FontTexture::set_text (std::string const& text)
{
  if (text.empty())
    throw Remesher::Exception("No characters given");

  this->pixel_width = CONSOLE_FONT_WIDTH * text.size() + text.size() - 1 + 2;
  this->pixel_height = CONSOLE_FONT_HEIGHT + 2;
  GLubyte* tex_data = new GLubyte[4 * this->pixel_width * this->pixel_height];

  for (std::size_t i = 0; i < this->pixel_width * this->pixel_height; ++i)
    this->put_pixel(&tex_data[i * 4], BG_COLOR);

  for (std::size_t i = 0; i < text.size(); ++i)
  {
    switch (text[i])
    {
      case '0': this->append_char(tex_data, i, CONSOLE_NUMBER_0); break;
      case '1': this->append_char(tex_data, i, CONSOLE_NUMBER_1); break;
      case '2': this->append_char(tex_data, i, CONSOLE_NUMBER_2); break;
      case '3': this->append_char(tex_data, i, CONSOLE_NUMBER_3); break;
      case '4': this->append_char(tex_data, i, CONSOLE_NUMBER_4); break;
      case '5': this->append_char(tex_data, i, CONSOLE_NUMBER_5); break;
      case '6': this->append_char(tex_data, i, CONSOLE_NUMBER_6); break;
      case '7': this->append_char(tex_data, i, CONSOLE_NUMBER_7); break;
      case '8': this->append_char(tex_data, i, CONSOLE_NUMBER_8); break;
      case '9': this->append_char(tex_data, i, CONSOLE_NUMBER_9); break;
      default: this->append_char(tex_data, i, CONSOLE_INVALID_CHAR); break;
    }
  }

  //this->texture_data_debug(tex_data);

  glGenTextures(1, &this->tex_id);
  glBindTexture(GL_TEXTURE_2D, this->tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)this->pixel_width,
      (GLsizei)this->pixel_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  delete [] tex_data;
}

/* ---------------------------------------------------------------- */

void
FontTexture::append_char (GLubyte* data, std::size_t off, char const* copy)
{
  std::size_t col_off = off * (CONSOLE_FONT_WIDTH + 1) + 1;
  for (std::size_t i = 0; i < CONSOLE_FONT_HEIGHT * CONSOLE_FONT_WIDTH; ++i)
  {
    std::size_t row = i / CONSOLE_FONT_WIDTH + 1;
    std::size_t col = col_off + (i % CONSOLE_FONT_WIDTH);
    std::size_t pos = 4 * (row * this->pixel_width + col);

    switch (copy[i])
    {
      case ' ': break;
      case '#': this->put_pixel(&data[pos], FG_COLOR); break;
      default:
        throw Remesher::Exception("Invalid character in font");
    }
  }
}

/* ---------------------------------------------------------------- */

void
FontTexture::put_pixel (GLubyte* data, GLubyte r, GLubyte g,
    GLubyte b, GLubyte a)
{
  data[0] = r;
  data[1] = g;
  data[2] = b;
  data[3] = a;
}

/* ---------------------------------------------------------------- */

void
FontTexture::texture_data_debug (GLubyte* data) const
{
  std::size_t total_pixels = this->pixel_width * this->pixel_height;
  for (std::size_t i = 0; i < total_pixels * 4; i += 4)
  {
    if (i != 0 && i % (4 * this->pixel_width) == 0)
      std::cout << std::endl;

    std::cout << (data[i] == 0 ? " " : "X");
  }
  std::cout << std::endl;
}
