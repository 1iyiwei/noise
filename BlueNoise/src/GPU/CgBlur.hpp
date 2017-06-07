/*
 * CgBlur.hpp
 *
 * a simple blur filter implemented in Cg fragment program
 *
 * Li-Yi Wei
 * 3/10/2003
 *
 */

#pragma warning (disable:4290)

#ifndef _CG_BLUR_HPP
#define _CG_BLUR_HPP

#include <GL/glew.h>
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "Exception.hpp"
#include "framebufferObject.h"

class CgBlur
{
public:
    CgBlur(const int height, const int width) throw(Exception);
    ~CgBlur(void);

    void Update(void);
    void Display(void);
    void Reset(void);
    void Next(void);

protected:
    string GetGLError(void) const;
    static void CheckError(void);

    // only called within Update()
    void Enable(void) const;
    void Disable(void) const;

    void RenderQuad(void) const;
    void RenderGeometry(void) const;

protected:
    CGcontext _context;
    CGprogram _program;
    CGprogram _program_rect;
    CGparameter _texture;
    CGprofile _fragment_profile;
    
    static char _FRAGMENT_PROGRAM[];
    static char _FRAGMENT_PROGRAM_RECT[];

    const int _height; 
    const int _width;

    const GLenum _tex_internal_format;
    const GLenum _tex_target;

    bool _ping_pong; // indicating the texture source at current pass
    GLuint _textures[2]; // the two textures for ping-pong blur effects
    FramebufferObject _fbo;
    GLint _current_draw_buffer;
    bool _reset;
};

#endif
