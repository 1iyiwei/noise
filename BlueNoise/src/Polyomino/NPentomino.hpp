/*
  NPentomino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _N_PENTOMINO_HPP
#define _N_PENTOMINO_HPP

#include "Pentomino.hpp"

class NPentomino : public Pentomino
{
public:
    // N-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1(right)-2-1(left)-1(left)
    NPentomino(void);
    NPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    NPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~NPentomino(void);

    NPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
