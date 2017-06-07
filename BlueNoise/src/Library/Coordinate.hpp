/*
  Coordinate.hpp

  Li-Yi Wei

  06/28/2007
*/

#ifndef _COORDINATE_HPP
#define _COORDINATE_HPP

#include <vector>
using namespace std;

#include <assert.h>

template <class T>
class CoordinateT
{
public:
    CoordinateT(const int dim) : _values(dim)
        {
            for(unsigned int i = 0; i < _values.size(); i++)
            {
                _values[i] = -1;
            }
        }

    int Dimension(void) const
        {
            return _values.size();
        }
    
    T & operator[](const int index)
        {
            assert((index >= 0) && (static_cast<unsigned int>(index) < _values.size()));
            return _values[index];
        }
    
    T operator[](const int index) const
        {
            assert((index >= 0) && (static_cast<unsigned int>(index) < _values.size()));
            return _values[index];
        }
    
protected:
    vector<T> _values;
};

#endif
