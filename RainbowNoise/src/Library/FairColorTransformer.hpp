/*
  FairColorTransformer.hpp

  treat the output class as fairly/equally as possible

  Li-Yi Wei
  09/26/2009

*/

#ifndef _FAIR_COLOR_TRANSFORMER_HPP
#define _FAIR_COLOR_TRANSFORMER_HPP

#include "ColorTransformer.hpp"

class FairColorTransformer : public ColorTransformer
{
public:
    FairColorTransformer(const int input_dimension, const vector<Vector> & output_basis);
    virtual ~FairColorTransformer(void);

    int Transform(const Vector & input_color, Vector & output_weight) const;

protected:
    vector<float> _output_weights; // non-zero component for each output basis

    vector< vector<int> > _output_indices;

    struct Sortee
    {
        Sortee(void): value(0), index(-1) {};

        int operator<(const Sortee & rhs) const {return (value < rhs.value);};

        float value;
        int index;
    };

    mutable vector<Sortee> _input_sorted;
};

#endif
