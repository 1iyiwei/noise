/*
  RMatrix.cpp

  Li-Yi Wei
  05/25/2008

*/

#include <algorithm>
#include <deque>
using namespace std;

#include <math.h>
#include "RMatrix.hpp"
#include "Math.hpp"
#include "Exception.hpp"

float RMatrix::UniformRValue(const int dimension, const vector<float> & r_values)
{
    float result = 0;

    for(unsigned int i = 0; i < r_values.size(); i++)
    {
        result += 1.0/pow(r_values[i]*1.0, dimension*1.0);
    }

    result = 1.0/pow(result*1.0, 1.0/dimension);

    return result;
}

Array<float> RMatrix::BuildRMatrix(const int dimension, const int num_classes_i, const Method method, const vector<float> & r_values)
{
    const int num_classes = num_classes_i > 0 ? num_classes_i : -num_classes_i;

    vector<int> output_size(2, num_classes);
    Array<float> output(output_size);

    output.Clear(-1.0);

    if((method == UNIFORM) && (r_values.size() == num_classes))
    {
        const float uniform_r = UniformRValue(dimension, r_values);
        
        output.Clear(uniform_r);

        vector<int> output_index(2, 0);

        for(output_index[0] = 0; output_index[0] < output_size[0]; output_index[0]++)
        {
            output_index[1] = output_index[0];
            output.Put(output_index, r_values[output_index[0]]);
        }
        
    } 
    else if((method == GEOMETRIC) && (r_values.size() == num_classes))
    {
        vector<int> output_index(2, 0);

        for(output_index[0] = 0; output_index[0] < output_size[0]; output_index[0]++)
            for(output_index[1] = 0; output_index[1] < output_size[1]; output_index[1]++)
            {
                output.Put(output_index, (r_values[output_index[0]] + r_values[output_index[1]])/2);
            }
    }
    else if((method == OPTIMAL) && (r_values.size() == num_classes))
    {
        vector<int> output_index(2, 0);

        // diagonal entries
        for(output_index[0] = 0; output_index[0] < output_size[0]; output_index[0]++)
        {
            output_index[1] = output_index[0];
            output.Put(output_index, r_values[output_index[0]]);
        }

        // priority groups
        deque<Sortee> sortees(r_values.size());
        for(unsigned int i = 0; i < sortees.size(); i++)
        {
            sortees[i].name = i;
            sortees[i].value = -r_values[i];
        } 

        sort(sortees.begin(), sortees.end());

        vector< vector<int> > priority_groups;
        {
            int current_group = -1;
            float last_r_value = 0;

            while(sortees.size() > 0)
            {
                if(last_r_value != sortees[0].value)
                {
                    current_group++;
                    last_r_value = sortees[0].value;
                }

                while(priority_groups.size() < (current_group+1))
                {
                    vector<int> empty;
                    priority_groups.push_back(empty);
                }

                priority_groups[current_group].push_back(sortees[0].name);
                last_r_value = sortees[0].value;
                sortees.pop_front();
            }
        }

        // assign off diagonal entries
        vector<int> c_done; // already processed classses
        float d_done = 0; // density of the classes already processed

        for(int k = 0; k < priority_groups.size(); k++)
        {
            for(int i = 0; i < priority_groups[k].size(); i++)
            {
                c_done.push_back(priority_groups[k][i]);
            }

            for(int i = 0; i < priority_groups[k].size(); i++)
            {
                d_done += 1.0/pow(r_values[priority_groups[k][i]]*1.0, dimension*1.0);
            } 

            for(int i = 0; i < priority_groups[k].size(); i++)
            {
                for(int j = 0; j < c_done.size(); j++)
                {
                    output_index[0] = priority_groups[k][i];
                    output_index[1] = c_done[j];

                    if(output_index[0] != output_index[1])
                    {
                        output.Put(output_index, 1.0/pow(d_done*1.0, 1.0/dimension));

                        swap(output_index[0], output_index[1]);
                        output.Put(output_index, 1.0/pow(d_done*1.0, 1.0/dimension));
                    }
                }
            }
        }
    }
    else if(/*(method == FULL) && */(r_values.size() == (num_classes+1)*num_classes/2))
    {
        int r_input_index = 0;
        vector<int> output_index(2, 0);
        for(output_index[0] = 0; output_index[0] < output_size[0]; output_index[0]++)
            for(output_index[1] = output_index[0]; output_index[1] < output_size[1]; output_index[1]++)
            {
                output.Put(output_index, r_values[r_input_index]);
                r_input_index++;
            } 
       
        vector<int> input_index(2, 0);
        float value = 0;
        for(output_index[0] = 0; output_index[0] < output_size[0]; output_index[0]++)
            for(output_index[1] = 0; output_index[1] < output_index[0]; output_index[1]++)
            {
                input_index[0] = output_index[1];
                input_index[1] = output_index[0];
                output.Get(input_index, value);
                output.Put(output_index, value);
            }
    }
    else
    {
        throw Exception("illegal combination of method and r_values size");
    }
   
    return output;
}

Array<float> RMatrix::BuildRMatrix(const int dimension, const Method method, const vector<float> & sample_density)
{
    vector<float> r_values(sample_density.size());

    for(unsigned int i = 0; i < r_values.size(); i++)
    {
        r_values[i] = Math::ComputeMaxMinDistance(dimension, sample_density[i]);
    }

    return BuildRMatrix(dimension, sample_density.size(), method, r_values);
}

Array<float> RMatrix::BuildRMatrix(const vector<float> & domain_size, const Method method, const vector<int> & num_samples)
{
    vector<float> sample_density(num_samples.size());

    float volume = 1;
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        volume *= domain_size[i];
    }

    for(unsigned int i = 0; i < sample_density.size(); i++)
    {
        sample_density[i] = num_samples[i]/volume;
    }

    return BuildRMatrix(domain_size.size(), method, sample_density);
}

vector<PriorityGroup> BuildPriorityGroups(const int dimension, const vector<int> & priority_values, const Array<float> & r_matrix, const int total_num_trials, const int total_target_num_samples)
{
    if((r_matrix.Dimension() != 2) || (r_matrix.Size(0) != priority_values.size()) || (r_matrix.Size(1) != priority_values.size()))
    {
        throw Exception("illegal r matrix!");
    }

    vector<PriorityGroup> priority_groups;

    for(unsigned int i = 0; i < priority_values.size(); i++)
    {
        while(priority_groups.size() < (priority_values[i]+1))
        {
            PriorityGroup empty;
            priority_groups.push_back(empty);
        }

        priority_groups[priority_values[i]].classes.push_back(i);
    }

    float total_density = 0;
    for(unsigned int i = 0; i < priority_values.size(); i++)
    {
        vector<int> index(2, i);
        float value = 0;
        if(!r_matrix.Get(index, value)) throw Exception("weird total density");
        total_density += 1/pow(value*1.0, dimension*1.0);
    }

    for(unsigned int i = 0; i < priority_groups.size(); i++)
    {
        // uniformly split num_trials for now
        priority_groups[i].num_trials = total_num_trials*priority_groups[i].classes.size()/priority_groups.size();

        float density = 0;
        for(unsigned int j = 0; j < priority_groups[i].classes.size(); j++)
        {
            vector<int> index(2, priority_groups[i].classes[j]);
            float value = 0;
            if(!r_matrix.Get(index, value)) throw Exception("weird density");
            density += 1/pow(value*1.0, dimension*1.0);
        }

        priority_groups[i].target_num_samples = static_cast<int>(ceil(total_target_num_samples*density/total_density));
    }

    return priority_groups;
}
