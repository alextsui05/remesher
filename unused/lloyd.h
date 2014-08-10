#ifndef LLOYD_HEADER
#define LLOYD_HEADER

#include "delaunay.h"
#include "defines.h"

REMESHER_NAMESPACE_BEGIN

class Lloyd
{
  private:
    DelaunayPtr delaunay;

  public:
    Lloyd (DelaunayPtr delaunay);

    DelaunayPtr iteration (void);
};

REMESHER_NAMESPACE_END

#endif /* LLOYD_HEADER */
