/*
  FPentomino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _F_PENTOMINO_HPP
#define _F_PENTOMINO_HPP

#include "Pentomino.hpp"

class FPentomino : public Pentomino
{
public:
    // F-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 2(right)-2(left)-1
    FPentomino(void);
    FPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    FPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~FPentomino(void);

    FPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
