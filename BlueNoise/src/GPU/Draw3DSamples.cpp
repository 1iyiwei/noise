/*
  Draw3DSamples.cpp

  Li-Yi Wei
  09/09/2007

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "SFT.hpp"
#include "FrameBuffer.hpp"

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
    Global(void) : radius(0.5), color_scheme(0), bounding_cube(2), spin(0.0), spin_delta(0), output_file_name(0), current_frame(0)
        {
            bounding_cube[0] = -1;
            bounding_cube[1] = 1;
        };

    float radius;
    int color_scheme;
    vector<float> bounding_cube;
    vector<Sample> samples;
    float spin;
    float spin_delta;
    char * output_file_name;
    int current_frame; // for dumping sequence
};

Global global;

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
        WriteColor(viewport[2], viewport[3], global.output_file_name);
    }
        break;

    default:
        break;
    }
}

double mfmod(const double a, const double b)
{
    return (a - floor(a/b)*b);
}

void Idle()
{
    if(global.spin_delta != 0)
    {
        global.spin += global.spin_delta;
        global.spin = mfmod(static_cast<double>(global.spin), static_cast<double>(360.0));

#if 0
        if((global.spin_delta != 0) && (global.current_frame < 360/global.spin_delta))
        {
            ostringstream file_name;
            file_name << "gpu_3d_" << setw(3) << setfill('0') << global.current_frame << ".ppm";
            global.current_frame++;
            
            int viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            WriteColor(viewport[2], viewport[3], file_name.str().c_str());
        }
#endif
    }
    
    glutPostRedisplay();
}

void DrawQuad(void)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glBegin(GL_QUADS);
  
    glColor3f(0.0, 0.0, 1.0);  // blue

    glVertex2f(-1, -1);
 
    glColor3f(0.0, 1.0, 0.0);  // green
    glVertex2f(1, -1);
  
    glColor3f(1.0, 0.0, 0.0);  // red
    glVertex2f(1, 1);
    
    glColor3f(1.0, 1.0, 0.0);  // yellow
    glVertex2f(-1, 1);
    
    glEnd();
}

void DrawSpheres(void)
{
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
   
    GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
    
    GLfloat eye[] = {0, 0, 2 + 0.1*(global.spin_delta != 0), 0};

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, eye);
    glEnable(GL_LIGHT0);

    {
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(40.0, 1.0, 1.0, 20.0);
   
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(eye[0], eye[1], eye[2], 0, 0, 0, 0, 1, 0);
        glTranslatef(0.0, 0.0, -eye[2]);
    }
    
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glClearColor(global.color_scheme, global.color_scheme, global.color_scheme, global.color_scheme);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const int slices = 20;
    const int stacks = 20;

    glPushMatrix();
    const float center = (global.bounding_cube[1] + global.bounding_cube[0])/2;
    const float scale = 2.0/(global.bounding_cube[1]-global.bounding_cube[0]);
    glRotatef(global.spin, 0.0, 1.0, 0.0);
    glColor4f(!global.color_scheme, !global.color_scheme, !global.color_scheme, 1);
    glDisable(GL_LIGHTING);
    glutWireCube(2.0);
    glEnable(GL_LIGHTING);

    //glTranslatef(-center, -center, -center);
    for(unsigned int i = 0; i < global.samples.size(); i++)
    {
        const Sample & sample = global.samples[i];
        
        glPushMatrix();
        glTranslatef((sample.coordinate[0]-center)*scale, (sample.coordinate[1]-center)*scale, (sample.coordinate[2]-center)*scale);
        glutSolidSphere(global.radius, slices, stacks);
        glPopMatrix();
    }
    glPopMatrix();
}

void Display(void)
{
    //DrawQuad();
    DrawSpheres();
    
    // done
    glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y)
{
    if((button == 2) && (state == GLUT_DOWN)) global.spin += 2.0;
    if((button == 0) && (state == GLUT_DOWN)) global.spin -= 2.0;
    glutPostRedisplay();
}

int ReadSamples(const char *input_file_name, const int dimension, vector<Sample> & samples)
{
    ifstream input(input_file_name); // read in samples
    
    samples.clear();
    
    if(!input)
    {
        return 0;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
            for(int i = 0; i < dimension; i++)
            {
                input >> sample.coordinate[i];
            }
            
            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

    return 1;
}

int WriteColor(const GLsizei width, const GLsizei height, const char * file_name)
{
    // read back
    const GLenum format = GL_RGBA;
    const GLenum type = GL_UNSIGNED_BYTE;
    
    unsigned char *data = new unsigned char[height*width*4];

    glReadPixels(0, 0, width, height, format, type, data);

    // write out
    vector< vector<FrameBuffer::P3> > pixels(height);
    {
        for(int i = 0; i < height; i++)
        {
            pixels[i] = vector<FrameBuffer::P3>(width);
        }
    }

    {
        int index = 0;
        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j++)
            {
                pixels[i][j].r = data[index++];
                pixels[i][j].g = data[index++];
                pixels[i][j].b = data[index++];
                index++;
            }
        }
    }

    delete[] data;

    // done
    return FrameBuffer::WritePPM(pixels, 255, file_name);
}


int Main(int argc, char **argv)
{
    if(argc < 9)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name output-file-name dimension radius color_scheme (0 for black background, 1 for white background) box_min box_max spin_delta (degrees, 0 for static)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    global.output_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    global.radius = atof(argv[++argCtr]);
    global.color_scheme = atoi(argv[++argCtr]);
    global.bounding_cube[0] = atof(argv[++argCtr]);
    global.bounding_cube[1] = atof(argv[++argCtr]);
    global.spin_delta = atof(argv[++argCtr]);
    
    if(dimension != 3)
    {
        cerr << "can handle only 3D!" << endl;
        return 1;
    }
    
    if(! ReadSamples(input_file_name, dimension, global.samples))
    {
        cerr << "failure to read from " << input_file_name << endl;
        return 1;
    }
    
    const int height = (global.spin_delta == 0 ? 512 : 480);
    const int width = (global.spin_delta == 0 ? 512 : 480);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    
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
        return 0;
    }
    catch(Exception e)
    {
        cerr << "Error : "<< e.Message() << endl;
        return 1;
    }
}
