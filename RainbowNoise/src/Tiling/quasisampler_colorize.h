/*
  The colorization code for Penrose tiling

  see http://facstaff.unca.edu/mcmcclur/mathematicaGraphics/PenroseColoring/

  Li-Yi Wei
  10/06/2008

 */

#ifndef QUASISAMPLER_COLORIZE_H
#define QUASISAMPLER_COLORIZE_H

#include <math.h>

#include "Random.hpp"
#include "Exception.hpp"

int Quasisampler::colorize(const unsigned num_colors, const double flip_probability, const int num_iterations)
{
    // random initialization    
    {
        TileLeafIterator it(root);
    
        unsigned color = 0;

        do 
        {
            color = static_cast<unsigned>(floor(Random::UniformRandom()*num_colors))%num_colors;

            it->setColor(color);
        } 
        while(it.next());
    }

    // iterate
    bool changed = 1;
    bool consistent_coloring = 0;

    for(int iteration = 0; (num_iterations < 0) || (iteration < num_iterations); iteration++)
    {
        changed = 0;
        consistent_coloring = 1;

        TileLeafIterator it(root);

        do 
        {
            TileNode * myself = *it;

            bool conflict_with_a_senior_neighbor = 0;
            bool conflict_with_a_junior_neighbor = 0;

            const double my_d2 = myself->getPRhomb().d2();

            for(unsigned int i = 0; i < it->numNeighbors(); i++)
            {
                const TileNode * neighbor = myself->getNeighbor(i);

                if(neighbor->getColor() == myself->getColor())
                {
                    consistent_coloring = 0;

                    const double neighbor_d2 = neighbor->getPRhomb().d2();

                    if((neighbor_d2 < my_d2) || ((neighbor_d2 == my_d2) && (neighbor->getFCode() < myself->getFCode())))
                    {
                        conflict_with_a_senior_neighbor = 1;
                    }
                    else
                    {
                        conflict_with_a_junior_neighbor = 1;
                    }
                }
            }

            bool recolor = 0;
            if(conflict_with_a_senior_neighbor || conflict_with_a_junior_neighbor)
            {
                const float choice = Random::UniformRandom();
                recolor = (conflict_with_a_senior_neighbor && (choice < flip_probability)) || (conflict_with_a_junior_neighbor && (choice < (1.0 - flip_probability)));
            }

            if(recolor)
            {
                unsigned color = myself->getColor();

                while(color == myself->getColor())
                {
                    color = static_cast<unsigned>(floor(Random::UniformRandom()*num_colors))%num_colors;
                }

                myself->setColor(color);

                changed = 1;
            }
        } 
        while(it.next());
        
        if(consistent_coloring)
        {
            return iteration;
        }
    }

    return -num_iterations;
}

#endif
