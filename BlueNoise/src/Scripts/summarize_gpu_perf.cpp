/*
  summarize_gpu_perf.cpp

  summarize gpu performance from a single file containing
  all perf numbers of a particular case

  Li-Yi Wei
  12/28/2007

*/

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <stdlib.h>

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " input_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];

    ifstream input(input_file_name);

    if(! input)
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    vector<float> time_values;
    vector<int> count_values;

    const int buf_size = 1024;
    char buf[buf_size];
    
    while(input.good())
    {
        input.getline(buf, buf_size);

        float time_value;
        int count_value;

        if(sscanf(buf, "total watch time %f", &time_value))
        {
            time_values.push_back(time_value);
        }

        if(sscanf(buf, "total # of samples %d", &count_value))
        {
            count_values.push_back(count_value);
        }
    }

    if((time_values.size() != count_values.size()))
    {
        cerr << "unmatched number of timing measurements and sample counts" << endl;
        return 1;
    }
    
    if(time_values.size() <= 0)
    {
        cerr << "nothing read" << endl;
        return 1;
    }

    double best_perf = 0;

    for(unsigned int i = 0; i < time_values.size(); i++)
    {
        const double current_perf = count_values[i]/time_values[i];

        if(current_perf > best_perf)
        {
            best_perf = current_perf;
        }
    }

    cout << fixed << "best_perf = " << best_perf << endl;

    // done
    return 0;
}
