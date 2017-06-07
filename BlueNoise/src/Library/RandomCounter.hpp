/*
  RandomCounter.hpp

  Li-Yi Wei
  08/02/2007

*/

#ifndef _RANDOM_COUNTER_HPP
#define _RANDOM_COUNTER_HPP

#include "Counter.hpp"
#include "SequentialCounter.hpp"

class RandomCounter : public Counter
{
public:
    RandomCounter(const int dimension, const int digit_min, const int digit_max);
    RandomCounter(const int dimension, const vector<int> & digit_min, const vector<int> & digit_max);
    ~RandomCounter(void);

    int Reset(void);

    int Get(vector<int> & value) const;
    
    int Next(void);

protected:
    SequentialCounter _counter;
    vector< vector<int> > _values;
    int _index;
};

#endif
