/*
 * GLslBlur.cpp
 *
 * Li-Yi Wei
 * 3/10/2003
 *
 */

#include "GLslBlur.hpp" 
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

GLslBlur::GLslBlur(const int height, const int width) throw(Exception) : _height(height), _width(width), _tex_internal_format(use_float_texture ? GL_FLOAT_RGBA_NV : GL_RGBA8), _tex_target(use_float_texture ? GL_TEXTURE_RECTANGLE_NV : GL_TEXTURE_2D), _ping_pong(0), _reset(1)
{
    // error message
    string result = "";

    // create textures
    CheckErrorsGL("BEGIN : Creating textures");
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

    CheckErrorsGL("END : Creating textures");

    // fbo stuff
    CheckErrorsGL("BEGIN : Configuring FBO");
    _fbo.Bind();
    
    _fbo.AttachTexture(_tex_target, _textures[0], GL_COLOR_ATTACHMENT0_EXT);
    _fbo.AttachTexture(_tex_target, _textures[1], GL_COLOR_ATTACHMENT1_EXT);

    // Optional: initialize depth renderbuffer
    _rb.Set(GL_DEPTH_COMPONENT24, width, height);
    _fbo.AttachRenderBuffer(_rb.GetId(), GL_DEPTH_ATTACHMENT_EXT);

    _fbo.IsValid();

    FramebufferObject::Disable();
    CheckErrorsGL("END : Configuring FBO");
    
    result = GetGLError(); if(result != "") throw Exception(result);

    // GLSL stuff
    CheckErrorsGL("BEGIN : Configuring Shader");
    _program_object = glCreateProgramObjectARB();

    _fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    glShaderSourceARB(_fragment_shader, 1, &_FRAGMENT_PROGRAM, 0);
    glCompileShaderARB(_fragment_shader);
    glAttachObjectARB(_program_object, _fragment_shader);
    
    glLinkProgramARB(_program_object);
    GLint progLinkSuccess;
    glGetObjectParameterivARB(_program_object, GL_OBJECT_LINK_STATUS_ARB,
      &progLinkSuccess);
    if(!progLinkSuccess)
    {
        throw Exception("Filter shader could not be linked");
    }

    _program_object_rect = glCreateProgramObjectARB();

    _fragment_shader_rect = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    glShaderSourceARB(_fragment_shader_rect, 1, &_FRAGMENT_PROGRAM_RECT, 0);
    glCompileShaderARB(_fragment_shader_rect);
    glAttachObjectARB(_program_object_rect, _fragment_shader_rect);
    
    glLinkProgramARB(_program_object_rect);
    glGetObjectParameterivARB(_program_object_rect, GL_OBJECT_LINK_STATUS_ARB,
      &progLinkSuccess);
    if(!progLinkSuccess)
    {
        throw Exception("Display shader could not be linked");
    }

    // Get location of the sampler uniform
    _tex_unit = glGetUniformLocationARB(_program_object, "texture");
    CheckErrorsGL("END : Configuring Shader");
    result = GetGLError(); if(result != "") throw Exception(result);
}

GLslBlur::~GLslBlur(void)
{
    // ought to do something about releasing GLSL resources...
}

void GLslBlur::Update(void)
{ 
    CheckErrorsGL("Update() : Begin");
    string result = GetGLError(); if(result != "") throw Exception(result);
  
    glGetIntegerv(GL_DRAW_BUFFER, &_current_draw_buffer); // Save the current Draw buffer

    CheckErrorsGL("Update() : after getting current draw buffer");

    _fbo.Bind();  

    if(_ping_pong == 0)
    {
        glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
    }
    else
    {
        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
    }

    CheckErrorsGL("Update() : After FBO setup");

    if(_reset)
    {
        _reset = 0;
       
        glUseProgramObjectARB(0);
        RenderGeometry();
    }
    else
    { 
        glUseProgramObjectARB(_program_object);
        glBindTexture(_tex_target, _textures[_ping_pong]);
        glUniform1iARB(_tex_unit, 0);
        glEnable(_tex_target);
        RenderQuad();
    }

    if(0) // debug
    {
        // display portion
        FramebufferObject::Disable();
        glDrawBuffer(_current_draw_buffer);
     
        glUseProgramObjectARB(_program_object);
        glBindTexture(_tex_target, _textures[!_ping_pong]);
        glUniform1iARB(_tex_unit, 0);
        glEnable(_tex_target);

        RenderQuad();
    }

    FramebufferObject::Disable();
    glDrawBuffer(_current_draw_buffer); 

    CheckErrorsGL("Update : Done");
}

void GLslBlur::Display(void)
{
    CheckErrorsGL("Display : Begin");

    FramebufferObject::Disable();
    glDrawBuffer(_current_draw_buffer); 

#ifdef _USE_FLOAT_TEXTURE
    glUseProgramObjectARB(_program_object_rect);
#endif

    glBindTexture(_tex_target, _textures[!_ping_pong]);
    glEnable(_tex_target);

    RenderQuad();

#ifdef _USE_FLOAT_TEXTURE
    glUseProgramObjectARB(0);
    // glDisable(_tex_target);
#endif

    CheckErrorsGL("Display : Done");
}

void GLslBlur::Reset(void)
{
    _reset = 1;
}

void GLslBlur::Next(void)
{
    _ping_pong = !_ping_pong;
}

void GLslBlur::RenderQuad(void) const
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

void GLslBlur::RenderGeometry(void) const
{
    glClear(GL_COLOR_BUFFER_BIT);
    
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
    
#ifdef _USE_FLOAT_TEXTURE
const char *GLslBlur::_FRAGMENT_PROGRAM = 
{
  "uniform samplerRect texture;"
    "void main(void)"
    "{"
    "   const float offset = 1.0;"
    "   vec2 texCoord = gl_TexCoord[0].xy;"
    "   vec4 c  = textureRect(texture, texCoord);"
    "   vec4 bl = textureRect(texture, texCoord + vec2(-offset, -offset));"
    "   vec4 l  = textureRect(texture, texCoord + vec2(-offset,     0.0));"
    "   vec4 tl = textureRect(texture, texCoord + vec2(-offset,  offset));"
    "   vec4 t  = textureRect(texture, texCoord + vec2(    0.0,  offset));"
    "   vec4 ur = textureRect(texture, texCoord + vec2( offset,  offset));"
    "   vec4 r  = textureRect(texture, texCoord + vec2( offset,     0.0));"
    "   vec4 br = textureRect(texture, texCoord + vec2( offset,  offset));"
    "   vec4 b  = textureRect(texture, texCoord + vec2(    0.0, -offset));"
    "   gl_FragColor = 1.0/9 * (c + bl + l + tl + t + ur + r + br + b);"
    "}"
};
#else
const char *GLslBlur::_FRAGMENT_PROGRAM = 
{
  "uniform sampler2D texture;"
    "void main(void)"
    "{"
    "   const float offset = 1.0/512;"
    "   vec2 texCoord = gl_TexCoord[0].xy;"
    "   vec4 c  = texture2D(texture, texCoord);"
    "   vec4 bl = texture2D(texture, texCoord + vec2(-offset, -offset));"
    "   vec4 l  = texture2D(texture, texCoord + vec2(-offset,     0.0));"
    "   vec4 tl = texture2D(texture, texCoord + vec2(-offset,  offset));"
    "   vec4 t  = texture2D(texture, texCoord + vec2(    0.0,  offset));"
    "   vec4 ur = texture2D(texture, texCoord + vec2( offset,  offset));"
    "   vec4 r  = texture2D(texture, texCoord + vec2( offset,     0.0));"
    "   vec4 br = texture2D(texture, texCoord + vec2( offset,  offset));"
    "   vec4 b  = texture2D(texture, texCoord + vec2(    0.0, -offset));"
    "   gl_FragColor = 1.0/9 * (c + bl + l + tl + t + ur + r + br + b);"
    "}"
};
#endif

const char *GLslBlur::_FRAGMENT_PROGRAM_RECT = 
{
  "uniform samplerRect texture;"
    "void main(void)"
    "{"
    "   const float offset = 1.0;"
    "   vec2 texCoord = gl_TexCoord[0].xy;"
    "   gl_FragColor  = textureRect(texture, texCoord);"
    "}"
};

string GLslBlur::GetGLError(void) const
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
