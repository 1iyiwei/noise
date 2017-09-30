/*
  SampledImportanceField.hpp

  Li-Yi Wei
  10/13/2010

*/

#ifndef _SAMPLED_IMPORTANCE_FIELD_HPP
#define _SAMPLED_IMPORTANCE_FIELD_HPP

#include "ImportanceField.hpp"
#include "Array.hpp"

class SampledImportanceField : public ImportanceField
{
public:
    SampledImportanceField(const Array<float> & importance, const float cell_size);
    ~SampledImportanceField(void);

    int Dimension(void) const;

    float Query(const Sample & sample) const;

    // the (sampled) data size
    const vector<int> & DataSize(void) const;
    float CellSize(void) const;

    static vector<float> DomainSize(const Array<float> & importance, const float cell_size);

    const Array<float> & Data(void) const;

protected:
    const vector<float> _domain_size;
    Array<float> _data;
    const float _cell_size;
    vector<int> _data_size;
    mutable vector<int> _index; // used in Query to avoid repeated memoc
};

#endif
