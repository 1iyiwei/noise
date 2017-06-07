/*
  PhaseRandomCounter.hpp

  Li-Yi Wei
  07/10/2007

*/

#ifndef _PHASE_RANDOM_COUNTER_HPP
#define _PHASE_RANDOM_COUNTER_HPP

#include <deque>
using namespace std;

#include "Counter.hpp"

class PhaseRandomCounter : public Counter
{
public:
    PhaseRandomCounter(const int dimension, const int digit_min, const int digit_max, const int clique_size);
    ~PhaseRandomCounter(void);

    int Reset(void);

    int Get(vector<int> & value) const;
    
    int Next(void);

    // class specific function
    int NumGroups(void) const;

    int GetID(int & value) const;

    // for debugging
    // return 1 if no two cells within clique_size distance have identical ids
    // return 0 else
    int Check(void) const;
    
protected:
    struct Entry
    {
        int group_id;
        vector<int> value;
    };
    
protected:
    const int _clique_size;
    vector<int> _digit_min;
    vector<int> _digit_max;
    deque<Entry> _values;
};

#endif
