/*
  CgSFT.hpp

  compute Sample-Fourier-Transform on a GPU for arbitrary dimensions

  Li-Yi Wei
  11/21/2007

*/

#ifndef _CG_SFT_HPP
#define _CG_SFT_HPP

#include <GL/glew.h>
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "Exception.hpp"
#include "Sample.hpp"
#include "FrameBuffer.hpp"
#include "framebufferObject.h"

class CgSFT
{
public:
    CgSFT(const int height, const int width) throw(Exception);
    ~CgSFT(void);

    // clear traces for all samples and start over
    // freq_start indicates the frequency of the (0, 0) corner of the buffer
    // freq_increment indicates the pixel increment for any (x,y) from (0,0)
    // their lengh must be equal and indicates the dimensionality of the space
    // freq_increment must have exactly 2 > zero components for 2D rendering
    // return 1 if successful, 0 else
    int Reset(const vector<int> & freq_start, const vector<int> & freq_increment);
    
    // render samples as buffer-sized rectangles
    // and accumulate impacts upon all samples
    int Update(const vector<Sample> & samples);

    void GetColor(Array<FrameBuffer::AF> & pixels);
    
protected:
    int Update(const Sample & sample);
    
    string GetGLError(void) const;
    static void CheckError(void);

    // only called within Update()
    void Enable(void) const;
    void Disable(void) const;
    
protected:
    const int _height; 
    const int _width;
    vector<int> _freq_start;
    vector<int> _freq_increment;
    int _row_selection, _col_selection; // selecting the 2 non-zero component
    
    CGcontext _context;
    CGprofile _fragment_profile;

    CGprogram _program_main;
    CGparameter _freq_start_main;
    CGparameter _freq_increment_main;
    CGparameter _pi_main;
        
    GLuint _texture_id; // render target
    FramebufferObject _fbo;
    GLint _current_draw_buffer;
    
    static char _FRAGMENT_PROGRAM_MAIN[];
    static char _FRAGMENT_PROGRAM_DISPLAY[];

    static const float _PI;
};

#endif
