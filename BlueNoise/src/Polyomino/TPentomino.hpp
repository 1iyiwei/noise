/*
  TPentomino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _T_PENTOMINO_HPP
#define _T_PENTOMINO_HPP

#include "Pentomino.hpp"

class TPentomino : public Pentomino
{
public:
    // T-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 3-1-1 (center aligned)
    TPentomino(void);
    TPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    TPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~TPentomino(void);

    TPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
