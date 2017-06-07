/*
  MonominoTiler.hpp

  Li-Yi Wei
  10/21/2007

*/

#ifndef _MONOMINO_TILER_HPP
#define _MONOMINO_TILER_HPP

#include "Monomino.hpp"
#include "PolyominoTiler.hpp"

class MonominoTiler : public PolyominoTiler
{
public:
    MonominoTiler(const int height, const int width);
    ~MonominoTiler(void);
    
    PolyominoTiling * Next(void);
    
protected:
    vector<PolyominoTiling *> _solutions;
};

#endif
