/*
  WarpJacobianField.hpp

  a Jacoian field derived from an underlying warp

  Li-Yi Wei
  10/15/2010

*/

#ifndef _WARP_JACOBIAN_FIELD
#define _WARP_JACOBIAN_FIELD

#include "JacobianField.hpp"
#include "Warp.hpp"

class WarpJacobianField : public JacobianField
{
public:
    // resolution is for sampling the mean/max eigen values
    WarpJacobianField(const Domain & domain, const Warp & warp, const vector<int> & resolution); 
    WarpJacobianField(const Domain & domain, const Warp & warp, const vector<int> & resolution, const float r_value);
    virtual ~WarpJacobianField(void);

    virtual int Forward(const Sample & sample, Matrix & jacobian) const;
    virtual int Inverse(const Sample & sample, Matrix & jacobian) const;

    virtual float ForwardMeanEigenValue(void) const;
    virtual float ForwardMaxEigenValue(void) const;
    virtual float ForwardMinEigenValue(void) const;

    virtual float InverseMeanEigenValue(void) const;
    virtual float InverseMaxEigenValue(void) const;
    virtual float InverseMinEigenValue(void) const;

protected:
    int CommonConstruct(void);

    float ForwardRatio(void) const;
    float InverseRatio(void) const;

    int EigenComputation(const Option forward_or_inverse, const vector<int> & resolution, float & mean, float & max, float & min) const;
    
protected:
    const Warp & _warp;
    const vector<int> _resolution;
    const float _r_value;

    float _forward_mean_eigen_value;
    float _forward_max_eigen_value;
    float _forward_min_eigen_value;

    float _inverse_mean_eigen_value;
    float _inverse_max_eigen_value;
    float _inverse_min_eigen_value;
};

#endif
