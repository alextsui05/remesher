#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER

#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <string>

class Texture
{
  public:
    GLuint glid;

  public:
    Texture (void);

    void load (std::string const& filename);
    void test_texture (void);
};

#endif /* TEXTURE_HEADER */
