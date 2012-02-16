/*
  SampledDistanceField.hpp

  Li-Yi Wei
  07/15/2007
  08/26/2008: modified for multi-class sampling
*/

#ifndef _SAMPLED_DISTANCE_FIELD
#define _SAMPLED_DISTANCE_FIELD

#include "DistanceField.hpp"
#include "Array.hpp"

class SampledDistanceField : public DistanceField
{
public:
    SampledDistanceField(const vector<float> & domain_size, const vector<float> & average_min_distances, const vector< Array<float> > & importances);
    ~SampledDistanceField(void);

    int NumClasses(void) const; 

    float Query(const Sample & sample) const;

    Boundary BoundaryCondition(const int which_dimension) const;
    
    // class specific functions below

    // the (sampled) data size
    const vector<int> & DataSize(void) const;
    float Query(const int class_id, const vector<int> & index) const;
    float MaxDistance(const int which_class) const;

protected:
    vector< Array<float> > _data;
    vector<int> _data_size;
    vector<float> _max_distances; // per class
    mutable vector<int> _index; // used in Query to avoid repeated memoc
};

#endif
