/*
  DiffScatter.cpp

  Li-Yi Wei
  09/05/2010

*/

#include <math.h>

#include "DiffScatter.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "OnionLayerCounter.hpp"
#include "SequentialCounter.hpp"

DiffScatter::Answer DiffScatter::Run(const vector<Diff> & diffs, const DiffKernel & kernel, const float cell_size, const float query_scale, Array<float> & output) const
{
    Answer answer;

    // output.Clear(0); // not reset to allow multiple pass

    vector<int> output_size;
    output.Size(output_size);

    vector<int> center_index(output_size);
    for(unsigned int i = 0; i < center_index.size(); i++)
    {
        center_index[i] = output_size[i]/2;
    }

    const float cell_radius = kernel.GetRadius()/cell_size + 1;

    const bool infinite_kernel = Math::IsINF(kernel.GetRadius());

    Counter * p_counter = 0;

    if(infinite_kernel)
    {
        vector<int> counter_max(output_size);
        for(unsigned int i = 0; i < counter_max.size(); i++)
        {
            counter_max[i] = output_size[i]-1;
        }

        p_counter = new SequentialCounter(output.Dimension(), vector<int>(output.Dimension(), 0), counter_max);
    }
    else
    {
        p_counter = new OnionLayerCounter(output.Dimension(), cell_radius*cell_radius);
    }

    Counter & counter = *p_counter;

    // Diff diff; // (output.Dimension());
    Coordinate query(output.Dimension());
    vector<int> diff_index(output_size);
    vector<int> index(output_size);
    float value = 0;

    for(unsigned int which_diff = 0; which_diff < diffs.size(); which_diff++)
    {
        const Diff & diff = diffs[which_diff];
        
        if(diff.diff.Dimension() != output.Dimension())
        {
            answer.message = "dimension mismatch";
            return answer;
        }
        
        for(unsigned int i = 0; i < diff_index.size(); i++)
        {
            diff_index[i] = (infinite_kernel ? 0 : static_cast<int>(floor(diff.diff[i]/cell_size)) + center_index[i]);
        }

        // if(!output.Get(diff_index, value)) continue;
        answer.count += (output.Get(diff_index, value) > 0)*diff.weight;

        counter.Reset();
        do
        {
            counter.Get(index);
            
            for(unsigned int i = 0; i < index.size(); i++)
            {
                index[i] += diff_index[i];
                query[i] = (index[i]-center_index[i])*query_scale;
            }

            if(output.Get(index, value))
            {
                value += kernel.Get(diff.diff, query)*diff.weight;

                if(! output.Put(index, value))
                {
                    answer.message = "cannot put value";
                    return answer;
                }
            }
        }
        while(counter.Next());
    }

    // cleanup
    if(p_counter)
    {
        delete p_counter;
        p_counter = 0;
    }

    // done
    return answer;
}
