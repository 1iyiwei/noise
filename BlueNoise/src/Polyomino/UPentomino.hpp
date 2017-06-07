/*
  UPentomino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _U_PENTOMINO_HPP
#define _U_PENTOMINO_HPP

#include "Pentomino.hpp"

class UPentomino : public Pentomino
{
public:
    // U-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 2 (empty middle)-3
    UPentomino(void);
    UPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    UPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~UPentomino(void);

    UPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
