/*
  CosCoverage.hpp

  Li-Yi Wei
  10/15/2013

*/

#ifndef _COS_COVERAGE_HPP
#define _COS_COVERAGE_HPP

#include "VoronoiCoverage.hpp"

class CosCoverage : public VoronoiCoverage
{
public:
    CosCoverage(void);

protected:

    virtual Real TriIntegral(const Point2D & start, const Point2D & end, const Real power) const;

    // compute \int_{start}^{end} |\cos|^{power} with power < 0
    static Real AbsCosIntegralMinus(const Real start, const Real end, const Real power);

    // compute above indefinite integral with negative odd integer power
    static Real CosIntegralMinusOdd(const Real angle, const int power);

    // compute above indefinite integral with negative even integer power
    static Real CosIntegralMinusEven(const Real angle, const int power);

protected:
    static Real Factorial2(Real base, int index);
};

#endif