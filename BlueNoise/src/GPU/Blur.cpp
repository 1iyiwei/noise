/*
  Blur.cpp

  an exercise for FBO

  Li-Yi Wei
  07/29/2007

*/

#include <iostream>
#include <sstream>
using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>

// #define _USE_GLSL_BLUR

#include "CgBlur.hpp"
#include "GLslBlur.hpp"
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

#ifdef _USE_GLSL_BLUR
GLslBlur * p_blur = 0;
#else
CgBlur * p_blur = 0;
#endif

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        throw ExitException("exit");
        break;
        
    default:
        p_blur->Reset();
        break;
    }
}

void Idle()
{
    glutPostRedisplay();
}

void Display(void)
{
    p_blur->Update();
    p_blur->Display();
    p_blur->Next();

    // done
    glutSwapBuffers();
}

int Main(int argc, char **argv)
{
    const int height = 512;
    const int width = 512;

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

#ifdef _USE_GLSL_BLUR
    p_blur = new GLslBlur(height, width);
#else
    p_blur = new CgBlur(height, width);
#endif

    glutMainLoop();

    delete p_blur; p_blur = 0;

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
