/*
  ThrowerDartShuffler.cpp

  Li-Yi Wei
  09/07/2009

*/

#include <algorithm>
using namespace std;

#include "ThrowerDartShuffler.hpp"
#include "Math.hpp"
#include "Exception.hpp"

ThrowerDartShuffler::ThrowerDartShuffler(EnergyField & energy_field, const PeakFinder & peak_finder, const TroughFinder & trough_finder) : DartShuffler(energy_field, peak_finder, trough_finder)
{
    // nothing else to do
}

ThrowerDartShuffler::~ThrowerDartShuffler(void)
{
    // nothing else to do
}

int ThrowerDartShuffler::Set(const vector<Sample> & samples)
{
    if(!DartShuffler::Set(samples))
    {
        return 0;
    }

    return LocalSet();
}

int ThrowerDartShuffler::Set(const vector<EnergySample> & samples)
{
    if(!DartShuffler::Set(samples))
    {
        return 0;
    }

    return LocalSet();
}

int ThrowerDartShuffler::LocalSet(void)
{
    // clear energy and make all samples invisible
    const vector<EnergySample *> & energy_samples = _energy_field.Get();
    for(unsigned int i = 0; i < energy_samples.size(); i++)
    {
        if(!energy_samples[i])
        {
            throw Exception("ThrowerDartShuffler::Set(): null energy sample");
        }

        EnergySample & energy_sample = *energy_samples[i];

        if(Math::IsNAN(_energy_field.EnergyMinus(energy_sample)))
        {
            throw Exception("ThrowerDartShuffler::Set(): cannot energy minus");
            return 0;
        }

        _energy_field.Visible(energy_sample, energy_sample.Fixed());
    }

    // num class
    int num_class = 0;
    Sample sample;
    for(unsigned int i = 0; i < energy_samples.size(); i++)
    {
        if(! energy_samples[i]->GetSample(sample))
        {
            throw Exception("ThrowerDartShuffler::LocalSet(): cannot get sample");
            return 0;
        }

        if((sample.id + 1) > num_class)
        {
            num_class = (sample.id + 1);
        }
    }

    // remaining samples
    vector< vector<EnergySample *> > samples_remaining(num_class);

    for(unsigned int i = 0; i < energy_samples.size(); i++)
    {
        if(!energy_samples[i])
        {
            throw Exception("ThrowerDartShuffler::Set(): null energy sample");
        }

        EnergySample & energy_sample = *energy_samples[i];

        if(energy_sample.Fixed()) continue;

        if(! energy_sample.GetSample(sample))
        {
            throw Exception("ThrowerDartShuffler::Set(): canot get sample");
            return 0;
        }

        if(sample.id >= 0)
        {
            if(sample.id < samples_remaining.size())
            {
                samples_remaining[sample.id].push_back(&energy_sample);
            }
            else
            {
                throw Exception("ThrowerDartShuffler::Set(): illegal sample id");
                return 0;
            }
        }
    }

    // counting
    vector<int> num_samples(samples_remaining.size(), 0);
    int total_num_samples = 0;
    for(unsigned int i = 0; i < num_samples.size(); i++)
    {
        num_samples[i] = samples_remaining[i].size();
        total_num_samples += num_samples[i];
    }

    // order energy samples for soft dart throwing
    _samples_remaining.clear();

    while(_samples_remaining.size() < total_num_samples)
    {
        float max_fill_rate = 0;
        int which_class = -1;
        
        for(unsigned int i = 0; i < samples_remaining.size(); i++)
        {
            const float fill_rate = samples_remaining[i].size()*1.0/max(1, num_samples[i]);

            if(fill_rate > max_fill_rate)
            {
                max_fill_rate = fill_rate;
                which_class = i;
            }
        }

        if(which_class < 0)
        {
            throw Exception("ThrowerDartShuffler::Set(): which_class < 0");
            return 0;
        }

        if(samples_remaining[which_class].size() <= 0)
        {
            throw Exception("ThrowerDartShuffler::Set(): empty queue");
            return 0;
        }

        _samples_remaining.push_back(samples_remaining[which_class][samples_remaining[which_class].size()-1]);
        samples_remaining[which_class].pop_back();
    }

    // done
    return 1;
}

float ThrowerDartShuffler::Next(void)
{
    // find peak
    if(_samples_remaining.size() > 0)
    {
        _p_energy_peak = _samples_remaining[0];
        _samples_remaining.erase(_samples_remaining.begin());
        
        _remove_peak_before_find_trough = 0;
    }
    else
    {
        if((_p_energy_peak = _peak_finder.Find(_energy_field)) == 0)
        {
            _p_energy_peak = _energy_field.Peak(0);
        }
        
        _remove_peak_before_find_trough = 1;
    }

    if(! _p_energy_peak)
    {
        throw Exception("ThrowerDartShuffler::Next(): cannot find peak");
        return Math::NAN;
    }

    EnergySample & peak = const_cast<EnergySample &>(*_p_energy_peak);

    return DartShuffler::Next(peak);
}
