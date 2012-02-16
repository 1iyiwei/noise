/*
  LloydTools.cpp

  Li-Yi Wei
  07/29/2009

*/

#include <fstream>
using namespace std;

#include "LloydTools.hpp"
#include "ShuffleTools.hpp"
#include "ComboLloydIndex.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

int LloydTools::FastenClassID(int & id)
{
    return ShuffleTools::FastenClassID(id);
}

int LloydTools::LoosenClassID(int & id)
{
    return ShuffleTools::LoosenClassID(id);
}

int LloydTools::CountSamples(const vector<LloydSite> & sites, vector<int> & num_sites)
{
#if 1
    vector<Sample> samples(sites.size());
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        samples[i] = sites[i].sample;
    }

    return Utility::CountSamples(samples, num_sites);
#else
    int max_class_id = -1;

    for(unsigned int i = 0; i < sites.size(); i++)
    {
        if(sites[i].sample.id > max_class_id)
        {
            max_class_id = sites[i].sample.id;
        }
    }

    const int num_classes = max_class_id + 1;
    num_sites = vector<int>(num_classes, 0);

    for(unsigned int i = 0; i < sites.size(); i++)
    {
        if((sites[i].sample.id < 0) || (sites[i].sample.id >= num_sites.size()))
        {
            return 0;
        }
        else
        {
            num_sites[sites[i].sample.id]++;
        }
    }
#endif

    // done
    return 1;
}

int LloydTools::ReadPoints(const int dimension, const int num_class, const LloydIndex & model_index, const string & input_file_name, vector<LloydSample> & points)
{
    ifstream input(input_file_name.c_str());

    if(! input)
    {
        return 0;
    }

    points.clear();

    LloydSample point(dimension, model_index);
    point.index.Clear(num_class);

    int num_masks = 0;
    int mask = 0;
    int lord = 0;
    vector<int> class_combo(num_class, 0);

    while(input.good())
    {
        point.index.Clear();

        input >> num_masks;
        
        if(input.good())
        {
            for(int i = 0; i < num_masks; i++)
            {
                input >> mask >> lord;

                if(input.good())
                {
                    Convert(mask, class_combo);

                    if(! point.index.Set(class_combo, lord, -1))
                    {
                        throw Exception("LloydTools::ReadPoints(): cannot set lord");
                        return 0;
                    }
                }
            }

            for(int i = 0; i < point.sample.coordinate.Dimension(); i++)
            {
                input >> point.sample.coordinate[i];
            }

            if(input.good())
            {
                points.push_back(point);
            }
        }
    }

    // done
    return 1;
}

int LloydTools::WritePoints(const int num_class, const int class_combo_mask, const vector<LloydSample> & points, const string & output_file_name)
{
    ofstream output(output_file_name.c_str());

    if(! output)
    {
        return 0;
    }

    int class_combo_mask_min = 0;
    int class_combo_mask_max = 0;
    if(class_combo_mask > 0)
    {
        // just single class
        class_combo_mask_min = class_combo_mask_max = class_combo_mask;
    }
    else
    {
        // all
        class_combo_mask_min = 1;
        class_combo_mask_max = (1<<num_class)-1;
    }

    vector<int> class_combo(num_class, 0);

    const int num_masks = class_combo_mask_max - class_combo_mask_min + 1;

    for(unsigned int which_point = 0; which_point < points.size(); which_point++)
    {
        const LloydSample & point = points[which_point];

        // lord ids
        output << num_masks;

        for(int mask = class_combo_mask_min; mask <= class_combo_mask_max; mask++)
        {
            Convert(mask, class_combo);
            
            const int lord = point.index.Get(class_combo);
          
            if(lord < 0) throw Exception("LloydTools::WritePoints(): negative lord index");

            output << " " << mask << " " << lord;
        }

        // sample location
        for(int i = 0; i < point.sample.coordinate.Dimension(); i++)
        {
            output << " " << point.sample.coordinate[i];
        }

        output << endl;
    }

    // done
    return 1;
}

void LloydTools::Convert(const int mask_in, vector<int> & mask_out)
{
    for(unsigned int i = 0; i < mask_out.size(); i++)
    {
        mask_out[i] = (mask_in >> i)%2;
    }
}
