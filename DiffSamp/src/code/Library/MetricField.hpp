/*
  MetricField.hpp

  the base class for all metric/measurement fields
  such as DistanceField and JacobianField

  Li-Yi Wei
  10/17/2010

*/

#ifndef _METRIC_FIELD_HPP
#define _METRIC_FIELD_HPP

#include "Sample.hpp"
#include "Domain.hpp"

class MetricField
{
public:
    MetricField(const Domain & domain);
    virtual ~MetricField(void) = 0;

    int Dimension(void) const;

    const Domain & GetDomain(void) const;

    const vector<float> & BoundingBox(void) const;

    // return 1 if inside this distance field, 0 else
    int Inside(const Sample & sample) const;
    
protected:
    static int LocateCell(const Sample & sample, const float cell_size, vector<int> & index);

protected:
    const Domain & _domain;
    vector<float> _domain_size;
};

#endif
