/*
  LTromino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _L_TROMINO_HPP
#define _L_TROMINO_HPP

#include "Tromino.hpp"

class LTromino : public Tromino
{
public:
    // L-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1-2 (left-aligned)
    LTromino(void);
    LTromino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    LTromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~LTromino(void);

    LTromino * Clone(void) const;

    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
