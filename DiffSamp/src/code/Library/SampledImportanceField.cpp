/*
  SampledImportanceField.cpp

  Li-Yi Wei
  10/13/2010

*/

#include <math.h>

#include "SampledImportanceField.hpp"
#include "Math.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

SampledImportanceField::SampledImportanceField(const Array<float> & importance, const float cell_size): _domain_size(DomainSize(importance, cell_size)), _data(importance), _cell_size(cell_size), _data_size(importance.Dimension()), _index(importance.Dimension())
{
    importance.Size(_data_size);
}

SampledImportanceField::~SampledImportanceField(void)
{
    // nothing else to do
}

int SampledImportanceField::Dimension(void) const
{
    return _data.Dimension();
}

float SampledImportanceField::Query(const Sample & sample) const
{
    if(sample.coordinate.Dimension() != _data.Dimension()) throw Exception("SampledImportanceField::Query(): dimension mismatch");

    assert(sample.coordinate.Dimension() == _index.size());
    
    const Array<float> & data = _data;

    for(unsigned int i = 0; i < _index.size(); i++)
    {
        _index[i] = static_cast<int>(floor((sample.coordinate[i])/(_domain_size[i])*(data.Size(i)/*-1*/)/*+0.5*/));
        
        if(_index[i] < 0) _index[i] = 0;
        if(_index[i] >= data.Size(i)) _index[i] = data.Size(i)-1;
    }

    float answer = -1;
    if(!data.Get(_index, answer)) throw Exception("SampledDistanceField::Query() : cannot get data");
    
    return answer;
}

const vector<int> & SampledImportanceField::DataSize(void) const
{
    return _data_size;
}

float SampledImportanceField::CellSize(void) const
{
    return _cell_size;
}

vector<float> SampledImportanceField::DomainSize(const Array<float> & importance, const float cell_size)
{
    vector<float> domain_size(importance.Dimension());

    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = importance.Size(i)*cell_size;
    }

    return domain_size;
}

const Array<float> & SampledImportanceField::Data(void) const
{
    return _data;
}
