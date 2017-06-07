/*
  CgPoisson.cpp

  Li-Yi Wei
  07/31/2007

*/

#include "CgPoisson.hpp"
#include "CgMD5.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

#include <math.h>

CgPoisson::CgPoisson(const TextureSize & texture_size, const float cell_size, const int clique_size, const vector< vector<int> > & input_neighbor_offsets, const DistanceField::Boundary boundary_condition, const NeighborOffsetStorage neighbor_offset_storage, const vector<int> & initial_noise_keys, const TextureSize & r_value_texture_size) : CgSampling(texture_size, clique_size, initial_noise_keys), _current_phases(RequiredDrawBuffers(texture_size.Dimension())), _neighbor_offset_texture(0)
{
    const DistanceFieldOption distance_field_option = r_value_texture_size.TotalWidth()*r_value_texture_size.TotalHeight() == 0 ? UNIFORM : ADAPTIVE;
    
    // error checking
    const int dimension = texture_size.Dimension();
    
    if(clique_size <= 0)
    {
        throw Exception("clique size must be > 0");
    }

    if(input_neighbor_offsets.size() <= 0)
    {
        throw Exception("no neighbor offsets");
    }

    for(unsigned int i = 0; i < input_neighbor_offsets.size(); i++)
    {
        if(input_neighbor_offsets[i].size() != dimension)
        {
            throw Exception("neighbor offset has wrong dimension");
        }
    }

    {
        GLint max_buffers;
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &max_buffers);

        if(max_buffers < RequiredDrawBuffers(dimension))
        {
            throw Exception("not enough GL_MAX_DRAW_BUFFERS");
        }

        // cerr << "max_buffers " << max_buffers << endl; // debug
    }
    
    vector< vector<int> > neighbor_offsets;
    if(! FlattenNeighborOffsets(texture_size, clique_size, input_neighbor_offsets, neighbor_offsets))
    {
        throw Exception("error in flattening neighbor offsets");
    }
        
    string result;

    // internal variable size consistency
    {
        if(_copy_textures.size() != _current_phases.size())
        {
            throw Exception("_copy_textures.size() != _current_phases.size()");
        }
    
        if(_copy_textures.size() != _noise_textures.size())
        {
            throw Exception("_copy_textures.size() != _noise_textures.size()");
        }

        if(_copy_textures.size() != _poisson_textures.size())
        {
            throw Exception("_copy_textures.size() != _poisson_textures.size()");
        }

        if(_copy_textures.size() != _init_textures.size())
        {
            throw Exception("_copy_textures.size() != _init_textures.size()");
        }

        if(2*_copy_textures.size() != _textures.size())
        {
            throw Exception("2*_copy_textures.size() != _textures.size()");
        }
    }
    
    // create textures
    for(unsigned int i = 0; i < _textures.size(); i++)
    {
        _textures[i] = 0;
        _textures[i] = GenerateRenderTexture(texture_size, _textures[i]);
    }

    // make sure this is consistent with GenerateRenderTexture
    const GLenum tex_target = GL_TEXTURE_RECTANGLE_NV;
    // const GLenum tex_internal_format = GL_FLOAT_RGBA_NV;

    // fbo stuff
    _fbo.Bind();

    for(unsigned int i = 0; i < _textures.size(); i++)
    {
        _fbo.AttachTexture(tex_target, _textures[i], GL_COLOR_ATTACHMENT0_EXT+i);
        // _fbo.AttachTexture(tex_target, _textures[1], GL_COLOR_ATTACHMENT1_EXT);
    }
    
    _fbo.IsValid();

    FramebufferObject::Disable();
    
    result = GetGLError(); if(result != "") throw Exception(result);

    // Cg stuff
    // cgSetErrorCallback(CheckError);
    
    _context = cgCreateContext();
    
    if(! _context)
    {
        throw Exception("null context");
    }

    _fragment_profile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(_fragment_profile);
    
    // copy program
    const string copy_program = GenerateCopyProgram(texture_size);

#if 0
    cout << copy_program.c_str() << endl;
#endif
    
    _program_copy = cgCreateProgram(_context, CG_SOURCE, copy_program.c_str(), _fragment_profile, "fragment", 0);

#if 0
    cout<<"---- BEGIN program copy ----"<<endl;
    cout<<cgGetProgramString(_program_copy, CG_COMPILED_PROGRAM);
    cout<<"---- END program copy ----"<<endl;
#endif
    
    if(_program_copy)
    {
        cgGLLoadProgram(_program_copy);

        CheckError();
    }

    // main program
    vector<int> num_neighborhood_offsets;

    if(neighbor_offset_storage == NEIGHBOR_OFFSET_PROGRAM_CONSTANT)
    {
        num_neighborhood_offsets.push_back(neighbor_offsets.size());
    }
    else //if(neighbor_offset_storage == NEIGHBOR_OFFSET_TEXTURE)
    {
        num_neighborhood_offsets = NeighborOffsetTextureSize(neighbor_offsets.size());
        if(1)
        {
            cerr << "NeighborOffsetTextureSize() = ";
            for(unsigned int i = 0; i < num_neighborhood_offsets.size(); i++)
            {
                cerr << num_neighborhood_offsets[i] << " ";
            }
            cerr << endl;
        }
    }
    const string main_program = GenerateMainProgram(texture_size, clique_size, num_neighborhood_offsets, boundary_condition, _noise_option, r_value_texture_size);

#if 0
    cout << main_program.c_str() << endl;
#endif
    
    _program_main = cgCreateProgram(_context, CG_SOURCE, main_program.c_str(), _fragment_profile, "fragment", 0);

#if 0
    cout<<"---- BEGIN program main ----"<<endl;
    cout<<cgGetProgramString(_program_main, CG_COMPILED_PROGRAM);
    cout<<"---- END program main ----"<<endl;
#endif
    
    if(_program_main)
    {
        cgGLLoadProgram(_program_main);

        CheckError();
    }

    // init program
    const string init_program = GenerateInitProgram(texture_size, clique_size);

#if 0
    cout << init_program.c_str() << endl;
#endif
    
    _program_init = cgCreateProgram(_context, CG_SOURCE, init_program.c_str(), _fragment_profile, "fragment", 0);

#if 0
    cout<<"---- BEGIN program init ----"<<endl;
    cout<<cgGetProgramString(_program_init, CG_COMPILED_PROGRAM);
    cout<<"---- END program init ----"<<endl;
#endif
    
    if(_program_init)
    {
        cgGLLoadProgram(_program_init);

        CheckError();
    }

    // display program
    const string display_program = GenerateDisplayProgram();

#if 0
    cout << display_program.c_str() << endl;
#endif
    
    _program_display = cgCreateProgram(_context, CG_SOURCE, display_program.c_str(), _fragment_profile, "fragment", 0);

#if 0
    cout<<"---- BEGIN program display ----"<<endl;
    cout<<cgGetProgramString(_program_display, CG_COMPILED_PROGRAM);
    cout<<"---- END program display ----"<<endl;
#endif
    
    if(_program_display)
    {
        cgGLLoadProgram(_program_display);

        CheckError();
    }

    // bind parameters
    if(_copy_textures.size() == 1)
    {
        _copy_textures[0] = cgGetNamedParameter(_program_copy, "texture");
    }
    else if(_copy_textures.size() == 2)
    {
        _copy_textures[0] = cgGetNamedParameter(_program_copy, "texture_0");
        _copy_textures[1] = cgGetNamedParameter(_program_copy, "texture_1");
    }
    else
    {
        throw Exception("MRT for > 8D not yet done!");
    }

    if(distance_field_option == UNIFORM)
    {
        _r_value = cgGetNamedParameter(_program_main, "r_value");
        _r_value_texture = 0;
    }
    else
    {
        _r_value = 0;
        _r_value_texture = cgGetNamedParameter(_program_main, "r_value_texture");
    }
    
    _cell_size_main = cgGetNamedParameter(_program_main, "cell_size");
    _clique_size_main = cgGetNamedParameter(_program_main, "clique_size");

    if(_current_phases.size() == 1)
    {
        _current_phases[0] = cgGetNamedParameter(_program_main, "current_phase");
    }
    else if(_current_phases.size() == 2)
    {
        _current_phases[0] = cgGetNamedParameter(_program_main, "current_phase_0");
        _current_phases[1] = cgGetNamedParameter(_program_main, "current_phase_1");
    }
    else
    {
        throw Exception("MRT for > 8D not yet done!");
    }
    _neighbor_offsets = cgGetNamedParameter(_program_main, "neighbor_offsets");
    
    if(_noise_textures.size() == 1)
    {
        if(_noise_option == TEXTURE_NOISE)
        {
            _noise_textures[0] = cgGetNamedParameter(_program_main, "noise_texture");
        }
        else if(_noise_option == MD5_NOISE)
        {
            _keys[0] = cgGetNamedParameter(_program_main, "key");
        }
        else
        {
            throw Exception("unknown noise option");
        }
    }
    else if(_noise_textures.size() == 2)
    {
        if(_noise_option == TEXTURE_NOISE)
        {
            _noise_textures[0] = cgGetNamedParameter(_program_main, "noise_texture_0");
            _noise_textures[1] = cgGetNamedParameter(_program_main, "noise_texture_1");
        }
        else if(_noise_option == MD5_NOISE)
        {
            _keys[0] = cgGetNamedParameter(_program_main, "key_0");
            _keys[1] = cgGetNamedParameter(_program_main, "key_1");
        }
        else
        {
            throw Exception("unknown noise option");
        }
    }
    else
    {
        throw Exception("MRT for > 8D not yet done!");
    }
    
    if(_poisson_textures.size() == 1)
    {
        _poisson_textures[0] = cgGetNamedParameter(_program_main, "poisson_texture");
    }
    else if(_poisson_textures.size() == 2)
    {
        _poisson_textures[0] = cgGetNamedParameter(_program_main, "poisson_texture_0");
        _poisson_textures[1] = cgGetNamedParameter(_program_main, "poisson_texture_1");
    }
    else
    {
        throw Exception("MRT for > 8D not yet done!");
    }
    _cell_size_init = cgGetNamedParameter(_program_init, "cell_size");
    if(_init_textures.size() == 1)
    {
        _init_textures[0] = cgGetNamedParameter(_program_init, "texture");
    }
    else if(_init_textures.size() == 2)
    {
        _init_textures[0] = cgGetNamedParameter(_program_init, "texture_0");
        _init_textures[1] = cgGetNamedParameter(_program_init, "texture_1");
    }
    else
    {
        throw Exception("MRT for > 4D not yet done!");
    }
    _display_texture = cgGetNamedParameter(_program_display, "texture");

    // set parameters
    float local_cell_size = cell_size;
    cgSetParameterValuefr(_cell_size_main, 1, &local_cell_size);
    cgSetParameterValuefr(_cell_size_init, 1, &local_cell_size);

    int local_clique_size = clique_size;
    cgSetParameterValueir(_clique_size_main, 1, &local_clique_size);

    if(neighbor_offset_storage == NEIGHBOR_OFFSET_PROGRAM_CONSTANT)
    {
        float *neighbor_offsets_array = new float[2*neighbor_offsets.size()];
        for(unsigned int i = 0; i < neighbor_offsets.size(); i++)
            for(unsigned int j = 0; j < 2; j++)
            {
                neighbor_offsets_array[2*i+j] = neighbor_offsets[i][j];
            }
        cgSetParameterValuefr(_neighbor_offsets, 2*neighbor_offsets.size(), neighbor_offsets_array);
        delete[] neighbor_offsets_array;
    }
    else if (neighbor_offset_storage == NEIGHBOR_OFFSET_TEXTURE)
    {
        _neighbor_offset_texture = GenerateNeighborOffsetTexture(neighbor_offsets, _neighbor_offset_texture, NeighborOffsetTextureSize(neighbor_offsets.size()));

        cgGLSetTextureParameter(_neighbor_offsets, _neighbor_offset_texture);
        cgGLEnableTextureParameter(_neighbor_offsets);
    }
    else
    {
        throw Exception("unsupported neighbor offset storage");
    }
    
    CheckError();

    // max exec instructions
    if(0)
    {
        int max_program_exe_instructions;
        glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_EXEC_INSTRUCTIONS_NV, &max_program_exe_instructions);
        cerr << "GL_MAX_PROGRAM_EXEC_INSTRUCTIONS_NV " << max_program_exe_instructions << endl;

        int max_program_loop_count;
        glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_LOOP_COUNT_NV, &max_program_loop_count);
        cerr << "GL_MAX_PROGRAM_LOOP_COUNT_NV " << max_program_loop_count << endl;
    }
}

CgPoisson::~CgPoisson(void)
{
    GLuint * texture_array = new GLuint[_textures.size()];
    for(unsigned int i = 0; i < _textures.size(); i++)
    {
        texture_array[i] = _textures[i];
    }
    glDeleteTextures(_textures.size(), texture_array);
    delete[] texture_array;
    
    if(_neighbor_offset_texture > 0) glDeleteTextures(1, &_neighbor_offset_texture);
    
    cgGLDisableProfile(_fragment_profile);
    
    cgDestroyContext(_context);
}


void CgPoisson::Update(const vector<int> & current_phase)
{ 
    if(current_phase.size() != (_texture_size.Dimension()))
    {
        throw Exception("current_phase has wrong dimensionality");
    }

#if 0
    if(current_phase.size() > 4)
    {
        throw Exception("current_phase only support up to 4D now!");
    }
#endif
    
    glGetIntegerv(GL_DRAW_BUFFER, &_current_draw_buffer); // Save the current Draw buffer
    _fbo.Bind();  

    cgGLEnableProfile(_fragment_profile);

    if(_poisson_textures.size() == 1)
    {
        glDrawBuffer(_ping_pong ? GL_COLOR_ATTACHMENT0_EXT : GL_COLOR_ATTACHMENT1_EXT);
    }
    else if(_poisson_textures.size() == 2)
    {
        const GLenum attachment0[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT2_EXT};
        const GLenum attachment1[] = {GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT3_EXT};
        
        glDrawBuffers(2, _ping_pong ? attachment0 : attachment1);
    }
    else
    {
        throw Exception("MRT for > 8D not yet done!");
    }

    for(unsigned int i = 0; i < _poisson_textures.size(); i++)
    {
        cgGLSetTextureParameter(_poisson_textures[i], _textures[_ping_pong+2*i]);
        cgGLEnableTextureParameter(_poisson_textures[i]);
        if(_noise_option == TEXTURE_NOISE) cgGLEnableTextureParameter(_noise_textures[i]);
    }
    if(_r_value_texture)
    {
        cgGLEnableTextureParameter(_r_value_texture);
    }
        
    // copy source into the target first so that we could later on use fragment kill
    cgGLBindProgram(_program_copy);
    RenderQuad();

    // do the real work
    int *current_phase_data = new int[current_phase.size()];
    for(unsigned int i = 0; i < current_phase.size(); i++)
    {
        current_phase_data[i] = current_phase[i];
    }
    for(unsigned int i = 0; i < _current_phases.size(); i++)
    {
        cgSetParameterValueir(_current_phases[i], min(static_cast<int>(4), static_cast<int>(current_phase.size()-4*i)), &current_phase_data[4*i]);
    }
    delete[] current_phase_data;

    if(_noise_option == MD5_NOISE)
    {
        int key = 0;
        
        for(unsigned int i = 0; i < current_phase.size(); i++)
        {
            key *= _clique_size;
            key += current_phase[i];
        }
        
        for(unsigned int k = 0; k < _keys.size(); k++)
        {
            cgSetParameter1i(_keys[k], key^_initial_keys[k]);
        }

        for(unsigned int k = 0; k < _initial_keys.size(); k++)
        {
            _initial_keys[k] = Hash(key^_initial_keys[(k+_initial_keys.size()-1)%_initial_keys.size()]);
        }
    }
    
    cgGLBindProgram(_program_main);
    RenderQuad();

    for(unsigned int i = 0; i < _poisson_textures.size(); i++)
    {
        if(_noise_option == TEXTURE_NOISE) cgGLDisableTextureParameter(_noise_textures[i]);
        cgGLDisableTextureParameter(_poisson_textures[i]);
    }
    if(_r_value_texture)
    {
        cgGLDisableTextureParameter(_r_value_texture);
    }
    
    cgGLDisableProfile(_fragment_profile);

    FramebufferObject::Disable();
    glDrawBuffer(_current_draw_buffer); 

    _ping_pong = !_ping_pong;

    CheckError();
}

vector<GLuint> CgPoisson::GenerateInitTexture(const TextureSize & texture_size, const float cell_size, const int clique_size, const vector<const Sample *> & samples, const vector<GLuint> & init_texture_id)
{
    if(RequiredDrawBuffers(texture_size.Dimension()) != init_texture_id.size())
    {
        throw Exception("RequiredDrawBuffers(texture_size.Dimension()) != init_texture_id.size()");
    }
    
    const int height = texture_size.TotalHeight();
    const int width = texture_size.TotalWidth();
        
    vector<GLuint> new_texture_id = init_texture_id;

    for(unsigned int i = 0; i < new_texture_id.size(); i++)
    {
        if(new_texture_id[i] <= 0)
        {
            glGenTextures(1, &new_texture_id[i]);
        } 
    }

    for(unsigned int k = 0; k < new_texture_id.size(); k++)
    {
        float *texture_data = new float[height*width*4];

        for(int i = 0; i < height*width*4; i++)
        {
            texture_data[i] = -1;
        }

        for(unsigned int i = 0; i < samples.size(); i++)
        {
            if(samples[i])
            {
                if(samples[i]->coordinate.Dimension() != texture_size.Dimension())
                {
                    throw Exception("incompatible dimension between samples and texture size spec");
                }

                
                vector<int> cell_index(samples[i]->coordinate.Dimension());

                int all_inside = 1;
                for(unsigned int j = 0; j < cell_index.size(); j++)
                {
                    cell_index[j] = static_cast<int>(floor(samples[i]->coordinate[j]/cell_size));

                    if((cell_index[j] < 0) || (cell_index[j] >= texture_size.Size(j)))
                    {
                        all_inside = 0;
                    }
                }

                if(all_inside)
                {
                    vector<int> flattened_index(2);

                    if(! FlattenIntegerCoordinate(texture_size, cell_index, flattened_index))
                    {
                        throw Exception("cannot FlattenIntegerCoordinate");
                    }
                    
                    const int linear_index = flattened_index[1]*texture_size.TotalWidth() + flattened_index[0];

                    for(unsigned int j = 0; j < 4; j++)
                    {
                        texture_data[4*linear_index+j] = -1;
                    }
                    
                    for(unsigned int j = 4*k; j < min(static_cast<int>(4*(k+1)), static_cast<int>(samples[i]->coordinate.Dimension())); j++)
                    {
                        texture_data[4*linear_index+j-4*k] = samples[i]->coordinate[j];
                    }
                }
            }
        }

        const GLenum tex_target = GL_TEXTURE_RECTANGLE_NV;
        const GLenum tex_internal_format = GL_FLOAT_RGBA_NV;

        const GLenum filterMode = (tex_target == GL_TEXTURE_RECTANGLE_NV) ? GL_NEAREST : GL_LINEAR;

        glBindTexture(tex_target, new_texture_id[k]);
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

GLuint CgPoisson::GenerateNeighborOffsetTexture(const vector< vector<int> > & flattened_neighbor_offsets, const GLuint neighbor_offset_texture_id, const vector<int> & offset_texture_size)
{
    if(offset_texture_size.size() != 2)
    {
        throw Exception("offset_texture_size.size() != 2");
    }
    
    const int height = offset_texture_size[1];
    const int width = offset_texture_size[0];
    
    GLuint new_texture_id = neighbor_offset_texture_id;

    if(new_texture_id <= 0)
    {
        glGenTextures(1, &new_texture_id);
    }

    {
        float *texture_data = new float[height*width*4];

        for(int i = 0; i < height*width; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                texture_data[4*i+j] = 0; // self checking as fill holders
            }
                
            if(i < flattened_neighbor_offsets.size())
            {
                if(flattened_neighbor_offsets[i].size() != 2)
                {
                    throw Exception("neighborhood offsets need to be flattened in GenerateNeighborOffsetTexture()");
                }

                for(int j = 0; j < 2; j++)
                {
                    texture_data[4*i+j] = flattened_neighbor_offsets[i][j];
                }
            }
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

int CgPoisson::FlattenNeighborOffsets(const TextureSize & texture_size, const int clique_size, const vector< vector<int> > & input, vector< vector<int> > & output)
{
    if(texture_size.Dimension() == 2)
    {
        output = input;
    }
    else
    {
#if 1
        output = vector< vector<int> >(input.size());

        for(unsigned int i = 0; i < input.size(); i++)
        {
            if(! FlattenIntegerCoordinate(texture_size, input[i], output[i]))
            {
                return 0;
            }
        }
#else
        vector<int> all_texture_size(texture_size.Dimension());
        {
            for(unsigned int i = 0; i < texture_size.width.size(); i++)
            {
                all_texture_size[i] = texture_size.width[i];
            }
            for(unsigned int i = 0; i < texture_size.height.size(); i++)
            {
                all_texture_size[i+texture_size.width.size()] = texture_size.height[i];
            }
        }
        
        vector<int> output_single(2);
        output.clear();
        
        for(unsigned int i = 0; i < input.size(); i++)
        {
            bool is_redundant = 0;

            if(input[i].size() != all_texture_size.size())
            {
                return 0;
            }
            
            for(unsigned int j = 0; j < input[i].size(); j++)
            {
                if((input[i][j] <= -all_texture_size[j]) || (input[i][j] >= all_texture_size[j]))
                {
                    is_redundant = 1;
                }
            }

            if(!is_redundant)
            {
                if(! FlattenIntegerCoordinate(texture_size, input[i], output_single))
                {
                    return 0;
                }

                output.push_back(output_single);
            }
        }
#endif
    }

    // done
    return 1;
}

int CgPoisson::FlattenIntegerCoordinate(const TextureSize & texture_size, const vector<int> & input, vector<int> & output)
{
    return CgSampling::FlattenIntegerCoordinate(texture_size, input, output);

#if 0
    if(texture_size.Dimension() != input.size())
    {
        // dimension mismatch
        return 0;
    }
    
    if(output.size() != 2) output = vector<int>(2);
    output[0] = output[1] = 0;

#if 1
    for(unsigned int i = 0; i < texture_size.width.size(); i++)
    {
        output[0] += input[i];
        if((i+1) < texture_size.width.size())
        {
            output[0] *= texture_size.width[i+1];
        }
    }
    for(unsigned int i = 0; i < texture_size.height.size(); i++)
    {
        output[1] += input[i+texture_size.width.size()];
        if((i+1) < texture_size.height.size())
        {
            output[1] *= texture_size.height[i+1];
        }
    }       
#else    
    if(texture_size.Dimension() == 2)
    {
        output = input;
    }
    else if(texture_size.Dimension() == 3)
    {
        if(texture_size.width.size() == 1)
        {
            output[0] = input[0];
            output[1] = input[2]+input[1]*texture_size.TileHeight();
        }
        else
        {
            output[0] = input[1]+input[0]*texture_size.TileWidth();
            output[1] = input[2];
        }
    }
    else if(texture_size.Dimension() == 4)
    {
        output[0] = input[1]+input[0]*texture_size.TileWidth();
        output[1] = input[3]+input[2]*texture_size.TileHeight();
    }
    else if(texture_size.Dimension() == 5)
    {
        if(texture_size.width.size() == 2)
        {
            output[0] = input[1]+input[0]*texture_size.width[1];
            output[1] = input[4]+(input[3]+input[2]*texture_size.height[1])*texture_size.height[2];
        }
        else
        {
            output[0] = input[2]+(input[1]+input[0]*texture_size.width[1])*texture_size.width[2];
            output[1] = input[4]+input[3]*texture_size.height[1];
        }
    }
    else if(texture_size.Dimension() == 6)
    {
        output[0] = input[2]+(input[1]+input[0]*texture_size.width[1])*texture_size.width[2];
        output[1] = input[5]+(input[4]+input[3]*texture_size.height[1])*texture_size.height[2];
    }
    else
    {
        throw Exception("can only handle up to 4D now!");
    }
#endif
    
    // done
    return 1;
#endif
}

string CgPoisson::GenerateParentIndexString(const TextureSize & texture_size, const int clique_size)
{
    const int dimension = texture_size.Dimension();
    
    string result;

    {
        ostringstream output_program;

        if(dimension == 2)
        {
            output_program <<
                " float2 parent_index = floor(cell_index/2);\n";
        }
        else if(dimension == 3)
        {
            output_program <<
                " float3 parent_index_3d = floor(cell_index_3d/2);\n";
            
            if(texture_size.width.size() == 1)
            {
                output_program <<
                    " float2 parent_index = float2(parent_index_3d.x, parent_index_3d.z + parent_index_3d.y*" << texture_size.height[1] << ");\n";
            }
            else if(texture_size.width.size() == 2)
            {
                output_program <<
                    " float2 parent_index = float2(parent_index_3d.y + parent_index_3d.x*" << texture_size.width[1] << ", parent_index_3d.z);\n";
            }
            else
            {
                throw Exception("impossible 3D texture_size");
            }
        }
        else if(dimension == 4)
        {
            if((texture_size.height.size() != 2) || (texture_size.width.size() != 2))
            {
                throw Exception("impossible 4D texture_size");
            }
            
            output_program <<
                " float4 parent_index_4d = floor(cell_index_4d/2);\n";

            output_program <<
                " float2 parent_index = float2(parent_index_4d.y + parent_index_4d.x*" << texture_size.width[1] << ", parent_index_4d.w + parent_index_4d.z*" << texture_size.height[1] << ");\n";
        }
        else if(dimension == 5)
        {
            output_program <<
                " float4 parent_index_5d_0 = floor(cell_index_5d_0/2);\n"
                " float1 parent_index_5d_1 = floor(cell_index_5d_1/2);\n";

            if(texture_size.width.size() == 2)
            {
                output_program <<
                    " float2 parent_index = float2(parent_index_5d_0.y + parent_index_5d_0.x*" << texture_size.width[1] << ", parent_index_5d_1.x + (parent_index_5d_0.w + parent_index_5d_0.z*" << texture_size.height[1] << ")*" << texture_size.height[2] << ");\n";
            }
            else if(texture_size.width.size() == 3)
            {
                output_program <<
                    " float2 parent_index = float2(parent_index_5d_0.z+(parent_index_5d_0.y+parent_index_5d_0.x*" << texture_size.width[1] << ")*" << texture_size.width[2] << ", parent_index_5d_1.x+parent_index_5d_0.w*" << texture_size.height[1] << ");\n";
            }
            else
            {
                throw Exception("impossible 5D texture_size");
            }
        }
        else if(dimension == 6)
        {
            if((texture_size.height.size() != 3) || (texture_size.width.size() != 3))
            {
                throw Exception("impossible 6D texture_size");
            }
            
            output_program <<
                " float4 parent_index_6d_0 = floor(cell_index_6d_0/2);\n"
                " float2 parent_index_6d_1 = floor(cell_index_6d_1/2);\n";

            output_program <<
                " float2 parent_index = float2(parent_index_6d_0.z+(parent_index_6d_0.y+parent_index_6d_0.x*" << texture_size.width[1] << ")*" << texture_size.width[2] << ", parent_index_6d_1.y+(parent_index_6d_1.x+parent_index_6d_0.w*" << texture_size.height[1] << ")*" << texture_size.height[2] << ");\n";
        }
        else
        {
            throw Exception("can only handle up to 6D now!");
        }
        
        result = output_program.str();
    }
    
    return result;
}

string CgPoisson::GenerateCellIndexNDString(const TextureSize & texture_size, const int clique_size)
{ 
    const int dimension = texture_size.Dimension();
    
    string result;

    {
        ostringstream output_program;

        if(dimension == 2)
        {
            throw Exception("no need to call GenerateCellIndexNDString() for 2D");
        }
        else if(dimension == 3)
        {
            if(texture_size.width.size() == 1)
            {
                output_program <<
                    " float3 cell_index_3d = float3(cell_index.x, floor(cell_index.y/" << texture_size.height[1] << "), fmod(cell_index.y, " << texture_size.height[1] << "));\n";
            }
            else if(texture_size.width.size() == 2)
            {
                output_program <<
                    " float3 cell_index_3d = float3(floor(cell_index.x/" << texture_size.width[1] << "), fmod(cell_index.x, " << texture_size.width[1] << "), cell_index.y);\n";
            }
            else
            {
                throw Exception("impossible 3D texture_size");
            }
        }
        else if(dimension == 4)
        {
            if((texture_size.height.size() != 2) || (texture_size.width.size() != 2))
            {
                throw Exception("impossible 4D texture_size");
            }
            
            output_program <<
                " float4 cell_index_4d = float4(floor(cell_index.x/" << texture_size.width[1] << "), fmod(cell_index.x, " << texture_size.width[1] << "), floor(cell_index.y/" << texture_size.height[1] << "), fmod(cell_index.y, " << texture_size.height[1] <<"));\n";
        }
        else if(dimension == 5)
        {
            if(texture_size.width.size() == 2)
            {
                output_program <<
                    " float4 cell_index_5d_0 = float4(floor(cell_index.x/" << texture_size.width[1] << "), fmod(cell_index.x, " << texture_size.width[1] << "), floor(cell_index.y/" << texture_size.height[1]*texture_size.height[2] << "), fmod(floor(cell_index.y/" << texture_size.height[2] << "), " << texture_size.height[1] <<"));\n";

                output_program <<
                    " float1 cell_index_5d_1 = float1(fmod(cell_index.y, " << texture_size.height[2] << "));\n";
            }
            else if(texture_size.width.size() == 3)
            {
                output_program <<
                    " float4 cell_index_5d_0 = float4(floor(cell_index.x/" << texture_size.width[1]*texture_size.width[2] << "), fmod(floor(cell_index.x/" << texture_size.width[2] << "), " << texture_size.width[1] << "), fmod(cell_index.x, " << texture_size.width[2] << "), floor(cell_index.y/" << texture_size.height[1] << "));\n";

                output_program <<
                    " float1 cell_index_5d_1 = float1(fmod(cell_index.y, " << texture_size.height[1] << "));\n";
            }
            else
            {
                throw Exception("impossible 5D texture_size");
            }
        }
        else if(dimension == 6)
        {
            if((texture_size.height.size() != 3) || (texture_size.width.size() != 3))
            {
                throw Exception("impossible 6D texture_size");
            }
            
            output_program <<
                " float4 cell_index_6d_0 = float4(floor(cell_index.x/" << texture_size.width[1]*texture_size.width[2] << "), fmod(floor(cell_index.x/" << texture_size.width[2] << "), " << texture_size.width[1] << "), fmod(cell_index.x, " << texture_size.width[2] << "), floor(cell_index.y/" << texture_size.height[1]*texture_size.height[2] << "));\n";
            
            output_program <<
                " float2 cell_index_6d_1 = float2(fmod(floor(cell_index.y/" << texture_size.height[2] << "), " << texture_size.height[1] << "), fmod(cell_index.y, " << texture_size.height[2] << "));\n";
        }
        else
        {
            throw Exception("can only handle up to 6D now!");
        }
        
        result = output_program.str();
    }

    return result;
}

string CgPoisson::GenerateMainProgram(const TextureSize & texture_size, const int clique_size, const vector<int> & num_neighbors, const DistanceField::Boundary boundary_condition, const NoiseOption noise_option, const TextureSize & r_value_texture_size)
{
    const int height = texture_size.TotalHeight();
    const int width = texture_size.TotalWidth();
    const int dimension = texture_size.Dimension();
    const int num_buffers = RequiredDrawBuffers(dimension);

    const DistanceFieldOption distance_field_option = r_value_texture_size.TotalWidth()*r_value_texture_size.TotalHeight() == 0 ? UNIFORM : ADAPTIVE;
    
    string result;

    {
        ostringstream output_program;

        if(noise_option == MD5_NOISE)
        {
            output_program << CgMD5::GenerateUniformRandom01Function() << endl;
        }
    
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
            output_program << " float4 col : COL;\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program << "float4 col" << i << " : COL" << i << ";\n";
            }
        }
        
        output_program <<
            "};\n"
            "\n"
            "FragmentOutput fragment(FragmentInput input,\n";

        if(distance_field_option == UNIFORM)
        {
            output_program <<
                "                        uniform float r_value,\n";
        }
        else
        {
            output_program <<
                "                        uniform samplerRECT r_value_texture,\n";    
        }

        output_program <<
            "                        uniform float cell_size,\n"
            "                        uniform float clique_size,\n";

        if(num_buffers == 1)
        {
            output_program << 
                "                        uniform float" << dimension << " current_phase,\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program << 
                    "                        uniform float" << min(static_cast<int>(4), static_cast<int>(dimension-4*i)) << " current_phase_" << i << ",\n";
            }
        }
        
        if(num_neighbors.size() == 1)
        //if(neighbor_offset_storage == NEIGHBOR_OFFSET_PROGRAM_CONSTANT)
        {
            output_program <<
            "                        uniform float2[" << num_neighbors[0] << "] neighbor_offsets,\n";
        }
        else if(num_neighbors.size() == 2)
        //else if(neighbor_offset_storage == NEIGHBOR_OFFSET_TEXTURE)
        {
            output_program <<
            "                        uniform samplerRECT neighbor_offsets,\n";
        }
        else
        {
            throw Exception("unsupported neighbor offset storage");
        }

        if(num_buffers == 1)
        {
            if(noise_option == TEXTURE_NOISE)
            {
                output_program <<
                    "                        uniform samplerRECT noise_texture,\n";
            }
            else if (noise_option == MD5_NOISE)
            {
                output_program <<
                    "                        uniform int key,\n";
            }
            else
            {
                throw Exception("unsupported noise option");
            }
        }
        else
        {
            if(noise_option == TEXTURE_NOISE)
            {
                for(int i = 0; i < num_buffers; i++)
                {
                    output_program <<
                        "                        uniform samplerRECT noise_texture_" << i << ",\n";
                }
            }
            else if (noise_option == MD5_NOISE)
            {
                for(int i = 0; i < num_buffers; i++)
                {
                    output_program <<
                        "                        uniform int key_" << i << ",\n";
                }
            }
            else
            {
                throw Exception("unsupported noise option");
            }
        }

        if(num_buffers == 1)
        {
            output_program <<
                "                        uniform samplerRECT poisson_texture)\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    "                        uniform samplerRECT poisson_texture_" << i;

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
            " FragmentOutput output;\n"
            " /* copy directly if already has sample or not in the current_phase */ \n";

        if(num_buffers == 1)
        {
            output_program <<
                " float4 result = float4(-1, -1, -1, -1);\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    " float4 result_" << i << " = float4(-1, -1, -1, -1);\n";
            }
        }
        
        output_program <<
            " float2 cell_index = floor(input.tex.xy);\n";

        // phase checking portion
        if(dimension == 2)
        {
            output_program <<
                " float2 my_phase = fmod(cell_index, clique_size);\n"
                " if((my_phase.x != current_phase.x) || (my_phase.y != current_phase.y))\n";
        }
        else if(dimension == 3)
        {
            output_program << GenerateCellIndexNDString(texture_size, clique_size);
            
            output_program <<
                " float3 my_phase = fmod(cell_index_3d, clique_size);\n"
                " if((my_phase.x != current_phase.x) || (my_phase.y != current_phase.y) || (my_phase.z != current_phase.z))\n";
        }
        else if(dimension == 4)
        {
            output_program << GenerateCellIndexNDString(texture_size, clique_size);
            
            output_program <<
                " float4 my_phase = fmod(cell_index_4d, clique_size);\n"
                " if((my_phase.x != current_phase.x) || (my_phase.y != current_phase.y) || (my_phase.z != current_phase.z) || (my_phase.w != current_phase.w))\n";
        }
        else if(dimension == 5)
        {
            output_program << GenerateCellIndexNDString(texture_size, clique_size);
            
            output_program <<
                " float4 my_phase_0 = fmod(cell_index_5d_0, clique_size);\n"
                " float1 my_phase_1 = fmod(cell_index_5d_1, clique_size);\n"
                " if((my_phase_0.x != current_phase_0.x) || (my_phase_0.y != current_phase_0.y) || (my_phase_0.z != current_phase_0.z) || (my_phase_0.w != current_phase_0.w) || (my_phase_1.x != current_phase_1.x))\n";
        }
        else if(dimension == 6)
        {
            output_program << GenerateCellIndexNDString(texture_size, clique_size);
            
            output_program <<
                " float4 my_phase_0 = fmod(cell_index_6d_0, clique_size);\n"
                " float2 my_phase_1 = fmod(cell_index_6d_1, clique_size);\n"
                " if((my_phase_0.x != current_phase_0.x) || (my_phase_0.y != current_phase_0.y) || (my_phase_0.z != current_phase_0.z) || (my_phase_0.w != current_phase_0.w) || (my_phase_1.x != current_phase_1.x) || (my_phase_1.y != current_phase_1.y))\n";
        }
        else if(dimension == 7)
        {
            output_program << GenerateCellIndexNDString(texture_size, clique_size);
            
            output_program <<
                " float4 my_phase_0 = fmod(cell_index_7d_0, clique_size);\n"
                " float3 my_phase_1 = fmod(cell_index_7d_1, clique_size);\n"
                " if((my_phase_0.x != current_phase_0.x) || (my_phase_0.y != current_phase_0.y) || (my_phase_0.z != current_phase_0.z) || (my_phase_0.w != current_phase_0.w) || (my_phase_1.x != current_phase_1.x) || (my_phase_1.y != current_phase_1.y) || (my_phase_1.z != current_phase_1.z))\n";
        }
        else if(dimension == 8)
        {
            output_program << GenerateCellIndexNDString(texture_size, clique_size);
            
            output_program <<
                " float4 my_phase_0 = fmod(cell_index_8d_0, clique_size);\n"
                " float4 my_phase_1 = fmod(cell_index_8d_1, clique_size);\n"
                " if((my_phase_0.x != current_phase_0.x) || (my_phase_0.y != current_phase_0.y) || (my_phase_0.z != current_phase_0.z) || (my_phase_0.w != current_phase_0.w) || (my_phase_1.x != current_phase_1.x) || (my_phase_1.y != current_phase_1.y) || (my_phase_1.z != current_phase_1.z) || (my_phase_1.w != current_phase_1.w))\n";
        }
        else
        {
            throw Exception("can only handle up to 8D now!");
        }
        
        output_program << 
            " {\n"
            "     discard;\n"
            " }\n";

        // existance checking portion
        output_program <<
            " else \n"
            " {\n";

        if(num_buffers == 1)
        {
            output_program <<
                "    result = texRECT(poisson_texture, cell_index.xy);\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    "    result_" << i << " = texRECT(poisson_texture_" << i << ", cell_index.xy);\n";
            }
        }
        
        if(dimension == 2)
        {
            output_program <<
                "    if((result.x >= 0) && (result.y >= 0))\n";
        }
        else if(dimension == 3)
        {
            output_program <<
                "    if((result.x >= 0) && (result.y >= 0) && (result.z >= 0))\n";
        }
        else if(dimension == 4)
        {
            output_program <<
                "    if((result.x >= 0) && (result.y >= 0) && (result.z >= 0) && (result.w >= 0))\n";
        }
        else if(dimension == 5)
        {
            output_program <<
                "    if((result_0.x >= 0) && (result_0.y >= 0) && (result_0.z >= 0) && (result_0.w >= 0) && (result_1.x >= 0))\n";
        }
        else if(dimension == 6)
        {
            output_program <<
                "    if((result_0.x >= 0) && (result_0.y >= 0) && (result_0.z >= 0) && (result_0.w >= 0) && (result_1.x >= 0) && (result_1.y >= 0))\n";
        }
        else if(dimension == 7)
        {
            output_program <<
                "    if((result_0.x >= 0) && (result_0.y >= 0) && (result_0.z >= 0) && (result_0.w >= 0) && (result_1.x >= 0) && (result_1.y >= 0) && (result_1.z >= 0))\n";
        }
        else if(dimension == 8)
        {
            output_program <<
                "    if((result_0.x >= 0) && (result_0.y >= 0) && (result_0.z >= 0) && (result_0.w >= 0) && (result_1.x >= 0) && (result_1.y >= 0) && (result_1.z >= 0) && (result_1.w >= 0))\n";
        }
        else
        {
            throw Exception("can only handle up to 8D now!");
        }
        
        output_program << 
            "    {\n"
            "       discard;\n"
            "    }\n";

        // conflict checking portion
        output_program << 
            "    /* random sample within this cell */ \n";

        if(num_buffers == 1)
        {
            if(noise_option == TEXTURE_NOISE)
            {
                output_program <<
                    "    result = texRECT(noise_texture, cell_index.xy);\n";
            }
            else if(noise_option == MD5_NOISE)
            {
                output_program <<
                    "    result = UniformRandom01(cell_index.xy, key);\n";
            }
            else
            {
                throw Exception("unsupported noise option");
            }
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                if(noise_option == TEXTURE_NOISE)
                {
                    output_program <<
                        "    result_" << i << " = texRECT(noise_texture_" << i << ", cell_index.xy);\n";
                }
                else if(noise_option == MD5_NOISE)
                {
                    output_program <<
                        "    result_" << i << " = UniformRandom01(cell_index.xy, key_" << i << ");\n";
                }
                else
                {
                    throw Exception("unsupported noise option");
                }
            }
        }
        
        if(dimension == 2)
        {
            output_program <<
                "    result.xy = (result.xy + cell_index)*cell_size;\n";
        }
        else if(dimension == 3)
        {
            output_program <<
                "    result.xyz = (result.xyz + cell_index_3d)*cell_size;\n";
        }
        else if(dimension == 4)
        {
            output_program <<
                "    result.xyzw = (result.xyzw + cell_index_4d)*cell_size;\n";
        }
        else if(dimension == 5)
        {
            output_program <<
                "    result_0.xyzw = (result_0.xyzw + cell_index_5d_0.xyzw)*cell_size;\n"
                "    result_1.x = (result_1.x + cell_index_5d_1.x)*cell_size;\n";
        }
        else if(dimension == 6)
        {
            output_program <<
                "    result_0.xyzw = (result_0.xyzw + cell_index_6d_0.xyzw)*cell_size;\n"
                "    result_1.xy = (result_1.xy + cell_index_6d_1.xy)*cell_size;\n";
        }
        else if(dimension == 7)
        {
            output_program <<
                "    result_0.xyzw = (result_0.xyzw + cell_index_7d_0.xyzw)*cell_size;\n"
                "    result_1.xyz = (result_1.xyz + cell_index_7d_1.xyz)*cell_size;\n";
        }
        else if(dimension == 8)
        {
            output_program <<
                "    result_0.xyzw = (result_0.xyzw + cell_index_8d_0.xyzw)*cell_size;\n"
                "    result_1.xyzw = (result_1.xyzw + cell_index_8d_1.xyzw)*cell_size;\n";
        }
        else
        {
            throw Exception("can only handle up to 8D now!");
        }
        
        output_program << 
                "    /* check neighbors */ \n";
        if(num_neighbors.size() == 1)
        //if(neighbor_offset_storage == NEIGHBOR_OFFSET_PROGRAM_CONSTANT)
        {
            output_program <<
                "    for(int i = 0; i < neighbor_offsets.length; i++)\n"
                "    {\n"
                "       float2 neighbor_index = cell_index.xy + neighbor_offsets[i].xy;\n";
        }
        else if(num_neighbors.size() == 2)
        //else if(neighbor_offset_storage == NEIGHBOR_OFFSET_TEXTURE)
        {
            output_program << 
                "    for(int j = 0; j < " << num_neighbors[1] << "; j++)\n"
                "    for(int i = 0; i < " << num_neighbors[0] << "; i++)\n";
            
            output_program <<
                "    {\n"
                "       float4 neighbor_offsets_i = texRECT(neighbor_offsets, float2(i, j));\n"
                "       float2 neighbor_index = cell_index.xy + neighbor_offsets_i.xy;\n";
        }
        else
        {
            throw Exception("unsupported neighbor offset storage");
        }
        
        if(boundary_condition == DistanceField::BOUNDARY_TOROIDAL)
        {
#if 1
            if(dimension > 2)
            {
                throw Exception("cannot handle toroidal boundary condition for > 2D now!");
            }
#endif
            
            output_program <<
                "       float2 texture_size = float2(" << width << ", " << height << ");\n"
                "       float2 corrected_index = fmod(fmod(neighbor_index, texture_size)+texture_size, texture_size);\n"
                "       float4 neighbor = texRECT(poisson_texture, corrected_index);\n"
                "       float4 neighbor_corrected = neighbor;\n"
                "       neighbor_corrected.xy += (neighbor_index - corrected_index)*cell_size;\n";
        }
        else // DistanceField::BOUNDARY_NONE
        {
            if(num_buffers == 1)
            {
                output_program <<
                    "       float4 neighbor = texRECT(poisson_texture, neighbor_index);\n"
                    "       float4 neighbor_corrected = neighbor;\n";
            }
            else
            {
                for(int i = 0; i < num_buffers; i++)
                {
                    output_program <<
                        "       float4 neighbor_" << i << " = texRECT(poisson_texture_" << i << ", neighbor_index);\n";
                    output_program <<
                        "       float4 neighbor_corrected_" << i << " = neighbor_" << i << ";\n";
                }
            }
        }

        if(distance_field_option != UNIFORM)
        {
            if(dimension != 2)
            {
                throw Exception("adaptive sampling code only works for 2D now!");
            }

            output_program <<
                "       float2 r_value_texture_size = float2(" << r_value_texture_size.TotalWidth() << ", " << r_value_texture_size.TotalHeight() << ");\n";
            
            output_program <<
                "       float r_value_1 = texRECT(r_value_texture, result.xy*r_value_texture_size);\n";

            output_program <<
                "       float r_value_2 = texRECT(r_value_texture, neighbor_corrected.xy*r_value_texture_size);\n";

            output_program <<
                "       float r_value = max(r_value_1, r_value_2);\n";
        }
        
        if(dimension == 2)
        {
            output_program <<
                "       if((neighbor.x >= 0) && (neighbor.y >= 0) && (distance(neighbor_corrected.xy, result.xy) < r_value))\n";
        }
        else if(dimension == 3)
        {
            output_program <<
                "       if((neighbor.x >= 0) && (neighbor.y >= 0) && (neighbor.z >= 0) && (distance(neighbor_corrected.xyz, result.xyz) < r_value))\n";
        }
        else if(dimension == 4)
        {
            output_program <<
                "       if((neighbor.x >= 0) && (neighbor.y >= 0) && (neighbor.z >= 0) && (neighbor.w >= 0) && (distance(neighbor_corrected.xyzw, result.xyzw) < r_value))\n";
        }
        else if(dimension == 5)
        {
            output_program <<
                "       if((neighbor_0.x >= 0) && (neighbor_0.y >= 0) && (neighbor_0.z >= 0) && (neighbor_0.w >= 0) && (neighbor_1.x >= 0) && ((distance(neighbor_corrected_0.xyzw, result_0.xyzw)*distance(neighbor_corrected_0.xyzw, result_0.xyzw) + distance(neighbor_corrected_1.x, result_1.x)*distance(neighbor_corrected_1.x, result_1.x)) < r_value*r_value))\n";
        }
        else if(dimension == 6)
        {
            output_program <<
                "       if((neighbor_0.x >= 0) && (neighbor_0.y >= 0) && (neighbor_0.z >= 0) && (neighbor_0.w >= 0) && (neighbor_1.x >= 0) && (neighbor_1.y >= 0) && ((distance(neighbor_corrected_0.xyzw, result_0.xyzw)*distance(neighbor_corrected_0.xyzw, result_0.xyzw) + distance(neighbor_corrected_1.xy, result_1.xy)*distance(neighbor_corrected_1.xy, result_1.xy)) < r_value*r_value))\n";
        }
        else if(dimension == 7)
        {
            output_program <<
                "       if((neighbor_0.x >= 0) && (neighbor_0.y >= 0) && (neighbor_0.z >= 0) && (neighbor_0.w >= 0) && (neighbor_1.x >= 0) && (neighbor_1.y >= 0) && (neighbor_1.z >= 0) && ((distance(neighbor_corrected_0.xyzw, result_0.xyzw)*distance(neighbor_corrected_0.xyzw, result_0.xyzw) + distance(neighbor_corrected_1.xyz, result_1.xyz)*distance(neighbor_corrected_1.xyz, result_1.xyz)) < r_value*r_value))\n";
        }
        else if(dimension == 8)
        {
            output_program <<
                "       if((neighbor_0.x >= 0) && (neighbor_0.y >= 0) && (neighbor_0.z >= 0) && (neighbor_0.w >= 0) && (neighbor_1.x >= 0) && (neighbor_1.y >= 0) && (neighbor_1.z >= 0) && (neighbor_1.w >= 0) && ((distance(neighbor_corrected_0.xyzw, result_0.xyzw)*distance(neighbor_corrected_0.xyzw, result_0.xyzw) + distance(neighbor_corrected_1.xyzw, result_1.xyzw)*distance(neighbor_corrected_1.xyzw, result_1.xyzw)) < r_value*r_value))\n";
        }
        else
        {
            throw Exception("can only handle up to 8D now!");
        }
        
        output_program <<
            "       {\n"
            "           discard;\n"
            "       }\n"
            "    }\n";

        if(num_buffers == 1)
        {
            output_program << "    output.col = result;\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program << "    output.col" << i << " = result_" << i << ";\n";
            }
        }
        
        output_program <<
            " }\n"
            " return output;\n"
            "}\n";

        result = output_program.str();
    }

    return result;
}

string CgPoisson::GenerateInitProgram(const TextureSize & texture_size, const int clique_size)
{
    const int dimension = texture_size.Dimension();
    const int num_buffers = RequiredDrawBuffers(dimension);

    TextureSize parent_texture_size(texture_size);
    {
        for(unsigned int i = 0; i < texture_size.height.size(); i++)
        {
            if(texture_size.height[i]%2)
            {
                throw Exception("texture_size.height not all even");
            }

            parent_texture_size.height[i] = texture_size.height[i]/2;
        }
        
        for(unsigned int i = 0; i < texture_size.width.size(); i++)
        {
            if(texture_size.width[i]%2)
            {
                throw Exception("texture_size.width not all even");
            }

            parent_texture_size.width[i] = texture_size.width[i]/2;
        }
    }
    
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
            output_program << " float4 col : COL;\n";
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
            "FragmentOutput fragment(FragmentInput input,\n"
            "                        uniform float cell_size,\n";

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
            " FragmentOutput output;\n"
            " float2 cell_index = floor(input.tex.xy);\n";

        if(dimension > 2)
        {
            output_program << GenerateCellIndexNDString(texture_size, clique_size);
        }

        output_program << GenerateParentIndexString(parent_texture_size, clique_size);

        if(num_buffers == 1)
        {
            output_program <<
                " float4 parent_value = texRECT(texture, parent_index);\n"
                " float4 result = float4(-1, -1, -1, -1);\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    " float4 parent_value_" << i << " = texRECT(texture_" << i << ", parent_index);\n";

                output_program <<
                    " float4 result_" << i << " = float4(-1, -1, -1, -1);\n";
            }
        }
        
        if(dimension == 2)
        {
            output_program <<
                " float2 boundary_low = cell_index*cell_size;\n"
                " float2 boundary_high = (cell_index+1)*cell_size;\n"
                " if((parent_value.x >= boundary_low.x) && (parent_value.y >= boundary_low.y) && (parent_value.x < boundary_high.x) && (parent_value.y < boundary_high.y))\n";
        }
        else if(dimension == 3)
        {
            output_program <<
                " float3 boundary_low = cell_index_3d*cell_size;\n"
                " float3 boundary_high = (cell_index_3d+1)*cell_size;\n"
                " if((parent_value.x >= boundary_low.x) && (parent_value.y >= boundary_low.y) && (parent_value.z >= boundary_low.z) && (parent_value.x < boundary_high.x) && (parent_value.y < boundary_high.y) && (parent_value.z < boundary_high.z))\n";
        }
        else if(dimension == 4)
        {
            output_program <<
                " float4 boundary_low = cell_index_4d*cell_size;\n"
                " float4 boundary_high = (cell_index_4d+1)*cell_size;\n"
                " if((parent_value.x >= boundary_low.x) && (parent_value.y >= boundary_low.y) && (parent_value.z >= boundary_low.z) && (parent_value.w >= boundary_low.w) && (parent_value.x < boundary_high.x) && (parent_value.y < boundary_high.y) && (parent_value.z < boundary_high.z) && (parent_value.w < boundary_high.w))\n";
        }
        else if(dimension == 5)
        {
            output_program <<
                " float4 boundary_low_0 = cell_index_5d_0*cell_size;\n"
                " float4 boundary_high_0 = (cell_index_5d_0+1)*cell_size;\n"
                " float1 boundary_low_1 = cell_index_5d_1.x*cell_size;\n"
                " float1 boundary_high_1 = (cell_index_5d_1.x+1)*cell_size;\n"
                " if((parent_value_0.x >= boundary_low_0.x) && (parent_value_0.y >= boundary_low_0.y) && (parent_value_0.z >= boundary_low_0.z) && (parent_value_0.w >= boundary_low_0.w) && (parent_value_1.x >= boundary_low_1.x) && (parent_value_0.x < boundary_high_0.x) && (parent_value_0.y < boundary_high_0.y) && (parent_value_0.z < boundary_high_0.z) && (parent_value_0.w < boundary_high_0.w) && (parent_value_1.x < boundary_high_1.x))\n";
        }
        else if(dimension == 6)
        {
            output_program <<
                " float4 boundary_low_0 = cell_index_6d_0*cell_size;\n"
                " float4 boundary_high_0 = (cell_index_6d_0+1)*cell_size;\n"
                " float2 boundary_low_1 = cell_index_6d_1.xy*cell_size;\n"
                " float2 boundary_high_1 = (cell_index_6d_1.xy+1)*cell_size;\n"
                " if((parent_value_0.x >= boundary_low_0.x) && (parent_value_0.y >= boundary_low_0.y) && (parent_value_0.z >= boundary_low_0.z) && (parent_value_0.w >= boundary_low_0.w) && (parent_value_1.x >= boundary_low_1.x) && (parent_value_1.y >= boundary_low_1.y) && (parent_value_0.x < boundary_high_0.x) && (parent_value_0.y < boundary_high_0.y) && (parent_value_0.z < boundary_high_0.z) && (parent_value_0.w < boundary_high_0.w) && (parent_value_1.x < boundary_high_1.x) && (parent_value_1.y < boundary_high_1.y))\n";
        }
        else if(dimension == 7)
        {
            output_program <<
                " float4 boundary_low_0 = cell_index_7d_0*cell_size;\n"
                " float4 boundary_high_0 = (cell_index_7d_0+1)*cell_size;\n"
                " float3 boundary_low_1 = cell_index_7d_1.xyz*cell_size;\n"
                " float3 boundary_high_1 = (cell_index_7d_1.xyz+1)*cell_size;\n"
                " if((parent_value_0.x >= boundary_low_0.x) && (parent_value_0.y >= boundary_low_0.y) && (parent_value_0.z >= boundary_low_0.z) && (parent_value_0.w >= boundary_low_0.w) && (parent_value_1.x >= boundary_low_1.x) && (parent_value_1.y >= boundary_low_1.y)  && (parent_value_1.z >= boundary_low_1.z) && (parent_value_0.x < boundary_high_0.x) && (parent_value_0.y < boundary_high_0.y) && (parent_value_0.z < boundary_high_0.z) && (parent_value_0.w < boundary_high_0.w) && (parent_value_1.x < boundary_high_1.x) && (parent_value_1.y < boundary_high_1.y) && (parent_value_1.z < boundary_high_1.z))\n";
        }
        else if(dimension == 8)
        {
            output_program <<
                " float4 boundary_low_0 = cell_index_7d_0*cell_size;\n"
                " float4 boundary_high_0 = (cell_index_7d_0+1)*cell_size;\n"
                " float4 boundary_low_1 = cell_index_7d_1*cell_size;\n"
                " float4 boundary_high_1 = (cell_index_7d_1+1)*cell_size;\n"
                " if((parent_value_0.x >= boundary_low_0.x) && (parent_value_0.y >= boundary_low_0.y) && (parent_value_0.z >= boundary_low_0.z) && (parent_value_0.w >= boundary_low_0.w) && (parent_value_1.x >= boundary_low_1.x) && (parent_value_1.y >= boundary_low_1.y) && (parent_value_1.z >= boundary_low_1.z)  && (parent_value_1.w >= boundary_low_1.w) && (parent_value_0.x < boundary_high_0.x) && (parent_value_0.y < boundary_high_0.y) && (parent_value_0.z < boundary_high_0.z) && (parent_value_0.w < boundary_high_0.w) && (parent_value_1.x < boundary_high_1.x) && (parent_value_1.y < boundary_high_1.y) && (parent_value_1.z < boundary_high_1.z) && (parent_value_1.w < boundary_high_1.w))\n";
        }
        else
        {
            throw Exception("can only handle up to 8D now!");
        }
        
        output_program <<
            " {\n";

        if(num_buffers == 1)
        {
            output_program <<
                "    result = parent_value;\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    "    result_" << i << " = parent_value_" << i << ";\n";
            }
        }
        
        output_program <<
            " }\n";
        
        if(num_buffers == 1)
        {
            output_program << " output.col = result;\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program << " output.col" << i << " = result_" << i << ";\n";
            }
        }
        
        output_program <<
            " return output;\n"
            "}\n";

        result = output_program.str();
    }

    return result;
}
