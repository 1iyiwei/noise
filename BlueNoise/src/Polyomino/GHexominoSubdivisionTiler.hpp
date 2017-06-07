/*
  GHexominoSubdivisionTiler.hpp

  a tiler to help figure out the production/subdivision rule
  of converting one G-hexomino into 81 children
  
  Li-Yi Wei
  11/12/2007

*/

#ifndef _G_HEXOMINO_SUBDIVISION_TILER_HPP
#define _G_HEXOMINO_SUBDIVISION_TILER_HPP

#include "GHexomino.hpp"
#include "PolyominoTiler.hpp"
#include "dlx/dlx.hpp"

class GHexominoSubdivisionTiler : public PolyominoTiler
{
public:
    GHexominoSubdivisionTiler(void);
    ~GHexominoSubdivisionTiler(void);
    
    PolyominoTiling * Next(void);

protected:
    int Serialize(const int row, const int col) const;
    
protected:
    const int _grid_height;
    const int _grid_width;
    
    vector<GHexomino> _rows;
    
    dlx * _dlx_solver;
    int * _solution_rows;
};

#endif
