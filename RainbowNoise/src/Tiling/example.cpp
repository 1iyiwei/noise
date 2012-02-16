/*

File: example.cpp

An example of the use of the Quasisampler class.
Reminder: This is a toy implementation, created
to aid in understanding how the system works.

This example generates a set of points for a 
constant-value function.

Usage: example [magnitude_factor=1000]

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
#include "quasisampler_prototype.h"


class MyQuasisampler : public Quasisampler
{
  unsigned const_val;

public:
  MyQuasisampler(unsigned val) : Quasisampler(100,100), const_val(val) {}

  unsigned getImportanceAt( Point2D pt ) 
  { 
    return const_val; // This function returns a constant value.
  }
};


typedef std::vector<Point2D> PointList;

int main(int argc, char* argv[])
{
  unsigned mag_factor = 1000;
  if (argc>1) mag_factor = atoi(argv[1]);

  // initialize sampler
  MyQuasisampler test(mag_factor);

  // generate points
  PointList points = test.getSamplingPoints();

  // print points
  for ( PointList::iterator it=points.begin(); it!=points.end(); it++ )
    std::cout << it->x << "," << it->y << std::endl;

  return 0;
}

