/*
  CgSampling.cpp

  Li-Yi Wei
  11/21/2007

*/

#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

#include <math.h>
#include <assert.h>

#include "CgSampling.hpp"
#include "Exception.hpp"
#include "Random.hpp"
#include "MDHash.hpp"

CgSampling::TextureSize::TextureSize(void)
{
    // nothing to do
}

CgSampling::TextureSize::TextureSize(const vector<int> & input)
{ 
    if(input.size() <= 0)
    {
        throw Exception("input.size() <= 0");
    }

    // determine width and height sizes
    if((input.size()%2) == 0)
    {
        width = vector<int>(input.size()/2);
        height = vector<int>(input.size()/2);
    }
    else
    {
        vector<int> products(2);
        for(unsigned int i = 0; i < 2; i++)
        {
            products[i] = 1;
            for(int j = 0; j < input.size()/2; j++)
            {
                products[i] *= input[j+i*(input.size()/2+1)];
            }
        }

        if(products[0] > products[1])
        {
            width = vector<int>(input.size()/2);
            height = vector<int>(input.size()-input.size()/2);
        }
        else
        {
            width = vector<int>(input.size()-input.size()/2);
            height = vector<int>(input.size()/2);
        }
    }
    
    // fill stuff
    assert((width.size() + height.size()) == input.size());

    for(unsigned int i = 0; i < width.size(); i++)
    {
        width[i] = input[i];
    }
    for(unsigned int i = 0; i < height.size(); i++)
    {
        height[i] = input[i+width.size()];
    }
}

CgSampling::TextureSize::operator vector<int>(void) const
{
    vector<int> output(width.size()+height.size());

    for(unsigned int i = 0; i < width.size(); i++)
    {
        output[i] = width[i];
    }
    for(unsigned int i = 0; i < height.size(); i++)
    {
        output[i+width.size()] = height[i];
    }

    return output;
}
    
CgSampling::TextureSize::TextureSize(const vector<int> & height_input, const vector<int> & width_input) : height(height_input), width(width_input)
{
    // nothing to do, no error checking
}
        
int CgSampling::TextureSize::Dimension(void) const
{
    return height.size()+width.size();
}

int CgSampling::TextureSize::Size(const int which_dimension) const
{
    if(which_dimension < 0)
    {
        return 0;
    }
    else if (which_dimension < width.size())
    {
        return width[which_dimension];
    }
    else if (which_dimension < (width.size()+height.size()))
    {
        return height[which_dimension-width.size()];
    }
    else
    {
        return 0;
    }
}

int CgSampling::TextureSize::TotalHeight(void) const
{
    int answer = (height.size() > 0);

    for(unsigned int i = 0; i < height.size(); i++)
    {
        answer *= height[i];
    }

    return answer;
}

int CgSampling::TextureSize::TotalWidth(void) const
{
    int answer = (width.size() > 0);

    for(unsigned int i = 0; i < width.size(); i++)
    {
        answer *= width[i];
    }

    return answer;
}

void CgSampling::TextureSize::Clear(const int value)
{
    for(unsigned int i = 0; i < width.size(); i++)
    {
        width[i] = value;
    }
    for(unsigned int i = 0; i < height.size(); i++)
    {
        height[i] = value;
    }
}

/*
int CgSampling::TextureSize::TileHeight(void) const
{
    if(height.size() > 0)
    {
        return height[height.size()-1];
    }
    else
    {
        return 0;
    }
}

int CgSampling::TextureSize::TileWidth(void) const
{
    if(width.size() > 0)
    {
        return width[width.size()-1];
    }
    else
    {
        return 0;
    }
}
*/

CgSampling::CgSampling(const TextureSize & texture_size, const int clique_size, const vector<int> & initial_noise_keys) : _texture_size(texture_size), _copy_textures(RequiredDrawBuffers(texture_size.Dimension())), _noise_textures(RequiredDrawBuffers(texture_size.Dimension())), _poisson_textures(RequiredDrawBuffers(texture_size.Dimension())), _init_textures(RequiredDrawBuffers(texture_size.Dimension())), _textures(2*RequiredDrawBuffers(texture_size.Dimension())), _ping_pong(0), _clique_size(clique_size), _noise_option(initial_noise_keys.size() > 0 ? MD5_NOISE : TEXTURE_NOISE), _initial_keys(initial_noise_keys), _keys(RequiredDrawBuffers(texture_size.Dimension()))
{
    if((texture_size.TotalHeight() <= 0) || (texture_size.TotalWidth() <= 0))
    {
        throw Exception("illegal texture_size specification");
    }

    if((_initial_keys.size() > 0) && (_initial_keys.size() != RequiredDrawBuffers(texture_size.Dimension())))
    {
        throw Exception("illegal initial noise keys size");
    }
    
    cgSetErrorCallback(CheckError);
}

CgSampling::~CgSampling(void)
{
    // nothing to do
}

void CgSampling::Initialize(const vector<GLuint> & initial_value_textures)
{
    if(initial_value_textures.size() != _init_textures.size())
    {
        throw Exception("initial_value_textures.size() != _init_textures.size()");
    }
    
    glGetIntegerv(GL_DRAW_BUFFER, &_current_draw_buffer); // Save the current Draw buffer
    _fbo.Bind();  

    cgGLEnableProfile(_fragment_profile);

    if(_init_textures.size() == 1)
    {
        glDrawBuffer(_ping_pong ? GL_COLOR_ATTACHMENT1_EXT : GL_COLOR_ATTACHMENT0_EXT);
    }
    else if(_init_textures.size() == 2)
    {
        const GLenum attachment0[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT2_EXT};
        const GLenum attachment1[] = {GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT3_EXT};
        
        glDrawBuffers(2, _ping_pong ? attachment1 : attachment0);
    }
    else
    {
        throw Exception("MRT for > 8D not yet done!");
    }

    for(unsigned int i = 0; i < _init_textures.size(); i++)
    {
        cgGLSetTextureParameter(_init_textures[i], initial_value_textures[i]);
        cgGLEnableTextureParameter(_init_textures[i]);
    }
    
    cgGLBindProgram(_program_init);
    RenderQuad();

    for(unsigned int i = 0; i < _init_textures.size(); i++)
    {
        cgGLDisableTextureParameter(_init_textures[i]);
    }
    
    cgGLDisableProfile(_fragment_profile);

    FramebufferObject::Disable();
    glDrawBuffer(_current_draw_buffer); 

    CheckError();
}

void CgSampling::SetMinimumDistance(const float r_value)
{
    float local_r_value = r_value;
    cgSetParameterValuefr(_r_value, 1, &local_r_value);

    CheckError();
}

void CgSampling::SetMinimumDistance(const GLuint r_value_texture)
{
    cgGLSetTextureParameter(_r_value_texture, r_value_texture);

    CheckError();
}

void CgSampling::SetWhiteNoise(const vector<GLuint> & white_noise_textures)
{
    if(_noise_textures.size() != white_noise_textures.size())
    {
        throw Exception("noise_textures.size() != white_noise_textures.size()");
    }

    for(unsigned int i = 0; i < _noise_textures.size(); i++)
    {
        cgGLSetTextureParameter(_noise_textures[i], white_noise_textures[i]);
    }
    
    CheckError();
}

void CgSampling::Display(void) const
{
    FramebufferObject::Disable();
    glDrawBuffer(_current_draw_buffer); 

    cgGLEnableProfile(_fragment_profile);
    cgGLBindProgram(_program_display);

    cgGLSetTextureParameter(_display_texture, _textures[_ping_pong]);
    cgGLEnableTextureParameter(_display_texture);
        
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, _textures[_ping_pong]);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    
    RenderQuad();
    
    cgGLDisableProfile(_fragment_profile);
}
    
void CgSampling::GetColor(Array<FrameBuffer::AF> & pixels)
{
    _fbo.Bind();
    
    glReadBuffer(_ping_pong ? GL_COLOR_ATTACHMENT1_EXT : GL_COLOR_ATTACHMENT0_EXT);

    const int height = _texture_size.TotalHeight();
    const int width = _texture_size.TotalWidth();
        
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

void CgSampling::GetColor(Array<FrameBuffer::P3> & pixels)
{
    //_fbo.Bind();
    
    //glReadBuffer(_ping_pong ? GL_COLOR_ATTACHMENT1_EXT : GL_COLOR_ATTACHMENT0_EXT);

    FramebufferObject::Disable();
    glReadBuffer(_current_draw_buffer);

    const int height = _texture_size.TotalHeight();
    const int width = _texture_size.TotalWidth();
        
    vector<int> spec(2); spec[0] = height; spec[1] = width;
    if(pixels.Dimension() != 2)
    {
        pixels = Array<FrameBuffer::P3>(spec);
    }
    else if((pixels.Size(0) != height) || (pixels.Size(1) != width))
    {
        pixels = Array<FrameBuffer::P3>(spec);
    }
    else
    {
        // do nothing as pixel array is in right dimension and size
    }

    unsigned char * internal_pixels = new unsigned char[height*width*4];
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, internal_pixels);

    FrameBuffer::P3 pixel;
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
        {
            const int internal_index = 4*(i*width+j);
            pixel.r = internal_pixels[internal_index+0];
            pixel.g = internal_pixels[internal_index+1];
            pixel.b = internal_pixels[internal_index+2];

            spec[0] = i; spec[1] = j;
            pixels.Put(spec, pixel);
        }

    delete[] internal_pixels;
}

vector<GLuint> CgSampling::GetResultTexture(void) const
{
    vector<GLuint> results(_textures.size()/2);

    for(unsigned int i = 0; i < results.size(); i++)
    {
        results[i] = _textures[_ping_pong+2*i];
    }

    return results;
}

void CgSampling::GetSamples(vector<const Sample *> & samples)
{
    const int num_buffers = RequiredDrawBuffers();
    
    _fbo.Bind();
    
    samples.clear();

    const int height = _texture_size.TotalHeight();
    const int width = _texture_size.TotalWidth();
        
    float * pixels = new float[height*width*4];

    vector<Sample *> all_samples(height*width);
    for(unsigned int i = 0; i < all_samples.size(); i++)
    {
        all_samples[i] = new Sample(_texture_size.Dimension());
    }

    for(int k = 0; k < num_buffers; k++)
    {
        glReadBuffer(_ping_pong ? GL_COLOR_ATTACHMENT1_EXT+k*2 : GL_COLOR_ATTACHMENT0_EXT+k*2);

        glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, pixels);
        
        for(unsigned int i = 0; i < all_samples.size(); i++)
        {
            for(unsigned int j = 4*k; j < min(static_cast<int>(4*(k+1)), static_cast<int>(all_samples[i]->coordinate.Dimension())); j++)
            {
                all_samples[i]->coordinate[j] = pixels[4*i+j-4*k];
            }
        }
    }
    
    delete[] pixels;
    
    for(unsigned int i = 0; i < all_samples.size(); i++)
    {
        int all_non_negative = 1;

        for(unsigned int j = 0; j < all_samples[i]->coordinate.Dimension(); j++)
        {
            if(all_samples[i]->coordinate[j] < 0)
            {
                all_non_negative = 0;
            }
        }
        
        if(all_non_negative)
        {
            samples.push_back(all_samples[i]);
        }
        else
        {
            delete all_samples[i];
            all_samples[i] = 0;
        }    
    }

    FramebufferObject::Disable();
    glReadBuffer(_current_draw_buffer);
}

int CgSampling::RequiredDrawBuffers(void) const
{
    return RequiredDrawBuffers(_texture_size.Dimension());
}

int CgSampling::RequiredDrawBuffers(const int dimension)
{
    return static_cast<int>(ceil(dimension/4.0));
}

void CgSampling::GetTextureSize(TextureSize & answer) const
{
    answer = _texture_size;
}

int CgSampling::ConvertTextureSize(const vector<int> & input, TextureSize & output)
{
    output = TextureSize(input);

    return 1;
}

GLuint CgSampling::GenerateWhiteNoise(const TextureSize & texture_size, const GLuint white_noise_texture_id)
{
    const int height = texture_size.TotalHeight();
    const int width = texture_size.TotalWidth();
    
    GLuint new_texture_id = white_noise_texture_id;

    if(new_texture_id <= 0)
    {
        glGenTextures(1, &new_texture_id);
    }

    {
        float *texture_data = new float[height*width*4];

        for(int i = 0; i < height*width*4; i++)
        {
            texture_data[i] = Random::UniformRandom();
        }

        const GLenum tex_target = GL_TEXTURE_RECTANGLE_NV;
        const GLenum tex_internal_format = GL_FLOAT_RGBA_NV;

        const GLenum filterMode = (tex_target == GL_TEXTURE_RECTANGLE_NV) ? GL_NEAREST : GL_LINEAR;

        glBindTexture(tex_target, new_texture_id);
        glTexImage2D(tex_target, 0, tex_internal_format, width, height, 0, GL_RGBA, GL_FLOAT, texture_data);

        string result = GetGLError(); if(result != "") throw Exception(result);
    
        glTexParameterf(tex_target, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameterf(tex_target, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexParameterf(tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        delete[] texture_data;

        result = GetGLError(); if(result != "") throw Exception(result);
    }
    
    return new_texture_id;
}

vector<GLuint> CgSampling::GenerateWhiteNoise(const TextureSize & texture_size, const vector<GLuint> & white_noise_texture_id)
{
    vector<GLuint> result(white_noise_texture_id);

    for(unsigned int i = 0; i < result.size(); i++)
    {
        result[i] = GenerateWhiteNoise(texture_size, white_noise_texture_id[i]);
    }
    
    return result;
}

int CgSampling::Hash(const int input)
{
    MDHash hash;
 
    MDHash::MessageBlock input_message(input);
    MDHash::MessageBlock output_message;

    hash.Hash(input_message, output_message);
            
    const int output = output_message;

    return output;
}

GLuint CgSampling::GenerateRValueTexture(const TextureSize & texture_size, const Array<float> & data, const GLuint r_value_texture_id)
{
    if(texture_size.Dimension() != data.Dimension())
    {
        throw Exception("texture_size.Dimension() != data.Dimension()");
    }

    for(unsigned int i = 0; i < texture_size.height.size(); i++)
    {
        if(texture_size.height[i] != data.Size(i))
        {
            throw Exception("texture_size.height[i] != data.Size(i)");
        }   
    }
    for(unsigned int i = 0; i < texture_size.width.size(); i++)
    {
        if(texture_size.width[i] != data.Size(i+texture_size.height.size()))
        {
            throw Exception("texture_size.width[i] != data.size(i+texture_size.height.size())");
        }
    }
    
    const int height = texture_size.TotalHeight();
    const int width = texture_size.TotalWidth();
        
    GLuint new_texture_id = r_value_texture_id;
    
    if(new_texture_id <= 0)
    {
        glGenTextures(1, &new_texture_id);
    }
    
    {
        vector<float> raw_data;
        data.Get(raw_data);
        
        float *texture_data = new float[height*width*4];

        for(int i = 0; i < height*width; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                texture_data[4*i+j] = raw_data[i];
            }
        }

        const GLenum tex_target = GL_TEXTURE_RECTANGLE_NV;
        const GLenum tex_internal_format = GL_FLOAT_RGBA_NV;

        const GLenum filterMode = GL_LINEAR; //(tex_target == GL_TEXTURE_RECTANGLE_NV) ? GL_NEAREST : GL_LINEAR;

        glBindTexture(tex_target, new_texture_id);
        glTexImage2D(tex_target, 0, tex_internal_format, width, height, 0, GL_RGBA, GL_FLOAT, texture_data);

        string result = GetGLError(); if(result != "") throw Exception(result);
    
        glTexParameterf(tex_target, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameterf(tex_target, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexParameterf(tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        delete[] texture_data;

        result = GetGLError(); if(result != "") throw Exception(result);
    }

    return new_texture_id;
}
     
GLuint CgSampling::GenerateRenderTexture(const TextureSize & texture_size, const GLuint render_texture_id)
{ 
    const int height = texture_size.TotalHeight();
    const int width = texture_size.TotalWidth();
        
    GLuint new_texture_id = render_texture_id;

    if(new_texture_id <= 0)
    {
        glGenTextures(1, &new_texture_id);
    }

    {
        const GLenum tex_target = GL_TEXTURE_RECTANGLE_NV;
        const GLenum tex_internal_format = GL_FLOAT_RGBA_NV;

        const GLenum filterMode = (tex_target == GL_TEXTURE_RECTANGLE_NV) ? GL_NEAREST : GL_LINEAR;

        glBindTexture(tex_target, new_texture_id);
        glTexImage2D(tex_target, 0, tex_internal_format, width, height, 0, GL_RGBA, GL_FLOAT, 0);

        string result = GetGLError(); if(result != "") throw Exception(result);
    
        glTexParameterf(tex_target, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameterf(tex_target, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexParameterf(tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        result = GetGLError(); if(result != "") throw Exception(result);
    }

    return new_texture_id;
}

vector<int> CgSampling::NeighborOffsetTextureSize(const int num_neighbor_offsets)
{
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

    vector<int> answer(2);

#if 1
    answer[1] = static_cast<int>(ceil(num_neighbor_offsets*1.0/max_texture_size));
#else
    answer[1] = static_cast<int>(ceil(sqrt(num_neighbor_offsets*1.0)));
#endif
    answer[0] = static_cast<int>(ceil(num_neighbor_offsets*1.0/answer[1]));
    
    return answer;
}

int CgSampling::FlattenIntegerCoordinate(const TextureSize & texture_size, const vector<int> & input, vector<int> & output)
{
    if(texture_size.Dimension() != input.size())
    {
        // dimension mismatch
        return 0;
    }
    
    output = vector<int>(2, 0);
        
    for(unsigned int i = 0; i < texture_size.width.size(); i++)
    {
        output[0] *= texture_size.width[i];
        output[0] += input[i];
    }
    for(unsigned int i = 0; i < texture_size.height.size(); i++)
    {
        output[1] *= texture_size.height[i];
        output[1] += input[i+texture_size.width.size()];
    }

    return 1;
}

int CgSampling::UnFlattenIntegerCoordinate(const TextureSize & texture_size, const vector<int> & input, vector<int> & output)
{
    if(input.size() != 2)
    {
        // dimension mismatch
        return 0;
    }

    if((texture_size.width.size() <= 0) || (texture_size.height.size() <= 0))
    {
        return 0;
    }
    
    output = vector<int>(texture_size.Dimension(), 0);
    
    vector<int> total = input;
    
    int total_width = texture_size.TotalWidth();
    for(unsigned int i = 0; i < texture_size.width.size(); i++)
    {
        total_width /= texture_size.width[i];
        output[i] = total[0]/total_width;
        total[0] = total[0]%total_width;
    }
    
    int total_height = texture_size.TotalHeight();
    for(unsigned int i = 0; i < texture_size.height.size(); i++)
    {
        total_height /= texture_size.height[i];
        output[i+texture_size.width.size()] = total[1]/total_height;
        total[1] = total[1]%total_height;
    }

    return 1;
}
    
string CgSampling::GenerateFlattenString(const TextureSize & texture_size, const string & input_variable, const string & output_variable)
{
    const int dimension = texture_size.Dimension();
    const int num_buffers = RequiredDrawBuffers(dimension);

    ostringstream output_program;
        
    if(dimension == 2)
    {
        // no need to do anything actually
        if(input_variable == output_variable)
        {
            // do nothing
        }
        else
        {
            output_program << output_variable << " = " << input_variable << "; ";
        }
    }
    else
    {
        output_program << output_variable << " = 0; ";
        
        for(unsigned int i = 0; i < texture_size.width.size(); i++)
        {
            output_program << output_variable << GenerateComponentChar(0) << " *=" << texture_size.width[i] << "; ";
            output_program << output_variable << GenerateComponentChar(0) << " += " << input_variable;
            if(num_buffers > 1) output_program << "_" << i/4;
            output_program << GenerateComponentChar(i%4) << "; ";
        }
        for(unsigned int i = 0; i < texture_size.height.size(); i++)
        {
            output_program << output_variable << GenerateComponentChar(1) << " *=" << texture_size.height[i] << "; ";
            output_program << output_variable << GenerateComponentChar(1) << " += " << input_variable;
            if(num_buffers > 1) output_program << "_" << (i+texture_size.width.size())/4;
            output_program << GenerateComponentChar((i+texture_size.width.size())%4) << "; ";
        }
    }

    return output_program.str();
}

string CgSampling::GenerateUnFlattenString(const TextureSize & texture_size, const string & input_variable, const string & output_variable)
{
    const int dimension = texture_size.Dimension();
    const int num_buffers = RequiredDrawBuffers(dimension);

    ostringstream output_program;
        
    if(dimension == 2)
    {
        // no need to do anything actually
        if(input_variable == output_variable)
        {
            // do nothing
        }
        else
        {
            output_program << output_variable << " = " << input_variable << "; ";
        }
    }
    else
    {
        for(int k = 0; k < num_buffers; k++)
        {
            output_program << output_variable;
            if(num_buffers > 1) output_program << "_" << k;
            output_program << " = 0; ";
        }
                
        const string total(input_variable + "_total");

        output_program << "float2 " << total << " = " << input_variable << "; ";
        
        int total_width = texture_size.TotalWidth();
        for(unsigned int i = 0; i < texture_size.width.size(); i++)
        {
            total_width /= texture_size.width[i];
            output_program << output_variable;
            if(num_buffers > 1) output_program << "_" << i/4;
            output_program << GenerateComponentChar(i%4) << " = floor(" << total << GenerateComponentChar(0) << "/" << total_width << "); ";
            output_program << total << GenerateComponentChar(0) << " = " << "fmod(" << total << GenerateComponentChar(0) << ", " << total_width << "); " ;
        }   
    
        int total_height = texture_size.TotalHeight();
        for(unsigned int i = 0; i < texture_size.height.size(); i++)
        {
            total_height /= texture_size.height[i];
            output_program << output_variable;
            if(num_buffers > 1) output_program << "_" << (i+texture_size.width.size())/4;
            output_program << GenerateComponentChar((i+texture_size.width.size())%4) << " = floor(" << total << GenerateComponentChar(1) << "/" << total_height << "); ";
            output_program << total << GenerateComponentChar(1) << " = " << "fmod(" << total << GenerateComponentChar(1) << ", " << total_height << "); " ;
        }
    }

    return output_program.str();
}

string CgSampling::GenerateComponentChar(const int which_dim)
{
    string output;
    
    switch(which_dim)
    {
    case 0:
        output = ".x"; break;
    case 1:
        output = ".y"; break;
    case 2:
        output = ".z"; break;
    case 3:
        output = ".w"; break;
    default:
        throw Exception("CgSampling::GenerateComponentChar() : illegal dimension");
    }

    return output;
}

string CgSampling::GenerateCopyProgram(const TextureSize & texture_size)
{
    const int num_buffers = RequiredDrawBuffers(texture_size.Dimension());
    
    string result;
    {
        ostringstream output_program;
        output_program << 
            "\n"
            "struct FragmentInput\n"
            "{\n"
            " float4 tex : TEX0;\n"
            " float4 col : COL0;\n"
            "};\n"
            "struct FragmentOutput\n"
            "{\n";

        if(num_buffers == 1)
        {
            output_program <<
                " float4 col : COL;\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program << " float4 col" << i << " : COL" << i << ";\n";
            }
        }
        
        output_program <<
            "};\n"
            "\n"
            "FragmentOutput fragment(FragmentInput input,\n";

        if(num_buffers == 1)
        {
            output_program <<
                "                        uniform samplerRECT texture)\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    "                        uniform samplerRECT texture_" << i;
                if(i < (num_buffers-1))
                {
                    output_program << ",\n";
                }
                else
                {
                    output_program << ")\n";
                }
            }
        }
        
        output_program <<
            "{\n"
            " FragmentOutput output;\n";

        if(num_buffers == 1)
        {
            output_program <<
                " output.col = texRECT(texture, input.tex.xy);\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    " output.col" << i << " = texRECT(texture_" << i << ", input.tex.xy);\n";
            }
        }
        
        output_program <<
            " return output;\n"
            "}\n";

        result = output_program.str();
    }

    return result;
}

string CgSampling::GenerateDisplayProgram(void)
{
    string result;
    
    {
        ostringstream output_program;
        output_program <<      
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

        result = output_program.str();
    }

    return result;
}

string CgSampling::GetGLError(void)
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

#ifdef _STATIC_CONTEXT_FOR_cgGetLastListing
CGcontext CgSampling::_context;
#endif

void CgSampling::CheckError(void)
{
    CGerror error = cgGetError();

    if(error != CG_NO_ERROR)
    {
        ostringstream error_message;
        error_message << cgGetErrorString(error);
#ifdef _STATIC_CONTEXT_FOR_cgGetLastListing
        error_message<< " : " << cgGetLastListing(_context);
#endif
        throw Exception(error_message.str());
    }
}

void CgSampling::RenderQuad(void) const
{
    const int height = _texture_size.TotalHeight();
    const int width = _texture_size.TotalWidth();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    // glClear(GL_COLOR_BUFFER_BIT); // don't do this as it would destroy results between different rendering passes, such as copy and main
    glBegin(GL_QUADS);
  
    glColor3f(0.0, 0.0, 1.0);  // blue
    glTexCoord3f(0, 0, 1.0);
    glVertex2f(-1, -1);
 
    glColor3f(0.0, 1.0, 0.0);  // green
    glTexCoord3f(width, 0, 1.0);
    glVertex2f(1, -1);
  
    glColor3f(1.0, 0.0, 0.0);  // red
    glTexCoord3f(width, height, 1.0);
    glVertex2f(1, 1);
    
    glColor3f(1.0, 1.0, 0.0);  // yellow
    glTexCoord3f(0, height, 1.0);
    glVertex2f(-1, 1);
    
    glEnd();
}
