/*
  DiscreteDartSwapper.cpp

  Li-Yi Wei
  08/17/2009

*/

#include <algorithm>
using namespace std;

#include "DiscreteDartSwapper.hpp"
#include "Math.hpp"
#include "Exception.hpp"

DiscreteDartSwapper::DiscreteDartSwapper(EnergyField & energy_field) : DartSwapper(energy_field)
{
    // nothing else to do
}

DiscreteDartSwapper::~DiscreteDartSwapper(void)
{
    // nothing else to do
}

int DiscreteDartSwapper::Set(const vector<Sample> & samples_and_points)
{
    return DartSwapper::Set(samples_and_points);
}

int DiscreteDartSwapper::Get(vector<Sample> & samples_and_points) const
{
    return DartSwapper::Get(samples_and_points);
}

float DiscreteDartSwapper::Next(void)
{
    throw Exception("DiscreteDartSwapper::Next(): shouldn't be called");
    return Math::NAN;
}

float DiscreteDartSwapper::Swap(EnergySample & s0, EnergySample & s1)
{
    if(!s0.GetSample(_s0) || !s1.GetSample(_s1))
    {
        throw Exception("DiscreteDartSwapper::Swap(): cannot get samples");
        return Math::NAN;
    }

    float old_energy0, old_energy1;
    {
        const EnergySample * p0 = _energy_field.Peak(s0, 1);
        const EnergySample * p1 = _energy_field.Peak(s1, 1);

        if(!p0 || !p0->GetValue(old_energy0) || !p1 || !p1->GetValue(old_energy1))
        {
            throw Exception("DiscreteDartSwapper::Swap(): cannot get values");
            return Math::NAN;
        }
    }

    if(!s0.Fixed() && !s1.Fixed())
    {
        swap(_s0.coordinate, _s1.coordinate);
    }

    const float diff0 = _energy_field.Move(s0, _s0);
    const float diff1 = _energy_field.Move(s1, _s1);

    float new_energy0, new_energy1;
    {
        const EnergySample * p0 = _energy_field.Peak(s0, 1);
        const EnergySample * p1 = _energy_field.Peak(s1, 1);

        if(!p0 || !p0->GetValue(new_energy0) || !p1 || !p1->GetValue(new_energy1))
        {
            throw Exception("DiscreteDartSwapper::Swap(): cannot get values");
            return Math::NAN;
        }
    }

#if 1
    return (max(new_energy0, new_energy1) - max(old_energy0, old_energy1));
#else
    return (diff0+diff1);
#endif
}
