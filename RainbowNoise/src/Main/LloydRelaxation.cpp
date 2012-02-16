/*
  LloydRelaxation.cpp

  Li-Yi Wei
  07/17/2009

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <memory>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "PlainBoxDomain.hpp"
#include "ToroidalBoxDomain.hpp"

#include "Lloyd.hpp"
#include "DiscreteLloyd.hpp"

#include "ClassWeight.hpp"
#include "FairClassWeight.hpp"
#include "VoronoiFairClassWeight.hpp"
#include "SingleClassWeight.hpp"
#include "AllClassWeight.hpp"
#include "RainbowClassWeight.hpp"

#include "LloydIndex.hpp"
#include "SoloLloydIndex.hpp"
#include "ComboLloydIndex.hpp"

#include "DiscreteLloydVoronoi.hpp"
#include "BruteDiscreteLloydVoronoi.hpp"
#include "BruteDiscreteBalzerVoronoi.hpp"

#include "DiscreteLloydCentroid.hpp"
#include "BruteDiscreteLloydCentroid.hpp"

#include "LloydTools.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
#ifdef _OUTPUT_POINTS
    const int argc_add = 2;
#else
    const int argc_add = 0;
#endif

    const int argc_min = 9 + argc_add;
    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " init_file_name (negative class id for points, non-negative for sites) dimension domain_size (dimension numbers) voronoi_scheme (lloyd, balzer) centroid_scheme (snap none) class_weight_scheme (single, all, fair, rainbow, voronoi) min_movement (stop criteria) max_num_iterations";
#ifdef _OUTPUT_POINTS
        cerr << " class_combo (int for binary mask) points_file_name";
#endif
        cerr << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);

    if(argc < (argc_min + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension, 1.0);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }

    const string voronoi_scheme = argv[++arg_ctr];
    const string centroid_scheme = argv[++arg_ctr];
    const string class_weight_scheme = argv[++arg_ctr];
    const float min_movement = atof(argv[++arg_ctr]);
    const int max_num_iterations = atoi(argv[++arg_ctr]);

#ifdef _OUTPUT_POINTS
    const int class_combo_int = atoi(argv[++arg_ctr]);
    const string points_file_name = argv[++arg_ctr];
#endif

    // init samples
    // process samples into points and sites
    vector<LloydSample> points;
    vector<LloydSite> sites;

    { 
        vector<Sample> samples;
        if(! Utility::ReadSamples(dimension, input_file_name, samples))
        {
            cerr << "cannot read samples from " << input_file_name << endl;
            return 1;
        }

        SoloLloydIndex solo_lloyd_index;
        ComboLloydIndex combo_lloyd_index;

        LloydIndex * p_lloyd_index = 0;
        if(voronoi_scheme == "lloyd")
        {
            p_lloyd_index = &solo_lloyd_index;
        }
        else if(voronoi_scheme == "balzer")
        {
            p_lloyd_index = &combo_lloyd_index;
        }
        else
        {
            cerr << "unsupported Voronoi scheme" << endl;
            return 1;
        }

        LloydSample point(dimension, *p_lloyd_index);

        for(unsigned int i = 0; i < samples.size(); i++)
        {
            point.sample = samples[i];
            point.sample.id = -1;
            points.push_back(point);
        }

        LloydSite site;
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            site.sample = samples[i];

            if(site.sample.id < 0)
            {
                site.fixed = LloydTools::LoosenClassID(site.sample.id);
            }

            if(site.sample.id >= 0)
            {      
                sites.push_back(site);
            }
        }
    }

    // counting
    vector<int> num_sites;    
    if(! LloydTools::CountSamples(sites, num_sites))
    {
        cerr << "error in counting samples" << endl;
        return 1;
    }

    const int num_class = num_sites.size();

    {
        cerr << "num_points: " << points.size() << ", num_sites:";
        for(unsigned int i = 0; i < num_sites.size(); i++)
        {
            cerr << " " << num_sites[i];
        }
        cerr << endl;
    }

    // domain
    // PlainBoxDomain domain(domain_size);
    ToroidalBoxDomain domain(domain_size);

    // class weight
    ClassWeight *p_class_weight = 0;
    
    if(class_weight_scheme == "fair")
    {
        p_class_weight = new FairClassWeight(num_sites);
    }
    else if(class_weight_scheme == "voronoi")
    {
        p_class_weight = new VoronoiFairClassWeight(num_sites);
    }
    else if(class_weight_scheme == "rainbow")
    {
        p_class_weight = new RainbowClassWeight(num_sites);
    }
    else if(class_weight_scheme == "single")
    {
        p_class_weight = new SingleClassWeight(num_sites);
    }
    else if(class_weight_scheme == "all")
    {
        p_class_weight = new AllClassWeight(num_sites);
    }
    else
    {
        cerr << "unsupported class weight scheme" << endl;
        return 1;
    }

    if(!p_class_weight)
    {
        throw Exception("null class weight");
        return 1;
    }

    auto_ptr<ClassWeight> auto_ptr_class_weight(p_class_weight);
    ClassWeight & class_weight = *auto_ptr_class_weight;

    // Voronoi
    DiscreteLloydVoronoi *p_voronoi = 0;

    if(voronoi_scheme == "lloyd")
    {
        p_voronoi = new BruteDiscreteLloydVoronoi(domain);
    }
    else if(voronoi_scheme == "balzer")
    {
        p_voronoi = new BruteDiscreteBalzerVoronoi(domain, num_class);
    }
    else
    {
        cerr << "unsupported Voronoi scheme" << endl;
        return 1;
    }

    auto_ptr<DiscreteLloydVoronoi> auto_ptr_voronoi(p_voronoi);
    DiscreteLloydVoronoi & voronoi = *auto_ptr_voronoi;

    // Centroid
    BruteDiscreteLloydCentroid brute_discrete_centroid(domain, class_weight);
    DiscreteLloydCentroid & centroid = brute_discrete_centroid;

    if(centroid_scheme == "snap")
    {
        centroid.Snap(1);
    }
    else
    {
        centroid.Snap(0);
    }

    // randomize points
    if(voronoi_scheme == "balzer")
    {
        Random::InitRandomNumberGenerator();

        Randomize(points);

        const string message = BruteDiscreteBalzerVoronoi::Init(domain, sites, points);

        if(message != "") throw Exception(message.c_str());
    }

    // Lloyd
    DiscreteLloyd lloyd(voronoi, centroid);

    if(! lloyd.Set(sites))
    {
        cerr << "cannot set Lloyd sites" << endl;
        return 1;
    }
 
    if(! lloyd.Set(points))
    {
        cerr << "cannot set Lloyd points" << endl;
        return 1;
    }

    // iterations
    int iteration = 0;
    int stop = 0;
    float movement = Math::INF;
    for(iteration = 0; (movement > min_movement) && ((max_num_iterations < 0) || (iteration < max_num_iterations)); iteration++)
    {
        if(! lloyd.Voronoi())
        {
            cerr << "error in the Lloyd Voronoi step" << endl;
            return 1;
        }

        movement = lloyd.Centroid();
        if(movement < 0)
        {
            cerr << "error in the Lloyd centroid step" << endl;
            return 1;
        }
    }

    // output
    cerr << iteration << " iterations with final movement " << movement << endl;

#ifdef _OUTPUT_POINTS
    if(! lloyd.Get(points))
    {
        cerr << "cannot get result Lloyd points" << endl;
        return 1;      
    }

    cerr << points.size() << " points" << endl;

    vector<int> class_combo(num_class, 0);
    for(unsigned int i = 0; i < class_combo.size(); i++)
    {
        class_combo[i] = (class_combo_int >> i)%2;
    }

    vector<Sample> samples(points.size());

    for(unsigned int i = 0; i < points.size(); i++)
    {
        int lord = -1;

        if(class_combo_int > 0)
        {
            lord = points[i].index.Get(class_combo);
        }
        else
        {
            float foo;
            if(! points[i].index.Get(-class_combo_int, lord, foo)) throw Exception("cannot get per class lord index");
        }

        if(lord < 0) throw Exception("negative lord index during visualization");

        samples[i] = points[i].sample;
        samples[i].id = lord;
    }

    if(!Utility::WriteSamples(samples, points_file_name))
    {
        cerr << "cannot write out points" << endl;
        return 1;
    }

    // check to see if every site has points
    if(class_combo_int <= 0)
    {
        vector<int> site_count(sites.size(), 0);

        for(unsigned int i = 0; i < samples.size(); i++)
        {
            site_count[samples[i].id]++;
        }

        for(unsigned int i = 0; i < site_count.size(); i++)
        {
            if(site_count[i] <= 0)
            {
                cerr << "empty site " << i << endl;
            }
        }
    }
#endif

    if(! lloyd.Get(sites))
    {
        cerr << "cannot get result Lloyd sites" << endl;
        return 1;      
    }

    cerr << sites.size() << " sites" << endl;

    vector<Sample> final_samples(sites.size());
    for(unsigned int i = 0; i < final_samples.size(); i++)
    {
        if(sites[i].fixed)
        {
            if(!LloydTools::FastenClassID(sites[i].sample.id))
            {
                throw Exception("cannot fasten class id");
            }
        }

        final_samples[i] = sites[i].sample;
    }

    if(!Utility::WriteSamples(final_samples, "cout"))
    {
        cerr << "cannot write out sites" << endl;
        return 1;
    }

    // done
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return Main(argc, argv);
    }
    catch(Exception e)
    {
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
