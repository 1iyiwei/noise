/*
  Math.cpp

  Li-Yi Wei
  07/15/2007

*/

#include <math.h>

#include "Math.hpp"

double Math::PI = 3.14159265;

float Math::MaxPackingDensity(const int dimension)
{
    float packing_density = -1;

    // from  http://mathworld.wolfram.com/HyperspherePacking.html
    
    switch(dimension)
    {
    case 2:
        packing_density = PI*sqrt(3.0)/6;
        break;
        
    case 3:
        packing_density = PI*sqrt(2.0)/6;
        break;
        
    case 4:
        packing_density = PI*PI/16;
        break;
        
    case 5:
        packing_density = PI*PI*sqrt(2.0)/30;
        break;
        
    case 6:
        packing_density = PI*PI*PI*sqrt(3.0)/144;
        break;
        
    case 7:
        packing_density = PI*PI*PI/105;
        break;
        
    case 8:
        packing_density = PI*PI*PI*PI/384;
        break;
        
    default:
        packing_density = -1;
    }

    return packing_density;
}
    
float Math::ComputeMaxMinDistance(const int dimension, const float num_samples)
{
    const float packing_density = MaxPackingDensity(dimension);
    
    const float answer = (packing_density > 0 ? 2*pow(packing_density/(PI*num_samples), 1.0/dimension) : -1);
    
    return answer;
}

int Math::ComputeMaxNumSamples(const int dimension, const float min_distance)
{ 
    const float packing_density = MaxPackingDensity(dimension);

    const int answer = (packing_density > 0 ? static_cast<int>(floor(packing_density/PI*pow(min_distance/2, -dimension))) : -1);

    return answer;
}

double Math::HyperSphereVolume(const int dimension, const float radius)
{
    if(dimension < 2)
    {
        return -1;
    }
    else
    {
        return pow(PI, 0.5*dimension)*pow(radius, dimension)/Gamma(dimension);
    }
}
    
double Math::Gamma(const int n)
{
    if(n <= 0)
    {
        return -1;
    }
    else
    {
        if(n%2 == 0)
        {
            // even case
            return Factorial(n/2);
        }
        else
        {
            // odd case
            return sqrt(PI)*DoubleFactorial(n)/pow(2.0, (n+1)/2);
        }
    }
}
    
double Math::Factorial(const int n)
{
    double answer = 1;

    for(int i = 1; i <= n; i++)
    {
        answer *= i;
    }

    return answer;
}

double Math::DoubleFactorial(const int n)
{
    double answer = 1;

    for(int i = n; i > 0; i-=2)
    {
        answer *= i;
    }

    return answer;
}
