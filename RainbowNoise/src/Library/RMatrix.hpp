/*
  RMatrix.hpp

  everything that is related to the construction of RMatrix

  Li-Yi Wei
  05/24/2008

*/

#ifndef _R_MATRIX_HPP
#define _R_MATRIX_HPP

#include <vector>
using namespace std;

#include "Array.hpp"

class RMatrix
{
public:
    typedef enum {UNIFORM, OPTIMAL, FULL, GEOMETRIC} Method;

    // positive num_classes for uniform diagonal entries
    // negative num_classes for priority diagonal entry computation
    // or full r_values specification
    static Array<float> BuildRMatrix(const int dimension, const int num_classes,const Method method, const vector<float> & r_values);

    // similar to above, but specify sample density per class instead of r
    // sample density = number of samples / sample space volume
    // use OPTIMAL method of course
    static Array<float> BuildRMatrix(const int dimension, const Method method, const vector<float> & sample_density);
    static Array<float> BuildRMatrix(const vector<float> & domain_size, const Method method, const vector<int> & num_samples);

protected:
    static float UniformRValue(const int dimension, const vector<float> & r_values);

    struct Sortee
    {
        Sortee(void) : name(0), value(0) {};
        Sortee(const int name_i, const float value_i) : name(name_i), value(value_i) {};

        int operator<(const Sortee & another) const
            {
                return (value < another.value);
            };

        int name;
        float value;
    };
};

class PriorityGroup
{
public:

    PriorityGroup(void) : num_trials(0), target_num_samples(0) {};

    vector<int> classes;
    int num_trials;
    int target_num_samples;
};

vector<PriorityGroup> BuildPriorityGroups(const int dimension, const vector<int> & priority_values, const Array<float> & r_matrix, const int total_num_trials, const int total_target_num_samples);

#endif
