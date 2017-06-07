/*
  VPentomino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _V_PENTOMINO_HPP
#define _V_PENTOMINO_HPP

#include "Pentomino.hpp"

class VPentomino : public Pentomino
{
public:
    // V-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1-1-3 (left-aligned)
    VPentomino(void);
    VPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    VPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~VPentomino(void);

    VPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
