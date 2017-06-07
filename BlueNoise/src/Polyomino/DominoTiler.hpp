/*
  DominoTiler.hpp

  Li-Yi Wei
  11/11/2007

*/

#ifndef _DOMINO_TILER_HPP
#define _DOMINO_TILER_HPP

#include "Domino.hpp"
#include "PolyominoTiler.hpp"
#include "dlx/dlx.hpp"

class DominoTiler : public PolyominoTiler
{
public:
    DominoTiler(const int height, const int width);
    ~DominoTiler(void);
    
    PolyominoTiling * Next(void);
    
protected:
    const int _grid_height;
    const int _grid_width;
    
    vector<Domino> _rows;
    
    dlx * _dlx_solver;
    int * _solution_rows;
};

#endif
