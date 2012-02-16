/*
  Utility.hpp

  Li-Yi Wei
  08/26/2008
  
*/

#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <string>
using namespace std;

#include "DistanceField.hpp"
#include "SampledDistanceField.hpp"
#include "Array.hpp"

class Utility
{
public:
    // sample IO
    static int ReadSamples(const int dimension, const string & input_file_name, vector<Sample> & samples);

    static int WriteSamples(const vector<Sample> & samples, const string & output_file_name);

    enum SampleMode {SAMPLE_ASCII, SAMPLE_BINARY};

    static int ReadSamples(const SampleMode & mode, const int dimension, const string & input_file_name, vector<Sample> & samples);

    static int WriteSamples(const SampleMode & mode, const vector<Sample> & samples, const string & output_file_name);

    // accounting
    static int CountSamples(const vector<Sample> & samples, vector<int> & num_samples_per_class);

    // distance field
    static const DistanceField * GetDistanceField(const vector<float> & domain_size, const vector<float> & min_distances, const string & importance_file_name);

    static const SampledDistanceField * GetDistanceField(const vector<float> & domain_size, const vector<float> & min_distances, const vector<string> & importance_file_names);

    // normalize the r values so that they take into account the average intensity of the importance images; in othe words, the input r values are assumed to be applied to importance images with identical averages
    static int NormalizeRValues(const vector<float> & domain_size, const vector<string> & importance_file_names, const vector<float> & input_min_distances, vector<float> & output_min_distances);

protected:
    static int ReadDistanceField(const int dimension, const string & distance_field_file_name, vector< Array<float> > & distance_field_data);
};

#endif

