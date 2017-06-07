/*
  GHexomino.hpp

  Li-Yi Wei

  10/27/2007

*/

#ifndef _G_HEXOMINO_HPP
#define _G_HEXOMINO_HPP

#include "Polyomino.hpp"

class GHexomino : public Polyomino
{
public:
    // G-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonocal tile: 1-3-2 all aligned on the left
    GHexomino(void);
    GHexomino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    GHexomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~GHexomino(void);

    GHexomino * Clone(void) const;

    int NumSubdivisionRules(void) const;
    int Subdivide(vector<Polyomino *> & children, const int which_rule) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
