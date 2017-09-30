/*
  SampleHolder.hpp

  the base class of all sample holders/containers
  holders only hold, but not own, samples

  Li-Yi Wei
  09/21/2010

*/

#ifndef _SAMPLE_HOLDER_HPP
#define _SAMPLE_HOLDER_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"
#include "Domain.hpp"

class SampleHolder
{
public:
    SampleHolder(const Domain & domain);
    virtual ~SampleHolder(void) = 0;

    const Domain & GetDomain(void) const;

    // return 1 if sample inside holder, 0 else
    virtual int Inside(const Sample & sample) const;

    // return 1 if successful, 0 else
    // sample is not cloned and will belong to the caller
    virtual int Add(const Sample & sample);

    // the inverse operation of Add
    // remove the sample from the holder
    // return 1 if successful, 0 else
    virtual int Remove(const Sample & sample);

    // clear all samples
    virtual void Clear(void);

    virtual void GetSamples(vector<const Sample *> & samples) const;

    // return neighbors *potentially* within radius of query
    // used for conflict check, and thus might be over conservative
    // for toroidal boundary condition, return original, not corrected, sample
    virtual int GetNeighbors(const Sample & query, const float radius, vector<const Sample *> & neighbors) const;

protected:
    const Domain & _domain;
};

#endif
