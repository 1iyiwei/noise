/*
  ShuffleTools.hpp

  Li-Yi Wei
  08/18/2009

*/

#ifndef _SHUFFLE_TOOLS_HPP
#define _SHUFFLE_TOOLS_HPP

#include <vector>
#include <string>
using namespace std;

#include "Domain.hpp"
#include "UniformKernelSize.hpp"

#include "EnergySampleStore.hpp"
#include "UnionEnergySampleTracker.hpp"

class ShuffleTools
{
public:
    // deal with fixed/moveable samples
    // both return 1 if successful, 0 else
    static int FastenClassID(int & id);
    static int LoosenClassID(int & id);

    static EnergySampleStore * BuildEnergySampleStore(const Domain & domain, const vector<float> & domain_size, const UniformKernelSize & kernel_size, const string & energy_store_method, const float kernel_radius_scale);

    static UnionEnergySampleTracker * BuildEnergySampleTracker(const vector<float> & domain_size, const vector<int> & num_samples, const int scale);
};
#endif
