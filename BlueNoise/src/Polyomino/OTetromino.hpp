/*
  OTetromino.hpp

  Li-Yi Wei

  11/10/2007

*/

#ifndef _O_TETROMINO_HPP
#define _O_TETROMINO_HPP

#include "Tetromino.hpp"

class OTetromino : public Tetromino
{
public:
    // X-oriented tile (canonical tile) has orientation ON and reflection R1
    OTetromino(void);
    OTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection);
    OTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset);
    ~OTetromino(void);

    OTetromino * Clone(void) const;

    void GetCells(vector<Int2> & cells) const;
    
protected:
};

#endif
