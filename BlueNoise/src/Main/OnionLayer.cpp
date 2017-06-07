/*
  OnionLayer.cpp

  test driver for OnionLayerCounter class
  plus some statistical measurements
  
  Li-Yi Wei
  07/18/2007

*/

#include <iostream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Math.hpp"
#include "Exception.hpp"
#include "OnionLayerCounter.hpp"
#include "NBallSliceCounter.hpp"

int TestOnionLayerCounter(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " dimension radius_square" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float radius_square = atof(argv[++argCtr]);

    OnionLayerCounter counter(dimension, radius_square);

    vector<int> value(dimension);

    counter.Reset();

    do
    {
        if(! counter.Get(value))
        {
            cerr << "cannot get counter value" << endl;
            return 1;
        }

        for(unsigned int i = 0; i < value.size(); i++)
        {
            cout << value[i] << " ";
        }
        cout << endl;
           
    }
    while(counter.Next());
    
    // done
    return 0;
}

int TestNBallSliceCounter(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " dimension radius_square" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float radius_square = atof(argv[++argCtr]);

    NBallSliceCounter counter(dimension, radius_square);

    vector<int> value(dimension);

    counter.Reset();

    do
    {
        if(! counter.Get(value))
        {
            cerr << "cannot get counter value" << endl;
            return 1;
        }

        for(unsigned int i = 0; i < value.size(); i++)
        {
            cout << value[i] << " ";
        }
        cout << endl;
    }
    while(counter.Next());
    
    // done
    return 0;
}

int OnionLayerStatistics(int argc, char **argv)
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " max_dimension" << endl;
        return 1;
    }

    int argCtr = 0;
    const int max_dimension = atoi(argv[++argCtr]);

    for(int dimension = 2; dimension <= max_dimension; dimension++)
    {
        const float radius_square = 9*dimension;

        const unsigned long hypercube_how_many = pow(2*floor(sqrt(radius_square))+1, dimension);
        
        int how_many = OnionLayerCounter::TotalCount(dimension, radius_square);

#if 0
        throw Exception("doing so would incur huge memory for high dimension!");
        OnionLayerCounter counter(dimension, radius_square);

        counter.Reset();

        how_many = 0;
        do
        {
            how_many++;
        }
        while(counter.Next());
#endif
        
        cout << dimension << " " << hypercube_how_many << " " << how_many << " " << Math::HyperSphereVolume(dimension, sqrt(radius_square)) << endl;
    }

    // done
    return 0;
}

int NBallSliceStatistics(int argc, char **argv)
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " max_dimension" << endl;
        return 1;
    }

    int argCtr = 0;
    const int max_dimension = atoi(argv[++argCtr]);

    for(int dimension = 2; dimension <= max_dimension; dimension++)
    {
        const float radius_square = 9*dimension; // 4 for uniform, 9 for adaptive

        const unsigned long hypercube_how_many = pow(2*floor(sqrt(radius_square))+1, dimension);
        
        NBallSliceCounter counter(dimension, radius_square);

        counter.Reset();

        int how_many = 0;
        do
        {
            how_many++;
        }
        while(counter.Next());
        
        cout << dimension << " " << hypercube_how_many << " " << how_many << " " << Math::HyperSphereVolume(dimension, sqrt(radius_square)) << endl;
    }

    // done
    return 0;
}

int HyperSphereVolumes(int argc, char **argv)
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " max_dimension" << endl;
        return 1;
    }

    int argCtr = 0;
    const int max_dimension = atoi(argv[++argCtr]);

    for(int dimension = 2; dimension <= max_dimension; dimension++)
    {
        const float radius_square = 9*dimension;
        
        cout << dimension << " " << log(Math::HyperSphereVolume(dimension, sqrt(radius_square)))/log(10.0) << endl;
        // cout << dimension << " " << pow(Math::PI, 0.5*dimension) << " " << pow(radius, dimension) << " " << Math::Gamma(dimension) << " " << (dimension%2 == 0? Math::Factorial(dimension/2) : Math::DoubleFactorial(dimension)) << " " << endl;
    }
}

int CompareOnionLayerAndNBallSlice(int argc, char **argv)
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " max_dimension" << endl;
        return 1;
    }

    int argCtr = 0;
    const int max_dimension = atoi(argv[++argCtr]);

    for(int dimension = 2; dimension <= max_dimension; dimension++)
    {
        const float radius_square = 9*dimension;
     
        const int onion_layer_count = OnionLayerCounter::TotalCount(dimension, radius_square);

        NBallSliceCounter nball_slice_counter(dimension, radius_square);
        nball_slice_counter.Reset();
        
        int nball_slice_count = 0;

        do
        {
            nball_slice_count++;
        }
        while(nball_slice_counter.Next());

        cout << dimension << "D radius_square " << radius_square << ": " << onion_layer_count << " " << nball_slice_count << endl;
        if(onion_layer_count != nball_slice_count)
        {
            cout << "error in " << dimension << "D" << endl;

            OnionLayerCounter onion_layer_counter(dimension, radius_square);

            vector<int> value(dimension);

            vector< vector<int> > onion_layer_values;
            vector< vector<int> > nball_slice_values;
            
            onion_layer_counter.Reset();
            do
            {
                onion_layer_counter.Get(value);

                onion_layer_values.push_back(value);
            }
            while(onion_layer_counter.Next());

            nball_slice_counter.Reset();
            do
            {
                nball_slice_counter.Get(value);

                nball_slice_values.push_back(value);
            }
            while(nball_slice_counter.Next());

            for(unsigned int i = 0; i < onion_layer_values.size(); i++)
            {
                int found = 0;

                for(unsigned int j = 0; j < nball_slice_values.size(); j++)
                {
                    if(onion_layer_values[i].size() == nball_slice_values[j].size())
                    {
                        int same = 1;

                        for(unsigned int k = 0; k < onion_layer_values[i].size(); k++)
                        {
                            if(onion_layer_values[i][k] != nball_slice_values[j][k])
                            {
                                same = 0;
                            }
                        }

                        if(same)
                        {
                            found = 1;
                            break;
                        }
                    }
                }

                if(! found)
                {
                    cout << "not found: ";
                    for(unsigned int k = 0; k < onion_layer_values[i].size(); k++)
                    {
                        cout << onion_layer_values[i][k] << " ";
                    }
                    cout << endl;
                }
            }
            
            return 1;
        }
    }

    // done
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        // return TestNBallSliceCounter(argc, argv);
        // return CompareOnionLayerAndNBallSlice(argc, argv); // still buggy for OnionLayer
        return NBallSliceStatistics(argc, argv);
    }
    catch(Exception e)
    {
        cerr << e.Message() << endl;
        return 1;
    }
}
