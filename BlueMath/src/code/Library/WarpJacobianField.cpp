/*
  WarpJacobianField.cpp

  Li-Yi Wei
  10/15/2010

*/

#include <math.h>

#include "WarpJacobianField.hpp"
#include "Math.hpp"
#include "SequentialCounter.hpp"
#include "Exception.hpp"

WarpJacobianField::WarpJacobianField(const Domain & domain, const Warp & warp, const vector<int> & resolution) : JacobianField(domain), _warp(warp), _resolution(resolution), _forward_mean_eigen_value(-1), _forward_max_eigen_value(-1), _forward_min_eigen_value(-1), _inverse_mean_eigen_value(-1), _inverse_max_eigen_value(-1), _inverse_min_eigen_value(-1), _r_value(0)
{
    if(! CommonConstruct())
    {
        throw Exception("WarpJacobianField::WarpJacobianField(): error!");
    }
}

WarpJacobianField::WarpJacobianField(const Domain & domain, const Warp & warp, const vector<int> & resolution, const float r_value) : JacobianField(domain), _warp(warp), _resolution(resolution), _forward_mean_eigen_value(-1), _forward_max_eigen_value(-1), _forward_min_eigen_value(-1), _inverse_mean_eigen_value(-1), _inverse_max_eigen_value(-1), _inverse_min_eigen_value(-1), _r_value(r_value)
{
    if(! CommonConstruct())
    {
        throw Exception("WarpJacobianField::WarpJacobianField(): error!");
    }
}

WarpJacobianField::~WarpJacobianField(void)
{
    // nothing else to do
}

int WarpJacobianField::Forward(const Sample & sample, Matrix & jacobian) const
{
    if(!_warp.ForwardJacobian(sample.coordinate, jacobian)) return 0;

    return jacobian.Multiply(ForwardRatio());
}

int WarpJacobianField::Inverse(const Sample & sample, Matrix & jacobian) const
{
    if(! _warp.InverseJacobian(sample.coordinate, jacobian)) return 0;
    
    return jacobian.Multiply(InverseRatio());
}

float WarpJacobianField::ForwardMeanEigenValue(void) const
{
    return _forward_mean_eigen_value*ForwardRatio();
}

float WarpJacobianField::ForwardMaxEigenValue(void) const
{
    return _forward_max_eigen_value*ForwardRatio();
}

float WarpJacobianField::ForwardMinEigenValue(void) const
{
    return _forward_min_eigen_value*ForwardRatio();
}

float WarpJacobianField::InverseMeanEigenValue(void) const
{
    return _inverse_mean_eigen_value*InverseRatio();
}

float WarpJacobianField::InverseMaxEigenValue(void) const
{
    return _inverse_max_eigen_value*InverseRatio();
}

float WarpJacobianField::InverseMinEigenValue(void) const
{
    return _inverse_min_eigen_value*InverseRatio();
}

int WarpJacobianField::CommonConstruct(void)
{
    if(! EigenComputation(INVERSE, _resolution, _inverse_mean_eigen_value, _inverse_max_eigen_value, _inverse_min_eigen_value))
    {
        return 0;
    }

    if(! EigenComputation(FORWARD, _resolution, _forward_mean_eigen_value, _forward_max_eigen_value, _forward_min_eigen_value))
    {
        return 0;
    }

    return 1;
}

float WarpJacobianField::ForwardRatio(void) const
{
    return (_r_value > 0 ? Ratio(FORWARD, _r_value, _forward_mean_eigen_value) : 1.0);
}

float WarpJacobianField::InverseRatio(void) const
{
    return (_r_value > 0 ? Ratio(INVERSE, _r_value, _inverse_mean_eigen_value) : 1.0);
}

int WarpJacobianField::EigenComputation(const Option option, const vector<int> & resolution, float & mean_eigen_value, float & max_eigen_value, float & min_eigen_value) const
{
    // simply feed sample a bunch of jacobians and find the largest singular value magnitude?

    vector<Matrix> jacobians;

    vector<float> cell_size(resolution.size());
    for(unsigned int i = 0; i < cell_size.size(); i++)
    {
        cell_size[i] = _domain_size[i]/resolution[i];
    }

    SequentialCounter counter(resolution.size(), vector<int>(resolution.size(), 0), resolution);

    mean_eigen_value = -1;
    max_eigen_value = -1;
    min_eigen_value = Math::INF;

    vector<float> eigen_values;
    vector<int> index(Dimension());
    Sample sample(Dimension());
    Matrix jacobian;

    float total_importance = 0;
    int total_count = 0;

    counter.Reset();
    do
    {
        counter.Get(index);

        if(sample.coordinate.Dimension() != index.size())
        {
            sample.coordinate = Coordinate(index.size());
        }

        for(int i = 0; i < sample.coordinate.Dimension(); i++)
        {
            sample.coordinate[i] = cell_size[i]*index[i];
        }

        if(option == FORWARD)
        {
            if(! _warp.ForwardJacobian(sample.coordinate, jacobian))
            {
                return 0;
            }
        }
        else
        {
            if(! _warp.InverseJacobian(sample.coordinate, jacobian))
            {
                return 0;
            }
        }

        jacobians.push_back(jacobian);

    }
    while(counter.Next());

    // done
    return JacobianField::EigenComputation(option, Dimension(), jacobians, mean_eigen_value, max_eigen_value, min_eigen_value);
}
