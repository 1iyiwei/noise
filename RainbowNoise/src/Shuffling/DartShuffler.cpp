/*
  DartShuffler.cpp

  Li-Yi Wei
  06/11/2009

*/

#include <algorithm>
using namespace std;

#include "DartShuffler.hpp"
#include "Math.hpp"
#include "Exception.hpp"

DartShuffler::DartShuffler(EnergyField & energy_field, const PeakFinder & peak_finder, const TroughFinder & trough_finder) : _energy_field(energy_field), _peak_finder(peak_finder), _trough_finder(trough_finder), _remove_peak_before_find_trough(1), _undo_if_energy_increase(0), _p_energy_peak(0), _p_energy_trough(0)
{
    // _remove_peak_before_find_trough: should be set
    // _undo_if_energy_increase: if set, could get stuck in local minimum 08/24/2009

    // nothing else to do
}

DartShuffler::~DartShuffler(void)
{
    // nothing else to do
}

int DartShuffler::Set(const vector<Sample> & samples)
{
    return _energy_field.Set(samples);
}

int DartShuffler::Set(const vector<EnergySample> & samples)
{
    return _energy_field.Set(samples);
}

int DartShuffler::Get(vector<Sample> & samples) const
{
    return _energy_field.Get(samples);
}

int DartShuffler::Get(vector<EnergySample> & samples) const
{
    return _energy_field.Get(samples);
}

float DartShuffler::Next(void) 
{
    // find peak
    if((_p_energy_peak = _peak_finder.Find(_energy_field)) == 0)
    {
        _p_energy_peak = _energy_field.Peak(0);
    }

    if(! _p_energy_peak)
    {
        throw Exception("DartShuffler::Next(): cannot find peak");
        return Math::NAN;
    }

    EnergySample & peak = const_cast<EnergySample &>(*_p_energy_peak);

    return Next(peak);
}

float DartShuffler::Next(EnergySample & peak)
{
    float old_peak_energy = 0;
    if(! peak.GetValue(old_peak_energy))
    {
        throw Exception("DartShuffler::Next(): cannot get peak energy");
        return Math::NAN;
    }

    if(peak.Fixed())
    {
        return old_peak_energy;
    }

    if(! peak.GetSample(_peak))
    {
        throw Exception("DartShuffler::Next(): cannot get peak sample");
        return Math::NAN;
    }

    // find trough
    if(_remove_peak_before_find_trough)
    {
        if(Math::IsNAN(_energy_field.EnergyMinus(peak)))
        {
            throw Exception("DartShuffler::Next(): cannot energy minus peak");
            return Math::NAN;
        }
        _energy_field.Visible(peak, 0);
    }

    _p_energy_trough = _energy_field.Trough(peak);

    if(_p_energy_trough)
    {
        if(! _p_energy_trough->GetSample(_trough))
        {
            throw Exception("DartShuffler::Next(): cannot get trough sample");
            return Math::NAN;
        }
    }
    else
    {
        _trough = _peak;
        if(! _trough_finder.Find(_energy_field, _trough))
        {
            throw Exception("DartShuffler::Next(): cannot find trough");
            return Math::NAN;
        }
    }

    if((!_p_energy_trough) && (_trough.id != _peak.id))
    {
        throw Exception("DartShuffler::Next(): _trough.id != _peak.id only accepted for swapping, not moving");
        return Math::NAN;
    }

    if((_p_energy_trough) && (_trough.id == _peak.id))
    {
        throw Exception("DartShuffler::Next(): _trough.id == _peak.id only accepted for moving, not swapping");
        return Math::NAN;
    }

    if(_remove_peak_before_find_trough)
    {
        _energy_field.Visible(peak, 1);
        if(Math::IsNAN(_energy_field.EnergyPlus(peak)))
        {
            throw Exception("DartShuffler::Next(): cannot energy plus peak");
            return Math::NAN;
        }
    }

    if(! peak.Visible())
    {
        _energy_field.Visible(peak, 1);
    }

    if(Math::IsNAN(Swap(peak, const_cast<EnergySample *>(_p_energy_trough), _trough)))
    {
        throw Exception("DartShuffler::Next(): cannot swap peak and trough");
        return Math::NAN;
    }

    if(Math::IsNAN(Undo(old_peak_energy, peak, const_cast<EnergySample *>(_p_energy_trough), _trough)))
    {
        throw Exception("DartShuffler::Next(): error in undo");
        return Math::NAN;
    }
    
    // done
#if 1
    float value = 0;
    if(! peak.GetValue(value))
    {
        throw Exception("DartShuffler::PeakEnergy(): cannot get peak energy");
        return Math::NAN;
    }

    return value;
#else
    return _energy_field.Get(peak);
#endif
}

float DartShuffler::PeakEnergy(void) const
{
    // find peak
    _p_energy_peak = _energy_field.Peak(1);

    if(!_p_energy_peak)
    {
        throw Exception("DartShuffler::PeakEnergy(): cannot find peak");
        return Math::NAN;
    }

    // measure energy
#if 1
    float value = 0;
    if(! _p_energy_peak->GetValue(value))
    {
        throw Exception("DartShuffler::PeakEnergy(): cannot get peak energy");
        return Math::NAN;
    }

    return value;
#else
    return _energy_field.Get(*_p_energy_peak);
#endif
}

float DartShuffler::Swap(EnergySample & s0, EnergySample & s1)
{
    if(!s0.GetSample(_s0) || !s1.GetSample(_s1))
    {
        throw Exception("DiscreteDartSwapper::Swap(): cannot get samples");
        return Math::NAN;
    }

    swap(_s0.coordinate, _s1.coordinate);

    const float diff0 = _energy_field.Move(s0, _s0);
    const float diff1 = _energy_field.Move(s1, _s1);
    
    return (diff0+diff1);
}

float DartShuffler::Swap(EnergySample & s0, Sample & s1)
{
    if(!s0.GetSample(_s0))
    {
        throw Exception("DiscreteDartSwapper::Swap(): cannot get sample");
        return Math::NAN;
    }

    swap(_s0.coordinate, s1.coordinate);

    const float diff0 = _energy_field.Move(s0, _s0);
    
    return (diff0);
}

float DartShuffler::Swap(EnergySample & s0, EnergySample * ps1, Sample & s1)
{
    float value = Math::NAN;

    // Essentially, I am using whether the trough is found via Sample or EnergySample to determine if I should swap or move. If the trough is returned as an EnergySample, then I swap. Otherwise I move. This means that for TribePointEnergyField I have to return trough via a dummy class DiscreteTroughFinder as I cannot move the "points", whereas for UnionPointEnergyField I have to return trough through the class itself as an EnergySample.

    if(ps1)
    {
        if(Math::IsNAN(value = Swap(s0, const_cast<EnergySample &>(*ps1))))
        {
            throw Exception("DartShuffler::Swap(): cannot swap peak and trough");
            return Math::NAN;
        }
    }
    else
    {
        if(Math::IsNAN(value = Swap(s0, s1)))
        {
            throw Exception("DartShuffler::Swap(): cannot move peak to trough");
            return Math::NAN;
        }
    }

    return value;
}

void DartShuffler::RemovePeakBeforeFindTrough(const bool preference)
{
    _remove_peak_before_find_trough = preference;
}

void DartShuffler::UndoIfEnergyIncrease(const bool preference)
{
    _undo_if_energy_increase = preference;
}

float DartShuffler::Undo(const float old_peak_energy, EnergySample & peak, EnergySample * energy_trough, Sample & sample_trough)
{
    if(_undo_if_energy_increase)
    {
        float new_previous_peak_energy = 0;
        float new_previous_trough_energy = 0;

        const EnergySample * peak_near_previous_peak = _energy_field.Peak(peak, 1);
        if(!peak_near_previous_peak || !peak_near_previous_peak->GetValue(new_previous_peak_energy))
        // if(! peak.GetValue(new_previous_peak_energy))
        {
            throw Exception("DartShuffler::Undo(): cannot get peak energy");
            return Math::NAN;
        }

        const EnergySample * peak_near_previous_trough = 0;
        if(energy_trough && (sample_trough.id >= 0) && (!(peak_near_previous_trough = _energy_field.Peak(*energy_trough, 1)) || !peak_near_previous_trough->GetValue(new_previous_trough_energy)))
        // if(energy_trough && (sample_trough.id >= 0) && (!energy_trough->GetValue(new_previous_trough_energy)))
        {
            throw Exception("DartShuffler::Undo(): cannot get trough energy");
            return Math::NAN;
        }

#if 0
        const float new_peak_energy = PeakEnergy();
#else
        // only need to make sure the swap did not introduce peak higher than the old one
        const float new_peak_energy = max(new_previous_peak_energy, new_previous_trough_energy);
#endif

        if(new_peak_energy > old_peak_energy)
        {
            if(Math::IsNAN(Swap(peak, const_cast<EnergySample *>(energy_trough), sample_trough)))
            {
                throw Exception("DartShuffler::Undo(): cannot swap peak and trough");
                return Math::NAN;
            }
        }
  
        // done, return energy diff 
        return min(static_cast<float>(0), new_peak_energy - old_peak_energy);
    }
    else
    {
        // nothing is changed
        return 0;
    }

}
