/*
  EnergySort.hpp

  keeping sorted lists pointing to energy samples across multiple classes

  Li-Yi Wei
  08/25/2009

*/

#ifndef _ENERGY_SORT_HPP
#define _ENERGY_SORT_HPP

#include <vector>
using namespace std;

#include "EnergySample.hpp"

class EnergySort
{
public:
    EnergySort(void);
    EnergySort(const int num_class);
    virtual ~EnergySort(void);

    int Set(const vector<const EnergySample *> & samples);
    
    // return the sample whose energy has rank-th smallest value in which_class
    // return 0 if rank out of bound or some other error conditions
    const EnergySample * Get(const int which_class, const int rank) const;

    // update the energy rank of the sample in a specific class
    // (e.g. when the energy value of sample got updated)
    // consistency check for the rank-th sample in which class must be sample
    // return 1 if update successful, 0 else
    int Update(const int which_class, const int rank, EnergySample & sample);

protected:
    struct SortElement
    {
        SortElement(void) {which_class = 0; sample = 0;};
        SortElement(const int which_class_in, const EnergySample & sample_in) : which_class(which_class_in), sample(&sample_in){};
        ~SortElement(void) {};

        int operator<(const SortElement & rhs) const {return sample->GetValue(which_class) < rhs.sample->GetValue(which_class);};

        int which_class;
        const EnergySample * sample;
    };

    // one list for each target class
    vector< vector<SortElement> > _lists;
    SortElement _element;
};

#endif

