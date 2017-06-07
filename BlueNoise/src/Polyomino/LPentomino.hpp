/*
  LPentomino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _L_PENTOMINO_HPP
#define _L_PENTOMINO_HPP

#include "Pentomino.hpp"

class LPentomino : public Pentomino
{
public:
    // L-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1-1-1-2 (left-aligned)
    LPentomino(void);
    LPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    LPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~LPentomino(void);

    LPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
