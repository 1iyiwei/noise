/*
  PolyominoTiling.cpp

  Li-Yi Wei
  10/26/2007

*/

#include <iostream>
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
#include "FrameBuffer.hpp"
#include "Exception.hpp"

string Orientation(const Polyomino::Orientation orientation)
{
    string result = "";

    switch(orientation)
    {
    case Polyomino::ON: result = "ON"; break;
    case Polyomino::OE: result = "OE"; break;
    case Polyomino::OS: result = "OS"; break;
    case Polyomino::OW: result = "OW"; break;
    case Polyomino::O0: result = "O0"; break;
    }

    return result;
}

string Reflection(const Polyomino::Reflection reflection)
{
    string result = "";

    switch(reflection)
    {
    case Polyomino::R1: result = "R1"; break;
    case Polyomino::R2: result = "R2"; break;
    case Polyomino::R0: result = "R0"; break;
    }

    return result;
}

int Main(int argc, char **argv)
{
    if(argc < 9)
    {
        cerr << "Usage: " << argv[0] << " tiling_type height width template_height template_width num_templates num_subdivisions output_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const string tiling_type = argv[++argCtr];
    const int height = atoi(argv[++argCtr]);
    const int width = atoi(argv[++argCtr]);
    const int template_height = atoi(argv[++argCtr]);
    const int template_width = atoi(argv[++argCtr]);
    const int num_templates = atoi(argv[++argCtr]);
    const int num_subdivisions = atoi(argv[++argCtr]);
    const char * output_file_name = argv[++argCtr];
    
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
    else if(tiling_type == "ghexomino")
    {
        if((height%template_height) || (width%template_width))
        {
            cerr << "height/width must be integer multiple of template_height/template_width" << endl;
            return 1;
        }

        if((height == template_height) && (width == template_width))
        {
            tiler = new GHexominoTiler(height, width);
        }
        else
        {
            cerr << "GHexominoSuperTiler not yet implemented!" << endl;
            return 1;
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
            cerr << "build seed tiling for subdivision" << endl;
            
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
            cerr << "cannot find tiling" << endl;
            return 1;
        }
    
        const string message = tiling->GetMessage();

        if(message != "")
        {
            cerr << "Error in tile construction: " << message << endl;
            return 1;
        }
    }

#if 0
    // for dumping out g-hexomino subdivision stuff
    if(tiling_type == "ghexomino_subdivision")
    {
        for(unsigned int i = 0; i < tiling->NumTiles(); i++)
        {
            const Polyomino * tile = tiling->GetTile(i);
            
            Polyomino::Int2 center;
            tile->GetCenter(center);

            Polyomino::Orientation orientation;
            tile->GetOrientation(orientation);

            Polyomino::Reflection reflection;
            tile->GetReflection(reflection);

            cout << "{" << center.row << ", " << center.col << ", Polyomino::" << Orientation(orientation) << ", Polyomino::" << Reflection(reflection) << "}," << endl;
        }
    }
#endif
    
    // output result
    cerr << "output result" << endl;

    if(!strstr(output_file_name, ".ppm"))
    {
        tiling->Write(output_file_name);
    }
    else
    {
        vector< vector<FrameBuffer::P3> > pixels(tiling->Size().row);
        for(unsigned int row = 0; row < pixels.size(); row++)
        {
            pixels[row] = vector<FrameBuffer::P3>(tiling->Size().col);

            for(unsigned int col = 0; col < pixels[row].size(); col++)
            {
                pixels[row][col].r = 0;
                pixels[row][col].g = 0;
                pixels[row][col].b = 0;
            }
        }

        vector<Polyomino::Int2> cells;
        for(int i = 0; i < tiling->NumTiles(); i++)
        {
            const Polyomino * tile = tiling->GetTile(i);

            FrameBuffer::P3 color;
            color.r = static_cast<int>(floor(Random::UniformRandom()*255+0.5));
            color.g = static_cast<int>(floor(Random::UniformRandom()*255+0.5));
            color.b = static_cast<int>(floor(Random::UniformRandom()*255+0.5));

            Polyomino::Int2 center;
            tile->GetCenter(center);
            tile->GetCells(cells);

            for(unsigned int j = 0; j < cells.size(); j++)
            {
                const int row = center.row + cells[j].row;
                const int col = center.col + cells[j].col;

                pixels[row][col] = color;
            }       
        }

        if(0)
        {
            // debug: try to color neighbors
            if(tiling->NumTiles())
            {
                int selection = static_cast<int>(floor(Random::UniformRandom()*(tiling->NumTiles()-1)));
                const Polyomino * tile = tiling->GetTile(selection);

                Polyomino::Int2 center;
                tile->GetCenter(center);
                tile->GetCells(cells);

                for(unsigned int j = 0; j < cells.size(); j++)
                {
                    const int row = center.row + cells[j].row;
                    const int col = center.col + cells[j].col;

                    pixels[row][col].g = 0;
                }

                // neighbors
                for(unsigned int i = 0; i < tile->NumNeighbors(); i++)
                {
                    const Polyomino::Neighbor neighbor = tile->GetNeighbor(i);

                    neighbor.polyomino->GetCenter(center);
                    neighbor.polyomino->GetCells(cells);

                    for(unsigned int j = 0; j < cells.size(); j++)
                    {
                        const int row = center.row + cells[j].row;
                        const int col = center.col + cells[j].col;

                        pixels[row][col].g = 255;
                    }
                }
            }
        }
    
        if(! FrameBuffer::WritePPM(pixels, 255, output_file_name))
        {
            cerr << "error in writing " << output_file_name << endl;
        }
    }
    
    // clean up
    if(tiler) delete tiler; tiler = 0;
    if(seed_tiling) delete seed_tiling; seed_tiling = 0;
    if(tiling) delete tiling; tiling = 0;
    
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
