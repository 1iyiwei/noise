/*
  JacobianField.hpp

  the base class of all Jacobian fields

  Li-Yi Wei
  10/15/2010

*/

#ifndef _JACOBIAN_FIELD_HPP
#define _JACOBIAN_FIELD_HPP

#include "MetricField.hpp"
#include "Matrix.hpp"

class JacobianField : public MetricField
{
public:
    // implicitly assuming that the sample domain starts with 0
    // (if not, coordinates can be easily shifted as a pre/post-process)
    JacobianField(const Domain & domain);
    virtual ~JacobianField(void) = 0;

    // return the jacobian value for a sample location
    // return 1 if successful, 0 else
    // note: query sample is on the *forward* space always!
    virtual int Forward(const Sample & sample, Matrix & jacobian) const;
    virtual int Inverse(const Sample & sample, Matrix & jacobian) const;

    // the bounding sphere radius containing all forward/inverse Jacobians
    // return negative for error
    virtual float ForwardMeanEigenValue(void) const;
    virtual float ForwardMaxEigenValue(void) const;
    virtual float ForwardMinEigenValue(void) const;

    virtual float InverseMeanEigenValue(void) const;
    virtual float InverseMaxEigenValue(void) const;
    virtual float InverseMinEigenValue(void) const;

protected:
    
    typedef enum {FORWARD, INVERSE} Option;

    static float Ratio(const Option option, const float r_value, const float mean_eigen_value);

    static int EigenComputation(const Option option, const int dimension, const vector<Matrix> & jacobians, float & mean_eigen_value, float & max_eigen_value, float & min_eigen_value);
};

#endif
