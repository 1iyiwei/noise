/*
  DiscreteTroughFinder.cpp

  Li-Yi Wei
  08/10/2009

*/

#include "DiscreteTroughFinder.hpp"
#include "Exception.hpp"

DiscreteTroughFinder::DiscreteTroughFinder(const TribePointEnergyField & energy_field) : _energy_field(energy_field)
{
    // nothing else to do
}

DiscreteTroughFinder::~DiscreteTroughFinder(void)
{
    // nothing else to do
}

int DiscreteTroughFinder::Find(const EnergyField & energy_field, Sample & answer) const
{
    if(&energy_field != &_energy_field)
    {
        throw Exception("DiscreteTroughFinder::Find(): pointer mismatch");
        return 0;
    }

    const EnergySample * p_trough = _energy_field.Trough(answer.id, 1);

    if(p_trough)
    {
        return p_trough->GetSample(answer);
    }
    else
    {
        return 0;
    }
}
