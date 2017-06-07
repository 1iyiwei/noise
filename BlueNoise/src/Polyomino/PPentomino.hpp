/*
  PPentomino.hpp

  Li-Yi Wei

  10/25/2007

*/

#ifndef _P_PENTOMINO_HPP
#define _P_PENTOMINO_HPP

#include "Pentomino.hpp"

class PPentomino : public Pentomino
{
public:
    // P-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 2-2-1 (with the bottom 1 on left)
    PPentomino(void);
    PPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    PPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~PPentomino(void);

    PPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
