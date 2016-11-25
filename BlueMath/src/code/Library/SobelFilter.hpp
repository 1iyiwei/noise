/*
  SobelFilter.hpp

  Li-Yi Wei
  10/27/2010

*/

#ifndef _SOBEL_FILTER_HPP
#define _SOBEL_FILTER_HPP

#include "Filter.hpp"

class SobelFilter : public Filter
{
public:
    
    SobelFilter(void);

    int SetOrientation(const int orientation);

    float Query(const Array<float> & image, const vector<int> & index) const;

protected:
    int _orientation; 
};

#endif
