/*
  ITromino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _I_TROMINO_HPP
#define _I_TROMINO_HPP

#include "Tromino.hpp"

class ITromino : public Tromino
{
public:
    // I-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1-1-1 
    ITromino(void);
    ITromino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    ITromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~ITromino(void);

    ITromino * Clone(void) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
