/*
  SampleRecord.cpp

  Li-Yi Wei
  06/25/2008

*/

#include <fstream>
using namespace std;

#include "SampleRecord.hpp"

int ReadSampleHistory(const char * input_file_name, vector<SampleRecord> & sample_history)
{
    sample_history.clear();

    ifstream input(input_file_name);

    if(! input)
    {
        return 0;
    }
    else
    {
        SampleRecord record;
        int foo;

        while(input.good())
        {
            input >> record.class_id >> foo;

            record.status = static_cast<SampleRecord::Status>(foo);

            if(input.good())
            {
                sample_history.push_back(record);
            }
        }

        return 1;
    }
}

int WriteSampleHistory(const char * output_file_name, const vector<SampleRecord> & sample_history)
{
    ofstream history_output(output_file_name);
    
    if(! history_output)
    {
        return 0;
    }
    else
    {
        for(unsigned int i = 0; i < sample_history.size(); i++)
        {
            history_output << sample_history[i].class_id << " " << static_cast<int>(sample_history[i].status) << endl;
        }

        return 1;
    }
}
