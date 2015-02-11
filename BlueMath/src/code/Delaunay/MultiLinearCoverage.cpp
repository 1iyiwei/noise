/*
  MultiLinearCoverage.cpp

  Li-Yi Wei
  11/07/2013

*/

#include "MultiLinearCoverage.hpp"
#include "Math.hpp"
#include "Math2D.hpp"
#include "Exception.hpp"

MultiLinearCoverage::Real MultiLinearCoverage::TriIntegral(const Point2D & start, const Point2D & end, const Real power) const
{
    Real answer = 0;

    if(power < 0)
    {
        throw Exception("MultiLinearCoverage::TriIntegral(): negative power!");
        answer = -1; // error
    }
    else
    {
        const int int_power = static_cast<int>(floor(power));
        
        if(static_cast<Real>(int_power) != power)
        {
            throw Exception("MultiLinearCoverage::TriIntegral(): non-integer power!");
        }

        answer = fabs(TriIntegralPlusEven(start, end, int_power));
    }

    return answer;
}

MultiLinearCoverage::Real MultiLinearCoverage::TriIntegralPlusEven(const Point2D & start, const Point2D & end, const int int_power)
{
    Real answer = 0;

    if(int_power < 0)
    {
        throw Exception("MultiLinearCoverage::TriIntegralPlusEven(): negative power!");
        answer = -1;
    }
    else if(int_power%2)
    {
        throw Exception("MultiLinearCoverage::TriIntegralPlusEven(): odd power!");
        answer = -1;
    }
    else
    {
        answer = 0;

        const int power_half = int_power/2;

        for(int k = 0; k <= power_half; k++)
        {
            const Real c_powerhalf_k = Math::Choose(power_half, k);

            const int k2 = 2*k;

            for(int m = 0; m <= k2; m++)
            {
                const Real c_k2_m = Math::Choose(k2, m);

                const int powerhalf_minus_k_2 = 2*(power_half-k);

                for(int l = 0; l <= powerhalf_minus_k_2; l++)
                {
                    const Real c_powerhalf_minus_k_2_l = Math::Choose(powerhalf_minus_k_2, l);

                    const int m_plus_l = m + l;
                    const int k2_minus_m_plus_powerhalf_minus_k_2_minus_l = k2 - m + powerhalf_minus_k_2 - l;

                    answer += c_powerhalf_k*c_k2_m*c_powerhalf_minus_k_2_l*pow(start.x, m*1.0)*pow(start.y, l*1.0)*pow(end.x, (k2-m)*1.0)*pow(end.y, (powerhalf_minus_k_2-l)*1.0)*Math::Factorial(m_plus_l)*Math::Factorial(k2_minus_m_plus_powerhalf_minus_k_2_minus_l)/Math::Factorial(2 + m_plus_l + k2_minus_m_plus_powerhalf_minus_k_2_minus_l);
                }
            }
        }

        answer *= CrossProduct(start, end);
    }

    return answer;
}

MultiLinearCoverage::Vector2D MultiLinearCoverage::TriIntegralPlusEvenGradient(const Point2D & start, const Point2D & end, const int int_power)
{
    Vector2D subGradient0;
    Vector2D subGradient1;
    Real integral = TriIntegralPlusEven(start, end, int_power);
    const Real det = CrossProduct(start, end);
    if(int_power < 0)
    {
        throw Exception("MultiLinearCoverage::TriIntegralPlusEvenGradient(): negative power!");
    }
    else if(int_power%2)
    {
        throw Exception("MultiLinearCoverage::TriIntegralPlusEvenGradient(): odd power!");
    }
    else
    {
        const int power_half = int_power/2;
        for(int k = 0; k <= power_half; k++)
        {
            const Real c_powerhalf_k = Math::Choose(power_half, k);
            const int k2 = 2*k;
            for(int m = 0; m <= k2; m++)
            {
                const Real c_k2_m = Math::Choose(k2, m);

                const int powerhalf_minus_k_2 = 2*(power_half-k);

                for(int l = 0; l <= powerhalf_minus_k_2; l++)
                {
                    const Real c_powerhalf_minus_k_2_l = Math::Choose(powerhalf_minus_k_2, l);

                    const int m_plus_l = m + l;
                    const int k2_minus_m_plus_powerhalf_minus_k_2_minus_l = k2 - m + powerhalf_minus_k_2 - l;

                    const Real scalar = c_powerhalf_k*c_k2_m*c_powerhalf_minus_k_2_l*Math::Factorial(m_plus_l)*Math::Factorial(k2_minus_m_plus_powerhalf_minus_k_2_minus_l)/Math::Factorial(2 + m_plus_l + k2_minus_m_plus_powerhalf_minus_k_2_minus_l);

                    const Real p0 = m;
                    const Real p1 = l;
                    const Real p2 = k2-m;
                    const Real p3 = powerhalf_minus_k_2-l;

                    // integral is pow(start.x, p0)*pow(start.y, p1)*pow(end.x, p2)*pow(end.y, p3)*scalar;
                    if (p0 > 0)
                    {
                        subGradient1.x += -p0 * pow(start.x, p0-1.0)*pow(start.y, p1)*pow(end.x, p2)*pow(end.y, p3)*scalar;
                    }
                    if (p2 > 0)
                    {
                        subGradient1.x += -p2 * pow(start.x, p0)*pow(start.y, p1)*pow(end.x, p2-1.0)*pow(end.y, p3)*scalar;
                    }
                    if (p1 > 0)
                    {
                        subGradient1.y += -p1 * pow(start.x, p0)*pow(start.y, p1-1.0)*pow(end.x, p2)*pow(end.y, p3)*scalar;
                    }
                    if (p3 > 0)
                    {
                        subGradient1.y += -p3 * pow(start.x, p0)*pow(start.y, p1)*pow(end.x, p2)*pow(end.y, p3-1.0)*scalar;
                    }
                }
            }
        }
    }
    subGradient0.x = integral*(-end.y + start.y);
    subGradient0.y = integral*(-start.x + end.x);
    subGradient1.x *= det;
    subGradient1.y *= det;
    return subGradient0 + subGradient1;
}

MultiLinearCoverage::Vector2D MultiLinearCoverage::TriIntegralGradient(const Point2D & start, const Point2D & end, const Real power) const
{
    Vector2D gradient;

    if(power < 0)
    {
        throw Exception("MultiLinearCoverage::TriIntegralGradient(): negative power!");
    }
    else
    {
        const int int_power = static_cast<int>(floor(power));

        if(static_cast<Real>(int_power) != power)
        {
            throw Exception("MultiLinearCoverage::TriIntegralGradient(): non-integer power!");
        }

        gradient = TriIntegralPlusEvenGradient(start, end, int_power);
    }
    return gradient;
}
