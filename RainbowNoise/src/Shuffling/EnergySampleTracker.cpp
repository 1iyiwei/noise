/*
  EnergySampleTracker.cpp

  Li-Yi Wei
  09/03/2009

*/

#include "EnergySampleTracker.hpp"
#include "Exception.hpp"

EnergySampleTracker::~EnergySampleTracker(void)
{
    // nothing else to do
}

int EnergySampleTracker::Set(const vector<const EnergySample *> & samples)
{
    throw Exception("EnergySampleTracker::Set(): shouldn't be called");
    return 0;
}

int EnergySampleTracker::Plus(const EnergySample & sample)
{
    return PlusMinus(sample, PLUS);
}

int EnergySampleTracker::Minus(const EnergySample & sample)
{
    return PlusMinus(sample, MINUS);
}

const EnergySample * EnergySampleTracker::Peak(void) const
{
    return 0;
}

const EnergySample * EnergySampleTracker::Trough(const EnergySample & peak) const
{
    return 0;
}

int EnergySampleTracker::Trough(Sample & sample) const
{
    return 0;
}

int EnergySampleTracker::PlusMinus(const EnergySample & sample, const PlusMinusOption plus_or_minus)
{
    return 0;
}
