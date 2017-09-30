/*
  DiscreteJacobianField.cpp

  Li-Yi Wei
  10/29/2010

*/

#include "DiscreteJacobianField.hpp"
#include "SampledDistanceField.hpp"
#include "SequentialCounter.hpp"
#include "Utility.hpp"
#include "Exception.hpp"
#include "SobelFilter.hpp"

DiscreteJacobianField::DiscreteJacobianField(const Domain & domain, const float cell_size) : JacobianField(domain), _cell_size(cell_size), _forward_mean_eigen_value(-1), _forward_max_eigen_value(-1), _forward_min_eigen_value(-1), _inverse_mean_eigen_value(-1), _inverse_max_eigen_value(-1), _inverse_min_eigen_value(-1)
{
    // nothing else to do
}

DiscreteJacobianField::~DiscreteJacobianField(void)
{
    // nothing else to do
}

int DiscreteJacobianField::Forward(const Sample & sample, Matrix & jacobian) const
{
    return GetJacobian(_forward_jacobians, sample, jacobian);
}

int DiscreteJacobianField::Inverse(const Sample & sample, Matrix & jacobian) const
{
    return GetJacobian(_inverse_jacobians, sample, jacobian);
}

float DiscreteJacobianField::ForwardMeanEigenValue(void) const
{
    return _forward_mean_eigen_value;
}

float DiscreteJacobianField::ForwardMaxEigenValue(void) const
{
    return _forward_max_eigen_value;
}

float DiscreteJacobianField::ForwardMinEigenValue(void) const
{
    return _forward_min_eigen_value;
}

float DiscreteJacobianField::InverseMeanEigenValue(void) const
{
    return _inverse_mean_eigen_value;
}

float DiscreteJacobianField::InverseMaxEigenValue(void) const
{
    return _inverse_max_eigen_value;
}

float DiscreteJacobianField::InverseMinEigenValue(void) const
{
    return _inverse_min_eigen_value;
}

const vector<int> & DiscreteJacobianField::DataSize(void) const
{
    return _data_size;
}

float DiscreteJacobianField::CellSize(void) const
{
    return _cell_size;
}

vector<float> DiscreteJacobianField::DomainSize(const Array<Matrix> & jacobian_array, const float cell_size)
{
    vector<float> domain_size(jacobian_array.Dimension());

    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = jacobian_array.Size(i)*cell_size;
    }

    return domain_size;
}

void DiscreteJacobianField::CommonConstruct(const Array<Matrix> & inverse_jacobians_in, const float r_value)
{
    inverse_jacobians_in.Size(_data_size);

    _forward_jacobians = _inverse_jacobians = inverse_jacobians_in;

    // compute forward jacobians
    vector<Matrix> inverse_jacobians;
    if(! _inverse_jacobians.Get(inverse_jacobians)) throw Exception("SampledJacobianField::SampledJacobianField(): cannot get inverse jacobians");

    vector<Matrix> forward_jacobians(inverse_jacobians);
    for(unsigned int i = 0; i < forward_jacobians.size(); i++)
    {
        if(! inverse_jacobians[i].Inverse(forward_jacobians[i])) throw Exception("SampledJacobianField::SampledJacobianField(): cannot compute forward jacobian");
    }
    
    // computation
    const int dimension = Dimension();

    if(! EigenComputation(FORWARD, dimension, forward_jacobians, _forward_mean_eigen_value, _forward_max_eigen_value, _forward_min_eigen_value)) throw Exception("SampledJacobianField::SampledJacobianField(): cannot eigen compute forward jacobians");

    if(! EigenComputation(INVERSE, dimension, inverse_jacobians, _inverse_mean_eigen_value, _inverse_max_eigen_value, _inverse_min_eigen_value)) throw Exception("SampledJacobianField::SampledJacobianField(): cannot eigen compute inverse jacobians");

    const float forward_ratio = (r_value > 0 ? Ratio(FORWARD, r_value, _forward_mean_eigen_value) : 1.0);
    const float inverse_ratio = (r_value > 0 ? Ratio(INVERSE, r_value, _inverse_mean_eigen_value) : 1.0);

    for(unsigned int i = 0; i < forward_jacobians.size(); i++)
    {
        forward_jacobians[i].Multiply(forward_ratio);
    }
    _forward_mean_eigen_value *= forward_ratio;
    _forward_max_eigen_value *= forward_ratio;
    _forward_min_eigen_value *= forward_ratio;

    for(unsigned int i = 0; i < inverse_jacobians.size(); i++)
    {
        inverse_jacobians[i].Multiply(inverse_ratio);
    }
    _inverse_mean_eigen_value *= inverse_ratio;
    _inverse_max_eigen_value *= inverse_ratio;
    _inverse_min_eigen_value *= inverse_ratio;

    // put back
    if(! _inverse_jacobians.Put(inverse_jacobians)) throw Exception("SampledJacobianField::SampledJacobianField(): cannot put inverse jacobians");
    if(! _forward_jacobians.Put(forward_jacobians)) throw Exception("SampledJacobianField::SampledJacobianField(): cannot put forward jacobians");
}

int DiscreteJacobianField::GetJacobian(const Array<Matrix> & jacobians, const Sample & sample, Matrix & jacobian) const
{
    if(! _domain.Adopt(sample, _sample))
    {
        throw Exception("DiscreteJacobianField::GetJacobian(): cannot adopt sample");
        return 0;
    }

    if(! LocateCell(_sample, _cell_size, _index))
    {
        throw Exception("DiscreteJacobianField::GetJacobian(): cannot locate cell");
        return 0;
    }

    for(unsigned int i = 0; i < _index.size(); i++)
    {
        if(_index[i] < 0) _index[i] = 0;
        if(_index[i] >= jacobians.Size(i)) _index[i] = jacobians.Size(i)-1;
    }

    if(!jacobians.Get(_index, jacobian))
    {
        throw Exception("DiscreteJacobianField::GetJacobian(): cannot get jacobian");
        return 0;
    }

    return 1;
}
