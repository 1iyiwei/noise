/*
  PPentominoSuperTiler.hpp

  use multiple smaller P-pentomino tilings to assemble a good one
  as described in "Sampling with Polyominos" SIGGRAPH 2007

  Li-Yi Wei
  10/26/2007

*/

#ifndef _P_PENTOMINO_SUPER_TILER_HPP
#define _P_PENTOMINO_SUPER_TILER_HPP

#include "PPentominoTiler.hpp"

class PPentominoSuperTiler : public PolyominoTiler
{
public:
    PPentominoSuperTiler(const int height, const int width, const int template_height, const int template_width, const int num_templates);
    ~PPentominoSuperTiler(void);
    
    PolyominoTiling * Next(void);
    
protected:
    const int _grid_height;
    const int _grid_width;
    const int _template_height;
    const int _template_width;

    vector<const PolyominoTiling *> _templates;
};

#endif
