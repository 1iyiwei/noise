/*
  PolyominoRelaxation.cpp

  Li-Yi Wei
  10/21/2007

*/

#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "MonominoTiler.hpp"
#include "DominoTiler.hpp"
#include "TrominoTiler.hpp"
#include "TetrominoTiler.hpp"
#include "PentominoTiler.hpp"

#include "PPentominoTiler.hpp"
#include "PPentominoSuperTiler.hpp"
#include "GHexominoTiler.hpp"
#include "GHexominoSubdivisionTiler.hpp"

#include "PolyominoSuberTiler.hpp"

#include "Random.hpp"
#include "Exception.hpp"

#include "LloydRelaxation.hpp"

int DumbRelaxOnce(PolyominoTiling & tiling)
{
    // get all tiles
    vector<Polyomino *> tiles(tiling.NumTiles());
    
    for(unsigned int i = 0; i < tiles.size(); i++)
    {
        tiles[i] = tiling.GetTile(i);
    }
        
    while(tiles.size() > 0)
    {
        int selection = floor(Random::UniformRandom()*(tiles.size()-1));
        if(selection >= tiles.size()) selection = tiles.size()-1;
        if(selection < 0) selection = 0;
            
        Polyomino * current = tiles[selection];
        tiles[selection] = tiles[tiles.size()-1];
        tiles.pop_back();
            
        Polyomino::Flt2 average(0, 0);
        
        for(int j = 0; j < current->NumNeighbors(); j++)
        {
            const Polyomino::Neighbor neighbor = current->GetNeighbor(j);
            
            if(!neighbor.polyomino)
            {
                cerr << "empty neighbor" << endl;
                return 0;
            }
                
            Polyomino::Flt2 neighbor_offset;
            neighbor.polyomino->GetOffset(neighbor_offset);
            
            average.row += (neighbor.offset.row + neighbor_offset.row);
            average.col += (neighbor.offset.col + neighbor_offset.col);
        }

        if(current->NumNeighbors() > 0)
        {
            average.row = average.row/current->NumNeighbors();
            average.col = average.col/current->NumNeighbors();

#if 1
            Polyomino::Flt2 insider = average;
            
            while(!current->IsInside(insider))
            {
                insider.row /= 2; insider.col /= 2;
            }
#else
            const Polyomino::Flt2 insider = current->PullInOffset(average);
#endif  
            current->SetOffset(insider);
        }
    }

    return 1;
}

class Sample2D
{
public:
    Sample2D(void) : polyomino(0) {};
    Sample2D(const LloydRelaxation::Real x, const LloydRelaxation::Real y) : point(x, y), polyomino(0) {};
    ~Sample2D(void) {};
    
    LloydRelaxation::Point2D point;
    Polyomino * polyomino;
};

int LloydRelaxOnce(PolyominoTiling & tiling)
{
    Polyomino::Int2 tiling_size = tiling.Size();
     
    vector<Sample2D> samples(tiling.NumTiles());
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        samples[i].polyomino = tiling.GetTile(i);
        if(!samples[i].polyomino)
        {
            cerr << "null polyomino" << endl;
            return 0;
        }

        Polyomino::Int2 center; samples[i].polyomino->GetCenter(center);
        Polyomino::Flt2 offset; samples[i].polyomino->GetOffset(offset);
        
        samples[i].point.x = center.row + offset.row;
        samples[i].point.y = center.col + offset.col;
    }

    vector<LloydRelaxation::Point2D *> points(samples.size());
    for(unsigned int i = 0; i < points.size(); i++)
    {
        points[i] = &(samples[i].point);
    }

    const string message = LloydRelaxation::RunOnce(0, tiling_size.row, 0, tiling_size.col, points);
    
    if(message != "")
    {
        cerr << "error in lloyd relaxation " << message << endl;
        return 0;
    }

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        samples[i].polyomino = tiling.GetTile(i);
        if(!samples[i].polyomino)
        {
            cerr << "null polyomino" << endl;
            return 0;
        }

        Polyomino::Int2 center; samples[i].polyomino->GetCenter(center);
        Polyomino::Flt2 offset(samples[i].point.x-center.row, samples[i].point.y-center.col);
        
        while(!(samples[i].polyomino->IsInside(offset)))
        {
            offset.row /= 2; offset.col /= 2;
        }
        
        samples[i].polyomino->SetOffset(offset);
    }
    
    // done
    return 1;
}

int Main(int argc, char **argv)
{
    if(argc < 7)
    {
        cerr << "Usage: " << argv[0] << " tiling_type grid_size template_size num_templates num_subdivisions num_iterations" << endl;
        return 1;
    }

    int argCtr = 0;
    string tiling_type = argv[++argCtr];
    const int grid_size = atoi(argv[++argCtr]);
    const int height = grid_size;
    const int width = grid_size;
    const int template_size = atoi(argv[++argCtr]);
    const int template_height = template_size;
    const int template_width = template_size;
    const int num_templates = atoi(argv[++argCtr]);
    const int num_subdivisions = atoi(argv[++argCtr]);
    const int num_iterations = atoi(argv[++argCtr]);

    Random::InitRandomNumberGenerator();
    
    // construct tiling
    cerr << "construct tiling " << tiling_type << endl;
    PolyominoTiler * tiler = 0;
    PolyominoTiling * seed_tiling = 0;
    PolyominoTiling * tiling = 0;

    if(tiling_type == "monomino")
    {
        tiler = new MonominoTiler(height, width);
    }
    else if(tiling_type == "domino")
    {
        tiler = new DominoTiler(height, width);
    }
    else if(tiling_type == "tromino")
    {
        tiler = new TrominoTiler(height, width);
    }
    else if(tiling_type == "tetromino")
    {
        tiler = new TetrominoTiler(height, width);
    }
    else if(tiling_type == "pentomino")
    {
        tiler = new PentominoTiler(height, width);
    }
    else if(tiling_type == "ppentomino")
    {
        if((height%template_height) || (width%template_width))
        {
            cerr << "height/width must be integer multiple of template_height/template_width" << endl;
            return 1;
        }

        if((height == template_height) && (width == template_width))
        {
            tiler = new PPentominoTiler(height, width);
        }
        else
        {
            tiler = new PPentominoSuperTiler(height, width, template_height, template_width, num_templates);
        }
    }
    else if(tiling_type == "ghexomino_subdivision")
    {
        tiler = new GHexominoSubdivisionTiler();
    }
    else
    {
        tiling = new PolyominoTiling(tiling_type.c_str());
    }

    if(num_subdivisions > 0)
    {
        if(! tiler)
        {
            // cerr << "null tiler so cannot subdivide" << endl;
        }
        else
        {
            seed_tiling = tiler->Next();

            if(! seed_tiling)
            {
                cerr << "cannot build seed tiling" << endl;
                return 1;
            }

            delete tiler;
            tiler = new PolyominoSuberTiler(*seed_tiling, num_subdivisions);
        }
    }
   
    if(tiler && !tiling)
    {
        cerr << "find solution " << endl;
        tiling = tiler->Next();
    }
    else if(tiling && !tiler)
    {
        // do nothing, read in
    }
    else
    {
        cerr << "strange tiler and tiling combination" << endl;
        return 1;
    }

    // error checking
    {
        if(!tiling)
        {
            cerr << "cannot find solution" << endl;
            return 1;
        }
    
        const string message = tiling->GetMessage();

        if(message != "")
        {
            cerr << "Error in tile construction: " << message << endl;
            return 1;
        }
    }
    
    // initialization
    cerr << "random initialization" << endl;
    for(int i = 0; i < tiling->NumTiles(); i++)
    {
        const Polyomino::Flt2 random_offset = tiling->GetTile(i)->RandomOffset();
        tiling->GetTile(i)->SetOffset(num_iterations >= 0? random_offset : Polyomino::Flt2(0, 0));
    }

    // relaxation
    cerr << "relaxation" << endl;
    for(int iteration = 0; iteration < num_iterations; iteration++)
    {
        cerr << ".";
        //if(!DumbRelaxOnce(*tiling)) return 1;
        if(!LloydRelaxOnce(*tiling)) return 1;
    }
    cerr << endl;

    // output stuff
    for(int i = 0; i < tiling->NumTiles(); i++)
    {
        const Polyomino * current = tiling->GetTile(i);

        if(current)
        {
            Polyomino::Int2 center; current->GetCenter(center);
            Polyomino::Flt2 offset; current->GetOffset(offset);

            Polyomino::Flt2 sample((center.row + offset.row)/tiling->Size().row, (center.col + offset.col)/tiling->Size().col);

            if(tiling_type == "ghexomino_subdivision")
            {
                const int final_height = min(tiling->Size().row*2/3, height);
                const int final_width = min(tiling->Size().col*2/3, width);

                sample.row = (center.row - tiling->Size().row/3 + offset.row)/final_height;
                sample.col = (center.col + offset.col)/final_width;
            }
            
            if((sample.row >= 0) && (sample.row <= 1) && (sample.col >= 0) && (sample.col <= 1))
            {
                cout << sample.row << " " << sample.col << endl;
            }
        }
    }

    // clean up
    delete tiler; tiler = 0;
    if(seed_tiling) delete seed_tiling; seed_tiling = 0;
    delete tiling; tiling = 0;
    
    // done
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return Main(argc, argv);
    }
    catch(Exception e)
    {
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
