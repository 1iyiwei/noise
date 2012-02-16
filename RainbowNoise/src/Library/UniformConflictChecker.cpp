/*
  UniformConflictChecker.cpp

  Li-Yi Wei
  04/26/2008

*/

#include "UniformConflictChecker.hpp"
#include "Exception.hpp"

UniformConflictChecker::UniformConflictChecker(const Array<float> & r_matrix) : _r_matrix(r_matrix), _max_r_value(0)
{
    // make sure the matrix is 2D
    if(r_matrix.Dimension() != 2)
    {
        throw Exception("r_matrix not 2D");
    }

    // find maximum value
    {
        vector<float> values;
        r_matrix.Get(values);

        for(unsigned int i = 0; i < values.size(); i++)
        {
            if(values[i] > _max_r_value)
            {
                _max_r_value = values[i];
            }
        }
    }
}

UniformConflictChecker::Status UniformConflictChecker::Check(const Sample & s1, const Sample & s2) const
{
    // vector<int> query(2);
    vector<int> & query = _query;
    query[0] = s1.id; query[1] = s2.id;

    float required_distance;

    if(! _r_matrix.Get(query, required_distance)) 
    {
        return CHECK_ERROR;
    }

    const float distance = Distance(s1, s2);

    if(distance < required_distance)
    {
        return CHECK_FAIL;
    }
    else
    {
        return CHECK_PASS;
    }
}

float UniformConflictChecker::MaxDistance(void) const
{
    return _max_r_value;
}
