/*
  ToroidalHack.cpp

  rid of peaks in the fourier spectrum caused by non-toroidal samples
  
  Li-Yi Wei
  11/02/2007

*/

#include <iostream>
using namespace std;

#include <math.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"

struct Pixel2
{
    Pixel2(void) : index(2) {};
    Pixel2(const int row, const int col) : index(2)
        {
            index[0] = row; index[1] = col;
        };
    
    vector<int> index;
    FrameBuffer::PF value;
};

int Main(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << "input-file-name output-file-name" << endl;
        return 1;
    }
    
    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];

    // input
    Array<FrameBuffer::PF> input_spectrum;

    if(! FrameBuffer::ReadPFM(input_file_name, input_spectrum))
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }

    // process
    vector<int> input_size;
    input_spectrum.Size(input_size);

    vector<int> dc_index = input_size;
    for(unsigned int i = 0; i < dc_index.size(); i++)
    {   
        dc_index[i] = input_size[i]/2;
    }
    
    // hack to rid of the non-toroidal results of Lloyd relaxation
    {
        if(dc_index.size() != 2)
        {
            throw Exception("non-toroidal hack applies to 2D only!");
        }

        vector<Pixel2> non_touchables;
        
#if 0
        // for 10x10 tiling of 20x20 tiles with 2 levels of subdivision
        // ppentomino_100x100_10x10_s2
        non_touchables.push_back(Pixel2(0, 0)); // dc center
        non_touchables.push_back(Pixel2(200, 0)); 
        non_touchables.push_back(Pixel2(-200, 0)); 
        non_touchables.push_back(Pixel2(0, 200)); 
        non_touchables.push_back(Pixel2(0, -200));
        non_touchables.push_back(Pixel2(240, 0)); 
        non_touchables.push_back(Pixel2(-240, 0)); 
        non_touchables.push_back(Pixel2(0, 240)); 
        non_touchables.push_back(Pixel2(0, -240));
        non_touchables.push_back(Pixel2(250, 0)); 
        non_touchables.push_back(Pixel2(-250, 0)); 
        non_touchables.push_back(Pixel2(0, 250)); 
        non_touchables.push_back(Pixel2(0, -250));
#endif

#if 0
        // for a single 10x10 initial tile with 5 subdivision
        // ppentomino_10x10_10x10_s5
        non_touchables.push_back(Pixel2(0, 0)); // dc center
        non_touchables.push_back(Pixel2(160, 0)); 
        non_touchables.push_back(Pixel2(-160, 0)); 
        non_touchables.push_back(Pixel2(0, 160)); 
        non_touchables.push_back(Pixel2(0, -160));
#endif

#if 0
        // ghexomino_subdivision_432x432_2_average_10_sft.pfm
        non_touchables.push_back(Pixel2(0, 0)); // dc center
        non_touchables.push_back(Pixel2(144, 0)); 
        non_touchables.push_back(Pixel2(-144, 0)); 
        non_touchables.push_back(Pixel2(0, 144)); 
        non_touchables.push_back(Pixel2(0, -144)); 
        non_touchables.push_back(Pixel2(192, 0)); 
        non_touchables.push_back(Pixel2(-192, 0)); 
        non_touchables.push_back(Pixel2(0, 192)); 
        non_touchables.push_back(Pixel2(0, -192)); 
        non_touchables.push_back(Pixel2(240, 0)); 
        non_touchables.push_back(Pixel2(-240, 0)); 
        non_touchables.push_back(Pixel2(0, 240)); 
        non_touchables.push_back(Pixel2(0, -240)); 
#endif
        
        // add dc center offset
        for(unsigned int i = 0; i < non_touchables.size(); i++)
        {
            vector<int> & index = non_touchables[i].index;
            
            for(unsigned int j = 0; j < index.size(); j++)
            {
                index[j] += dc_index[j];
            }
        }
        
        // get values for non-touchables
        for(unsigned int i = 0; i < non_touchables.size(); i++)
        {
            input_spectrum.Get(non_touchables[i].index, non_touchables[i].value);
        }

        // average out row and col axis
        for(int dim = 0; dim < 2; dim++)
            for(int i = 0; i < input_size[(dim+1)%2]; i++)
            {
                vector<int> index(2), index_0(2), index_1(2);
                index[dim] = dc_index[dim]; index[(dim+1)%2] = i;
                index_0[dim] = dc_index[dim]-1; index_0[(dim+1)%2] = i;
                index_1[dim] = dc_index[dim]+1; index_1[(dim+1)%2] = i;
                
                FrameBuffer::PF n0, n1;
                input_spectrum.Get(index_0, n0);
                input_spectrum.Get(index_1, n1);

                FrameBuffer::PF me;
                me.r = (n0.r + n1.r)/2;
                me.g = (n0.g + n1.g)/2;
                me.b = (n0.b + n1.b)/2;

                input_spectrum.Put(index, me);
            }
        
        // put values for non-touchables
        for(unsigned int i = 0; i < non_touchables.size(); i++)
        {
            input_spectrum.Put(non_touchables[i].index, non_touchables[i].value);
        }
    }
    
    // output
    if(! FrameBuffer::WritePFM(input_spectrum, output_file_name))
    {
        cerr << "error in writing " << output_file_name << endl;
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
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
