/*
  CgSFT.cpp

  Li-Yi Wei
  11/21/2007

*/

#include <iostream>
#include <sstream>
using namespace std;

#include "CgSFT.hpp"

const float CgSFT::_PI = 3.1415926;

CgSFT::CgSFT(const int height, const int width) throw(Exception) : _height(height), _width(width), _row_selection(-1), _col_selection(-1)
{
    // error message
    string result = "";

    // create textures
    glGenTextures(1, &_texture_id);

    const GLenum _tex_target = GL_TEXTURE_RECTANGLE_NV;
    const GLenum _tex_internal_format = GL_FLOAT_RGBA_NV; 
    {
        const GLenum filterMode = (_tex_target == GL_TEXTURE_RECTANGLE_NV) ? GL_NEAREST : GL_LINEAR;

        glBindTexture(_tex_target, _texture_id);
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
    glGetIntegerv(GL_DRAW_BUFFER, &_current_draw_buffer);
    
    _fbo.Bind();
    
    _fbo.AttachTexture(_tex_target, _texture_id, GL_COLOR_ATTACHMENT0_EXT);

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
    
    _program_main = cgCreateProgram(_context, CG_SOURCE, _FRAGMENT_PROGRAM_MAIN, _fragment_profile, "fragment", 0);

#if 0
    cout<<"---- PROGRAM BEGIN ----"<<endl;
    cout<<cgGetProgramString(_program_main, CG_COMPILED_PROGRAM);
    cout<<"---- PROGRAM END ----"<<endl;
#endif
    
    if(_program_main)
    {
        cgGLLoadProgram(_program_main);

        CheckError();
    }

    // parameters
    _freq_start_main = cgGetNamedParameter(_program_main, "freq_start");
    _freq_increment_main = cgGetNamedParameter(_program_main, "freq_increment");
    _pi_main = cgGetNamedParameter(_program_main, "pi");

    float pi = _PI;
    cgSetParameterValuefr(_pi_main, 1, &pi);
    
    // enable stuff
    cgGLEnableProfile(_fragment_profile);
    
    cgGLBindProgram(_program_main);
}

CgSFT::~CgSFT(void)
{
    cgGLDisableProfile(_fragment_profile);
    
    cgDestroyContext(_context);

    glDeleteTextures(1, &_texture_id);
}

int CgSFT::Reset(const vector<int> & freq_start, const vector<int> & freq_increment)
{
    if(freq_start.size() != freq_increment.size())
    {
        return 0;
    }
    
    _row_selection = _col_selection = -1;
    for(unsigned int i = 0; i < freq_increment.size(); i++)
    {
        if(freq_increment[i] > 0)
        {
            if(_row_selection < 0)
            {
                _row_selection = i;
            }
            else if (_col_selection < 0)
            {
                _col_selection = i;
            }
            else
            {
                // error
                return 0;
            }
        }
    }

    if((_row_selection < 0) || (_col_selection < 0))
    {
        return 0;
    }
    
    _freq_start = freq_start;
    _freq_increment = freq_increment;

    float freq_start_cg[2]; float freq_increment_cg[2];

    freq_start_cg[0] = freq_start[_row_selection];
    freq_start_cg[1] = freq_start[_col_selection];
    cgSetParameterValuefr(_freq_start_main, 2, freq_start_cg);
    
    freq_increment_cg[0] = freq_increment[_row_selection];
    freq_increment_cg[1] = freq_increment[_col_selection];
    cgSetParameterValuefr(_freq_increment_main, 2, freq_increment_cg);
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    return 1;
}

int CgSFT::Update(const vector<Sample> & samples)
{
    Enable();
    
    _fbo.Bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
    
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!Update(samples[i])) return 0;
    }
    
    FramebufferObject::Disable();
    glDrawBuffer(_current_draw_buffer);

    Disable();

    return 1;
}

int CgSFT::Update(const Sample & sample)
{
    if(sample.coordinate.Dimension() != _freq_start.size())
    {
        return 0;
    }

    // compute "fixed" component sum
    float fixed_component_sum = 0;

    for(int i = 0; i < sample.coordinate.Dimension(); i++)
    {
        if((i != _row_selection) && (i != _col_selection))
        {
            fixed_component_sum += sample.coordinate[i]*abs(_freq_start[i]);
        }
    }
    
    glMultiTexCoord3f(GL_TEXTURE0, sample.coordinate[_row_selection], sample.coordinate[_col_selection], fixed_component_sum);

    glBegin(GL_QUADS);

    glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
    glVertex2f(-1, -1);

    glMultiTexCoord2f(GL_TEXTURE1, _width, 0);
    glVertex2f(1, -1);

    glMultiTexCoord2f(GL_TEXTURE1, _width, _height);
    glVertex2f(1, 1);

    glMultiTexCoord2f(GL_TEXTURE1, 0, _height);
    glVertex2f(-1, 1);
    
    glEnd();

    return 1;
}

string CgSFT::GetGLError(void) const
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

void CgSFT::CheckError(void)
{
    CGerror error = cgGetError();

    if(error != CG_NO_ERROR)
    {
        throw Exception(cgGetErrorString(error));
    }
}

void CgSFT::Enable(void) const
{
    cgGLEnableProfile(_fragment_profile);
    
    cgGLBindProgram(_program_main);
    
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);
}

void CgSFT::Disable(void) const
{
    cgGLDisableProfile(_fragment_profile);

    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
}

// the first 2 components of tex encodes the sample location of the selected 2 dimensions, and the 3rd component encodes the sum of the remaining (inactive) dimensions computed on a CPU (\sum (position_i * frequency_i))
char CgSFT::_FRAGMENT_PROGRAM_MAIN[] =
"\n"
"struct FragmentInput\n"
"{\n"
" float2 pos : TEX1;\n"
" float4 tex : TEX0;\n"
"};\n"
"struct FragmentOutput\n"
"{\n"
" float4 col : COL;\n"
"};\n"
"\n"
"FragmentOutput fragment(FragmentInput input, uniform float2 freq_start, uniform float2 freq_increment, uniform float pi)\n"
"{\n"
" float2 freq = floor(input.pos) * freq_increment + freq_start;\n"
" freq = freq * input.tex.xy;\n"
" float sum = freq.x + freq.y + input.tex.z;\n"
" sum = 2*pi*sum;\n"
" FragmentOutput output;\n"
" output.col.r = cos(sum);\n"
" output.col.g = sin(sum);\n"
" output.col.b = 0;\n"
" output.col.a = 1;\n"
" return output;\n"
"}\n";

void CgSFT::GetColor(Array<FrameBuffer::AF> & pixels)
{
    _fbo.Bind();
    
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

    const int height = _height;
    const int width = _width;
    
    vector<int> spec(2); spec[0] = height; spec[1] = width;
    if(pixels.Dimension() != 2)
    {
        pixels = Array<FrameBuffer::AF>(spec);
    }
    else if((pixels.Size(0) != height) || (pixels.Size(1) != width))
    {
        pixels = Array<FrameBuffer::AF>(spec);
    }
    else
    {
        // do nothing as pixel array is in right dimension and size
    }

    float * internal_pixels = new float[height*width*4];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, internal_pixels);

    FrameBuffer::AF pixel;
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
        {
            const int internal_index = 4*(i*width+j);
            pixel.r = internal_pixels[internal_index+0];
            pixel.g = internal_pixels[internal_index+1];
            pixel.b = internal_pixels[internal_index+2];
            pixel.a = internal_pixels[internal_index+3];

            spec[0] = i; spec[1] = j;
            pixels.Put(spec, pixel);
        }

    delete[] internal_pixels;

    FramebufferObject::Disable();
    glReadBuffer(_current_draw_buffer);
}
