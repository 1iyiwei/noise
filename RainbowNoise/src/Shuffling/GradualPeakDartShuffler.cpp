/*
  GradualPeakDartShuffler.cpp

  Li-Yi Wei
  08/26/2009

*/

#include <algorithm>
using namespace std;

#include "GradualPeakDartShuffler.hpp"
#include "Math.hpp"
#include "Exception.hpp"

GradualPeakDartShuffler::GradualPeakDartShuffler(EnergyField & energy_field, const PeakFinder & peak_finder, const TroughFinder & trough_finder, const int max_num_peaks_to_look_per_iteration) : DartShuffler(energy_field, peak_finder, trough_finder), _max_num_peaks_to_look_per_iteration(max_num_peaks_to_look_per_iteration), _old_energy(Math::INF)
{
    UndoIfEnergyIncrease(1);
}

GradualPeakDartShuffler::~GradualPeakDartShuffler(void)
{
    // nothing else to do
}

int GradualPeakDartShuffler::Set(const vector<Sample> & samples)
{
    _old_energy = Math::INF;
    return DartShuffler::Set(samples);
}

int GradualPeakDartShuffler::Set(const vector<EnergySample> & samples)
{
    _old_energy = Math::INF;
    return DartShuffler::Set(samples);
}

float GradualPeakDartShuffler::Next(void)
{
    float new_energy = DartShuffler::Next();

    if((new_energy >= _old_energy) && (_max_num_peaks_to_look_per_iteration != 0))
    {
        const vector<const EnergySample *> & samples = const_cast<const EnergyField &>(_energy_field).Get();

        if(_list.size() != samples.size())
        {
            _list = vector<SortElement>(samples.size());
        }

        for(unsigned int i = 0; i < _list.size(); i++)
        {
            _list[i].sample = samples[i];
        }

        sort(_list.begin(), _list.end());

        bool change = 0;
        int num_peaks_looked = 0;
        for(int i = static_cast<int>(_list.size())-1; !change && ((_max_num_peaks_to_look_per_iteration < 0) || (num_peaks_looked < _max_num_peaks_to_look_per_iteration)) && (i >= 0); i--)
        {
            num_peaks_looked++;

            if(!_list[i].sample)
            {
                throw Exception("GradualPeakDartShuffler::Next(): empty peak pointer");
                return Math::NAN;
            }

            EnergySample & peak = const_cast<EnergySample &>(*_list[i].sample);

            float old_value = 0;
            if(! peak.GetValue(old_value))
            {
                throw Exception("GradualPeakDartShuffler::Next(): cannot get energy value");
                return Math::NAN;
            }

            if(old_value <= 0) break;

            const float new_value = DartShuffler::Next(peak);

            if(new_value < old_value)
            {
                change = 1;
            }

            new_energy = new_value;
        }

#if 0
        if(change)
        {
            const EnergySample * p_energy_peak = _energy_field.Peak(1);
            
            if((!p_energy_peak) || (!p_energy_peak->GetValue(new_energy)))
            {
                throw Exception("GradualPeakDartShuffler::Next(): cannot get peak energy");
                return Math::NAN;
            }
        }
#endif
        _old_energy = new_energy;

        if(! change && (_max_num_peaks_to_look_per_iteration < 0))
        {
            return Math::INF;
        }
    }

    return (_old_energy = new_energy);
}
