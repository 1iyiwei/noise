/*
  SFTGPU.cpp

  compute Sample Fourier Transform on a GPU (see also SFT.cpp under Main)
  
  Li-Yi Wei
  11/21/2007

*/


#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "SFT.hpp"
#include "CgSFT.hpp"
#include "framebufferObject.h"
#include "glErrorUtil.h"

class ExitException : public Exception
{
public:
    ExitException(const string & message);
};

ExitException::ExitException(const string & message) : Exception(message)
{
    // nothing to do
}

struct Global
{
    vector<Sample> samples;
    CgSFT * p_sft;
    char * output_file_name;
};

Global global;

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        throw ExitException("exit");
        break;
        
    default:
        break;
    }
}

void Idle(void)
{
    glutPostRedisplay();
}

void Display(void)
{
#if 1
    if(!global.p_sft->Update(global.samples)) throw Exception("cannot update");

    // done
    glutSwapBuffers();
    
    global.samples.clear();
    
    throw ExitException("exit");
#else
    if(global.samples.size() > 0)
    {
        vector<Sample> sample;
        sample.push_back(global.samples[global.samples.size()-1]);
        global.samples.pop_back();

        if(!global.p_sft->Update(sample)) throw Exception("cannot update");
        
        glutSwapBuffers();
    }
    else
    {
        throw ExitException("exit");
    }
#endif
    
}

int CountPositive(vector<int> & numbers)
{
    int positive_count = 0;
    for(unsigned int i = 0; i < numbers.size(); i++)
    {
        positive_count += (numbers[i] > 0);
    }

    return positive_count;
}

int Main(int argc, char **argv)
{
    if(argc < 8)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name output-file-name dimension dimension-specs (*dimension* integers with positive ones indicate the 2D slices and the rest negative/zero ones indicate constant sampling frequency) dft-image-size dft-scale (i.e. maximum frequency in pixels. when dft-scale == dft-image-size no scaling occurs. use <= 0 for default built in value)" << endl;
        return 1;
    }
    
    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    global.output_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    vector<int> dimension_spec;
    if(argc < (6+dimension))
    {
        cerr << "insufficient number of arguments" << endl;
        return 1;
    }
    else
    {
        for(int i = 0; i < dimension; i++)
        {
            dimension_spec.push_back(atoi(argv[++argCtr]));
        }
    }

    const int display_dimension = CountPositive(dimension_spec);
    
    if(display_dimension != 2)
    {
        cerr << "dimension spec must have exactly 2 positive numbers" << endl;
        return 1;
    }

    const int fft_image_size = atoi(argv[++argCtr]);
    const int dft_scale_spec = atoi(argv[++argCtr]);
    
    if((dft_scale_spec > 0) && (dft_scale_spec%fft_image_size != 0))
    {
        cerr << "illegal dft-scale specification" << endl;
        return 1;
    }

    ifstream input(input_file_name);

    // read in samples
    cerr << "read in samples " << endl;
    
    global.samples.clear();
    
    if(!input)
    {
        return 0;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
#ifdef _SFT_READ_VALUE
            input >> sample.value;
#endif
            for(int i = 0; i < dimension; i++)
            {
                input >> sample.coordinate[i];
            }
            
            if(input.good())
            {
                global.samples.push_back(sample);
            }
        }
    }

    cerr << "total " << global.samples.size() << " samples read" << endl;
    
    // GPU stuff
    const int height = fft_image_size;
    const int width = fft_image_size;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(Keyboard);
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
      
    // Initialize the "OpenGL Extension Wrangler" library
    glewInit();

    string result;

    if(!glewIsSupported( "GL_VERSION_2_0 GL_ARB_fragment_program GL_EXT_framebuffer_object GL_NV_float_buffer" )) 
    {
        ostringstream error_message;

        error_message << "Error: failed to get minimal extensions for demo\n";
        error_message << "This sample requires:\n";
        error_message << "  OpenGL version 2.0\n";
        error_message << "  GL_ARB_fragment_program\n";
        error_message << "  GL_EXT_framebuffer_object" << endl;

        result = error_message.str();

        if(result != "") throw Exception(result);
    }

    global.p_sft = new CgSFT(height, width);

    vector<int> freq_start(dimension_spec.size());
    vector<int> freq_increment(dimension_spec.size());

    for(unsigned int i = 0; i < dimension_spec.size(); i++)
    {
        freq_start[i] = -fft_image_size/2;
        freq_increment[i] = dimension_spec[i] > 0 ? ((dft_scale_spec > 0) ? dft_scale_spec/fft_image_size : 1) : 0;
    }
    
    if(!global.p_sft->Reset(freq_start, freq_increment))
    {
        cerr << "cannot reset" << endl;
        return 1;
    }
    
    glutMainLoop();

    delete global.p_sft; global.p_sft = 0;

    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return Main(argc, argv);
    }
    catch(ExitException e)
    {
        // output stuff
        cerr << "output stuff" << endl;
        
        Array<FrameBuffer::AF> input_pixels;
        global.p_sft->GetColor(input_pixels);

        vector<int> output_size;
        input_pixels.Size(output_size);
        
        Array<FrameBuffer::PF> output_pixels(output_size);
        {
            vector<FrameBuffer::AF> input_raw_pixels;
            input_pixels.Get(input_raw_pixels);

            vector<FrameBuffer::PF> output_raw_pixels;
            output_pixels.Get(output_raw_pixels);

            for(unsigned int i = 0; i < output_raw_pixels.size(); i++)
            {
                const float input_value = sqrt(input_raw_pixels[i].r*input_raw_pixels[i].r + input_raw_pixels[i].g*input_raw_pixels[i].g);
                
                output_raw_pixels[i].r = output_raw_pixels[i].g = output_raw_pixels[i].b = input_value;
            }
            
            output_pixels.Put(output_raw_pixels);
        }

        if(! FrameBuffer::WritePFM(output_pixels, global.output_file_name))
        {
            cerr << "cannot write into " << global.output_file_name << endl;
            return 1;
        }
        
        return 0;
    }
    catch(Exception e)
    {
        cerr << "Error : "<< e.Message() << endl;
        return 1;
    }
}
