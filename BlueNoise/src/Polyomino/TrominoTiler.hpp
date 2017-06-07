/*
  TrominoTiler.hpp

  Li-Yi Wei
  11/11/2007

*/

#ifndef _TROMINO_TILER_HPP
#define _TROMINO_TILER_HPP

#include "Tromino.hpp"
#include "PolyominoTiler.hpp"
#include "dlx/dlx.hpp"

class TrominoTiler : public PolyominoTiler
{
public:
    TrominoTiler(const int height, const int width);
    ~TrominoTiler(void);
    
    PolyominoTiling * Next(void);
    
protected:
    const int _grid_height;
    const int _grid_width;
    
    vector<const Tromino *> _rows;
    
    dlx * _dlx_solver;
    int * _solution_rows;
};

#endif
