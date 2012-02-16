/*
  PenroseSampling.cpp

  generate samples according to [Ostromoukhov et al. SIGGRAPH 2004]

  Li-Yi Wei
  10/04/2008

*/


#include <iostream>
using namespace std;

#include <stdlib.h>

#include "quasisampler_prototype.h"
#include "Exception.hpp"

class PenroseSampler : public Quasisampler
{
public:
    PenroseSampler(const double domain_width, const double domain_height, const double correction_amount, const unsigned uniform_field_value) : Quasisampler(domain_width, domain_height, correction_amount), _uniform_field_value(uniform_field_value) {}
     
    unsigned getImportanceAt(Point2D point) {return _uniform_field_value;}

protected:
    const unsigned _uniform_field_value;
};

int Main(int argc, char **argv)
{
    int min_argc = 5;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " domain_width domain_height correction_amount (1.0 by default) uniform_field_value" << endl;
        return 1;
    }

    int argCtr = 0;
    const double domain_width = atof(argv[++argCtr]);
    const double domain_height = atof(argv[++argCtr]);
    const double correction_amount = atof(argv[++argCtr]);
    const unsigned uniform_field_value = atoi(argv[++argCtr]);

    PenroseSampler sampler(domain_width, domain_height, correction_amount, uniform_field_value);

    sampler.buildAdaptiveSubdivision();

    const vector<Point2D> rhombs = sampler.getRhombPoints();

    int max_color = 0;
    for(unsigned int i = 0; i < rhombs.size(); i++)
    {
        if(rhombs[i].color > max_color) max_color = rhombs[i].color;
        cout << rhombs[i].color << " " << rhombs[i].x << " " << rhombs[i].y << endl;
    }

    const vector<Point2D> samples = sampler.getSamplingPoints();

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        cout << (max_color+1) << " " << samples[i].x << " " << samples[i].y << endl;
    }

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
