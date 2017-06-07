/*
  LloydRelaxationVis.cpp

  Li-Yi Wei
  11/01/2007

*/


#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#include "LloydRelaxation.hpp"
#include "Random.hpp"
#include "Exception.hpp"

#define _RANDOM_JITTER

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
    vector<LloydRelaxation::Point2D *> points;
    LloydRelaxation::Real x_min, x_max, y_min, y_max;
    LloydRelaxation::Real toroidal_jitter_amount;
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

template <class T>
T mod(const T a, const T b)
{
    return (a-floor(a/b)*b);
}

LloydRelaxation::Real mymod(const LloydRelaxation::Real a, const LloydRelaxation::Real b)
{
    return mod<LloydRelaxation::Real>(a, b);
}

LloydRelaxation::Point2D Jitter(const LloydRelaxation::Point2D & input, const LloydRelaxation::Point2D & jitter, const LloydRelaxation::Real x_min, const LloydRelaxation::Real x_max, const LloydRelaxation::Real y_min, const LloydRelaxation::Real y_max)
{
    LloydRelaxation::Point2D result(mymod(input.x - x_min + jitter.x, x_max-x_min) + x_min, mymod(input.y - y_min + jitter.y, y_max-y_min) + y_min);

    if((result.x < x_min) || (result.x > x_max) || (result.y < y_min) || (result.y > y_max))
    {
        cerr << "mymod(" << input.x - x_min + jitter.x << ", " << x_max-x_min << ") = " << mymod(input.x - x_min + jitter.x, x_max-x_min) << endl;
        cerr << "mymod(" << input.y - y_min + jitter.y << ", " << y_max-y_min << ") = " << mymod(input.y - y_min + jitter.y, y_max-y_min) << endl;
        cerr << "mymod(" << input.x << "-" << x_min << "+" << jitter.x << ", " << x_max << "-" << x_min << ")+" << x_min << " = " << mymod(input.x - x_min + jitter.x, x_max-x_min)+x_min << endl;
        cerr << "mymod(" << input.y << "-" << y_min << "+" << jitter.y << ", " << y_max << "-" << y_min << ")+" << y_min << " = " << mymod(input.y - y_min + jitter.y, y_max-y_min)+y_min << endl;
        cerr << "input (" << input.x << ", " << input.y << "), jitter (" << jitter.x << ", " << jitter.y << "), jittered (" << result.x << ", " << result.y << ")" << endl;
        throw Exception("jittered point out of bound");
    }

    return result;
}

void Idle()
{
    const LloydRelaxation::Real x_min = global.x_min;
    const LloydRelaxation::Real x_max = global.x_max;
    const LloydRelaxation::Real y_min = global.y_min;
    const LloydRelaxation::Real y_max = global.y_max;
    vector<LloydRelaxation::Point2D *> & points = global.points;
    
#ifdef _RANDOM_JITTER
    const LloydRelaxation::Point2D jitter(global.toroidal_jitter_amount*Random::UniformRandom()*(global.x_max - global.x_min), global.toroidal_jitter_amount*Random::UniformRandom()*(global.y_max - global.y_min));

    for(unsigned int i = 0; i < points.size(); i++)
    {
        *points[i] = Jitter(*points[i], jitter, global.x_min, global.x_max, global.y_min, global.y_max);
    }
#endif
    
    const string result = LloydRelaxation::RunOnce(x_min, x_max, y_min, y_max, points);

#ifdef _RANDOM_JITTER
    for(unsigned int i = 0; i < points.size(); i++)
    {
        *points[i] = Jitter(*points[i], LloydRelaxation::Point2D(-jitter.x, -jitter.y), global.x_min, global.x_max, global.y_min, global.y_max);
    }
#endif
    
    if(result != "")
    {
        throw Exception(result.c_str());
    }
    
    // check 
    for(unsigned int i = 0; i < points.size(); i++)
    {
        const LloydRelaxation::Point2D & point = *points[i];
        
        if((point.x < x_min) || (point.x > x_max) || (point.y < y_min) || (point.y > y_max))
        {
            cerr << "out of range point: " << point.x << ", " << point.y << endl;
            throw Exception("out of range point");
        }
            
        if(point.NumEdgeRings() > 0)
        {
            cerr << "points[" << i << "] not cleaned of edges" << endl;
            throw Exception("points not cleaned of edges");
        }
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

void DrawPoints(void)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POINTS);
    glColor3f(1, 1, 1);
    for(unsigned int i = 0; i < global.points.size(); i++)
    {
        const LloydRelaxation::Point2D & point = *global.points[i];
        glVertex2f(point.x, point.y);
    }
    glEnd();
}

// debug stuff
void DrawLines(void)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    vector<LloydRelaxation::Point2D> points;
    
    points.push_back(LloydRelaxation::Point2D(-0.12483, 0.000253576));
    points.push_back(LloydRelaxation::Point2D(-0.0373632, -0.00269916));
    points.push_back(LloydRelaxation::Point2D(-0.015926, -0.0123725));
    points.push_back(LloydRelaxation::Point2D(0.00942068, -0.0100031));
    points.push_back(LloydRelaxation::Point2D(0.0284921, -0.0097805));

    const float scale = 7;
    
    glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    for(unsigned int i = 0; i < points.size(); i++)
    {
        glVertex2f(points[i].x*scale, points[i].y*scale);
        const int j = (i+1)%points.size();
        glVertex2f(points[j].x*scale, points[j].y*scale);
    }
    glEnd();

    glBegin(GL_POINTS);
    glColor3f(1, 0, 0);
    glVertex2f(0, 0);
    glEnd();
}

void Display(void)
{
    DrawPoints();
    
    // done
    glutSwapBuffers();
}

int Main(int argc, char **argv)
{
    Random::InitRandomNumberGenerator();
    
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " num-points toroidal-jitter-amount" << endl;
        return 1;
    }

    int argCtr = 0;
    const int num_points = atoi(argv[++argCtr]);
    global.toroidal_jitter_amount = atof(argv[++argCtr]);
    
    cerr << "init points" << endl;
    global.points = vector<LloydRelaxation::Point2D *>(num_points);

    for(unsigned int i = 0; i < global.points.size(); i++)
    {
        global.points[i] = new LloydRelaxation::Point2D(Random::UniformRandom()*2-1, Random::UniformRandom()*2-1);
    }

    global.x_min = global.y_min = -1;
    global.x_max = global.y_max = 1;
        
    const int height = 512;
    const int width = 512;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(Keyboard);
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
        for(unsigned int i = 0; i < global.points.size(); i++)
        {
            delete global.points[i];
        }

        global.points.clear();
        
        return 0;
    }
    catch(Exception e)
    {
        cerr << "Error : "<< e.Message() << endl;
        return 1;
    }
}
