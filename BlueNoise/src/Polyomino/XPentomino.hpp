/*
  XPentomino.hpp

  Li-Yi Wei

  11/10/2007

*/

#ifndef _X_PENTOMINO_HPP
#define _X_PENTOMINO_HPP

#include "Pentomino.hpp"

class XPentomino : public Pentomino
{
public:
    // X-oriented tile (canonical tile) has orientation ON and reflection R1
    XPentomino(void);
    XPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    XPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~XPentomino(void);

    XPentomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
