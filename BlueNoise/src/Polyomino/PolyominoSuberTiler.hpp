/*
  PolyominoSuberTiler.hpp

  tiler based on subdivision

  the name "Suber" is a hybrid between subdivision and super
  since the behavior of suber-tiler is kind of reverse (scale) of super-tiler

  I know suber is not an English word, though
  
  Li-Yi Wei
  10/27/2007

*/

#ifndef _POLYOMINO_SUBER_TILER_HPP
#define _POLYOMINO_SUBER_TILER_HPP

#include "PolyominoTiler.hpp"

class PolyominoSuberTiler : public PolyominoTiler
{
public:
    PolyominoSuberTiler(const PolyominoTiling & seed_tiling, const int num_subdivisions);
    ~PolyominoSuberTiler(void);
    
    PolyominoTiling * Next(void);
    
protected:
    const PolyominoTiling & _seed_tiling;
    const int _num_subdivisions;
};

#endif
