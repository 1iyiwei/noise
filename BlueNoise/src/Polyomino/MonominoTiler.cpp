/*
  MonominoTiler.cpp

  Li-Yi Wei
  10/21/2007

*/

#include "MonominoTiler.hpp"

MonominoTiler::MonominoTiler(const int height, const int width)
{
    PolyominoTiling * tiling = new PolyominoTiling(height, width);

    vector<const Polyomino *> tiles(height*width);

    for(unsigned int i = 0; i < tiles.size(); i++)
    {
        const Monomino::Int2 center(i/width, i%width);
        
        tiles[i] = new Monomino(center);
    }

    if(! tiling->SetTiles(tiles))
    {
        tiling->SetMessage("cannot set tiles");
    }

    // build neighbors list
    for(int i = 0; i < tiling->NumTiles(); i++)
    {
        Polyomino * tile = tiling->GetTile(i);

        if(tile == 0)
        {
            tiling->SetMessage("found null tile in MonominoTiler::Build()");
        }

        Polyomino::Int2 center; tile->GetCenter(center);

        tile->ClearNeighborList();
        
        for(int j = -1; j <= 1; j++)
            for(int k = -1; k <= 1; k++)
            {
                const int neighbor_row = ((center.row+j)%height+height)%height;
                const int neighbor_col = ((center.col+k)%width+width)%width;
                
                const Polyomino * neighbor = tiling->GetGridCell(neighbor_row, neighbor_col);

                if(neighbor && (neighbor != tile))
                {
                    // add
                    tile->AddNeighbor(Polyomino::Neighbor(neighbor, Polyomino::Int2(j, k)));
                }
            }
    }
    
    // done
    _solutions.push_back(tiling);
}

MonominoTiler::~MonominoTiler(void)
{
    for(unsigned int i = 0; i < _solutions.size(); i++)
    {
        delete _solutions[i];
    }
}
    
PolyominoTiling * MonominoTiler::Next(void)
{
    PolyominoTiling * result = 0;

    if(_solutions.size() > 0)
    {
        result = _solutions[_solutions.size()-1];
        _solutions.pop_back();
    }

    return result;
}
