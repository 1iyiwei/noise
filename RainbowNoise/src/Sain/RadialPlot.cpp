/*
  Plot out the radial mean and variance

  Li-Yi Wei
  07/04/2007

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <assert.h>
//#include <float.h>
//#include <limits.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"

// for plotting the radial and aniso diagrams
struct Radial
{
public:
    Radial(void) : mean(0), variance(0), count(0) {};
    
public:
    float mean;
    float variance;
    int count;
};

int Debug(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " dimension fft_image_size" << endl;
        return 1;
    }
    
    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const int fft_image_size = atoi(argv[++argCtr]);

    vector<Radial> radial(fft_image_size/2+1);

    {
        SequentialCounter counter(dimension, 0, fft_image_size-1);
        counter.Reset();
        vector<int> index(dimension);

        do
        {
            counter.Get(index);
           
            unsigned int entry = 0;
            {
                for(unsigned int i = 0; i < index.size(); i++)
                {
                    entry += (index[i]-fft_image_size/2)*(index[i]-fft_image_size/2);
                }
            }
                
            entry = static_cast<unsigned int>(floor(sqrt(entry*1.0)));
            
            if(entry < radial.size())
            {
                radial[entry].count++;
            }
        }
        while(counter.Next());
    }

    cout << "radial count:";
    for(unsigned int i = 0; i < radial.size(); i++)
    {
        cout << " " << radial[i].count;
    }
    cout << endl;

    return 0;
}

int Main(int argc, char **argv)
{
    if(argc < 5)
    {
        cerr << "Usage: " << argv[0] << " spectrum-file-list-name output-spectrum-name radial-mean-name radial-variance-name" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_list_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const char * radial_mean_file_name = argv[++argCtr];
    const char * radial_variance_file_name = argv[++argCtr];

    vector<string> input_file_names;
    {
        ifstream input_file_list(input_file_list_name);
        if(!input_file_list)
        {
            cerr << "cannot read from " << input_file_list_name << endl;
            return 1;
        }
        else
        {
            while(input_file_list.good())
            {
                string input_file_name;
                input_file_list >> input_file_name;

                if(input_file_list.good())
                {
                    input_file_names.push_back(input_file_name);
                }
            }
        }
    }

    assert(input_file_names.size() > 0);

    Array<FrameBuffer::PF> fourier_magnitude_image;

    if(! FrameBuffer::ReadPFM(input_file_names[0].c_str(), fourier_magnitude_image))
    {
        cerr << "error in reading " << input_file_names[0] << endl;
        return 1;
    }
    
    const int fft_image_size = fourier_magnitude_image.Size(0);

    if(fft_image_size <= 0)
    {
        cerr << "wrong input size" << endl;
        return 1;
    }

    const int dimension = fourier_magnitude_image.Dimension();

#if 0
    if(dimension != 2)
    {
        cerr << "can only handle 2D for now" << endl;
        return 1;
    }
#endif
    
    int ppm_or_pfm = -1;

    if(strstr(output_file_name, ".ppm"))
    {
        if(dimension <= 2)
        {
            cerr << "output is ppm " << endl;
            ppm_or_pfm = 0;
        }
        else
        {
            cerr << "output is pfm " << endl;
            ppm_or_pfm = 1;
        }
    }
    else if(strstr(output_file_name, ".pfm"))
    {
        cerr << "output is pfm " << endl;
        ppm_or_pfm = 1;
    }
    else
    {
        cerr << "output file must be ppm or pfm" << endl;
        return 1;
    }
    
    // compute averaged spectrum
    vector<int> spectrum_size;
    fourier_magnitude_image.Size(spectrum_size);
    Array<float> fourier_magnitude(spectrum_size);
    fourier_magnitude.Clear(0);

    vector<FrameBuffer::PF> fourier_magnitude_image_data;
    fourier_magnitude_image.Get(fourier_magnitude_image_data);
    
    vector<float> fourier_magnitude_data;
    fourier_magnitude.Get(fourier_magnitude_data);
    
    for(unsigned int which_input = 0; which_input < input_file_names.size(); which_input++)
    {
        cerr << "process " << input_file_names[which_input] << endl;
        
        // read in
        if(! FrameBuffer::ReadPFM(input_file_names[which_input].c_str(), fourier_magnitude_image))
        {
            cerr << "error in reading " << input_file_names[which_input] << endl;
            return 1;
        }

        if(fourier_magnitude_image.Dimension() != dimension)
        {
            cerr << "wrong dimension " << endl;
            return 1;
        }
        
        for(int which_dim = 0; which_dim < fourier_magnitude_image.Dimension(); which_dim++)
        {
            if(fft_image_size != fourier_magnitude_image.Size(which_dim))
            {
                cerr << "wrong input size" << endl;
                return 1;
            }
        }
        
        // accumulate
        cerr << "accumulate " << which_input << endl;
        {
            fourier_magnitude_image.Get(fourier_magnitude_image_data);

            if(fourier_magnitude_image_data.size() != fourier_magnitude_data.size())
            {
                cerr << "weird " << endl;
                return 1;
            }

            for(unsigned int i = 0; i < fourier_magnitude_data.size(); i++)
            {
                fourier_magnitude_data[i] += fourier_magnitude_image_data[i].g*fourier_magnitude_image_data[i].g;
            }
        }
    }

    {
        // normalization
        for(unsigned int i = 0; i < fourier_magnitude_data.size(); i++)
        {
            fourier_magnitude_data[i] = sqrt(fourier_magnitude_data[i]/input_file_names.size());
        }
    }

    if(! fourier_magnitude.Put(fourier_magnitude_data))
    {
        cerr << "cannot put back data" << endl;
        return 1;
    }
    
    // output averaged spectrum
    cerr << "process averaged spectrum" << endl;
    if(ppm_or_pfm == 0)
    {
        // ppm
        throw Exception("ppm no longer supported; use pfm + PFM2PPM");
    }
    else
    {
        Array<FrameBuffer::PF> output(spectrum_size);

        vector<FrameBuffer::PF> output_data;
        if(! output.Get(output_data))
        {
            cerr << "error in getting output data" << endl;
            return 1;
        }

        if(output_data.size() != fourier_magnitude_data.size())
        {
            cerr << "output_data.size() != fourier_magnitude_data.size()" << endl;
            return 1;
        }
        else
        {
            for(unsigned int i = 0; i < output_data.size(); i++)
            {
                output_data[i].r = output_data[i].g = output_data[i].b = fourier_magnitude_data[i];
            }
        }

        if(! output.Put(output_data))
        {
            cerr << "error in putting back output data" << endl;
            return 1;
        }
        
        if(! FrameBuffer::WritePFM(output, output_file_name))
        {
            cerr << "error in writing " << output_file_name << endl;
            return 1;
        }
    }
    
    // radial diagram
    cerr << "process radial plots" << endl;
    vector<Radial> radial(fft_image_size/2+1);
    {
        // mean
        {
            SequentialCounter counter(dimension, 0, fft_image_size-1);
            counter.Reset();
            vector<int> index(dimension);

	    do
            {
                counter.Get(index);
                
                unsigned int entry = 0;
                {
                    for(unsigned int i = 0; i < index.size(); i++)
                    {
                        entry += (index[i]-fft_image_size/2)*(index[i]-fft_image_size/2);
                    }
                }
                
                entry = static_cast<unsigned int>(floor(sqrt(entry*1.0)));
                    
                if(entry < radial.size())
                {
                    float value;
                    if(! fourier_magnitude.Get(index, value))
                    {
                        cerr << "error in getting data" << endl;
                        return 1;
                    }
                    radial[entry].mean += value*value;
                    radial[entry].count++;
                }
            }
	    while(counter.Next());
        }
        {
            for(unsigned int i = 0; i < radial.size(); i++)
            {
                if(radial[i].count)
                {
                    radial[i].mean = radial[i].mean/radial[i].count;
                }
            }
        }

        // variance
        {
            SequentialCounter counter(dimension, 0, fft_image_size-1);
            counter.Reset();
            vector<int> index(dimension);

	    do
            {
                counter.Get(index);
                
                unsigned int entry = 0;
                {
                    for(unsigned int i = 0; i < index.size(); i++)
                    {
                        entry += (index[i]-fft_image_size/2)*(index[i]-fft_image_size/2);
                    }
                }
                
                entry = static_cast<unsigned int>(floor(sqrt(entry*1.0)));
                    
                if(entry < radial.size())
                {
                    float value;
                    if(! fourier_magnitude.Get(index, value))
                    {
                        cerr << "error in getting data" << endl;
                        return 1;
                    }
                    
                    const float foo = value*value-radial[entry].mean;
                    radial[entry].variance += foo*foo;
                }
            }
	    while(counter.Next());
        }

        {
            for(unsigned int i = 0; i < radial.size(); i++)
            {
                if((radial[i].count > 1) && (radial[i].mean > 0))
                {
                    radial[i].variance /= radial[i].mean*radial[i].mean*(radial[i].count-1);
                }
            }
        }
        
        ofstream radial_mean_output(radial_mean_file_name);
        if(! radial_mean_output)
        {
            cerr << "cannot write to " << radial_mean_file_name << endl;
            return 1;
        }
        else
        {
            for(unsigned int i = 1; i < radial.size(); i++)
            {
                radial_mean_output << radial[i].mean << " ";
            }
            radial_mean_output << endl;
        }

        ofstream radial_variance_output(radial_variance_file_name);
        if(! radial_variance_output)
        {
            cerr << "cannot write to " << radial_variance_file_name << endl;
            return 1;
        }
        else
        {
            for(unsigned int i = 1; i < radial.size(); i++)
            {
                radial_variance_output << 10*log(radial[i].variance > 0 ? radial[i].variance:1)/log(10.0) << " ";
            }
            radial_variance_output << endl;
        }
#if 0
        cout << "radial mean: ";
        {
            for(unsigned int i = 0; i < radial.size(); i++)
            {
                cout << radial[i].mean << " ";
            }
            cout << endl;
        }
        cout << "radial variance: ";
        {
            for(unsigned int i = 0; i < radial.size(); i++)
            {
                // output in db
                cout << 10*log(radial[i].variance > 0 ? radial[i].variance:1)/log(10.0) << " ";
            }
            cout << endl;
        }
#endif
    }

    // done
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        // return Debug(argc, argv); // debug
        return Main(argc, argv);
    }
    catch(Exception e)
    {
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
