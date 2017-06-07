/*
  NBallSliceCounter.hpp

  give you all integer lattices inside a hypersphere
  but not in an inside-to-outside order
  
  Li-Yi Wei
  07/23/2007

*/

#ifndef _NBALL_SLICE_COUNTER_HPP
#define _NBALL_SLICE_COUNTER_HPP

#include "Counter.hpp"

class NBallSliceCounter : public Counter
{
public:
    NBallSliceCounter(const int dimension, const float radius_square);
    ~NBallSliceCounter(void);

    int Reset(void);

    int Get(vector<int> & value) const;

    // class specific function for acceleration
    int Get(vector<int> & value, float & norm_square) const;
    
    int Next(void);
    
protected:
    const float _radius_square;
    const float _radius;
    vector<int> _limit;
    vector<int> _value_square_sum; // _value_square_sum[i] = sum_{k=i+1}^{dimension} (value[k]*value[k])
    vector<int> _value;
};

#endif
