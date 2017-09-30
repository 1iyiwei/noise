/*
  JacobianField.cpp

  Li-Yi Wei
  10/15/2010

*/

#include <math.h>

#include "JacobianField.hpp"
#include "Math.hpp"
#include "Exception.hpp"

JacobianField::JacobianField(const Domain & domain) : MetricField(domain)
{
    // nothing else to do
}

JacobianField::~JacobianField(void)
{
    // nothing else to do
}

int JacobianField::Forward(const Sample & sample, Matrix & jacobian) const
{
    // shouldn't be called
    throw Exception("JacobianField::Forward() should not be called");
    return 0;
}

int JacobianField::Inverse(const Sample & sample, Matrix & jacobian) const
{
    // shouldn't be called
    throw Exception("JacobianField::Inverse() should not be called");
    return 0;
}

float JacobianField::ForwardMeanEigenValue(void) const
{
    return -1;
}

float JacobianField::ForwardMaxEigenValue(void) const
{
    return -1;
}

float JacobianField::ForwardMinEigenValue(void) const
{
    return -1;
}

float JacobianField::InverseMeanEigenValue(void) const
{
    return -1;
}

float JacobianField::InverseMaxEigenValue(void) const
{
    return -1;
}

float JacobianField::InverseMinEigenValue(void) const
{
    return -1;
}

float JacobianField::Ratio(const Option option, const float r_value, const float mean_eigen_value)
{
    if((r_value <= 0) || (mean_eigen_value <= 0)) throw Exception("JacobianField::Ratio(): illegal inputs");

    float ratio = 0;

    if(option == FORWARD)
    {
        ratio = r_value/mean_eigen_value;
    }
    else if(option == INVERSE)
    {
        ratio = 1.0/(mean_eigen_value*r_value);
    }
    else
    {
        throw Exception("JacobianField::Ratio(): unknown option");
    }

    return ratio;
}

int JacobianField::EigenComputation(const Option option, const int dimension, const vector<Matrix> & jacobians, float & mean_eigen_value, float & max_eigen_value, float & min_eigen_value)
{
    mean_eigen_value = -1;
    max_eigen_value = -1;
    min_eigen_value = Math::INF;

    vector<float> eigen_values;

    float total_importance = 0;
    int total_count = 0;

    Matrix jacobian_transpose, jacobian2;
    for(unsigned int which_jacobian = 0; which_jacobian < jacobians.size(); which_jacobian++)
    {
        const Matrix & jacobian = jacobians[which_jacobian];

        if(! jacobian.Transpose(jacobian_transpose)) return 0;

        if(! jacobian_transpose.Multiply(jacobian, jacobian2)) return 0;
        
        if(! jacobian2.EigenValue(eigen_values))
        {
            throw Exception("JacobianField::EigenComputation(): error in EigenValue");
            return 0;
        }

        for(unsigned int i = 0; i < eigen_values.size(); i++)
        {
            if(eigen_values[i] < 0)
            {
                throw Exception("JacobianField::EigenComputation(): negative eigen value");
                return 0;
            }
            else
            {
                eigen_values[i] = sqrt(eigen_values[i]);
            }
        }

        for(unsigned int i = 0; i < eigen_values.size(); i++)
        {
            if(max_eigen_value < fabs(eigen_values[i]))
            {
                max_eigen_value = fabs(eigen_values[i]);
            }

            if(min_eigen_value > fabs(eigen_values[i]))
            {
                min_eigen_value = fabs(eigen_values[i]);
            }
        }

        const float importance = (option == FORWARD ? 1.0/sqrt(fabs(jacobian2.EigenProduct())) : sqrt(fabs(jacobian2.EigenProduct())));

        total_importance += importance;
        total_count++;
    }

    // mean eigen value
    const float mean_importance = total_importance/total_count;
    
    if(option == FORWARD)
    {
        mean_eigen_value = pow(1.0*mean_importance, -1.0/dimension);
    }
    else
    {
        mean_eigen_value = pow(1.0*mean_importance, 1.0/dimension);
    }

    if(mean_eigen_value <= 0)
    {
        throw Exception("JacobianField::EigenComputation(): mean_eigen_value <= 0");
        return 0;
    }

    // done
    return 1;
}
