/*
  PPentominoSuperTiler.cpp

  Li-Yi Wei
  10/26/2007

*/

#include "PPentominoSuperTiler.hpp"
#include "Random.hpp"
#include "Exception.hpp"

#include <math.h>

PPentominoSuperTiler::PPentominoSuperTiler(const int height, const int width, const int template_height, const int template_width, const int num_templates) : _grid_height(height), _grid_width(width), _template_height(template_height), _template_width(template_width)
{
    if((height%template_height) || (width%template_width))
    {
        throw Exception("PPentominoSuperTiler::PPentominoSuperTiler(): height/width must be integer multiple of template_height/template_width");
    }

    PPentominoTiler tiler(template_height, template_width);

    while(_templates.size() < num_templates)
    {
        const PolyominoTiling * tiling = tiler.Next();

        if(tiling)
        {
            _templates.push_back(tiling);
        }
        else
        {
            break;
        }
    }

    if(_templates.size() <= 0)
    {
        throw Exception("PPentominoSuperTiler::PPentominoSuperTiler(): templates.size() <= 0");
    }
}

PPentominoSuperTiler::~PPentominoSuperTiler(void)
{
    for(unsigned int i = 0; i < _templates.size(); i++)
    {
        delete _templates[i];
    }
}

PolyominoTiling * PPentominoSuperTiler::Next(void)
{
    PolyominoTiling * tiling = new PolyominoTiling(_grid_height, _grid_width);

    const int num_template_rows = _grid_height/_template_height;
    const int num_template_cols = _grid_width/_template_width;

    vector<const Polyomino *> tiles; // to fill in below
    for(int template_row = 0; template_row < num_template_rows; template_row++)
        for(int template_col = 0; template_col < num_template_cols; template_col++)
        {
            int selection = static_cast<int>(floor(Random::UniformRandom()*_templates.size()));

            if(selection < 0) selection = 0;
            if(selection >= _templates.size()) selection = _templates.size()-1;

            const PolyominoTiling * tiling_template = _templates[selection];

            for(int i = 0; i < tiling_template->NumTiles(); i++)
            {
                Polyomino * baby = tiling_template->GetTile(i)->Clone();

                Polyomino::Int2 center;
                tiling_template->GetTile(i)->GetCenter(center);

                baby->SetCenter(Polyomino::Int2(center.row + template_row*_template_height, center.col + template_col*_template_width));
                baby->ClearNeighborList();

                tiles.push_back(baby);
            }
        }

    if(! tiling->SetTiles(tiles))
    {
        tiling->SetMessage("cannot set tiles");
    }

    // build neighbors list
    BuildNeighborList(*tiling);
    
    return tiling;
}
    
