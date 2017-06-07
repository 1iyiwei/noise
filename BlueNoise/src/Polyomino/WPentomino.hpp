/*
  WPentomino.hpp

  Li-Yi Wei

  11/10/2007

*/

#ifndef _W_PENTOMINO_HPP
#define _W_PENTOMINO_HPP

#include "Pentomino.hpp"

class WPentomino : public Pentomino
{
public:
    // W-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1(left)-2-2(right)
    WPentomino(void);
    WPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    WPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~WPentomino(void);

    WPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
