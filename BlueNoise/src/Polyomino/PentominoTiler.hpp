/*
  PentominoTiler.hpp

  Li-Yi Wei
  11/11/2007

*/

#ifndef _PENTOMINO_TILER_HPP
#define _PENTOMINO_TILER_HPP

#include "Pentomino.hpp"
#include "PolyominoTiler.hpp"
#include "dlx/dlx.hpp"

class PentominoTiler : public PolyominoTiler
{
public:
    PentominoTiler(const int height, const int width);
    ~PentominoTiler(void);
    
    PolyominoTiling * Next(void);
    
protected:
    const int _grid_height;
    const int _grid_width;
    
    vector<const Pentomino *> _rows;
    
    dlx * _dlx_solver;
    int * _solution_rows;
};

#endif
