/*
  ITetromino.hpp

  Li-Yi Wei

  11/11/2007

*/

#ifndef _I_TETROMINO_HPP
#define _I_TETROMINO_HPP

#include "Tetromino.hpp"

class ITetromino : public Tetromino
{
public:
    // I-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 1-1-1-1
    ITetromino(void);
    ITetromino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    ITetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~ITetromino(void);

    ITetromino * Clone(void) const;

    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
