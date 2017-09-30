/*
  Filter.hpp

  the base class for all filters

  Li-Yi Wei
  10/27/2010

*/

#ifndef _FILTER_HPP
#define _FILTER_HPP

#include <vector>
using namespace std;

#include "Array.hpp"

class Filter
{
public:
    virtual ~Filter(void) = 0;

    virtual float Query(const Array<float> & image, const vector<int> & index) const;
};

#endif
