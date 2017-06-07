/*
  modified from the code provided by Kopf
  http://johanneskopf.de/publications/blue_noise/tilesets/index.html

  Li-Yi Wei
  10/18/2007

*/

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>

#if 0
void InitRandom(void)
{
    srand48(time(0));
}

double UniformRandom(void)
{
    return drand48();
}
#endif

int freadi(FILE * fIn)
{
	int iTemp;
	fread(&iTemp, sizeof(int), 1, fIn);
	return iTemp;
}

float freadf(FILE * fIn)
{
	float fTemp;
	fread(&fTemp, sizeof(float), 1, fIn);
	return fTemp;
}

inline int sqri(int a) { return a*a; };

struct Vec2
{
    float x, y;
};

struct Tile
{
    int n, e, s, w;
    int numSubtiles, numSubdivs, numPoints, numSubPoints;
    int ** subdivs;
    Vec2 * points, * subPoints;
};

struct TileSet
{
    Tile * tiles;
    int numTiles, numSubtiles, numSubdivs;
};

TileSet loadTileSet(const char * fileName)
{
    FILE * fin = fopen(fileName, "rb");

    TileSet tile_set;
    tile_set.tiles = 0;
    tile_set.numTiles = tile_set.numSubtiles = tile_set.numSubdivs = 0;
        
    if(! fin)
    {
        cerr << "cannot open " << fileName << endl;
        return tile_set;
    }
    
    tile_set.numTiles = freadi(fin);
    tile_set.numSubtiles = freadi(fin);
    tile_set.numSubdivs = freadi(fin);

    Tile * tiles = new Tile[tile_set.numTiles];
    tile_set.tiles = tiles;
    
    for (int i = 0; i < tile_set.numTiles; i++)
    {
        tiles[i].n = freadi(fin);
        tiles[i].e = freadi(fin);
        tiles[i].s = freadi(fin);
        tiles[i].w = freadi(fin);
        
        tiles[i].subdivs = new int * [tile_set.numSubdivs];
        for (int j = 0; j < tile_set.numSubdivs; j++)
        {
            int * subdiv = new int[sqri(tile_set.numSubtiles)];
            for (int k = 0; k < sqri(tile_set.numSubtiles); k++)
                subdiv[k] = freadi(fin);
            tiles[i].subdivs[j] = subdiv;
        }
        
        tiles[i].numPoints = freadi(fin);
        
        tiles[i].points = new Vec2[tiles[i].numPoints];
        for (int j = 0; j < tiles[i].numPoints; j++)
        {
            tiles[i].points[j].x = freadf(fin);
            tiles[i].points[j].y = freadf(fin);
            freadi(fin);freadi(fin);freadi(fin);freadi(fin);
        }
        
        tiles[i].numSubPoints = freadi(fin);
        tiles[i].subPoints = new Vec2[tiles[i].numSubPoints];
        for (int j = 0; j < tiles[i].numSubPoints; j++)
        {
            tiles[i].subPoints[j].x = freadf(fin);
            tiles[i].subPoints[j].y = freadf(fin);
            freadi(fin);freadi(fin);freadi(fin);freadi(fin);
        }
    }
    
    fclose(fin);

    return tile_set;
}

int main(int argc, char ** argv)  
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " tile-set-file-name tiling-size output-file-name" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int tiling_size = atoi(argv[++argCtr]);
    const char * output_file_name = argv[++argCtr];

    // read stuff
    const TileSet tile_set = loadTileSet(input_file_name);

    {
        cerr << "numTiles: " << tile_set.numTiles << ", numSubtiles: " << tile_set.numSubtiles << ", numSubdivs: " << tile_set.numSubdivs << endl;
        
        for(int i = 0; i < tile_set.numTiles; i++)
        {
            cerr << "tiles[" << i << "].numPoints " << tile_set.tiles[i].numPoints << endl;
            cerr << "tiles[" << i << "].numSubPoints " << tile_set.tiles[i].numSubPoints << endl;
        }
    }

    // processing
    vector< vector<const Tile *> > tiling(tiling_size);
    for(unsigned int i = 0; i < tiling.size(); i++)
    {
        tiling[i] = vector<const Tile *>(tiling_size);

        for(unsigned int j = 0; j < tiling[i].size(); j++)
        {
            tiling[i][j] = 0;
        }
    }

    // build tile set in scanline order
    srandom(time(0)); // InitRandom();
    
    for(unsigned int i = 0; i < tiling.size(); i++)
    {
        for(unsigned int j = 0; j < tiling[i].size(); j++)
        {
            if(tiling[i][j] == 0)
            {
                const int i_n = (i+tiling.size()-1)%tiling.size();
                const int i_s = (i+1)%tiling.size();
                const int j_w = (j+tiling[i].size()-1)%tiling[i].size();
                const int j_e = (j+1)%tiling[i].size();

                Tile required;

                required.n = tiling[i_n][j] > 0 ? tiling[i_n][j]->s : -1;
                required.s = tiling[i_s][j] > 0 ? tiling[i_s][j]->n : -1;
                required.w = tiling[i][j_w] > 0 ? tiling[i][j_w]->e : -1;
                required.e = tiling[i][j_e] > 0 ? tiling[i][j_e]->w : -1;
                
                vector<const Tile *> candidates;
                for(int k = 0; k < tile_set.numTiles; k++)
                {
                    const Tile * current = &(tile_set.tiles[k]);
                    
                    if(((required.n < 0) || (required.n == current->n)) &&
                       ((required.s < 0) || (required.s == current->s)) &&
                       ((required.w < 0) || (required.w == current->w)) &&
                       ((required.e < 0) || (required.e == current->e)))
                    {
                        candidates.push_back(current);
                    }
                }

                if(candidates.size() <= 0)
                {
                    cerr << "cannot find candidate" << endl;
                    return 1;
                }
                else
                {
                    const int selection = random()%candidates.size();

                    tiling[i][j] = candidates[selection];
                }
            }
        }
    }
    
    // output
    ofstream output(output_file_name);

    if(! output)
    {
        cerr << "cannot write into " << output_file_name << endl;
        return 1;
    }

    for(unsigned int i = 0; i < tiling.size(); i++)
    {
        for(unsigned int j = 0; j < tiling[i].size(); j++)
        {
            if(tiling[i][j] == 0)
            {
                cerr << "strange tiling[i][j] == 0" << endl;
                return 1;
            }
            else
            {
                const float x_base = j*1.0/tiling_size;
                const float y_base = i*1.0/tiling_size;
                
                for(int k = 0; k < tiling[i][j]->numPoints; k++)
                {
                    output << tiling[i][j]->points[k].x/tiling[i].size() + x_base << " " << tiling[i][j]->points[k].y/tiling.size() + y_base << endl;
                }
            }
        }
    }
    
    // done
    return 0;
}
