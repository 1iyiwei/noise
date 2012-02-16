/*
  ShuffleTools.cpp

  Li-Yi Wei
  08/18/2009

*/

#include <iostream>
using namespace std;

#include <math.h>

#include "ShuffleTools.hpp"

#include "BagEnergySampleStore.hpp"
#include "GridEnergySampleStore.hpp"

#include "GridEnergySampleTracker.hpp"

#include "Exception.hpp"

int ShuffleTools::FastenClassID(int & id)
{
    if(id >= 0)
    {
        id = -id-2;
    }

    return (id < -1);
}

int ShuffleTools::LoosenClassID(int & id)
{
    if(id < -1)
    {
        id = -id-2;
    }
    
    return (id >= 0);
}

EnergySampleStore * ShuffleTools::BuildEnergySampleStore(const Domain & domain, const vector<float> & domain_size, const UniformKernelSize & kernel_size, const string & energy_store_method, const float kernel_radius_scale)
{
    EnergySampleStore * p_energy_store = 0;

    const string grid_name = "grid";

    if(energy_store_method.find(grid_name) == 0)
    {
        const int grid_size = atoi(energy_store_method.substr(grid_name.length(), energy_store_method.length()).c_str());

        GridEnergySampleStore::GridSpec grid_spec;
        grid_spec.cell_size = 1.0/grid_size;
        grid_spec.domain_size = vector<int>(domain_size.size());
        for(unsigned int i = 0; i < grid_spec.domain_size.size(); i++)
        {
            grid_spec.domain_size[i] = static_cast<int>(ceil(domain_size[i]/grid_spec.cell_size));
        }

        p_energy_store = new GridEnergySampleStore(domain, grid_spec, kernel_size, kernel_radius_scale);
    }
    else
    {
        p_energy_store = new BagEnergySampleStore;
    }

    return p_energy_store;
}

UnionEnergySampleTracker * ShuffleTools::BuildEnergySampleTracker(const vector<float> & domain_size, const vector<int> & num_samples, const int scale)
{
    const int dimension = domain_size.size();
    const int num_class = num_samples.size();

    if(dimension <= 0)
    {
        return 0;
    }

    // handle only cubic domain now
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        if(domain_size[i] != domain_size[(i+1)%domain_size.size()])
        {
            throw Exception("ShuffleTools::BuildEnergySampleTracker(): can only handle cubical domain now");
            return 0;
        }
    }

    vector<UnionEnergySampleTracker::Member> trackers;
    UnionEnergySampleTracker::Member member;

    int last_num_samples = -1;

    for(unsigned int i = 0; i < num_samples.size(); i++)
    {
        if(num_samples[i] != last_num_samples)
        {
            member.class_id_min = member.class_id_max = i;
            trackers.push_back(member);
        }

        if(trackers.size() > 0)
        {
            trackers[trackers.size()-1].class_id_max = i;
        }

        last_num_samples = num_samples[i];
    }

    for(unsigned int i = 0; i < trackers.size(); i++)
    {         
        UnionEnergySampleTracker::Member & member = trackers[i];

        GridEnergySampleTracker::GridSpec spec;
        spec.domain_size = GridEnergySampleTracker::GridSize(dimension, num_samples[member.class_id_min], scale);
        spec.cell_size = domain_size[0]/spec.domain_size[0];

        member.p_tracker = new GridEnergySampleTracker(spec, member.class_id_min, member.class_id_max);

#if 0 // debug
        {
            cerr << "tracker[" << i << "]: " << member.class_id_min << " " << member.class_id_max << " " << spec.cell_size << " (";
            for(unsigned int j = 0; j < spec.domain_size.size(); j++)
            {
                cerr << spec.domain_size[j];
                if((j+1) < spec.domain_size.size()) cerr << " ";
            }
            cerr << ")" << endl;
        }
#endif

    }

    return new UnionEnergySampleTracker(trackers);
}
