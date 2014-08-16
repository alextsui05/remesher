#ifndef GLSTUFF_FONT_TEXTURE_HEADER
#define GLSTUFF_FONT_TEXTURE_HEADER

#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <string>
#include "libremesh/refptr.h"

class FontTexture;
typedef RefPtr<FontTexture> FontTexturePtr;

class FontTexture
{
  private:
    GLuint tex_id;
    std::size_t pixel_width;
    std::size_t pixel_height;

  protected:
    FontTexture (void);

    void append_char (GLubyte* data, std::size_t off, char const* copy);
    void put_pixel (GLubyte* data, GLubyte r, GLubyte g, GLubyte b, GLubyte a);
    void texture_data_debug (GLubyte* data) const;

    void set_text (std::string const& text);

  public:
    static FontTexturePtr create (std::string const& text);
    ~FontTexture (void);

    void bind (void) const;
    void clear (void);

    std::size_t get_width (void) const;
    std::size_t get_height (void) const;
};

/* ---------------------------------------------------------------- */

inline FontTexturePtr
FontTexture::create (std::string const& text)
{
  FontTexturePtr ret(new FontTexture);
  ret->set_text(text);
  return ret;
}

inline std::size_t
FontTexture::get_width (void) const
{
  return this->pixel_width;
}

inline std::size_t
FontTexture::get_height (void) const
{
  return this->pixel_height;
}

#endif /* GLSTUFF_FONT_TEXTURE_HEADER */
