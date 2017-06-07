/*
  white value noise distributed regularly on a [0 1]x[0 1] domain

  Li-Yi Wei
  06/30/2007

*/

#include <iostream>
#include <vector>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

void InitRandomNumberGenerator(void)
{
    srand48(time(0));
}

double UniformRandom(void)
{
    return drand48();
}

int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        cerr << "Usage: " << argv[0] << " grid_size num_iterations(positive for low pass, 0 for no-filter, negative for high pass) gaussian_filter_variance (pixels) gaussia_filter_radius (pixels)" << endl;
        return 1;
    }

    const int grid_size = atoi(argv[1]);
    const int num_iterations = atoi(argv[2]);
    const float filter_variance = atof(argv[3]);
    const int filter_radius = atoi(argv[4]);
    
    InitRandomNumberGenerator();

    // produce white noise
    vector< vector<float> > white_noise(grid_size);
    {
        for(int i = 0; i < grid_size; i++)
        {
            white_noise[i] = vector<float>(grid_size);

              for(int j = 0; j < grid_size; j++)
              {
                  white_noise[i][j] = UniformRandom(); 
              }
        }
    }

    // low-pass filtering
    float total_filter_weight = 0;
    {
        for(int i = -filter_radius; i <= filter_radius; i++)
        {
            for(int j = -filter_radius; j <= filter_radius; j++)
            {
                total_filter_weight += exp(-(i*i+j*j)/(filter_variance*filter_variance));
            }
        }
    }

    vector< vector<float> > lowpass = white_noise;

    for(int iteration = 0; iteration < abs(num_iterations); iteration++)
    {
        vector< vector<float> > foo = lowpass;

        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++)
            {
                foo[i][j] = 0;
                
                for(int i_o = -filter_radius; i_o <= filter_radius; i_o++)
                    for(int j_o = -filter_radius; j_o <= filter_radius; j_o++)
                    {
                        const int i_f = ((i+i_o)%grid_size+grid_size)%grid_size;
                        const int j_f = ((j+j_o)%grid_size+grid_size)%grid_size;
                        foo[i][j] += exp(-(i_o*i_o+j_o*j_o)/(filter_variance*filter_variance))*lowpass[i_f][j_f];
                    }

                foo[i][j] /= total_filter_weight;
            }
        
        lowpass = foo;
    }
    
    vector< vector<float> > final_noise = white_noise;

    if(num_iterations < 0)
    {
        // high pass
        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++)
            {
                final_noise[i][j] = (white_noise[i][j]-lowpass[i][j]+1.0)*0.5;
            }
    }
    else if(num_iterations > 0)
    {
        final_noise = lowpass;
    }
    else
    {
        final_noise = white_noise;
    }
    
    // output
    for(int i = 0; i < grid_size; i++)
    {
        for(int j = 0; j < grid_size; j++)
        {
            cout << final_noise[i][j] << " " << (i+0.5)/grid_size << (j+0.5)/grid_size << endl;
        }
    }
    
    return 0;
}
