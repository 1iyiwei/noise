/*
  DiscreteJacobianField.hpp

  the base class for all sampled Jacobian fields

  Li-Yi Wei
  10/29/2010

*/

#ifndef _DISCRETE_JACOBIAN_FIELD_HPP
#define _DISCRETE_JACOBIAN_FIELD_HPP

#include "JacobianField.hpp"
#include "Matrix.hpp"

class DiscreteJacobianField : public JacobianField
{
public:
    DiscreteJacobianField(const Domain & domain, const float cell_size);
    virtual ~DiscreteJacobianField(void) = 0;

    virtual int Forward(const Sample & sample, Matrix & jacobian) const;
    virtual int Inverse(const Sample & sample, Matrix & jacobian) const;

    virtual float ForwardMeanEigenValue(void) const;
    virtual float ForwardMaxEigenValue(void) const;
    virtual float ForwardMinEigenValue(void) const;

    virtual float InverseMeanEigenValue(void) const;
    virtual float InverseMaxEigenValue(void) const;
    virtual float InverseMinEigenValue(void) const;

    // class specific functions below

    // the (sampled) data size
    const vector<int> & DataSize(void) const;
    float CellSize(void) const;

    static vector<float> DomainSize(const Array<Matrix> & jacobian_array, const float cell_size);

protected:
    void CommonConstruct(const Array<Matrix> & inverse_jacobians, const float r_value);

    int GetJacobian(const Array<Matrix> & jacobians, const Sample & sample, Matrix & jacobian) const;

protected:
    Array<Matrix> _forward_jacobians;
    Array<Matrix> _inverse_jacobians;
    vector<int> _data_size;

    const float _cell_size;

    float _forward_mean_eigen_value;
    float _forward_max_eigen_value;
    float _forward_min_eigen_value;

    float _inverse_mean_eigen_value;
    float _inverse_max_eigen_value;
    float _inverse_min_eigen_value;

    mutable Sample _sample;
    mutable vector<int> _index;
};

#endif
