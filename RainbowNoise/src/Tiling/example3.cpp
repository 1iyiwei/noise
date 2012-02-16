/*

File: example3.cpp

An example of the use of the Quasisampler class.
Reminder: This is a toy implementation, created
to aid in understanding how the system works.

This example generates a set of points for a 
grayscale bitmap.

Usage: example3 image.pgm [magnitude_factor=200]

This is a toy (non-optimized) implementation of the importance sampling
technique proposed in the paper:
"Fast Hierarchical Importance Sampling with Blue Noise Properties",
by Victor Ostromoukhov, Charles Donohue and Pierre-Marc Jodoin,
to be presented at SIGGRAPH 2004.


Implementation by Charles Donohue,
Based on Mathematica code by Victor Ostromoukhov.
Universite de Montreal
05.08.04

*/

#include <iostream>
#include <fstream>
#include "quasisampler_prototype.h"
using namespace std;

class ImageQuasisampler : public Quasisampler
{
  unsigned *data;
  unsigned w,h;

public:
  ImageQuasisampler(char* filename, double mag) 
  {
    // Load the grayscale image
    if ( !loadPGM(filename, mag) ) { data=0; width=height=0; }  
  }

  // Simple PGM parser (Low fault tolerance)
  bool loadPGM(char* filename, double mag=1.0)
  {
    if (!filename) { cerr << "Could not load PGM: no filename given." << endl; return false; } 
    char buffer[80];
    ifstream infile(filename);
    if ( !infile.is_open() ) { cerr << "Could not open file: " << filename << endl; return false; }
    infile.getline(buffer,80);
    if ( strcmp(buffer,"P2")  )
    {  cerr << "PGM file header not recognized (P2 type only)" << endl; return false;  }
    do { infile.getline(buffer,80); } while ( buffer[0]=='#' );
    w = atoi(buffer);
    char *tmp = strchr(buffer,' '); tmp++; // skip whitespace 
    h = atoi(tmp);
    do { infile.getline(buffer,80); } while ( buffer[0]=='#' );
    unsigned maxval;
    maxval = atoi(buffer);  // nb: not used.  
    data = new unsigned[w*h];   
    for (unsigned i=0; i<w*h; i++) infile >> data[i];
    infile.close();

    if (mag != 1.0) for (unsigned i=0; i<w*h; i++) data[i] = (unsigned)(data[i]*mag);
    
    width = w; height = h;
    return true;
  }

  unsigned getImportanceAt( Point2D pt ) 
  { 
    // Nearest pixel sampling.
    return data[ w * ((unsigned)(height - pt.y)) + (unsigned)(pt.x) ];
  }
};


typedef std::vector<Point2D> PointList;

int main(int argc, char* argv[])
{
  if (argc<2)
  {
    std::cout << "Usage: " << argv[0] << " image.pgm [magnitude_factor = 200]" << std::endl;
  }
  
  double mag_factor = 200.0;
  if (argc>2) mag_factor = atof(argv[2]);

  // initialize sampler
  ImageQuasisampler test( argv[1], mag_factor );

  // generate points
  PointList points = test.getSamplingPoints();

  // print points
  for ( PointList::iterator it=points.begin(); it!=points.end(); it++ )
    std::cout << it->x << "," << it->y << std::endl;

  return 0;
}

