/*
  LTetromino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _L_TETROMINO_HPP
#define _L_TETROMINO_HPP

#include "Tetromino.hpp"

class LTetromino : public Tetromino
{
public:
    // L-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1-1-2 (left-aligned)
    LTetromino(void);
    LTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    LTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~LTetromino(void);

    LTetromino * Clone(void) const;
    
    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
