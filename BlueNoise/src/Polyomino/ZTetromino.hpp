/*
  ZTetromino.hpp

  Li-Yi Wei

  11/10/2007

*/

#ifndef _Z_TETROMINO_HPP
#define _Z_TETROMINO_HPP

#include "Tetromino.hpp"

class ZTetromino : public Tetromino
{
public:
    // Z-oriented tile (canonical tile) has orientation ON and reflection R1
    // canonical tile: 2(left)-2(right)
    ZTetromino(void);
    ZTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    ZTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~ZTetromino(void);

    ZTetromino * Clone(void) const;

    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
