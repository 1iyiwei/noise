/*
  PolyominoSuberTiler.cpp

  Li-Yi Wei
  10/27/2007

*/

#include "PolyominoSuberTiler.hpp"
#include "Random.hpp"
#include "Exception.hpp"

#include <math.h>

PolyominoSuberTiler::PolyominoSuberTiler(const PolyominoTiling & seed_tiling, const int num_subdivisions) : _seed_tiling(seed_tiling), _num_subdivisions(num_subdivisions)
{
    // nothing to do
}

PolyominoSuberTiler::~PolyominoSuberTiler(void)
{
    // nothing to do
}

PolyominoTiling * PolyominoSuberTiler::Next(void)
{
    // clone all the tiles to start with
    vector<const Polyomino *> tiles;

    for(int i = 0; i < _seed_tiling.NumTiles(); i++)
    {
        tiles.push_back(_seed_tiling.GetTile(i)->Clone());
    }

    // get all children tiles
    int num_children = 0;
    for(int i = 0; i < _num_subdivisions; i++)
    {
        vector<const Polyomino *> children;

        for(unsigned int j = 0; j < tiles.size(); j++)
        {
            const int num_rules = tiles[j]->NumSubdivisionRules();

            if(num_rules <= 0)
            {
                throw Exception("PolyominoSuberTiler::Next(): no subdivision rule");
            }

            int which_rule = static_cast<int>(floor(Random::UniformRandom()*num_rules));

            if(which_rule < 0) which_rule = 0;
            if(which_rule >= num_rules) which_rule = num_rules-1;
            
            vector<Polyomino *> my_children;

            if(! tiles[j]->Subdivide(my_children, which_rule))
            {
                throw Exception("PolyominoSuberTiler::Next(): cannot subdivide");
            }

            if(num_children <= 0)
            {
                num_children = my_children.size();
            }
            else
            {
                if(num_children != my_children.size())
                {
                    throw Exception("PolyominoSuberTiler::Next(): varying children size");
                }
            }

            for(unsigned int k = 0; k < my_children.size(); k++)
            {
                children.push_back(my_children[k]);
            }
        }

        // children succeed parent
        for(unsigned int j = 0; j < tiles.size(); j++)
        {
            delete tiles[j];
        }

        tiles = children;
    }

    // put tiles into tiling
    const float spawn_factor = pow(num_children, _num_subdivisions);
    const int scaling = floor(sqrt(spawn_factor)+0.5);
    if(scaling*scaling != static_cast<int>(floor(spawn_factor)))
    {
        throw Exception("PolyominoSuberTiler::Next(): non-square tiling");
    }

    const int height = _seed_tiling.Size().row * scaling;
    const int width = _seed_tiling.Size().col * scaling;

    PolyominoTiling * tiling = new PolyominoTiling(height, width);

    if(! tiling->SetTiles(tiles))
    {
        tiling->SetMessage("cannot set tiles");
    }

    BuildNeighborList(*tiling);

    // done
    return tiling;
}
