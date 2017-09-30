/*
 * MD5.cpp
 *
 * Li-Yi Wei
 * 12/16/2007
 *
 */

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>

#include <time.h>

#include "CgMD5.hpp"

class ExitException : public Exception
{
public:
    ExitException(const string & message);
};

ExitException::ExitException(const string & message) : Exception(message)
{
    // nothing to do
}

int animate = 0;
unsigned long key = 0;
CgMD5 * pMD5 = 0;

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        throw ExitException("exit");
        break;
        
    default:
        glutPostRedisplay();
        break;
    }
}

void Idle()
{
    if(pMD5)
    {
        pMD5->SetKey(key);
    }

    if(animate) key++;
    
    glutPostRedisplay();
}

void RenderQuad(void)
{
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

void Display(void)
{
    RenderQuad();
    
    glutSwapBuffers();
}

void Reshape(int width, int height)
{
    if(pMD5)
    {
        pMD5->SetSize(height, width);
    }

    glViewport(0, 0, width, height);
}

int Main(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " window_size animate" << endl;
        return 1;
    }

    int argCtr = 0;
    const int windowSize = atoi(argv[++argCtr]);
    animate = atoi(argv[++argCtr]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowSize, windowSize);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(Keyboard);
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    glutReshapeFunc(Reshape);
    
    glewInit();

    string result;

    if(!glewIsSupported( "GL_VERSION_2_0 GL_ARB_fragment_program GL_EXT_framebuffer_object GL_NV_float_buffer GL_EXT_gpu_shader4" )) 
    {
        ostringstream error_message;

        error_message << "Error: failed to get minimal extensions for demo\n";
        error_message << "This sample requires:\n";
        error_message << "  OpenGL version 2.0\n";
        error_message << "  GL_ARB_fragment_program\n";
        error_message << "  GL_EXT_framebuffer_object";
        error_message << "  GL_EXT_gpu_shader4" << endl;

        result = error_message.str();

        if(result != "") throw Exception(result);
    }

    key = time(0);
    pMD5 = new CgMD5();
        
    glutMainLoop();

    delete pMD5;
    
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
        delete pMD5;
        
        return 0;
    }
    catch(Exception e)
    {
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
