/*
  ColorTransformer.hpp

  the base class for all color transfomers

  Li-Yi Wei
  09/26/2009

*/

#ifndef _COLOR_TRANSFORMER_HPP
#define _COLOR_TRANSFORMER_HPP

#include <vector>
using namespace std;

class ColorTransformer
{
public:
    typedef vector<float> Vector;

    // each basis vector component can be either 0 or a fixed positive value
    ColorTransformer(const int input_dimension, const vector<Vector> & output_basis);
    virtual ~ColorTransformer(void) = 0;

    virtual int Transform(const Vector & input_color, Vector & output_weight) const;

protected:
    const int _input_dimension;
    const vector<Vector> _output_basis;
};

#endif
