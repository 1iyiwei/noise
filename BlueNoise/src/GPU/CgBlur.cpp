/*
 * CgBlur.cpp
 *
 * Li-Yi Wei
 * 3/10/2003
 *
 */

#include "CgBlur.hpp" 
#include "glErrorUtil.h"

#include <iostream>
#include <sstream>
using namespace std;

#define _USE_FLOAT_TEXTURE

#ifdef _USE_FLOAT_TEXTURE
static const bool use_float_texture = 1;
#else
static const bool use_float_texture = 0;
#endif

CgBlur::CgBlur(const int height, const int width) throw(Exception) : _height(height), _width(width), _tex_internal_format(use_float_texture ? GL_FLOAT_RGBA_NV : GL_RGBA8), _tex_target(use_float_texture ? GL_TEXTURE_RECTANGLE_NV : GL_TEXTURE_2D), _ping_pong(0), _reset(1)
{
    // error message
    string result = "";

    // create textures
    glGenTextures(2, _textures);

    for(int i = 0; i < 2; i++)
    {
        const GLenum filterMode = (_tex_target == GL_TEXTURE_RECTANGLE_NV) ? GL_NEAREST : GL_LINEAR;

        glBindTexture(_tex_target, _textures[i]);
        glTexImage2D(_tex_target, 0, _tex_internal_format, width, height, 0, GL_RGBA, GL_FLOAT, 0);

        result = GetGLError(); if(result != "") throw Exception(result);
    
        glTexParameterf(_tex_target, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameterf(_tex_target, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexParameterf(_tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(_tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, _tex_target, _textures[i], 0);

        result = GetGLError(); if(result != "") throw Exception(result);
    }

    // fbo stuff
    _fbo.Bind();
    
    _fbo.AttachTexture(_tex_target, _textures[0], GL_COLOR_ATTACHMENT0_EXT);
    _fbo.AttachTexture(_tex_target, _textures[1], GL_COLOR_ATTACHMENT1_EXT);

    _fbo.IsValid();

    FramebufferObject::Disable();
    
    result = GetGLError(); if(result != "") throw Exception(result);

    // Cg stuff
    cgSetErrorCallback(CheckError);
    
    _context = cgCreateContext();
    
    if(! _context)
    {
        throw Exception("null context");
    }

    _fragment_profile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(_fragment_profile);
    
    _program = cgCreateProgram(_context, CG_SOURCE, _FRAGMENT_PROGRAM, _fragment_profile, "fragment", 0);

#if 0
    cout<<"---- PROGRAM BEGIN ----"<<endl;
    cout<<cgGetProgramString(_program, CG_COMPILED_PROGRAM);
    cout<<"---- PROGRAM END ----"<<endl;
#endif
    
    if(_program)
    {
        cgGLLoadProgram(_program);

        CheckError();
    }

    _program_rect = cgCreateProgram(_context, CG_SOURCE, _FRAGMENT_PROGRAM_RECT, _fragment_profile, "fragment", 0);

#if 0
    cout<<"---- PROGRAM BEGIN ----"<<endl;
    cout<<cgGetProgramString(_program_rect, CG_COMPILED_PROGRAM);
    cout<<"---- PROGRAM END ----"<<endl;
#endif
    
    if(_program_rect)
    {
        cgGLLoadProgram(_program_rect);

        CheckError();
    }

    // texture
    _texture = cgGetNamedParameter(_program, "texture");

    for(int i = 0; i < 2; i++)
    {
        cgGLSetTextureParameter(_texture, _textures[i]);
        cgGLEnableTextureParameter(_texture);
    }

    // enable stuff
    cgGLEnableProfile(_fragment_profile);
    
    cgGLBindProgram(_program);
}

CgBlur::~CgBlur(void)
{
    cgGLDisableTextureParameter(_texture);
    
    cgGLDisableProfile(_fragment_profile);
    
    cgDestroyContext(_context);
}

void CgBlur::Update(void)
{
    CheckErrorsGL("BEGIN : Update()");
    Enable();

    glGetIntegerv(GL_DRAW_BUFFER, &_current_draw_buffer); // Save the current Draw buffer
    _fbo.Bind();  

    if(_ping_pong == 0)
    {
        glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
    }
    else
    {
        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
    }

    CheckErrorsGL("BEGIN : After FBO setup");

    if(_reset)
    {
        _reset = 0;
        Disable();
        RenderGeometry();
    }
    else
    {
        cgGLBindProgram(_program);
        cgGLSetTextureParameter(_texture, _textures[_ping_pong]);
        cgGLEnableTextureParameter(_texture);
  
        RenderQuad();
    }

    if(0) // debug
    {
        // display portion
        FramebufferObject::Disable();
        cgGLSetTextureParameter(_texture, _textures[!_ping_pong]);
        cgGLEnableTextureParameter(_texture);
        glDrawBuffer(_current_draw_buffer); 
        RenderQuad();
    }

    Disable();
    FramebufferObject::Disable();
    glDrawBuffer(_current_draw_buffer); 

    CheckErrorsGL("BEGIN : After Update()");
}

void CgBlur::Display(void)
{
    FramebufferObject::Disable();
    glDrawBuffer(_current_draw_buffer); 

#ifdef _USE_FLOAT_TEXTURE
    Enable();
    cgGLBindProgram(_program_rect);
    //cgGLSetTextureParameter(_texture, _textures[!_ping_pong]);
    //cgGLEnableTextureParameter(_texture);
#endif

    glBindTexture(_tex_target, _textures[!_ping_pong]);
    glEnable(_tex_target);

    RenderQuad();

#ifdef _USE_FLOAT_TEXTURE
    Disable();
    // glDisable(_tex_target);
#endif
}

void CgBlur::Reset(void)
{
    _reset = 1;
}

void CgBlur::Next(void)
{
    _ping_pong = !_ping_pong;
}

void CgBlur::RenderQuad(void) const
{
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
  
    glColor3f(0.0, 0.0, 1.0);  // blue
    glTexCoord3f(0, 0, 1.0);
    glVertex2f(-1, -1);
 
    glColor3f(0.0, 1.0, 0.0);  // green
#ifdef _USE_FLOAT_TEXTURE
    glTexCoord3f(_width, 0, 1.0);
#else
    glTexCoord3f(1.0, 0, 1.0);
#endif
    glVertex2f(1, -1);
  
    glColor3f(1.0, 0.0, 0.0);  // red
#ifdef _USE_FLOAT_TEXTURE
    glTexCoord3f(_width, _height, 1.0);
#else
    glTexCoord3f(1.0, 1.0, 1.0);
#endif
    glVertex2f(1, 1);
    
    glColor3f(1.0, 1.0, 0.0);  // yellow
#ifdef _USE_FLOAT_TEXTURE
    glTexCoord3f(0, _height, 1.0);
#else
    glTexCoord3f(0, 1.0, 1.0);
#endif
    glVertex2f(-1, 1);
    
    glEnd();
}

void CgBlur::RenderGeometry(void) const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    const int num_quads = 16;
    const float quad_size = 2.0/num_quads;
    
    glBegin(GL_QUADS);

    float color = 0;
    
    for(float x_min = -1.0; x_min < 1.0; x_min += quad_size)
    {
        for(float y_min = -1.0; y_min < 1.0; y_min += quad_size)
        {
            glColor3f(color, color, 1.0-color);
            glVertex2f(x_min, y_min);
            glVertex2f(x_min+quad_size, y_min);
            glVertex2f(x_min+quad_size, y_min+quad_size);
            glVertex2f(x_min, y_min+quad_size);
            
            color = 1.0 - color;
        }

        color = 1.0 - color;
    }
    
    glEnd();
}

void CgBlur::CheckError(void) 
{
    CGerror error = cgGetError();

    if(error != CG_NO_ERROR)
    {
        throw Exception(cgGetErrorString(error));
    }
}
    
void CgBlur::Enable(void) const
{
    for(int i = 0; i < 2; i++)
    {
        cgGLSetTextureParameter(_texture, _textures[i]);
        cgGLEnableTextureParameter(_texture);
    }

    cgGLEnableProfile(_fragment_profile);
    
    cgGLBindProgram(_program);
}

void CgBlur::Disable(void) const
{
    for(int i = 0; i < 2; i++)
    {
        cgGLSetTextureParameter(_texture, _textures[i]);
        cgGLDisableTextureParameter(_texture);
    }
    
    cgGLDisableProfile(_fragment_profile);
}
    
#ifdef _USE_FLOAT_TEXTURE
char CgBlur::_FRAGMENT_PROGRAM[] =
"\n"
"struct FragmentInput\n"
"{\n"
" float4 tex : TEX0;\n"
" float4 col : COL0;\n"
"};\n"
"struct FragmentOutput\n"
"{\n"
" float4 col : COL;\n"
"};\n"
"\n"
"FragmentOutput fragment(FragmentInput input,\n"
"                        uniform samplerRECT texture)\n"
"{\n"
" FragmentOutput output;\n"
" float4 result = float4(0, 0, 0, 0);\n"
" for(int i = -1; i <= 1; i++)\n"
"   for(int j = -1; j <= 1; j++)\n"
"   {\n"
"      result += \n"
"           texRECT(texture, input.tex.xy + input.tex.z * float2(i, j))/9;\n"
"   }\n"
" output.col = result;\n"
" return output;\n"
"}\n";
#else
char CgBlur::_FRAGMENT_PROGRAM[] =
"\n"
"struct FragmentInput\n"
"{\n"
" float4 tex : TEX0;\n"
" float4 col : COL0;\n"
"};\n"
"struct FragmentOutput\n"
"{\n"
" float4 col : COL;\n"
"};\n"
"\n"
"FragmentOutput fragment(FragmentInput input,\n"
"                        uniform sampler2D texture)\n"
"{\n"
" FragmentOutput output;\n"
" float4 result = float4(0, 0, 0, 0);\n"
" for(int i = -1; i <= 1; i++)\n"
"   for(int j = -1; j <= 1; j++)\n"
"   {\n"
"      result += \n"
"           tex2D(texture, input.tex.xy + input.tex.z * float2(i/512.0, j/512.0))/9;\n"
"   }\n"
" output.col = result;\n"
" return output;\n"
"}\n";
#endif

char CgBlur::_FRAGMENT_PROGRAM_RECT[] =
"\n"
"struct FragmentInput\n"
"{\n"
" float4 tex : TEX0;\n"
" float4 col : COL0;\n"
"};\n"
"struct FragmentOutput\n"
"{\n"
" float4 col : COL;\n"
"};\n"
"\n"
"FragmentOutput fragment(FragmentInput input,\n"
"                        uniform samplerRECT texture)\n"
"{\n"
" FragmentOutput output;\n"
" output.col = texRECT(texture, input.tex.xy);\n"
" return output;\n"
"}\n";

string CgBlur::GetGLError(void) const
{
    string result;
    
    const GLenum err = glGetError();

    if(err != GL_NO_ERROR)
    {
        ostringstream error_message;
        error_message << "GL error: " << gluErrorString(err);
        result = error_message.str();
    }

    return result;
}
