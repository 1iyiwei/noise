/*
  MetricField.cpp

  Li-Yi Wei
  10/17/2010

*/

#include <math.h>

#include "MetricField.hpp"
#include "Exception.hpp"

MetricField::MetricField(const Domain & domain) : _domain(domain)
{
    if(! _domain.BoundingBox(_domain_size))
    {
        throw Exception("MetricField::MetricField(): cannot compute bounding box!");
    }
}

MetricField::~MetricField(void)
{
    // nothing to do
}

int MetricField::Dimension(void) const
{
    return _domain.Dimension();
}

const Domain & MetricField::GetDomain(void) const
{
    return _domain;
}

const vector<float> & MetricField::BoundingBox(void) const
{
    return _domain_size;
}

int MetricField::Inside(const Sample & sample) const
{
    return _domain.Inside(sample);
}

int MetricField::LocateCell(const Sample & sample, const float cell_size, vector<int> & index)
{
    if(index.size() != sample.coordinate.Dimension())
    {
        index = vector<int>(sample.coordinate.Dimension());
    }

    for(unsigned int i = 0; i < index.size(); i++)
    {
        index[i] = static_cast<int>(floor(sample.coordinate[i]/cell_size));
    }

    return 1;
}
