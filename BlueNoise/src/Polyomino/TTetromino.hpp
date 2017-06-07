/*
  TTetromino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _T_TETROMINO_HPP
#define _T_TETROMINO_HPP

#include "Tetromino.hpp"

class TTetromino : public Tetromino
{
public:
    // T-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 3-1 (center aligned)
    TTetromino(void);
    TTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    TTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~TTetromino(void);

    TTetromino * Clone(void) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
