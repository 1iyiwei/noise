/*
  GHexominoTiler.hpp

  Li-Yi Wei
  10/27/2007

*/

#ifndef _G_HEXOMINO_TILER_HPP
#define _G_HEXOMINO_TILER_HPP

#include "GHexomino.hpp"
#include "PolyominoTiler.hpp"
#include "dlx/dlx.hpp"

class GHexominoTiler : public PolyominoTiler
{
public:
    GHexominoTiler(const int height, const int width);
    ~GHexominoTiler(void);
    
    PolyominoTiling * Next(void);
    
protected:
    const int _grid_height;
    const int _grid_width;
    
    vector<GHexomino> _rows;
    
    dlx * _dlx_solver;
    int * _solution_rows;
};

#endif
