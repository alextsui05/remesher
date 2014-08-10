#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER

#include <GL/gl.h>
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
