/*
  PolyominoTiler.hpp

  Li-Yi Wei
  10/25/2007

*/

#ifndef _POLYOMINO_TILER_HPP
#define _POLYOMINO_TILER_HPP

#include "PolyominoTiling.hpp"

class PolyominoTiler
{
public:
    // payee will be responsible for the memory
    virtual PolyominoTiling * Next(void);

// protected:
    static void BuildNeighborList(PolyominoTiling & tiling);
};

#endif
