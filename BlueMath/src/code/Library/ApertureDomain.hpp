/*
  ApertureDomain.hpp

  the base class for apertures

  Chia-Kai Liang
  09/04/2013

*/

#ifndef _APERTURE_DOMAIN_HPP
#define _APERTURE_DOMAIN_HPP

#include "Domain.hpp"

// CKL XXX: To match the class Domain, we shift the origin of the aperture from
//          [0, 0] to [radius, radius]
enum ApertureType
{
    RectangularAperture = 0,
    CircularAperture,
    HexagonalAperture
};

class ApertureDomain : public Domain
{
public:
    ApertureDomain(float radius, ApertureType type);
    virtual ~ApertureDomain(void);

    int Dimension(void) const
    {
        return 2;
    }

    int Inside(const Sample & query) const;

    int BoundingBox(vector<float> & answer) const;

protected:
    ApertureType _type;
    float _radius;
};

#endif //_APERTURE_DOMAIN_HPP
