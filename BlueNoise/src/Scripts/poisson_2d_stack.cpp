/*
  poisson_2d_stack

  generate a 1D stack of uncorrelated 2D poisson disk samples

  Li-Yi Wei
  09/13/2007

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

void InitRandomNumberGenerator(void)
{
    srand48(time(0));
}

double UniformRandom(void)
{
    return drand48();
}

int main(int argc, char **argv)
{
    if(argc < 6)
    {
        cerr << "Usage: " << argv[0] << " r k jitter num_slices output_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const float r_number = atof(argv[++argCtr]);
    const float k_number = atof(argv[++argCtr]);
    const float jitter = atof(argv[++argCtr]);
    const int num_slices = atoi(argv[++argCtr]);
    const char * output_file_name = argv[++argCtr];
    const char * temp_file_name = "foo_stacks_09132007yasutalking.txt";

    InitRandomNumberGenerator();
 
    const int dimension = 2;

    ofstream output(output_file_name);

    if(! output)
    {
        cerr << "cannot write into " << output << endl;
        return 1;
    }

    // random seeds
    vector<unsigned long> random_seeds(num_slices);
    for(unsigned int i = 0; i < random_seeds.size(); i++)
    {
        random_seeds[i] = static_cast<unsigned long>(UniformRandom()*LONG_MAX);
    }

    for(int which_slice = 0; which_slice < num_slices; which_slice++)
    {
        const float random_offset = UniformRandom();
        
        {
            ostringstream command_stream;
         
            command_stream << "src/Main/Release/AdaptiveNDim 2 " << r_number << " " << k_number << " 0 1.0 uniform 1 1 " << random_seeds[which_slice] << " > " << temp_file_name;

            system(command_stream.str().c_str());
        }

        {
            ifstream input(temp_file_name);

            if(! input)
            {
                cerr << "cannot read from " << temp_file_name << endl;
                return 1;
            }
        
            vector<float> sample(dimension);
    
            while(input.good())
            {
#ifdef _SFT_READ_VALUE
                input >> sample.value;
#endif
                for(int i = 0; i < dimension; i++)
                {
                    input >> sample[i];
                }
            
                if(input.good())
                {
                    for(int i = 0; i < dimension; i++)
                    {
                        output << (sample[i]+random_offset)-floor(sample[i]+random_offset) << " ";
                    }

                    output << (which_slice+0.5+0.5*jitter*(2*UniformRandom()-1))/num_slices << endl;
                }
            }
        }
    }

    {
        ostringstream command_stream;
        command_stream << "rm -f " << temp_file_name;
        system(command_stream.str().c_str());
    }

    // done
    output.close();

    // done
    return 0;
}
