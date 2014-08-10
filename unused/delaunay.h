#ifndef DELAUNAY_HEADER
#define DELAUNAY_HEADER

#include <vector>
#include "vec2.h"
#include "refptr.h"
#include "defines.h"

REMESHER_NAMESPACE_BEGIN

struct DPoint
{
  Vec2f coord;
  bool is_border;
  std::vector<unsigned int> tris;

  DPoint (Vec2f const& coord);
  void remove_tri (unsigned int tri_idx);
  void replace_tri (unsigned int tri_idx, unsigned int newtri);
  void replace_tri (unsigned int tri_idx,
      unsigned int newtri1, unsigned int newtri2);
};

/* ---------------------------------------------------------------- */

struct DPoly
{
  std::vector<unsigned int> points;
};

/* ---------------------------------------------------------------- */

struct DTri
{
  bool flagged;
  unsigned int points[3];

  DTri (unsigned int a, unsigned int b, unsigned int c);
};

/* ---------------------------------------------------------------- */

class Delaunay;
typedef RefPtr<Delaunay> DelaunayPtr;

class Delaunay
{
  private:
    std::vector<DPoint> vertices;
    std::vector<DTri> mesh;
    DPoly border;

  protected:
    Delaunay (void);

    /* Calculates the edge equation value of a vertex v
     * for a line passing through p with gardient m[1] / m[0].
     * Returns > 0 if v is right of, ceq < 0 left of and = 0 on the line.*/
    float ceq (Vec2f const& p, Vec2f const& m, Vec2f const& v);

    /* Inserts a new border vertex relying on first and second. */
    void insert_border_vertex (unsigned int newidx, int first, int second);
    /* Inserts a new interior vertex searching and subdividing a triangle. */
    void insert_interior_vertex (unsigned int newidx);

    /* Tests if a change, given by a point and a triangle, violates
     * the delaunay condition. A valid delaunay trianglulation is
     * restored then. Basically, the outer edge opposite to the given
     * point is tested and the triangle must have this point.
     */
    void restore_delaunay (unsigned int pnt_idx, unsigned int tri_idx);

    /* Checks if a given point is inside the circumcenter
     * of a given triangle.
     */
    bool point_in_circumcenter (unsigned int pnt_idx, unsigned int tri_idx);

    /* Removes the specified face and replaces it with three new faces. */
    void refine_face (unsigned int tri_idx, unsigned int pnt_idx);
    /* There is a special case if a vertex falls on an edge. */
    void refine_edge (unsigned int tri_idx, unsigned int edge_idx,
        unsigned int pnt_idx);

    /* Returns the index of the adjacent triangle for vertices
     * v1 and v2 but not triangle tri_idx. Returns -1 on failure. */
    int find_adjacent_tri (unsigned int v1, unsigned int v2,
        unsigned int tri_idx);

  public:
    static DelaunayPtr create (void);

    void insert (Vec2f const& coord);
    void clear (void);

    std::vector<DPoint> const& get_vertices (void) const;
    std::vector<DPoint>& get_vertices (void);
    std::vector<DTri> const& get_mesh (void) const;
    DPoly const& get_border (void) const;

    /* Checks consistency of the triangulation and reports on STDOUT. */
    void consistency_check (void);

    /* Returns the circumcenter of a triangle. */
    Vec2f calc_circumcenter (unsigned int tri_idx);

    /* This function returns true if the new vertex is outside
     * of any existing triangle, i.e. outside of the border.
     * An interval is then returned in the first and second arguments.
     * These values correspond to the border indices. */
    bool border_interval (Vec2f const& v, int& first, int& second);
};

/* ---------------------------------------------------------------- */

inline
DPoint::DPoint (Vec2f const& coord)
{
  this->coord = coord;
  this->is_border = false;
}

inline void
DPoint::remove_tri (unsigned int tri_idx)
{
  for (unsigned int i = 0; i < this->tris.size(); ++i)
    if (this->tris[i] == tri_idx)
    {
      this->tris.erase(this->tris.begin() + i);
      return;
    }
}

inline void
DPoint::replace_tri (unsigned int tri_idx, unsigned int newtri)
{
  for (unsigned int i = 0; i < this->tris.size(); ++i)
    if (this->tris[i] == tri_idx)
    {
      this->tris.insert(this->tris.begin() + i, newtri);
      this->tris.erase(this->tris.begin() + i + 1);
      return;
    }
}

inline void
DPoint::replace_tri (unsigned int tri_idx,
    unsigned int newtri1, unsigned int newtri2)
{
  for (unsigned int i = 0; i < this->tris.size(); ++i)
    if (this->tris[i] == tri_idx)
    {
      this->tris.insert(this->tris.begin() + i, newtri2);
      this->tris.insert(this->tris.begin() + i, newtri1);
      this->tris.erase(this->tris.begin() + i + 2);
      return;
    }
}

inline
DTri::DTri (unsigned int a, unsigned int b, unsigned int c)
{
  this->flagged = false;
  this->points[0] = a;
  this->points[1] = b;
  this->points[2] = c;
}

inline
Delaunay::Delaunay (void)
{
}

inline DelaunayPtr
Delaunay::create (void)
{
  return DelaunayPtr(new Delaunay);
}

inline void
Delaunay::clear (void)
{
  this->border.points.clear();
  this->mesh.clear();
  this->vertices.clear();
}

inline float
Delaunay::ceq (Vec2f const& p, Vec2f const& m, Vec2f const& v)
{
  return (v[0] - p[0]) * m[1] - (v[1] - p[1]) * m[0];
}

inline std::vector<DPoint> const&
Delaunay::get_vertices (void) const
{
  return this->vertices;
}

inline std::vector<DPoint>&
Delaunay::get_vertices (void)
{
  return this->vertices;
}

inline std::vector<DTri> const&
Delaunay::get_mesh (void) const
{
  return this->mesh;
}

inline DPoly const&
Delaunay::get_border (void) const
{
  return this->border;
}

REMESHER_NAMESPACE_END

#endif /* DELAUNAY_HEADER */
