/*
  IPentomino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _I_PENTOMINO_HPP
#define _I_PENTOMINO_HPP

#include "Pentomino.hpp"

class IPentomino : public Pentomino
{
public:
    // I-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1-1-1-1-1 
    IPentomino(void);
    IPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    IPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~IPentomino(void);

    IPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
