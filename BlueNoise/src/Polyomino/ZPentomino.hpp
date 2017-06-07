/*
  ZPentomino.hpp

  Li-Yi Wei

  11/10/2007

*/

#ifndef _Z_PENTOMINO_HPP
#define _Z_PENTOMINO_HPP

#include "Pentomino.hpp"

class ZPentomino : public Pentomino
{
public:
    // Z-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 2(left)-1-2(right)
    ZPentomino(void);
    ZPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    ZPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~ZPentomino(void);

    ZPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
