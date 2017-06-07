/*
  TetrominoTiler.hpp

  Li-Yi Wei
  11/11/2007

*/

#ifndef _TETROMINO_TILER_HPP
#define _TETROMINO_TILER_HPP

#include "Tetromino.hpp"
#include "PolyominoTiler.hpp"
#include "dlx/dlx.hpp"

class TetrominoTiler : public PolyominoTiler
{
public:
    TetrominoTiler(const int height, const int width);
    ~TetrominoTiler(void);
    
    PolyominoTiling * Next(void);
    
protected:
    const int _grid_height;
    const int _grid_width;
    
    vector<const Tetromino *> _rows;
    
    dlx * _dlx_solver;
    int * _solution_rows;
};

#endif
