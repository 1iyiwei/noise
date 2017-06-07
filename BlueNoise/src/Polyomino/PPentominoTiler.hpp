/*
  PPentominoTiler.hpp

  Li-Yi Wei
  10/25/2007

*/

#ifndef _P_PENTOMINO_TILER_HPP
#define _P_PENTOMINO_TILER_HPP

#include "PPentomino.hpp"
#include "PolyominoTiler.hpp"
#include "dlx/dlx.hpp"

class PPentominoTiler : public PolyominoTiler
{
public:
    PPentominoTiler(const int height, const int width);
    ~PPentominoTiler(void);
    
    PolyominoTiling * Next(void);
    
protected:
    const int _grid_height;
    const int _grid_width;
    
    vector<PPentomino> _rows;
    
    dlx * _dlx_solver;
    int * _solution_rows;
};

#endif
