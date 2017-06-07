/*
 * GLslBlur.hpp
 *
 * a simple blur filter implemented in Cg fragment program
 *
 * Li-Yi Wei
 * 7/30/2003
 *
 */

#pragma warning (disable:4290)

#ifndef _GLSL_BLUR_HPP
#define _GLSL_BLUR_HPP

#include <GL/glew.h>
#include <GL/glut.h>

#include "Exception.hpp"
#include "framebufferObject.h"
#include "renderbuffer.h"

class GLslBlur
{
public:
    GLslBlur(const int height, const int width) throw(Exception);
    ~GLslBlur(void);

    void Update(void);
    void Display(void);
    void Reset(void);
    void Next(void);

protected:
    string GetGLError(void) const;
   
    void RenderQuad(void) const;
    void RenderGeometry(void) const;

protected:    
    GLhandleARB   _program_object;    // the program used to update
    GLhandleARB   _fragment_shader; 
    GLhandleARB   _program_object_rect;    // the program used to display
    GLhandleARB   _fragment_shader_rect;
    GLint         _tex_unit; 

    static const char *_FRAGMENT_PROGRAM;
    static const char *_FRAGMENT_PROGRAM_RECT;

    const int _height; 
    const int _width;

    const GLenum _tex_internal_format;
    const GLenum _tex_target;

    bool _ping_pong; // indicating the texture source at current pass
    GLuint _textures[2]; // the two textures for ping-pong blur effects
    FramebufferObject _fbo;
    Renderbuffer _rb;	
    GLint _current_draw_buffer;
    bool _reset;
};

#endif
