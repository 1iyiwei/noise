/*
  CosCoverage.cpp

  Li-Yi Wei
  10/15/2013

*/

#include <cmath>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;

#include "CosCoverage.hpp"
#include "Math.hpp"
#include "Math2D.hpp"
#include "Exception.hpp"

CosCoverage::CosCoverage(void)
{
    throw Exception("CosCoverage::CosCoverage(): this class is not really working!");
}

CosCoverage::Real CosCoverage::TriIntegral(const Point2D & start, const Point2D & end, const Real power) const
{
    Real answer = 0;

    if(power < 0)
    {
        throw Exception("CosCoverage::TriIntegral(): negative power!");
        answer = -1; // error
    }
    else
    {
        const Real angle_start = Math2D::Angle(start);
        const Real angle_end = Math2D::Angle(end);
        const Real angle_phase = Math2D::Angle(start-end);

        const Real power_plus_2 = power + 2.0;

        const Real distance = Distance(start, end);

        if(distance <= 0)
        {
            answer = 0;
#if 0
            stringstream message;
            message << "(" << start.x << ", " << start.y << ") (" << end.x << ", " << end.y << ")";

            throw Exception("CosCoverage::TriIntegral(): Distance(start, end) <= 0 : " + message.str());
#endif
        }
        else
        {
            answer = 1.0/(power_plus_2)*pow(fabs(-start.x*end.y + end.x*start.y)/distance, power_plus_2)*fabs(AbsCosIntegralMinus(angle_start + angle_phase, angle_end + angle_phase, -power_plus_2));

            // cerr << "(" << start.x << ", " << start.y << ") (" << end.x << ", " << end.y << ")" << " " << angle_start << " " << angle_end << " " << angle_phase << " -> " << answer << endl;
        }
    }

    return answer;
}

CosCoverage::Real CosCoverage::AbsCosIntegralMinus(const Real start, const Real end, const Real power)
{
    Real answer = 0;

    const int int_power = static_cast<int>(floor(power));
    
    if(static_cast<Real>(int_power) != power)
    {
        throw Exception("CosCoverage::AbsCosIntegralMinus(): non integer power!");

        answer = -1; // only supports integer power now
    }
    else
    {
        if(int_power%2)
        {
            throw Exception("CosCoverage::AbsCosIntegralMinus(): odd power!");

            answer = -1; // only supports even integer power now

            // divide the triangle further according to quadrants
            // and call each with CosIntegralMinusOdd()
        }
        else
        {
            answer = CosIntegralMinusEven(end, int_power) - CosIntegralMinusEven(start, int_power);
        }
    }

    return answer;
}

CosCoverage::Real CosCoverage::CosIntegralMinusOdd(const Real angle, const int power)
{
    Real answer = 0;

    if((power >= 0) || (power%2 == 0))
    {
        // error

        throw Exception("CosCoverage::CosIntegralMinusOdd(): positive even power!");

        answer = -1;
    }
    else
    {
        answer = 0;
        
        for(int k = 0; k <= (-power-3)/2; k++)
        {
            const int power_2k_1 = power + 2*k + 1;
            answer += Factorial2(power, k)/Factorial2(power-1, k)*pow(cos(angle), 1.0*power_2k_1)/power_2k_1;
        }

        answer *= -sin(angle);

        answer += Factorial2(power, (-power-1)/2)/Factorial2(power-1, (-power-1)/2)*log(fabs(tan(angle) + 1.0/cos(angle)));
    }

    return answer;
}

CosCoverage::Real CosCoverage::CosIntegralMinusEven(const Real angle, const int power)
{
    Real answer = 0;

    if((power >= 0) || (power%2))
    {
        // error

        throw Exception("CosCoverage::CosIntegralMinusEven(): positive odd power!");

        answer = -1;
    }
    else
    {
        answer = 0;
        
        for(int k = 0; k <= -power/2 - 1; k++)
        {
            const int power_2k_1 = power + 2*k + 1;
            answer += Factorial2(power, k)/Factorial2(power-1, k)*pow(cos(angle), 1.0*power_2k_1)/power_2k_1;
        }

        answer *= -sin(angle);
    }

    return answer;
}

CosCoverage::Real CosCoverage::Factorial2(Real base, int index)
{
    Real answer = 1.0;

    for(int i = 1; i <= index; i++)
    {
        answer *= (base + 2*i);
    }

    return answer;
}