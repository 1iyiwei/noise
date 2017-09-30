/*
  SobelFilter.cpp

  Li-Yi Wei
  10/27/2010

*/

#include "SobelFilter.hpp"
#include "Exception.hpp"

SobelFilter::SobelFilter(void) : _orientation(-1)
{
    // nothing else to do
}

int SobelFilter::SetOrientation(const int orientation)
{
    if(orientation < 0) return 0;

    _orientation = orientation;
    return 1;
}

float SobelFilter::Query(const Array<float> & image, const vector<int> & query) const
{
    if(image.Dimension() != query.size())
    {
        throw Exception("SobelFilter::Query(): dimension mismatch");
    }

    if((_orientation < 0) || (_orientation >= image.Dimension()))
    {
        throw Exception("SobelFilter::Query(): illegal orientation");
    }

    if(image.Dimension() != 2)
    {
        throw Exception("SobelFilter::Query(): support only 2D image now!");
    }

    float output = 0;
    float value = 0;
    vector<int> index(query);

    // the following code handles 2D only!
    for(int i = -1; i <= 1; i+=2)
        for(int j = -1; j <= 1; j++)
        {
            index[_orientation] = i + query[_orientation];
            index[1-_orientation] = j + query[1-_orientation];
            
            const float weight = i*(1 + (j==0));

            if(image.Get(index, value))
            {
                output += value*weight;
            }
        }
    
    return output;
}
