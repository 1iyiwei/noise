/*
  MakeBlobs.cpp

  splat a bunch of Gaussians onto a canvas

  Li-Yi Wei
  08/28/2008

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <math.h>
#include <stdlib.h>

#include "Exception.hpp"
#include "Sample.hpp"
#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"

class Blob
{
public:
    Blob(void) : _scale(0) {};

    Blob(const float scale, const Coordinate & mean, const Coordinate & std) : _scale(scale), _mean(mean), _std(std) 
        { 
            if(mean.Dimension() != std.Dimension()) throw Exception("mean.Dimension() != std.Dimension()");
        };

    int Dimension(void) const
        {
            return _mean.Dimension();
        };

    float Scale(void) const
        {
            return _scale;
        };

    const Coordinate & Mean(void) const
        {
            return _mean;
        };

    const Coordinate & Std(void) const
        {
            return _std;
        };

    float Evaluate(const Coordinate & query) const
        {
            if(query.Dimension() != _mean.Dimension())
            {
                // error
                return 0;
            }
            else
            {
                float value = 0;

                for(int i = 0; i < query.Dimension(); i++)
                {
                    const float foo = (query[i] - _mean[i])/_std[i];
                    value += foo*foo;
                }

                return _scale*exp(-value);
            }
        };

protected:
    float _scale;
    Coordinate _mean, _std;
};

int WriteBlobs(const char * file_name, const vector<Blob> & blobs)
{
    if(blobs.size() <= 0) return 0;

    ofstream output(file_name);

    if(! output)
    {
        return 0;
    }

    // number of blobs
    output << blobs.size() << endl;

    // dimension
    output << blobs[0].Dimension() << endl;

    for(unsigned int i = 0; i < blobs.size(); i++)
    {
        if(blobs[i].Dimension() != blobs[0].Dimension()) return 0;

        // scale
        output << blobs[i].Scale();

        // mean
        const Coordinate mean = blobs[i].Mean();
        for(int j = 0; j < mean.Dimension(); j++)
        {
            output << " " << mean[j];
        }

        // std
        const Coordinate std = blobs[i].Std();
        for(int j = 0; j < std.Dimension(); j++)
        {
            output << " " << std[j];
        }

        output << endl;
    }

    // done
    return 1;
}

string ReadBlobs(const char * file_name, vector<Blob> & blobs)
{
    ifstream input(file_name);

    if(! input)
    {
        return "cannot open input file";
    }

    // number of blobs
    int num_blobs = 0;
    input >> num_blobs;

    if(num_blobs <= 0) return "num_blobs <= 0";

    blobs.clear();
    
    // dimension
    int dimension = 0;
    input >> dimension;

    if(dimension <= 0) return "dimension <= 0";

    float scale = 0;
    Coordinate mean(dimension);
    Coordinate std(dimension);
    for(int i = 0; i < num_blobs; i++)
    {
        // scale
        input >> scale;

        // mean
        for(int j = 0; j < mean.Dimension(); j++)
        {
            input >> mean[j];
        }

        // std
        for(int j = 0; j < std.Dimension(); j++)
        {
            input >> std[j];
        }

        if(!input.good()) return "!input.good()";
        blobs.push_back(Blob(scale, mean, std));
    }

    // done
    return "";
}

int TileBlobs(const vector<float> & domain_size, const vector<Blob> & input, vector<Blob> & output)
{
    const int dimension = domain_size.size();
    SequentialCounter counter(dimension, -1, 1);

    vector<Blob> result;

    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(input[i].Dimension() != dimension)
        {
            // cerr << input[i].Dimension() << "!=" <<  dimension << endl;
            return 0;
        }

        const Coordinate & mean = input[i].Mean();

        vector<int> index(dimension);
        Coordinate new_mean = mean;

        counter.Reset();
        do
        {
            counter.Get(index);

            for(int j = 0; j < new_mean.Dimension(); j++)
            {
                new_mean[j] = mean[j] + index[j]*domain_size[j];
            }

            result.push_back(Blob(input[i].Scale(), new_mean, input[i].Std()));
        }
        while(counter.Next());
    }

    output = result;

    return 1;
}

int Main(int argc, char **argv)
{
    const int min_argc = 5;
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_file_name (ascii) output_file_name (pfm) output_size (pixels per unit domain size) boundary_condition (0 for none, 1 for toroidal) domain_size (optional, dimension numbers) color (optional, 3 floats in [0 1])" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const int output_size_1d = atoi(argv[++argCtr]);
    const int boundary_condition = atoi(argv[++argCtr]);

    // read in blobs
    vector<Blob> blobs;
    const string message = ReadBlobs(input_file_name, blobs);
    if(message != "")
    {
        cerr << "error in reading " << input_file_name << ", error message: " << message << endl;
        return 1;
    }

#if 0
    // debug
    if(! WriteBlobs(output_file_name, blobs))
    {
        cerr << "error in writing " << output_file_name << endl;
        return 1;
    }
    else
    {
        cerr << "debug done" << endl;
        return 0;
    }
#endif

    const int dimension = blobs[0].Dimension();

    vector<float> domain_size(dimension, 1.0); // hardwired for now

    if((argCtr + dimension) < argc)
    {
        for(unsigned int i = 0; i < domain_size.size(); i++)
        {
            domain_size[i] = atof(argv[++argCtr]);
        }

        cerr << "domain size:";
        for(unsigned int i = 0; i < domain_size.size(); i++)
        {
            cerr << " " << domain_size[i];
        }
        cerr << endl;
    }

    FrameBuffer::PF color;
    if((argCtr + 3) < argc)
    {
        color.r = atof(argv[++argCtr]);
        color.g = atof(argv[++argCtr]);
        color.b = atof(argv[++argCtr]);
    }
    else
    {
        color.r = color.g = color.b = 1.0;
    }
    
    if(boundary_condition == 1)
    {
        if(! TileBlobs(domain_size, blobs, blobs))
        {
            cerr << "cannot tile blobs" << endl;
            return 1;
        }
    }

    // write output image
    vector<int> output_size(dimension, output_size_1d);
    for(unsigned int i = 0; i < output_size.size(); i++)
    {
        output_size[i] = static_cast<int>(floor(output_size_1d*domain_size[i]+0.5));
    }

    Array<FrameBuffer::PF> output(output_size);
    vector<int> output_size_minus_1(output_size);
    for(unsigned int i = 0; i < output_size_minus_1.size(); i++)
    {
        output_size_minus_1[i] = output_size[i] - 1;
    }
    SequentialCounter counter(dimension, vector<int>(dimension, 0), output_size_minus_1);
    counter.Reset();
    vector<int> index(dimension);
    Coordinate query(dimension);
    FrameBuffer::PF pixel;

    do
    {
        counter.Get(index);

        for(int i = 0; i < query.Dimension(); i++)
        {
            query[i] = index[i]*domain_size[i]/output_size[i];
        }

        float value = 0;
        for(unsigned int i = 0; i < blobs.size(); i++)
        {
            value += blobs[i].Evaluate(query);
        }

        pixel.r = value*color.r;
        pixel.g = value*color.g;
        pixel.b = value*color.b;
        if(! output.Put(index, pixel) )
        {
            cerr << "cannot write output" << endl;
            return 1;
        }
    }
    while(counter.Next());

    if(! FrameBuffer::WritePFM(output, output_file_name))
    {
        cerr << "cannot write " << output_file_name << endl;
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
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
