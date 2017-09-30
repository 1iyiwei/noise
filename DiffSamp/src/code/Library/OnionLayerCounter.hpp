/*
  OnionLayerCounter.hpp

  starting from the center (all 0)
  enumerate all integer indices from inside to ouside
  that are within a hypersphere with radius

  used mainly for conflict checking
  
  Li-Yi Wei
  07/17/2007

*/

#ifndef _ONION_LAYER_COUNTER_HPP
#define _ONION_LAYER_COUNTER_HPP

#include "Counter.hpp"

class OnionLayerCounter : public Counter
{
public:
    OnionLayerCounter(const int dimension, const float radius_square);
    ~OnionLayerCounter(void);

    int Reset(void);

    int Get(vector<int> & value) const;
    
    int Next(void);

    // tell you how many entries it will have without actually allocating memory
    static int TotalCount(const int dimension, const float radius_square);
    
protected:
    void BruteForceSortConstruction(const int dimension, const float radius_square, vector< vector<int> > & values);

    // inspired by Eric Andres
    void PostalSortConstruction(const int dimension, const float radius_square, vector< vector<int> > & values);
    
protected:
    int _pointer; // points to the current value
    vector< vector<int> > _values; // sorted indices in onion layer order
};

#endif
