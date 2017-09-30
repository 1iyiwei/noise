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
    CoordinateT(void) {}

    CoordinateT(const int dim) : _values(dim)
        {
            for(unsigned int i = 0; i < _values.size(); i++)
            {
                _values[i] = -1;
            }
        }

    CoordinateT(const CoordinateT & rhs) : _values(rhs._values)
        {
            // nothing else to do
        }

    CoordinateT & operator=(const CoordinateT & rhs)
        {
            if(this != &rhs)
            {
                _values = rhs._values;
            }

            return *this;
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
    
    void Clear(const T & value)
        {
            for(unsigned int i = 0; i < _values.size(); i++)
            {
                _values[i] = value;
            }
        }

protected:
    vector<T> _values;
};

template<class T>
int operator==(const CoordinateT<T> & s1, const CoordinateT<T> & s2)
{
    if(s1.Dimension() != s2.Dimension()) return 0;

    for(int i = 0; i < s1.Dimension(); i++)
    {
        if(s1[i] != s2[i]) return 0;
    }

    return 1;
}

template<class T>
int operator!=(const CoordinateT<T> & s1, const CoordinateT<T> & s2)
{
    return !(s1 == s2);
}

#endif
