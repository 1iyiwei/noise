/*
  EnergySample.hpp

  sample + energy value

  Li-Yi Wei
  07/07/2009

*/

#ifndef _ENERGY_SAMPLE_HPP
#define _ENERGY_SAMPLE_HPP

#include "Sample.hpp"

// #define _ENERGY_MULTI_VALUE // potential bug

class EnergySample
{
public:
    EnergySample(void);
    EnergySample(const int num_class);
    ~EnergySample(void);

    int NumClass(void) const;
    int NumClass(const int num_class);

    int GetSample(Sample & sample) const;
    int SetSample(const Sample & sample);
    int SetSample(void); // remote/empty the sample

    int GetValue(const int class_id, float & value) const;
    int SetValue(const int class_id, const float value);
    int GetValue(float & value) const;
    int SetValue(const float value);

    // the sample is visible or not in terms of energy impact
    bool Visible(void) const;
    void Visible(const bool value);

    // is the sample fixed and thus non-moveable?
    bool Fixed(void) const;
    void Fixed(const bool value);

    // protected: // dangerous functions don't use
    float GetValue(const int class_id) const;
    float GetValue(void) const;

protected:
    Sample _sample;
    float _value;
    vector<float> _values; // one value per (hypothetical) class id for _sample; useful for points
    bool _empty;
    bool _visibility;
    bool _fixed; // moveable or not

    static const float _DEFAULT_ENERGY_VALUE;
};

#endif
