/*
  white position noise distributed regularly on a [0 1]x[0 1] domain

  currently 2D only
  and there is this interesting question of the dimensions of domain and range
  
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

struct Coordinate
{
public:

    float x, y;
};

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
    vector< vector<Coordinate> > white_noise(grid_size);
    {
        for(int i = 0; i < grid_size; i++)
        {
            white_noise[i] = vector<Coordinate>(grid_size);

              for(int j = 0; j < grid_size; j++)
              {
                  white_noise[i][j].x = UniformRandom();
                  white_noise[i][j].y = UniformRandom(); 
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

    vector< vector<Coordinate> > lowpass = white_noise;

    for(int iteration = 0; iteration < abs(num_iterations); iteration++)
    {
        vector< vector<Coordinate> > foo = lowpass;

        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++)
            {
                foo[i][j].x = 0;
                foo[i][j].y = 0;
                
                for(int i_o = -filter_radius; i_o <= filter_radius; i_o++)
                    for(int j_o = -filter_radius; j_o <= filter_radius; j_o++)
                    {
                        const int i_f = ((i+i_o)%grid_size+grid_size)%grid_size;
                        const int j_f = ((j+j_o)%grid_size+grid_size)%grid_size;
                        foo[i][j].x += exp(-(i_o*i_o+j_o*j_o)/(filter_variance*filter_variance))*lowpass[i_f][j_f].x;
                        foo[i][j].y += exp(-(i_o*i_o+j_o*j_o)/(filter_variance*filter_variance))*lowpass[i_f][j_f].y;
                    }

                foo[i][j].x /= total_filter_weight;
                foo[i][j].y /= total_filter_weight;
            }
        
        lowpass = foo;
    }
    
    vector< vector<Coordinate> > final_noise = white_noise;

    if(num_iterations < 0)
    {
        // high pass
        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++)
            {
                final_noise[i][j].x = (white_noise[i][j].x-lowpass[i][j].x+1.0)*0.5;
                final_noise[i][j].y = (white_noise[i][j].y-lowpass[i][j].y+1.0)*0.5;
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

    // normalization
    {
        Coordinate coordinate_max, coordinate_min;
        coordinate_max.x = coordinate_max.y = -100000000;
        coordinate_min.x = coordinate_min.y = +100000000;
        
        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++)
            {
                if(coordinate_max.x < final_noise[i][j].x)
                    coordinate_max.x = final_noise[i][j].x;
                
                if(coordinate_max.y < final_noise[i][j].y)
                    coordinate_max.y = final_noise[i][j].y;
                
                if(coordinate_min.x > final_noise[i][j].x)
                    coordinate_min.x = final_noise[i][j].x;
                
                if(coordinate_min.y > final_noise[i][j].y)
                    coordinate_min.y = final_noise[i][j].y;
            }

        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++)
            {
                final_noise[i][j].x = (final_noise[i][j].x-coordinate_min.x)/(coordinate_max.x - coordinate_min.x);
                final_noise[i][j].y = (final_noise[i][j].y-coordinate_min.y)/(coordinate_max.y - coordinate_min.y);
            }
    }
    
    // output
    for(int i = 0; i < grid_size; i++)
    {
        for(int j = 0; j < grid_size; j++)
        {
            cout << 1 << " " << final_noise[i][j].x << " " << final_noise[i][j].y << endl;
        }
    }
    
    return 0;
}
