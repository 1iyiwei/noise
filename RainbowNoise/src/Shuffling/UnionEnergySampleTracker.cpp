/*
  UnionEnergySampleTracker.cpp

  Li-Yi Wei
  09/04/2009

*/

#include "UnionEnergySampleTracker.hpp"
#include "Exception.hpp"

UnionEnergySampleTracker::UnionEnergySampleTracker(const vector<Member> & trackers) : _trackers(trackers)
{
    for(unsigned int i = 0; i < _trackers.size(); i++)
    {
        if(!_trackers[i].p_tracker)
        {
            throw Exception("UnionEnergySampleTracker::UnionEnergySampleTracker(): null tracker pointer");
        }
    }
}

UnionEnergySampleTracker::~UnionEnergySampleTracker(void)
{
    // nothing else to do
}

int UnionEnergySampleTracker::Set(const vector<const EnergySample *> & samples)
{
    vector< vector<const EnergySample *> > groups(_trackers.size());

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        const EnergySample * p_sample = samples[i];

        if(!p_sample || !p_sample->GetSample(_sample))
        {
            throw Exception("UnionEnergySampleTracker::Set(): illegal input sample");
            return 0;
        }

        const int internal_id = MapClassID(_sample.id);

        if((internal_id < 0) || (internal_id >= groups.size()))
        {
            throw Exception("UnionEnergySampleTracker::Set(): illegal class id");
            return 0;
        }

        groups[internal_id].push_back(p_sample);
    }

    for(unsigned int i = 0; i < _trackers.size(); i++)
    {
        if(! _trackers[i].p_tracker->Set(groups[i]))
        {
            throw Exception("UnionEnergySampleTracker::Set(): error in setting samples");
            return 0;
        }
    }

    // done
    return 1;
}

const EnergySample * UnionEnergySampleTracker::Peak(void) const
{
    // no action for now
    return 0;
}

const EnergySample * UnionEnergySampleTracker::Trough(const EnergySample & peak) const
{
    // no action for now
    return 0;
}

int UnionEnergySampleTracker::Trough(Sample & sample) const
{
    const int internal_id = MapClassID(sample.id);

    if((internal_id < 0) || (internal_id >= _trackers.size()))
    {
        throw Exception("UnionEnergySampleTracker::Trough(): illegal class id");
        return 0;
    }

    return _trackers[internal_id].p_tracker->Trough(sample);
}

int UnionEnergySampleTracker::PlusMinus(const EnergySample & sample, const PlusMinusOption plus_or_minus)
{
    if(!sample.GetSample(_sample))
    {
        throw Exception("UnionEnergySampleTracker::PlusMinus(): cannot get sample");
        return 0;
    }

    const int internal_id = MapClassID(_sample.id);

    if((internal_id < 0) || (internal_id >= _trackers.size()))
    {
        throw Exception("UnionEnergySampleTracker::PlusMinus(): illegal class id");
        return 0;
    }

    return _trackers[internal_id].p_tracker->PlusMinus(sample, plus_or_minus);
}

int UnionEnergySampleTracker::MapClassID(const int external_class_id) const
{
    int answer = -1;

    for(unsigned int i = 0; i < _trackers.size(); i++)
    {
        if((_trackers[i].class_id_min <= external_class_id) && (external_class_id <= _trackers[i].class_id_max))
        {
            answer = i;
            break;
        }
    }

    return answer;
}
