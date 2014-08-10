#ifndef REMESHER_FIDELITY_FLIPS_HEADER
#define REMESHER_FIDELITY_FLIPS_HEADER

#include "defines.h"
#include "edgeflips.h"
#include "vertexref.h"

REMESHER_NAMESPACE_BEGIN

class FidelityFlips : public EdgeFlipsBase
{
  private:
    TriangleMeshPtr rmesh;
    VertexInfoListPtr rvinfo;

    VertexRefListPtr vref;

  private:
    bool check_flip_edge (std::size_t v1idx, std::size_t v2idx,
        std::size_t v0idx, std::size_t v3idx,
        std::size_t face1, std::size_t face2);

  public:
    FidelityFlips (void);

    void set_reference_mesh (TriangleMeshPtr rmesh);
    void set_reference_vinfo (VertexInfoListPtr rvinfo);
    void set_evolving_refs (VertexRefListPtr vref);
};

/* ---------------------------------------------------------------- */

inline
FidelityFlips::FidelityFlips (void)
{
  this->set_max_iterations(5);
}

inline void
FidelityFlips::set_reference_mesh (TriangleMeshPtr rmesh)
{
  this->rmesh = rmesh;
}

inline void
FidelityFlips::set_reference_vinfo (VertexInfoListPtr rvinfo)
{
  this->rvinfo = rvinfo;
}

inline void
FidelityFlips::set_evolving_refs (VertexRefListPtr vref)
{
  this->vref = vref;
}

REMESHER_NAMESPACE_END

#endif /* REMESHER_FIDELITY_FLIPS_HEADER */
