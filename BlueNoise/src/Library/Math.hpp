/*
  Math.hpp

  Li-Yi Wei
  07/15/2007

*/

#ifndef _MATH_HPP
#define _MATH_HPP

class Math
{
public:
    static float MaxPackingDensity(const int dimension);
    static float ComputeMaxMinDistance(const int dimension, const float num_samples);
    static int ComputeMaxNumSamples(const int dimension, const float min_distance);

    static double HyperSphereVolume(const int dimension, const float radius);
    
    // compute Gamma(n/2+1) as in http://en.wikipedia.org/wiki/Hypersphere
    static double Gamma(const int n);
    
    static double Factorial(const int n);
    static double DoubleFactorial(const int n);

public:
    static double PI;
};

#endif
