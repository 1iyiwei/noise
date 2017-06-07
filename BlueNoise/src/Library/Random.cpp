/*
  Random.cpp

  Li-Yi Wei
  06/28/2007

*/

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Random.hpp"

// Li-Yi: this is not working yet.... 07/07/2007
#define _WIN32_USE_RAND48

#ifdef WIN32
#ifdef _WIN32_USE_RAND48
extern "C" {
void srand48(long seed);
double drand48(void);
}
#endif
#endif

void Random::InitRandomNumberGenerator(void)
{
#ifdef WIN32
#ifndef _WIN32_USE_RAND48
    srand(time(0));
    // srandom(time(0));
#else
    srand48(time(0));
#endif
#else
    srand48(time(0));
#endif
}

void Random::InitRandomNumberGenerator(const unsigned long seed)
{
#ifdef WIN32
#ifndef _WIN32_USE_RAND48
    srand(seed);
    // srandom(time(0));
#else
    srand48(seed);
#endif
#else
    srand48(seed);
#endif
}
    
double Random::UniformRandom(void)
{
#ifdef WIN32
#ifndef _WIN32_USE_RAND48
    return static_cast<double>(rand())/RAND_MAX;
    // return static_cast<double>(random())/0xffffffff; // 2^32-1
#else
    return drand48();
#endif
#else
    return drand48();
#endif
}
