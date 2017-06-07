/*
  YPentomino.hpp

  Li-Yi Wei

  11/10/2007

*/

#ifndef _Y_PENTOMINO_HPP
#define _Y_PENTOMINO_HPP

#include "Pentomino.hpp"

class YPentomino : public Pentomino
{
public:
    // Y-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1-2-1-1 (aligned on the right)
    YPentomino(void);
    YPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    YPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~YPentomino(void);

    YPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
