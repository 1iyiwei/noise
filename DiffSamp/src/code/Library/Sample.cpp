/*
  Sample.cpp

  Li-Yi Wei
  06/28/2007

*/

#include "Sample.hpp"
#include "Exception.hpp"

Sample::Sample(void) : id(-1), value(1.0), r(0.0)
{
    // nothing to do
}

Sample::Sample(const int dim) : id(-1), value(1.0), r(0.0), coordinate(dim)
{
    // nothing to do
}

Sample::~Sample(void)
{
    // nothing to do
}

int operator==(const Sample & s1, const Sample & s2)
{
    return ((s1.id == s2.id) && (s1.value == s2.value) &&(s1.r == s2.r) && (s1.coordinate == s2.coordinate));
}

int operator!=(const Sample & s1, const Sample & s2)
{
    return !(s1 == s2);
}

float Distance2(const Sample & s1, const Sample & s2)
{
    if(s1.coordinate.Dimension() != s2.coordinate.Dimension())
    {
        throw Exception("Distance2(): samples have different dimensions");
        return -1;
    }
    else
    {
        float sum = 0;
        for(int i = 0; i < s1.coordinate.Dimension(); i++)
        {
            const float diff = s1.coordinate[i] - s2.coordinate[i];
            sum += diff*diff;
        }

        return sum;
    }
}
