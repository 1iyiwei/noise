/*
  draw objects according to sample distribution

  Li-Yi Wei
  10/11/2008

*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Exception.hpp"
#include "Sample.hpp"
#include "FrameBuffer.hpp"
#include "Random.hpp"
#include "Math.hpp"

struct ObjectSample
{
    ObjectSample(void): depth(0), angle(0) {};

    Sample sample;
    float depth;
    float angle;
};

struct Global
{
    vector<float> domain_size;
    int num_classes;
    vector<GLuint> texture_ids;
    vector<float> object_sizes;
    vector<ObjectSample> object_samples;
    string output_file_name;
    int background_color;
    vector<int> background_texture_size;
    int batch_mode;
    GLint num_aux_buffers;
};

Global global;

class ExitException : public Exception
{
public:
    ExitException(const string & message);
};

ExitException::ExitException(const string & message) : Exception(message)
{
    // nothing to do
}

int WriteColor(const GLsizei width, const GLsizei height, const char * file_name);

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        throw ExitException("exit");
        break;
        
    case 'd':
    case 'D':
    {
        int viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        if(!WriteColor(viewport[2], viewport[3], global.output_file_name.c_str()))
        {
            throw Exception("error in writing framebuffer");
        }
    }
    break;

    default:
        glutPostRedisplay();
        break;
    }
}

void Idle(void)
{
    // glutPostRedisplay();
}

void RenderQuad(void)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, global.texture_ids[static_cast<int>(floor(Random::UniformRandom()*global.texture_ids.size()))%global.texture_ids.size()]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
  
    glColor3f(0.0, 0.0, 1.0);  // blue
    glTexCoord2f(0, 0);
    glVertex2f(-1, -1);
 
    glColor3f(0.0, 1.0, 0.0);  // green
    glTexCoord2f(1, 0);
    glVertex2f(1, -1);
  
    glColor3f(1.0, 0.0, 0.0);  // red
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    
    glColor3f(1.0, 1.0, 0.0);  // yellow
    glTexCoord2f(0, 1);
    glVertex2f(-1, 1);
    
    glEnd();
}

struct Vertex
{
    float x, y, z;
};

void RenderSamples(void)
{
    if(global.domain_size.size() != 2) throw Exception("illegal domain_size");
   
    if(global.texture_ids.size() < global.num_classes)
    {
        cerr << global.texture_ids.size() << "<" << global.num_classes << endl;
        throw Exception("wrong number of texture_ids");
    }

    if(global.object_sizes.size() != global.num_classes) 
    {
        cerr << global.object_sizes.size() << "!=" << global.num_classes << endl;
        throw Exception("wrong number of object_sizes");
    }

    vector<Sample> samples;

    glDepthFunc(GL_LESS);
    glAlphaFunc(GL_GREATER, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);

    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glClearColor(global.background_color, global.background_color, global.background_color, global.background_color);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    if(global.texture_ids.size() > global.num_classes)
    {
        if(global.background_texture_size.size() != 2) throw Exception("wrong background texture size");

        int viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        vector<float> tex_coord(2);
        for(unsigned int i = 0; i < tex_coord.size(); i++)
        {
            tex_coord[i] = 1.0*viewport[2+i]/global.background_texture_size[i];
        }

        // draw background texture
        glBindTexture(GL_TEXTURE_2D, global.texture_ids[global.texture_ids.size()-1]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

        glBegin(GL_QUADS);
  
        glColor3f(0.0, 0.0, 1.0);  // blue
        glTexCoord2f(0, 0);
        glVertex2f(-1, -1);
 
        glColor3f(0.0, 1.0, 0.0);  // green
        glTexCoord2f(tex_coord[0], 0);
        glVertex2f(1, -1);
  
        glColor3f(1.0, 0.0, 0.0);  // red
        glTexCoord2f(tex_coord[0], tex_coord[1]);
        glVertex2f(1, 1);
    
        glColor3f(1.0, 1.0, 0.0);  // yellow
        glTexCoord2f(0, tex_coord[1]);
        glVertex2f(-1, 1);
    
        glEnd();
    }

    glEnable(GL_DEPTH_TEST);

    vector<Vertex> vertices(4);
    Vertex offset;
    for(int class_id = 0; class_id < global.num_classes; class_id++)
    {
        glBindTexture(GL_TEXTURE_2D, global.texture_ids[class_id]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glBegin(GL_QUADS);

        glColor3f(1, 1, 1);

        const float object_size = 0.5*global.object_sizes[class_id];

        for(unsigned int which_sample = 0; which_sample < global.object_samples.size(); which_sample++)
        {
            const ObjectSample & object_sample = global.object_samples[which_sample];
            const Sample & sample = object_sample.sample;

            if(sample.id != class_id) continue;

            // random depth
            const float depth = object_sample.depth; // Random::UniformRandom();
            for(unsigned int i = 0; i < vertices.size(); i++)
            {
                vertices[i].z = depth;
            }
            
            // random angle
            const float angle = object_sample.angle; // 2*Math::PI*Random::UniformRandom();
            const float cos_angle = cos(angle);
            const float sin_angle = sin(angle);

            for(unsigned int i = 0; i < vertices.size(); i++)
            {
                vertices[i].x = object_size*(2*((i == 1) || (i == 2))-1); 
                vertices[i].y = object_size*(2*((i == 2) || (i == 3))-1);

                offset.x = cos_angle*vertices[i].x - sin_angle*vertices[i].y;
                offset.y = sin_angle*vertices[i].x + cos_angle*vertices[i].y;

                vertices[i].x = sample.coordinate[0] + offset.x;
                vertices[i].y = sample.coordinate[1] + offset.y;
            }
   
            for(unsigned int i = 0; i < vertices.size(); i++)
            {
                glTexCoord2f((i == 1) || (i == 2), (i == 2) || (i == 3));
                glVertex3f(vertices[i].x/global.domain_size[0]*2-1, vertices[i].y/global.domain_size[1]*2-1, vertices[i].z);
            }
        }

        glEnd();
    }
}

void Display(void)
{
    if(global.batch_mode)
    {
        if(global.num_aux_buffers > 0) glDrawBuffer(GL_AUX0);
    }

#if 1
    RenderSamples();
#else
    RenderQuad();
#endif

    if(global.batch_mode)
    {
        int viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        if(global.num_aux_buffers > 0) glReadBuffer(GL_AUX0);
        if(!WriteColor(viewport[2], viewport[3], global.output_file_name.c_str()))
        {
            throw Exception("error in writing framebuffer");
        }

        throw ExitException("done");
    }

    glutSwapBuffers();
}

void Reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

GLuint LoadTexture(const char * file_name, vector<int> & texture_size)
{
    // read image
    Array<FrameBuffer::P3> pixels;
    int max_value;
    if(! FrameBuffer::ReadPPM(file_name, pixels, max_value))
    {
        cerr << "cannot read from " << file_name << endl;
        return 0;
    }

    {
        // scaling to make sure that max_value becomes 255
        vector<FrameBuffer::P3> all_pixels;
        pixels.Get(all_pixels);
        
        for(unsigned int i = 0; i < all_pixels.size(); i++)
        {
            all_pixels[i].r = all_pixels[i].r*255/max_value;
            all_pixels[i].g = all_pixels[i].g*255/max_value;
            all_pixels[i].b = all_pixels[i].b*255/max_value;
        }

        pixels.Put(all_pixels);
    }

    pixels.Size(texture_size);
    if(texture_size.size() != 2)
    {
        cerr << file_name << " not a 2D texture" << endl;
        return 0;
    }

    const int height = texture_size[0]; 
    const int width = texture_size[1]; 

    GLuint handle = 0;
    glGenTextures(1, &handle);

    if(handle <= 0)
    {
        cerr << "wrong texture handle " << handle << endl;
        return 0;
    }

    // Basic OpenGL texture state setup
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Fill in the texture map
    vector<FrameBuffer::P3> input_data;
    pixels.Get(input_data);
    unsigned char *data = new unsigned char[height*width*4];
    unsigned char *dp = data;
    for (int i = 0; i < height*width; ++i)
    {
        data[4*i+0] = input_data[i].r;
        data[4*i+1] = input_data[i].g;
        data[4*i+2] = input_data[i].b;
        data[4*i+3] = 255*(((input_data[i].r + input_data[i].g + input_data[i].b) > 0) && ((input_data[i].r + input_data[i].g + input_data[i].b) < 255*3));
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    delete[] data;

    return handle;
}

GLuint LoadTexture(const char * file_name)
{
    vector<int> texture_size;
    return LoadTexture(file_name, texture_size);
}

int WriteColor(const GLsizei width, const GLsizei height, const char * file_name)
{
    // read back
    const GLenum format = GL_RGBA;
    const GLenum type = GL_UNSIGNED_BYTE;
    
    unsigned char *data = new unsigned char[height*width*4];

    glReadPixels(0, 0, width, height, format, type, data);

    // write out
    vector<int> fb_size(2);
    fb_size[0] = height; fb_size[1] = width;
    Array<FrameBuffer::P3> output(fb_size);

    vector<FrameBuffer::P3> pixels;
    output.Get(pixels);
    {
        for(unsigned int index = 0; index < pixels.size(); index++)
        {
            pixels[index].r = data[4*index + 0];
            pixels[index].g = data[4*index + 1];
            pixels[index].b = data[4*index + 2];
        }
    }
    output.Put(pixels);
    delete[] data;

    // done
    return FrameBuffer::WritePPM(output, 255, file_name);
}

int Main(int argc, char **argv)
{
    const int min_argc = 12;
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " sample_file_name output_file_name dimension domain_size (dimenion floats) num_classes object_file_name (num_classes strings) object_sizes (num_classes floats) window_size_per_unit_domain background_color (0 for black, 1 for white, string for background texture) batch_mode boundary_condition (0 for none, 1 for toroidal)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    global.output_file_name = output_file_name;
    const int dimension = atoi(argv[++argCtr]);

    if(dimension != 2)
    {
        cerr << "dimension has to be 2" << endl;
        return 1;
    }

    vector<float> domain_size(dimension);
    if((argCtr + domain_size.size()) < argc)
    {
        for(unsigned int i = 0; i < domain_size.size(); i++)
        {
            domain_size[i] = atof(argv[++argCtr]);
        }
    }
    else
    {
        cerr << "not enough input arguments for domain_size" << endl;
        return 1;
    }
    global.domain_size = domain_size;

    const int num_classes = atoi(argv[++argCtr]);
    if(num_classes < 1)
    {
        cerr << "num_classes must be >= 1" << endl;
        return 1;
    }
    global.num_classes = num_classes;

    if(argc < (min_argc + dimension + 2*num_classes - 3))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<string> object_file_names(num_classes);
    if((argCtr + object_file_names.size()) < argc)
    {
        for(unsigned int i = 0; i < object_file_names.size(); i++)
        {
            object_file_names[i] = argv[++argCtr];
        }
    }
    else
    {
        cerr << "not enough input arguments for object_file_names" << endl;
        return 1;
    }

    vector<float> object_sizes(num_classes);
    if((argCtr + object_sizes.size()) < argc)
    {
        for(unsigned int i = 0; i < object_sizes.size(); i++)
        {
            object_sizes[i] = atof(argv[++argCtr]);
        }
    }
    else
    {
        cerr << "not enough input arguments for object_sizes" << endl;
        return 1;
    }
    global.object_sizes = object_sizes;

    const int win_size_per_unit_domain = atoi(argv[++argCtr]);

    const string background_name = argv[++argCtr];
    global.background_color = atoi(background_name.c_str());
    global.batch_mode = atoi(argv[++argCtr]);

    const int boundary_condition = atoi(argv[++argCtr]); // make sure the output image is toroidal or not

    ifstream input(input_file_name); // read in samples
    cout << "read in samples " << endl;
    
    if(!input)
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }
    else
    {
        global.object_samples.clear();

        Sample sample(dimension);
        ObjectSample object_sample;

        while(input.good())
        {
            input >> sample.id;

            for(int i = 0; i < dimension; i++)
            {
                input >> sample.coordinate[i];
            }

            object_sample.sample = sample;
            object_sample.depth = Random::UniformRandom();
            object_sample.angle = 2*Math::PI*Random::UniformRandom();

            if(input.good())
            {
                global.object_samples.push_back(object_sample);
            }
        }
    }

    // swap x/y
    {
        swap(global.domain_size[0], global.domain_size[1]);

        for(unsigned int i = 0; i < global.object_samples.size(); i++)
        {
            swap(global.object_samples[i].sample.coordinate[0], global.object_samples[i].sample.coordinate[1]);
        }
    }

    cout << "total " << global.object_samples.size() << " samples read" << endl;
    
    if(boundary_condition)
    {
        vector<ObjectSample> mirror_copies;
    
        ObjectSample object_sample;

        for(int block_row = -1; block_row <= 1; block_row++)
            for(int block_col = -1; block_col <= 1; block_col++)
            {
                if((block_row == 0) && (block_col == 0)) continue;

                for(unsigned int i = 0; i < global.object_samples.size(); i++)
                {
                    object_sample = global.object_samples[i];

                    object_sample.sample.coordinate[0] += block_row*global.domain_size[0];
                    object_sample.sample.coordinate[1] += block_col*global.domain_size[1];
                    mirror_copies.push_back(object_sample);
                }
            }

        global.object_samples.insert(global.object_samples.end(), mirror_copies.begin(), mirror_copies.end());
    }

    cout << "total " << global.object_samples.size() << " samples rendered" << endl;
    
    if(strstr(output_file_name, ".fig"))
    {
        vector<FrameBuffer::L2F> l2f_samples;

        for(unsigned int i = 0; i < global.object_samples.size(); i++)
        {
            FrameBuffer::L2F sample;
            sample.color_index = global.object_samples[i].sample.id;
            sample.x = global.object_samples[i].sample.coordinate[0];
            sample.y = global.object_samples[i].sample.coordinate[1];
            
            l2f_samples.push_back(sample);
        }

        vector<FrameBuffer::Picture> palette;
        
        for(unsigned int i = 0; i < object_file_names.size(); i++)
        {
            FrameBuffer::Picture picture;

            picture.name = object_file_names[i];
            picture.width = object_sizes[i];
            picture.height = object_sizes[i];

            palette.push_back(picture);
        }

        vector<float> region(4);
        region[0] = 0; region[1] = global.domain_size[0];
        region[2] = 0; region[3] = global.domain_size[1];

        const int boundary = 0;
        const int color_scheme = 0;

        return !FrameBuffer::WriteFIG(l2f_samples, palette, region, boundary, color_scheme, output_file_name);
    }

    const int win_width = static_cast<int>(floor(global.domain_size[0]*win_size_per_unit_domain + 0.5));
    const int win_height = static_cast<int>(floor(global.domain_size[1]*win_size_per_unit_domain + 0.5));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(Keyboard);
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    glutReshapeFunc(Reshape);
    
    glewInit();

    cerr << "read and load textures" << endl;
    global.texture_ids = vector<GLuint>(num_classes);
    for(unsigned int i = 0; i < global.texture_ids.size(); i++)
    {
        global.texture_ids[i] = LoadTexture(object_file_names[i].c_str());
        if(global.texture_ids[i] <= 0)
        {
            cerr << "error in loading texture " << i << endl;
            return 1; // error
        }
    }

    if(strstr(background_name.c_str(), ".ppm"))
    {
        global.texture_ids.push_back(LoadTexture(background_name.c_str(), global.background_texture_size));
        if(global.texture_ids[global.texture_ids.size()-1] <= 0)
        {
            cerr << "error in loading background texture " << background_name << endl;
            return 1; // error
        }
    }

    global.num_aux_buffers = -1;
    glGetIntegerv(GL_AUX_BUFFERS, &global.num_aux_buffers);

    Random::InitRandomNumberGenerator();

    glutMainLoop();

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
        for(unsigned int i = 0; i < global.texture_ids.size(); i++)
        {
            GLuint tex_id = global.texture_ids[i];
            glDeleteTextures(1, &tex_id);
            global.texture_ids[i] = 0;
        }

        return 0;
    }
    catch(Exception e)
    {
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
