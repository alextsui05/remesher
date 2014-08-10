#include <iostream>

#include "libremesh/exception.h"
#include "libremesh/helpers.h"
#include "gltriangledebug.h"

GLTriangleDebug::GLTriangleDebug (void)
{
  this->rtype = DEBUG_RENDER_TYPE_NOTHING;
}

/* ---------------------------------------------------------------- */

void
GLTriangleDebug::draw_impl (void)
{
  if (this->mesh.get() == 0)
    return;

  switch (this->rtype)
  {
    case DEBUG_RENDER_TYPE_FACES:
      if (this->mesh->get_faces().size() / 3 != this->tex_list.size())
        this->generate_textures(this->mesh->get_faces().size() / 3);
      break;

    case DEBUG_RENDER_TYPE_VERTICES:
      if (this->mesh->get_vertices().size() != this->tex_list.size())
        this->generate_textures(this->mesh->get_vertices().size());
      break;

    default:
    case DEBUG_RENDER_TYPE_NOTHING:
      return;
  }

  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glColor3f(1.0f, 1.0f, 1.0f);

  switch (this->rtype)
  {
    case DEBUG_RENDER_TYPE_FACES: this->draw_face_textures(); break;
    case DEBUG_RENDER_TYPE_VERTICES: this->draw_vertex_textures(); break;
    default: break;
  }

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

/* ---------------------------------------------------------------- */

void
GLTriangleDebug::generate_textures (std::size_t amount)
{
  if (amount < this->tex_list.size())
  {
    this->tex_list.resize(amount);
  }
  else
  {
    for (std::size_t i = this->tex_list.size(); i < amount; ++i)
      this->tex_list.push_back(FontTexture::create
          (Remesher::Helpers::get_string(i)));
  }
}

/* ---------------------------------------------------------------- */

void
GLTriangleDebug::draw_vertex_textures (void)
{
  Remesher::MeshVertexList& verts = this->mesh->get_vertices();
  Remesher::MeshNormalList& normals = this->mesh->get_vertex_normals();
  if (normals.empty())
    throw Remesher::Exception("Mesh normals required for debugging");

  float size_y = MY_MAX(MY_MIN(0.2f / std::sqrt
      ((float)verts.size()), 0.05f), 0.0001f);

  for (std::size_t i = 0; i < this->tex_list.size(); ++i)
  {
    Remesher::Vec3f const& v = verts[i];
    Remesher::Vec3f vn = normals[i];
    if (vn[0] == 0.0f && vn[1] == 0.0f && vn[2] == 0.0f)
      vn = Remesher::Vec3f(0.0f, 0.0f, 1.0f);

    Remesher::Vec3f other(vn[0] - 1.0f, vn[1] - 2.0f, 0.0f);
    other.norm_self();
    Remesher::Vec3f d1 = vn.cross(other);
    Remesher::Vec3f d2 = vn.cross(d1);

    FontTexturePtr ft = this->tex_list[i];
    float size_x = size_y * (float)ft->get_width() / (float)ft->get_height();

    Remesher::Vec3f p1 = v - d1 * size_x - d2 * size_y + vn * size_y;
    Remesher::Vec3f p2 = v + d1 * size_x - d2 * size_y + vn * size_y;
    Remesher::Vec3f p3 = v + d1 * size_x + d2 * size_y + vn * size_y;
    Remesher::Vec3f p4 = v - d1 * size_x + d2 * size_y + vn * size_y;

    ft->bind();
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3fv(&p1[0]);
    glTexCoord2f(1.0f, 1.0f); glVertex3fv(&p2[0]);
    glTexCoord2f(1.0f, 0.0f); glVertex3fv(&p3[0]);
    glTexCoord2f(0.0f, 0.0f); glVertex3fv(&p4[0]);
    glEnd();
  }
}

/* ---------------------------------------------------------------- */

void
GLTriangleDebug::draw_face_textures (void)
{
  Remesher::MeshVertexList& verts = this->mesh->get_vertices();
  Remesher::MeshFaceList& faces = this->mesh->get_faces();
  Remesher::MeshNormalList& normals = this->mesh->get_face_normals();
  if (normals.empty())
    throw Remesher::Exception("Mesh normals for required for debugging");

  float dist = 0.001f;

  for (std::size_t i = 0; i < this->tex_list.size(); ++i)
  {
    Remesher::Vec3f const& a = verts[faces[i * 3]];
    Remesher::Vec3f const& b = verts[faces[i * 3 + 1]];
    Remesher::Vec3f const& c = verts[faces[i * 3 + 2]];
    float qlen1 = (b - a).qlength();
    float qlen2 = (c - b).qlength();
    float qlen3 = (a - c).qlength();

    float ref_len;
    Remesher::Vec3f d1;

#if 1
    if (qlen1 <= qlen2 && qlen1 <= qlen3)
    {
      ref_len = (b - a).length();
      d1 = (b - a).norm();
    }
    else if (qlen2 <= qlen1 && qlen2 <= qlen3)
    {
      ref_len = (c - b).length();
      d1 = (c - b).norm();
    }
    else
    {
      ref_len = (a - c).length();
      d1 = (a - c).norm();
    }
#else
    if (qlen1 >= qlen2 && qlen1 >= qlen3)
    {
      ref_len = (b - a).length();
      d1 = (b - a).norm();
    }
    else if (qlen2 >= qlen1 && qlen2 >= qlen3)
    {
      ref_len = (c - b).length();
      d1 = (c - b).norm();
    }
    else
    {
      ref_len = (a - c).length();
      d1 = (a - c).norm();
    }
#endif

    Remesher::Vec3f d2 = d1.cross(normals[i]);
    Remesher::Vec3f const& v = (a + b + c) / 3.0f;

    FontTexturePtr ft = this->tex_list[i];
    float size_x = ref_len * 0.1f;
    float size_y = size_x * (float)ft->get_height() / (float)ft->get_width();

    Remesher::Vec3f p1 = v - d1 * size_x - d2 * size_y + normals[i] * dist;
    Remesher::Vec3f p2 = v + d1 * size_x - d2 * size_y + normals[i] * dist;
    Remesher::Vec3f p3 = v + d1 * size_x + d2 * size_y + normals[i] * dist;
    Remesher::Vec3f p4 = v - d1 * size_x + d2 * size_y + normals[i] * dist;

    ft->bind();
    glBegin(GL_QUADS);
    glTexCoord2f(1.0f, 1.0f); glVertex3fv(&p1[0]);
    glTexCoord2f(0.0f, 1.0f); glVertex3fv(&p2[0]);
    glTexCoord2f(0.0f, 0.0f); glVertex3fv(&p3[0]);
    glTexCoord2f(1.0f, 0.0f); glVertex3fv(&p4[0]);
    glEnd();
  }
}
