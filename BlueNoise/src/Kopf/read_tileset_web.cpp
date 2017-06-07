/*
  modified from the code provided by Kopf
  http://johanneskopf.de/publications/blue_noise/tilesets/index.html

  Li-Yi Wei
  10/18/2007

*/

#include <iostream>
using namespace std;

#include <stdlib.h>

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

//void loadTileSet(const char * fileName)
int main(int argc, char ** argv)  
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " tile-set-file-name" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * fileName = argv[++argCtr];
    
    FILE * fin = fopen(fileName, "rb");

    if(! fin)
    {
        cerr << "cannot open " << fileName << endl;
        return 1;
    }
    
    const int numTiles = freadi(fin);
    const int numSubtiles = freadi(fin);
    const int numSubdivs = freadi(fin);

    cerr << "numTiles: " << numTiles << ", numSubtiles: " << numSubtiles << ", numSubdivs: " << numSubdivs << endl;
    
    Tile * tiles = new Tile[numTiles];
    
    for (int i = 0; i < numTiles; i++)
    {
        tiles[i].n = freadi(fin);
        tiles[i].e = freadi(fin);
        tiles[i].s = freadi(fin);
        tiles[i].w = freadi(fin);
        
        tiles[i].subdivs = new int * [numSubdivs];
        for (int j = 0; j < numSubdivs; j++)
        {
            int * subdiv = new int[sqri(numSubtiles)];
            for (int k = 0; k < sqri(numSubtiles); k++)
                subdiv[k] = freadi(fin);
            tiles[i].subdivs[j] = subdiv;
        }
        
        tiles[i].numPoints = freadi(fin);
        cerr << "tiles[" << i << "].numPoints " << tiles[i].numPoints << endl;
        
        tiles[i].points = new Vec2[tiles[i].numPoints];
        for (int j = 0; j < tiles[i].numPoints; j++)
        {
            tiles[i].points[j].x = freadf(fin);
            tiles[i].points[j].y = freadf(fin);
            freadi(fin);freadi(fin);freadi(fin);freadi(fin);
        }
        
        tiles[i].numSubPoints = freadi(fin);
        cerr << "tiles[" << i << "].numSubPoints " << tiles[i].numSubPoints << endl;
        tiles[i].subPoints = new Vec2[tiles[i].numSubPoints];
        for (int j = 0; j < tiles[i].numSubPoints; j++)
        {
            tiles[i].subPoints[j].x = freadf(fin);
            tiles[i].subPoints[j].y = freadf(fin);
            freadi(fin);freadi(fin);freadi(fin);freadi(fin);
        }
    }
    
    fclose(fin);

    return 0;
}
