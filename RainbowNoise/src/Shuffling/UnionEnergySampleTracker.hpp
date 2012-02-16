/*
  UnionEnergySampleTracker.hpp

  class managing a collection of energy sample trackers
  
  Li-Yi Wei
  09/04/2009

*/

#ifndef _UNION_ENERGY_SAMPLE_TRACKER_HPP
#define _UNION_ENERGY_SAMPLE_TRACKER_HPP

#include <vector>
using namespace std;

#include "EnergySampleTracker.hpp"

class UnionEnergySampleTracker : public EnergySampleTracker
{
public:
    struct Member
    {
        Member(void): p_tracker(0), class_id_min(0), class_id_max(-1) {};

        EnergySampleTracker * p_tracker;
        int class_id_min;
        int class_id_max;
    };

public:
    UnionEnergySampleTracker(const vector<Member> & trackers);
    virtual ~UnionEnergySampleTracker(void);

    virtual int Set(const vector<const EnergySample *> & samples);

    virtual const EnergySample * Peak(void) const;
    virtual const EnergySample * Trough(const EnergySample & peak) const;

    virtual int Trough(Sample & sample) const;

protected:
    virtual int PlusMinus(const EnergySample & sample, const PlusMinusOption plus_or_minus);

    // map from external class id to internal index
    int MapClassID(const int external_class_id) const;

protected:
    const vector<Member> _trackers;
    mutable Sample _sample;
};

#endif

