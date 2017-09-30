/*
  ApertureDomain.cpp

  Chia-Kai Liang
  09/04/2013

*/

#include "ApertureDomain.hpp"
#include "Math.hpp"
#include "Exception.hpp"

ApertureDomain::ApertureDomain(float radius, ApertureType type):
    _radius(radius), _type(type)
{
}

ApertureDomain::~ApertureDomain(void)
{
}

int ApertureDomain::Inside(const Sample & query) const
{
    CoordinateType u,v;
    if(2 == query.coordinate.Dimension())
    {
        u = query.coordinate[0];
        v = query.coordinate[1];
    }
    else if (4 == query.coordinate.Dimension())
    {
        u = query.coordinate[2];
        v = query.coordinate[3];
    }
    else
    {
        throw Exception("ApertureDomain::Inside(): dimension mismatch");
    }

    float uShifted = u - _radius;
    float vShifted = v - _radius;

    switch (_type)
    {
    case RectangularAperture:
        if ((abs(uShifted) <= _radius) && (abs(vShifted) <= _radius))
        {
            return 1;
        }
        break;
    case CircularAperture:
        if (uShifted*uShifted + vShifted*vShifted <= _radius*_radius)
        {
            return 1;
        }
        break;
    case HexagonalAperture:
        {
            float uShiftedAbs = abs(uShifted);
            float vShiftedAbs = abs(vShifted);
            if (vShiftedAbs < _radius*0.5f && uShiftedAbs < _radius*Math::Sqrt3()*0.5f)
            {
                return 1;
            }
            else if (uShiftedAbs*Math::RecipSqrt3() + vShiftedAbs< _radius)
            {
                return 1;
            }
        }
        break;
    default:
        throw Exception("ApertureDomain::Inside(): unknown aperture type");
    }
    return 0;
}

int ApertureDomain::BoundingBox(vector<float> & answer) const
{
    answer.push_back(2*_radius);
    answer.push_back(2*_radius);
    return 1;
}
