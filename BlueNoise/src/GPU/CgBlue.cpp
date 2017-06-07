/*
  CgBlue.cpp

  Li-Yi Wei
  11/22/2007

*/

#include "CgBlue.hpp"
#include "CgMD5.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

#include <math.h>

// Li-Yi (11/22/2007) almost verbatim copy from CgPoisson except the handling of a few Cg parameters such as current_phase
CgBlue::CgBlue(const TextureSize & texture_size, const float cell_size, const int clique_size, const vector< vector<int> > & input_neighbor_offsets, const DistanceField::Boundary boundary_condition, const NeighborOffsetStorage neighbor_offset_storage, const vector<int> & initial_noise_keys, const TextureSize & r_value_texture_size) : CgSampling(texture_size, clique_size, initial_noise_keys), _neighbor_offsets(RequiredDrawBuffers(texture_size.Dimension())), _neighbor_offset_textures(RequiredDrawBuffers(texture_size.Dimension()), 0)
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

    // no flattening, just dicing up into chunks of 4-vectors
    vector< vector< vector<int> > > neighbor_offsets;
    if(! DiceNeighborOffsets(texture_size, input_neighbor_offsets, neighbor_offsets))
    {
        throw Exception("error in dicing neighbor offsets");
    }
    
    string result;

    // internal variable size consistency
    {
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

    if(neighbor_offsets.size() <= 0)
    {
        throw Exception("neighbor_offsets.size() <= 0");
    }
    
    if(neighbor_offset_storage == NEIGHBOR_OFFSET_PROGRAM_CONSTANT)
    {
        num_neighborhood_offsets.push_back(neighbor_offsets[0].size());
    }
    else //if(neighbor_offset_storage == NEIGHBOR_OFFSET_TEXTURE)
    {
        num_neighborhood_offsets = NeighborOffsetTextureSize(neighbor_offsets[0].size());
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

    if(_neighbor_offsets.size() == 1)
    {
        _neighbor_offsets[0] = cgGetNamedParameter(_program_main, "neighbor_offsets");
    }
    else if(_neighbor_offsets.size() == 2)
    {
        _neighbor_offsets[0] = cgGetNamedParameter(_program_main, "neighbor_offsets_0");
        _neighbor_offsets[1] = cgGetNamedParameter(_program_main, "neighbor_offsets_1");
    }
    else
    {
        throw Exception("MRT for > 8D not yet done!");
    }
    
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
        throw Exception("MRT for > 8D not yet done!");
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
        for(unsigned int k = 0; k < neighbor_offsets.size(); k++)
        {
            const int diced_dim = (neighbor_offsets[k].size() > 0 ? neighbor_offsets[k][0].size() : 0);

            if(diced_dim < 0) throw Exception("diced_dim < 0");
            
            float *neighbor_offsets_array = new float[diced_dim*neighbor_offsets[k].size()];
            for(unsigned int i = 0; i < neighbor_offsets[k].size(); i++)
                for(unsigned int j = 0; j < neighbor_offsets[k][i].size(); j++)
                {
                    neighbor_offsets_array[diced_dim*i+j] = neighbor_offsets[k][i][j];
                }
            cgSetParameterValuefr(_neighbor_offsets[k], diced_dim*neighbor_offsets[k].size(), neighbor_offsets_array);
            delete[] neighbor_offsets_array;
        }
    }
    else if (neighbor_offset_storage == NEIGHBOR_OFFSET_TEXTURE)
    {
        _neighbor_offset_textures = GenerateNeighborOffsetTexture(neighbor_offsets, _neighbor_offset_textures, NeighborOffsetTextureSize(neighbor_offsets[0].size()));

        if(_neighbor_offsets.size() != _neighbor_offset_textures.size())
        {
            throw Exception("_neighbor_offsets.size() != _neighbor_offset_textures.size()");
        }
        
        for(unsigned int i = 0; i < _neighbor_offsets.size(); i++)
        {
            cgGLSetTextureParameter(_neighbor_offsets[i], _neighbor_offset_textures[i]);
            cgGLEnableTextureParameter(_neighbor_offsets[i]);
        }
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

CgBlue::~CgBlue(void)
{
    GLuint * texture_array = new GLuint[_textures.size()];
    for(unsigned int i = 0; i < _textures.size(); i++)
    {
        texture_array[i] = _textures[i];
    }
    glDeleteTextures(_textures.size(), texture_array);
    delete[] texture_array;

    for(unsigned int i = 0; i < _neighbor_offset_textures.size(); i++)
    {
        GLuint foo = _neighbor_offset_textures[i];
        if(foo > 0) glDeleteTextures(1, &foo);
    }
    
    cgGLDisableProfile(_fragment_profile);
    
    cgDestroyContext(_context);
}

// Li-Yi (11/22/2007): almost verbatim copy of the function from CgPoisson except the use of RenderQuad(current_phase) instead of massive fragment kills
void CgBlue::Update(const vector<int> & current_phase)
{ 
    if(current_phase.size() != (_texture_size.Dimension()))
    {
        throw Exception("current_phase has wrong dimensionality");
    }

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
    CgSampling::RenderQuad();

    // do the real work
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
    RenderQuad(current_phase);

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

// Li-Yi (11/22/2007): this code is a verbatim copy of the one from CgPoisson.cpp. I know it is ugly but for now I would like to be able to keep this function static. (FlattenIntegerCoordinate needs to be virtualized.)
vector<GLuint> CgBlue::GenerateInitTexture(const TextureSize & texture_size, const float cell_size, const int clique_size, const vector<const Sample *> & samples, const vector<GLuint> & init_texture_id)
{
    if(RequiredDrawBuffers(texture_size.Dimension()) != init_texture_id.size())
    {
        throw Exception("RequiredDrawBuffers(texture_size.Dimension()) != init_texture_id.size()");
    }

    TextureSize phase_group_size_temp = texture_size;

    if(! ComputePhaseGroupSize(texture_size, clique_size, phase_group_size_temp))
    {
        throw Exception("!ComputePhaseGroupSize(texture_size, clique_size, phase_group_size)");
    }

    const vector<int> phase_group_size(phase_group_size_temp);
    
    const int height = texture_size.TotalHeight();
    const int width = texture_size.TotalWidth();
        
    vector<GLuint> new_texture_id = init_texture_id;

    for(unsigned int i = 0; i < new_texture_id.size(); i++)
    {
        GLuint foo = new_texture_id[i];
        if(foo <= 0)
        {
            glGenTextures(1, &foo);
        }
        new_texture_id[i] = foo;
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

                    if(! PackFlattenIntegerCoordinate(texture_size, phase_group_size, clique_size, cell_index, flattened_index))
                    {
                        throw Exception("cannot PackFlattenIntegerCoordinate");
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

vector<GLuint> CgBlue::GenerateNeighborOffsetTexture(const vector< vector< vector<int> > > & diced_neighbor_offsets, const vector<GLuint> & neighbor_offset_texture_id, const vector<int> & offset_texture_size)
{  
    if(diced_neighbor_offsets.size() != neighbor_offset_texture_id.size())
    {
        throw Exception("diced_neighbor_offsets.size() != neighbor_offset_texture_id.size()");
    }
    
    if(offset_texture_size.size() != 2)
    {
        throw Exception("offset_texture_size.size() != 2");
    }

    const int height = offset_texture_size[1];
    const int width = offset_texture_size[0];

    cerr << "offset_texture_size: " << offset_texture_size[0] << ", " << offset_texture_size[1] << endl;
    
    vector<GLuint> new_texture_id = neighbor_offset_texture_id;

    for(unsigned int i = 0; i < new_texture_id.size(); i++)
    {
        GLuint foo = new_texture_id[i];
        if(foo <= 0)
        {
            glGenTextures(1, &foo);
        }
        new_texture_id[i] = foo;
    }

    for(unsigned int which_texture = 0; which_texture < new_texture_id.size(); which_texture++)
    {
        float *texture_data = new float[height*width*4];

        const vector< vector<int> > current_neighbor_offsets = diced_neighbor_offsets[which_texture];

        for(int i = 0; i < height*width; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                texture_data[4*i+j] = 0; // self checking as fill holders
            }

            if(i < current_neighbor_offsets.size())
            {
                if(current_neighbor_offsets[i].size() > 4)
                {
                    throw Exception("neighborhood offsets need to be diced by DiceNeighborOffsets()");
                }

                for(int j = 0; j < current_neighbor_offsets[i].size(); j++)
                {
                    texture_data[4*i+j] = current_neighbor_offsets[i][j];
                }
            }
        }

        const GLenum tex_target = GL_TEXTURE_RECTANGLE_NV;
        const GLenum tex_internal_format = GL_FLOAT_RGBA_NV;

        const GLenum filterMode = (tex_target == GL_TEXTURE_RECTANGLE_NV) ? GL_NEAREST : GL_LINEAR;

        glBindTexture(tex_target, new_texture_id[which_texture]);
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

int CgBlue::DiceNeighborOffsets(const TextureSize & texture_size, const vector< vector<int> > & input, vector< vector< vector<int> > > & output)
{
    const int dimension = texture_size.Dimension();
    const int num_buffers = RequiredDrawBuffers(dimension);

    if(output.size() != num_buffers)
    {
        output = vector< vector< vector<int> > >(num_buffers);
    }

    for(unsigned int i = 0; i < output.size(); i++)
    {
        output[i] = vector< vector<int> >(input.size());

        for(unsigned int j = 0; j < output[i].size(); j++)
        {
            if(input[j].size() != dimension) return 0;
            
            const int index_begin = 4*i;
            const int index_length = min(4, dimension-index_begin);

            output[i][j] = vector<int>(index_length);

            for(unsigned int k = 0; k < output[i][j].size(); k++)
            {
                output[i][j][k] = input[j][k+index_begin];
            }
        }
    }
    
    return 1;
}

int CgBlue::ComputePhaseGroupSize(const TextureSize & texture_size, const int clique_size, TextureSize & phase_group_size)
{
    phase_group_size = texture_size;

    for(unsigned int i = 0; i < phase_group_size.height.size(); i++)
    {
        if((texture_size.height[i]%clique_size) != 0) return 0;
        phase_group_size.height[i] = texture_size.height[i]/clique_size;
    }
    for(unsigned int i = 0; i < phase_group_size.width.size(); i++)
    {
        if((texture_size.width[i]%clique_size) != 0) return 0;
        phase_group_size.width[i] = texture_size.width[i]/clique_size;
    }

    return 1;
}

int CgBlue::PackIntegerCoordinate(const TextureSize & phase_group_size_input, const int clique_size, const vector<int> & input, vector<int> & output)
{
    const vector<int> phase_group_size(phase_group_size_input);
    
    if(phase_group_size.size() != input.size())
    {
        return 0;
    }

    output = input;

    for(unsigned int i = 0; i < output.size(); i++)
    {
        // y = x/C + (x%C)*G
        output[i] = input[i]/clique_size + (input[i]%clique_size)*phase_group_size[i];
    }
    
    return 1;
}

int CgBlue::UnPackIntegerCoordinate(const TextureSize & phase_group_size_input, const int clique_size, const vector<int> & input, vector<int> & output)
{
    const vector<int> phase_group_size(phase_group_size_input);
 
    if(phase_group_size.size() != input.size())
    {
        return 0;
    }

    output = input;
    
    for(unsigned int i = 0; i < output.size(); i++)
    {
        // x = y/G + (y%G)*C
        output[i] = input[i]/phase_group_size[i] + (input[i]%phase_group_size[i])*clique_size;
    }
    
    return 1;
}

string CgBlue::GeneratePackString(const TextureSize & phase_group_size_input, const int clique_size, const string & input_variable, const string & output_variable)
{
    const vector<int> phase_group_size(phase_group_size_input);
    
    const int dimension = phase_group_size.size();
    const int num_buffers = RequiredDrawBuffers(dimension);
    
    ostringstream output_program;

    for(int k = 0; k < num_buffers; k++)
    {
        output_program << output_variable;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << " = floor(" << input_variable;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << "/" << clique_size << ") + fmod(" << input_variable;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << ", " << clique_size << ")*float" << min(4, dimension-4*k) << "(";

        for(unsigned int i = 0; i < min(4, static_cast<int>(phase_group_size.size()-4*k)); i++)
        {
            output_program << phase_group_size[i+4*k];
            
            if(i < (min(4, static_cast<int>(phase_group_size.size()-4*k))-1)) output_program << ", ";
        }
        
        output_program << "); ";
    }

    return output_program.str();
}

string CgBlue::GenerateUnPackString(const TextureSize & phase_group_size_input, const int clique_size, const string & input_variable, const string & output_variable)
{
    const vector<int> phase_group_size(phase_group_size_input);
    
    const int dimension = phase_group_size.size();
    const int num_buffers = RequiredDrawBuffers(dimension);

    ostringstream output_program;

    for(int k = 0; k < num_buffers; k++)
    {       
        output_program << output_variable;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << " = floor(" << input_variable;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << "/float" << min(4, dimension-4*k) << "(";

        for(unsigned int i = 0; i < min(4, static_cast<int>(phase_group_size.size()-4*k)); i++)
        {
            output_program << phase_group_size[i+4*k];
            if(i < (min(4, static_cast<int>(phase_group_size.size()-4*k))-1)) output_program << ", ";
        }
        
        output_program << "))" << " + fmod(" << input_variable;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << ", " << "float" << min(4, dimension-4*k) << "(";
        
        for(unsigned int i = 0; i < min(4, static_cast<int>(phase_group_size.size()-4*k)); i++)
        {
            output_program << phase_group_size[i+4*k];
            if(i < (min(4, static_cast<int>(phase_group_size.size()-4*k))-1)) output_program << ", ";
        }
        
        output_program << "))*" << clique_size << "; ";
    }
    
    return output_program.str();
}

int CgBlue::FlattenIntegerCoordinate(const TextureSize & texture_size, const TextureSize & phase_group_texture_size, const int clique_size, const vector<int> & input, vector<int> & output)
{
    const vector<int> phase_group_size(phase_group_texture_size);
    
    TextureSize clique_texture_size(texture_size);
    clique_texture_size.Clear(clique_size);
    
    if((texture_size.Dimension() != phase_group_size.size()) || (texture_size.Dimension() != input.size()))
    {
        // dimension mismatch
        return 0;
    }
    
    vector<int> input_div(input);
    vector<int> input_mod(input);
    for(unsigned int i = 0; i < input.size(); i++)
    {
        input_div[i] = input[i]/phase_group_size[i];
        input_mod[i] = input[i]%phase_group_size[i];
    }
    
    vector<int> flattened_div(2), flattened_mod(2);

    if(! CgSampling::FlattenIntegerCoordinate(clique_texture_size, input_div, flattened_div))
    {
        return 0;
    }

    if(! CgSampling::FlattenIntegerCoordinate(phase_group_texture_size, input_mod, flattened_mod))
    {
        return 0;
    }
    
    output = vector<int>(2, 0);

    output[0] = phase_group_texture_size.TotalWidth()*flattened_div[0] + flattened_mod[0];
    output[1] = phase_group_texture_size.TotalHeight()*flattened_div[1] + flattened_mod[1];
    
    return 1;
}

int CgBlue::UnFlattenIntegerCoordinate(const TextureSize & texture_size, const TextureSize & phase_group_texture_size, const int clique_size, const vector<int> & input, vector<int> & output)
{
    const vector<int> phase_group_size(phase_group_texture_size);
    
    TextureSize clique_texture_size(texture_size);
    clique_texture_size.Clear(clique_size);
    
    if((texture_size.Dimension() != phase_group_size.size()) || (input.size() != 2))
    {
        // dimension mismatch
        return 0;
    }
       
    vector<int> flattened_div(2), flattened_mod(2);

    flattened_div[0] = input[0]/phase_group_texture_size.TotalWidth();
    flattened_div[1] = input[1]/phase_group_texture_size.TotalHeight();
    flattened_mod[0] = input[0]%phase_group_texture_size.TotalWidth();
    flattened_mod[1] = input[1]%phase_group_texture_size.TotalHeight();
    
    vector<int> output_div(input);
    vector<int> output_mod(input);

    if(! CgSampling::UnFlattenIntegerCoordinate(clique_texture_size, flattened_div, output_div))
    {
        return 0;
    }

    if(! CgSampling::UnFlattenIntegerCoordinate(phase_group_texture_size, flattened_mod, output_mod))
    {
        return 0;
    }

    output = vector<int>(phase_group_size.size(), 0);
    for(unsigned int i = 0; i < output.size(); i++)
    {
        output[i] = phase_group_size[i]*output_div[i] + output_mod[i];
    }
    
    return 1;
}

string CgBlue::GenerateFlattenString(const TextureSize & texture_size, const TextureSize & phase_group_texture_size, const int clique_size, const string & input_variable, const string & output_variable)
{
    const vector<int> phase_group_size(phase_group_texture_size);
    
    TextureSize clique_texture_size(texture_size);
    clique_texture_size.Clear(clique_size);
    
    if(texture_size.Dimension() != phase_group_size.size())
    {
        // dimension mismatch
        return 0;
    }

    const int dimension = phase_group_size.size();
    const int num_buffers = RequiredDrawBuffers(dimension);
    
    ostringstream output_program;
    
    const string input_div(input_variable + "_div");
    const string input_mod(input_variable + "_mod");

    for(int k = 0; k < num_buffers; k++)
    {
        output_program << "float" << min(4, dimension-4*k) << " " << input_div;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << " = floor(" << input_variable;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << "/" << "float" << min(4, dimension-4*k) << "(";

        for(unsigned int i = 0; i < min(4, static_cast<int>(phase_group_size.size()-4*k)); i++)
        {
            output_program << phase_group_size[i+4*k];
            if(i < (min(4, static_cast<int>(phase_group_size.size()-4*k))-1)) output_program << ", ";
        }

        output_program << ")); ";
        
        output_program << "float" << min(4, dimension-4*k) << " " << input_mod;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << " = fmod(" << input_variable;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << ", " << "float" << min(4, dimension-4*k) << "(";

        for(unsigned int i = 0; i < min(4, static_cast<int>(phase_group_size.size()-4*k)); i++)
        {
            output_program << phase_group_size[i+4*k];
            if(i < (min(4, static_cast<int>(phase_group_size.size()-4*k))-1)) output_program << ", ";
        }

        output_program << ")); ";
    }
    
    const string flattened_div(input_variable + "_flattened_div");
    const string flattened_mod(input_variable + "_flattened_mod");

    output_program << "float2 " << flattened_div << "; ";
    output_program << "float2 " << flattened_mod << "; ";

    output_program << CgSampling::GenerateFlattenString(clique_texture_size, input_div, flattened_div) << " ";

    output_program << CgSampling::GenerateFlattenString(phase_group_texture_size, input_mod, flattened_mod) << " ";

    output_program << output_variable << " = float2(" << phase_group_texture_size.TotalWidth() << ", " << phase_group_texture_size.TotalHeight() << ")*" << flattened_div << " + " << flattened_mod << "; ";
    
    return output_program.str();
}

string CgBlue::GenerateUnFlattenString(const TextureSize & texture_size, const TextureSize & phase_group_texture_size, const int clique_size, const string & input_variable, const string & output_variable)
{
    const vector<int> phase_group_size(phase_group_texture_size);
    
    TextureSize clique_texture_size(texture_size);
    clique_texture_size.Clear(clique_size);
    
    if(texture_size.Dimension() != phase_group_size.size())
    {
        // dimension mismatch
        return 0;
    }

    const int dimension = phase_group_size.size();
    const int num_buffers = RequiredDrawBuffers(dimension);

    ostringstream output_program;
    
    const string flattened_div(input_variable + "_flattened_div");
    const string flattened_mod(input_variable + "_flattened_mod");

    output_program << "float2 " << flattened_div << "; ";
    output_program << "float2 " << flattened_mod << "; ";

    output_program << flattened_div << " = floor(" << input_variable << "/float2(" << phase_group_texture_size.TotalWidth() << ", " << phase_group_texture_size.TotalHeight() << ")); ";
    output_program << flattened_mod << " = fmod(" << input_variable << ", float2(" << phase_group_texture_size.TotalWidth() << ", " << phase_group_texture_size.TotalHeight() << ")); ";
    
    const string output_div(output_variable + "_div");
    const string output_mod(output_variable + "_mod");

    for(int k = 0; k < num_buffers; k++)
    {
        output_program << "float" << min(4, dimension-4*k) << " " << output_div;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << "; ";
        output_program << "float" << min(4, dimension-4*k) << " " << output_mod;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << "; ";
    }
        
    output_program << CgSampling::GenerateUnFlattenString(clique_texture_size, flattened_div, output_div) << " ";
    
    output_program << CgSampling::GenerateUnFlattenString(phase_group_texture_size, flattened_mod, output_mod) << " ";

    for(int k = 0; k < num_buffers; k++)
    {
        output_program << output_variable;
        if(num_buffers > 1) output_program << "_" << k;
        output_program << " = float" << min(4, dimension-4*k) << "(";

        for(unsigned int i = 0; i < min(4, static_cast<int>(phase_group_size.size()-4*k)); i++)
        {
            output_program << phase_group_size[i+4*k];
            if(i < (min(4, static_cast<int>(phase_group_size.size()-4*k))-1)) output_program << ", ";
        }

        output_program << ")*" << output_variable << "_div";
        if(num_buffers > 1) output_program << "_" << k;
        output_program << " + " << output_variable << "_mod";
        if(num_buffers > 1) output_program << "_" << k;
        output_program << "; ";
    }
        
    return output_program.str();
}

int CgBlue::PackFlattenIntegerCoordinate(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const vector<int> & input, vector<int> & output)
{
    vector<int> packed = phase_group_size;

    if(! PackIntegerCoordinate(phase_group_size, clique_size, input, packed))
    {
        return 0;
    }

    if(! FlattenIntegerCoordinate(texture_size, phase_group_size, clique_size, packed, output))
    {
        return 0;
    }
    
    return 1;
}

int CgBlue::UnFlattenUnPackIntegerCoordinate(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const vector<int> & input, vector<int> & output)
{
    vector<int> packed = phase_group_size;

    if(! UnFlattenIntegerCoordinate(texture_size, phase_group_size, clique_size, input, packed))
    {
        return 0;
    }
    
    if(! UnPackIntegerCoordinate(phase_group_size, clique_size, packed, output))
    {
        return 0;
    }
       
    return 1;
}

string CgBlue::GeneratePackFlattenString(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const string & input_variable, const string & packed_variable, const string & output_variable)
{
    ostringstream output_program;

    output_program << GeneratePackString(phase_group_size, clique_size, input_variable, packed_variable) << " ";

    output_program << GenerateFlattenString(texture_size, phase_group_size, clique_size, packed_variable, output_variable) << "\n";
    
    return output_program.str();
}

string CgBlue::GenerateUnFlattenUnPackString(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const string & input_variable, const string & packed_variable, const string & output_variable)
{
    ostringstream output_program;

    output_program << GenerateUnFlattenString(texture_size, phase_group_size, clique_size, input_variable, packed_variable) << " ";
    
    output_program << GenerateUnPackString(phase_group_size, clique_size, packed_variable, output_variable) << "\n";

    return output_program.str();
}

string CgBlue::GenerateComponentString(const int dimension)
{
    ostringstream output_program;

    if((dimension <= 0) || (dimension > 4))
    {
        throw Exception("CgBlue::GenerateComponentString(): illegal dimension");
    }
    
    if(dimension >= 1)
    {
        output_program << ".x";
    }
    if(dimension >= 2)
    {
        output_program << "y";
    }
    if(dimension >= 3)
    {
        output_program << "z";
    }
    if(dimension >= 4)
    {
        output_program << "w";
    }

    return output_program.str();
}

string CgBlue::GenerateMainProgram(const TextureSize & texture_size, const int clique_size, const vector<int> & num_neighbors, const DistanceField::Boundary boundary_condition, const NoiseOption noise_option, const TextureSize & r_value_texture_size)
{
    const int height = texture_size.TotalHeight();
    const int width = texture_size.TotalWidth();
    const int dimension = texture_size.Dimension();
    const int num_buffers = RequiredDrawBuffers(dimension);

    const DistanceFieldOption distance_field_option = r_value_texture_size.TotalWidth()*r_value_texture_size.TotalHeight() == 0 ? UNIFORM : ADAPTIVE;

    TextureSize phase_group_size_temp = texture_size;

    if(! ComputePhaseGroupSize(texture_size, clique_size, phase_group_size_temp))
    {
        throw Exception("!ComputePhaseGroupSize(texture_size, clique_size, phase_group_size)");
    }

    const TextureSize phase_group_size(phase_group_size_temp);
    
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

    if(num_neighbors.size() == 1)
    //if(neighbor_offset_storage == NEIGHBOR_OFFSET_PROGRAM_CONSTANT)
    {
        if(num_buffers == 1)
        {
            output_program <<
                "                        uniform float2[" << num_neighbors[0] << "] neighbor_offsets,\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    "                        uniform float2[" << num_neighbors[0] << "] neighbor_offsets_" << i << ",\n";
            }
        }
    }
    else if(num_neighbors.size() == 2)
    //else if(neighbor_offset_storage == NEIGHBOR_OFFSET_TEXTURE)
    {
        if(num_buffers == 1)
        {
            output_program <<
                "                        uniform samplerRECT neighbor_offsets,\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    "                        uniform samplerRECT neighbor_offsets_" << i << ",\n";
            }
        }
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

     // existance checking portion
    output_program <<
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

    if(num_buffers == 1)
    {
        output_program <<
            "    float" << dimension << "    unflattened_cell_index;\n"
            "    float" << dimension << "    packed_cell_index;\n"
            "    float" << dimension << "    packed_neighbor_index;\n";
    }
    else
    {
        for(int i = 0; i < num_buffers; i++)
        {
            output_program <<
                "    float" << min(4, dimension-i*4) << " unflattened_cell_index_" << i << ";\n";
            output_program <<
                "    float" << min(4, dimension-i*4) << " packed_cell_index_" << i << ";\n";
            output_program <<
                "    float" << min(4, dimension-i*4) << " packed_neighbor_index_" << i << ";\n";
        } 
    }
    
    output_program << "    " << GenerateUnFlattenUnPackString(texture_size, phase_group_size, clique_size, "cell_index", "packed_cell_index", "unflattened_cell_index") << "\n";

    if(dimension == 2)
    {
        output_program <<
            "    result.xy = (result.xy + unflattened_cell_index)*cell_size;\n";
    }
    else if(dimension == 3)
    {
        output_program <<
            "    result.xyz = (result.xyz + unflattened_cell_index)*cell_size;\n";
    }
    else if(dimension == 4)
    {
        output_program <<
            "    result.xyzw = (result.xyzw + unflattened_cell_index)*cell_size;\n";
    }
    else if(dimension == 5)
    {
        output_program <<
            "    result_0.xyzw = (result_0.xyzw + unflattened_cell_index_0.xyzw)*cell_size;\n"
            "    result_1.x = (result_1.x + unflattened_cell_index_1.x)*cell_size;\n";
    }
    else if(dimension == 6)
    {
        output_program <<
            "    result_0.xyzw = (result_0.xyzw + unflattened_cell_index_0.xyzw)*cell_size;\n"
            "    result_1.xy = (result_1.xy + unflattened_cell_index_1.xy)*cell_size;\n";
    }
    else if(dimension == 7)
    {
        output_program <<
            "    result_0.xyzw = (result_0.xyzw + unflattened_cell_index_0.xyzw)*cell_size;\n"
            "    result_1.xyz = (result_1.xyz + unflattened_cell_index_1.xyz)*cell_size;\n";
    }
    else if(dimension == 8)
    {
        output_program <<
            "    result_0.xyzw = (result_0.xyzw + unflattened_cell_index_0.xyzw)*cell_size;\n"
            "    result_1.xyzw = (result_1.xyzw + unflattened_cell_index_1.xyzw)*cell_size;\n";
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
            "    {\n";

        if(num_buffers == 1)
        {
            output_program <<
                " float" << dimension << " unflattened_neighbor_index = unflattened_cell_index + neighbor_offsets[i];\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    " float" << min(4, dimension-i*4) << " unflattened_neighbor_index_" << i << " = unflattened_cell_index_" << i << " + neighbor_offsets_" << i << "[i];\n";
            } 
        }
    }
    else if(num_neighbors.size() == 2)
    //else if(neighbor_offset_storage == NEIGHBOR_OFFSET_TEXTURE)
    {
        output_program << 
            "    for(int j = 0; j < " << num_neighbors[1] << "; j++)\n"
            "    for(int i = 0; i < " << num_neighbors[0] << "; i++)\n";
           
        if(num_buffers == 1)
        { 
            output_program <<
                "    {\n"
                "       float4 neighbor_offsets_i = texRECT(neighbor_offsets, float2(i, j));\n";
            
            output_program <<
                " float" << dimension << " unflattened_neighbor_index = unflattened_cell_index + neighbor_offsets_i" << GenerateComponentString(dimension) << ";\n";
        }
        else
        {
            output_program <<
                "    {\n";
            
            for(int i = 0; i < num_buffers; i++)
            {

                output_program <<
                    "       float4 neighbor_offsets_i_" << i << " = texRECT(neighbor_offsets_" << i << ", float2(i, j));\n";
                
                output_program <<
                    "       float" << min(4, dimension-i*4) << " unflattened_neighbor_index_" << i << " = unflattened_cell_index_" << i << " + neighbor_offsets_i_" << i << GenerateComponentString(min(4, dimension-i*4)) << ";\n";
            }
        
        }
    }
    else
    {
        throw Exception("unsupported neighbor offset storage");
    }

    output_program <<
        "       float2 neighbor_index;\n";
    output_program << "       " << GeneratePackFlattenString(texture_size, phase_group_size, clique_size, "unflattened_neighbor_index", "packed_neighbor_index", "neighbor_index") << "\n";
    
    if(boundary_condition == DistanceField::BOUNDARY_TOROIDAL)
    {
        vector<int> texture_size_vec(texture_size);
        
        if(num_buffers == 1)
        {
            output_program <<
                "       float" << dimension << " texture_size = float" << dimension << "(";

            for(unsigned int i = 0; i < texture_size_vec.size(); i++)
            {
                output_program << texture_size_vec[i];
                if(i < (texture_size_vec.size()-1)) output_program << ", ";
            }
            output_program << ");\n";
            
            output_program <<
                "       float" << dimension << " unflattened_corrected_index = fmod(fmod(unflattened_neighbor_index, texture_size)+texture_size, texture_size);\n";

            output_program <<
                "       float2 corrected_index;\n";
            
            output_program << "       " << GeneratePackFlattenString(texture_size, phase_group_size, clique_size, "unflattened_corrected_index", "packed_cell_index", "corrected_index") << "\n";
    
            output_program <<
                "       float4 neighbor = texRECT(poisson_texture, corrected_index);\n"
                "       float4 neighbor_corrected = neighbor;\n"
                "       neighbor_corrected" << GenerateComponentString(dimension) << " += (unflattened_neighbor_index - unflattened_corrected_index)*cell_size;\n";
        }
        else
        {
            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    "       float" << min(4, dimension-4*i) << " texture_size_" << i << " = float" << min(4, dimension-4*i) << "(";     

                for(unsigned int j = 0; j < min(4, static_cast<int>(texture_size_vec.size()-4*i)); j++)
                {
                    output_program << texture_size_vec[j+4*i];
                    if(j < (min(4, static_cast<int>(texture_size_vec.size()-4*i))-1)) output_program << ", ";
                }
                output_program << ");\n";
            
                output_program <<
                    "       float" << min(4, dimension-4*i) << " unflattened_corrected_index_" << i << " = fmod(fmod(unflattened_neighbor_index_" << i << ", texture_size_" << i << ")+texture_size_" << i << ", texture_size_" << i << ");\n";
            }

            output_program <<
                "       float2 corrected_index;\n";
            
            output_program << "       " << GeneratePackFlattenString(texture_size, phase_group_size, clique_size, "unflattened_corrected_index", "packed_cell_index", "corrected_index") << "\n";

            for(int i = 0; i < num_buffers; i++)
            {
                output_program <<
                    "       float4 neighbor_" << i << " = texRECT(poisson_texture_" << i << ", corrected_index);\n";
                output_program <<
                    "       float4 neighbor_corrected_" << i << " = neighbor_" << i << ";\n";
                output_program <<
                    "       neighbor_corrected_" << i << GenerateComponentString(min(4, static_cast<int>(dimension-4*i))) << " += (unflattened_neighbor_index_" << i << " - unflattened_corrected_index_" << i << ")*cell_size;\n";
            }
        }
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
        throw Exception("adaptive sampling code not yet done");
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
        // output_program << "result = texRECT(poisson_texture, cell_index.xy);\n"; // debug
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

    return output_program.str();
}

string CgBlue::GenerateInitProgram(const TextureSize & texture_size, const int clique_size)
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

    TextureSize phase_group_size_temp = texture_size;

    if(! ComputePhaseGroupSize(texture_size, clique_size, phase_group_size_temp))
    {
        throw Exception("!ComputePhaseGroupSize(texture_size, clique_size, phase_group_size)");
    }

    const TextureSize phase_group_size(phase_group_size_temp);

    TextureSize parent_phase_group_size(phase_group_size);
    {
        for(unsigned int i = 0; i < parent_phase_group_size.height.size(); i++)
        {
            parent_phase_group_size.height[i] = phase_group_size.height[i]/2;
        }
        for(unsigned int i = 0; i < parent_phase_group_size.width.size(); i++)
        {
            parent_phase_group_size.width[i] = phase_group_size.width[i]/2;
        }
    }
    
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
        " float2 cell_index = floor(input.tex.xy);\n"
        " float2 parent_index;\n";

    if(num_buffers == 1)
    {
        output_program <<
            " float" << dimension << " unflattened_cell_index;\n"
            " float" << dimension << " packed_cell_index;\n"
            " float" << dimension << " unflattened_parent_index;\n"
            " float" << dimension << " packed_parent_index;\n";
    }
    else
    {
        for(int i = 0; i < num_buffers; i++)
        {
            output_program <<
                " float" << min(4, dimension-i*4) << " unflattened_cell_index_" << i << ";\n";
            output_program <<
                " float" << min(4, dimension-i*4) << " packed_cell_index_" << i << ";\n";
            output_program <<
                " float" << min(4, dimension-i*4) << " unflattened_parent_index_" << i << ";\n";
            output_program <<
                " float" << min(4, dimension-i*4) << " packed_parent_index_" << i << ";\n";
        }
        
    }
    
    output_program << " " << GenerateUnFlattenUnPackString(texture_size, phase_group_size, clique_size, "cell_index", "packed_cell_index", "unflattened_cell_index") << "\n";

    if(num_buffers == 1)
    {
        output_program <<
            " unflattened_parent_index = floor(unflattened_cell_index/2);\n";
    }
    else
    {
        for(int i = 0; i < num_buffers; i++)
        {
            output_program <<
                " unflattened_parent_index_" << i << " = floor(unflattened_cell_index_" << i << "/2);\n";
        }
    }

    output_program << " " << GeneratePackFlattenString(parent_texture_size, parent_phase_group_size, clique_size, "unflattened_parent_index", "packed_parent_index", "parent_index") << "\n";
    
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
            " float2 boundary_low = unflattened_cell_index*cell_size;\n"
            " float2 boundary_high = (unflattened_cell_index+1)*cell_size;\n"
            " if((parent_value.x >= boundary_low.x) && (parent_value.y >= boundary_low.y) && (parent_value.x < boundary_high.x) && (parent_value.y < boundary_high.y))\n";
    }
    else if(dimension == 3)
    {
        output_program <<
            " float3 boundary_low = unflattened_cell_index*cell_size;\n"
            " float3 boundary_high = (unflattened_cell_index+1)*cell_size;\n"
            " if((parent_value.x >= boundary_low.x) && (parent_value.y >= boundary_low.y) && (parent_value.z >= boundary_low.z) && (parent_value.x < boundary_high.x) && (parent_value.y < boundary_high.y) && (parent_value.z < boundary_high.z))\n";
    }
    else if(dimension == 4)
    {
        output_program <<
            " float4 boundary_low = unflattened_cell_index*cell_size;\n"
            " float4 boundary_high = (unflattened_cell_index+1)*cell_size;\n"
            " if((parent_value.x >= boundary_low.x) && (parent_value.y >= boundary_low.y) && (parent_value.z >= boundary_low.z) && (parent_value.w >= boundary_low.w) && (parent_value.x < boundary_high.x) && (parent_value.y < boundary_high.y) && (parent_value.z < boundary_high.z) && (parent_value.w < boundary_high.w))\n";
    }
    else if(dimension == 5)
    {
        output_program <<
            " float4 boundary_low_0 = unflattened_cell_index_0*cell_size;\n"
            " float4 boundary_high_0 = (unflattened_cell_index_0+1)*cell_size;\n"
            " float1 boundary_low_1 = unflattened_cell_index_1.x*cell_size;\n"
            " float1 boundary_high_1 = (unflattened_cell_index_1.x+1)*cell_size;\n"
            " if((parent_value_0.x >= boundary_low_0.x) && (parent_value_0.y >= boundary_low_0.y) && (parent_value_0.z >= boundary_low_0.z) && (parent_value_0.w >= boundary_low_0.w) && (parent_value_1.x >= boundary_low_1.x) && (parent_value_0.x < boundary_high_0.x) && (parent_value_0.y < boundary_high_0.y) && (parent_value_0.z < boundary_high_0.z) && (parent_value_0.w < boundary_high_0.w) && (parent_value_1.x < boundary_high_1.x))\n";
    }
    else if(dimension == 6)
    {
        output_program <<
            " float4 boundary_low_0 = unflattened_cell_index_0*cell_size;\n"
            " float4 boundary_high_0 = (unflattened_cell_index_0+1)*cell_size;\n"
            " float2 boundary_low_1 = unflattened_cell_index_1.xy*cell_size;\n"
            " float2 boundary_high_1 = (unflattened_cell_index_1.xy+1)*cell_size;\n"
            " if((parent_value_0.x >= boundary_low_0.x) && (parent_value_0.y >= boundary_low_0.y) && (parent_value_0.z >= boundary_low_0.z) && (parent_value_0.w >= boundary_low_0.w) && (parent_value_1.x >= boundary_low_1.x) && (parent_value_1.y >= boundary_low_1.y) && (parent_value_0.x < boundary_high_0.x) && (parent_value_0.y < boundary_high_0.y) && (parent_value_0.z < boundary_high_0.z) && (parent_value_0.w < boundary_high_0.w) && (parent_value_1.x < boundary_high_1.x) && (parent_value_1.y < boundary_high_1.y))\n";
    }
    else if(dimension == 7)
    {
        output_program <<
            " float4 boundary_low_0 = unflattened_cell_index_0*cell_size;\n"
            " float4 boundary_high_0 = (unflattened_cell_index_0+1)*cell_size;\n"
            " float3 boundary_low_1 = unflattened_cell_index_1.xyz*cell_size;\n"
            " float3 boundary_high_1 = (unflattened_cell_index_1.xyz+1)*cell_size;\n"
            " if((parent_value_0.x >= boundary_low_0.x) && (parent_value_0.y >= boundary_low_0.y) && (parent_value_0.z >= boundary_low_0.z) && (parent_value_0.w >= boundary_low_0.w) && (parent_value_1.x >= boundary_low_1.x) && (parent_value_1.y >= boundary_low_1.y)  && (parent_value_1.z >= boundary_low_1.z) && (parent_value_0.x < boundary_high_0.x) && (parent_value_0.y < boundary_high_0.y) && (parent_value_0.z < boundary_high_0.z) && (parent_value_0.w < boundary_high_0.w) && (parent_value_1.x < boundary_high_1.x) && (parent_value_1.y < boundary_high_1.y) && (parent_value_1.z < boundary_high_1.z))\n";
    }
    else if(dimension == 8)
    {
        output_program <<
            " float4 boundary_low_0 = unflattened_cell_index_0*cell_size;\n"
            " float4 boundary_high_0 = (unflattened_cell_index_0+1)*cell_size;\n"
            " float4 boundary_low_1 = unflattened_cell_index_1*cell_size;\n"
            " float4 boundary_high_1 = (unflattened_cell_index_1+1)*cell_size;\n"
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

    // output_program << "    result = parent_value;\n"; // debug
    
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
    
    return output_program.str();
}

void CgBlue::RenderQuad(const vector<int> & current_phase) const
{
    if(current_phase.size() != _texture_size.Dimension())
    {
        throw Exception("current_phase.size() != _texture_size.Dimension()");
    }

    TextureSize phase_group_size(_texture_size);
    
    if(!ComputePhaseGroupSize(_texture_size, _clique_size, phase_group_size))
    {
        throw Exception("!ComputePhaseGroupSize(_texture_size, _clique_size, phase_group_size)");
    }

    for(unsigned int i = 0; i < current_phase.size(); i++)
    {
        if((current_phase[i] < 0) || (current_phase[i] >= _clique_size))
        {
            throw Exception("current_phase out of range");
        }
    }
    
    TextureSize clique_texture_size(_texture_size);
    clique_texture_size.Clear(_clique_size);
    
    vector<int> flattened_phase(2);

    if(! CgSampling::FlattenIntegerCoordinate(clique_texture_size, current_phase, flattened_phase))
    {
        throw Exception("cannot flatten current_phase_group");
    }
    
    const int total_height = _texture_size.TotalHeight();
    const int total_width = _texture_size.TotalWidth();

    const int phase_height = phase_group_size.TotalHeight();
    const int phase_width = phase_group_size.TotalWidth();

    // relative to [0 1] 
    const float height_start = flattened_phase[1]*phase_height*1.0/total_height;
    const float width_start = flattened_phase[0]*phase_width*1.0/total_width;
    
    const float height_end = (flattened_phase[1]+1)*phase_height*1.0/total_height;
    const float width_end = (flattened_phase[0]+1)*phase_width*1.0/total_width;
    

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    // glClear(GL_COLOR_BUFFER_BIT); // don't do this as it would destroy results between different rendering passes, such as copy and main
    glBegin(GL_QUADS);
  
    glColor3f(0.0, 0.0, 1.0);  // blue
    glTexCoord3f(width_start*total_width, height_start*total_height, 1.0);
    glVertex2f(-1 + 2*width_start, -1 + 2*height_start);
 
    glColor3f(0.0, 1.0, 0.0);  // green
    glTexCoord3f(width_end*total_width, height_start*total_height, 1.0);
    glVertex2f(-1 + 2*width_end, -1 + 2*height_start);
  
    glColor3f(1.0, 0.0, 0.0);  // red
    glTexCoord3f(width_end*total_width, height_end*total_height, 1.0);
    glVertex2f(-1 + 2*width_end, -1 + 2*height_end);
    
    glColor3f(1.0, 1.0, 0.0);  // yellow
    glTexCoord3f(width_start*total_width, height_end*total_height, 1.0);
    glVertex2f(-1 + 2*width_start, -1 + 2*height_end);
    
    glEnd();
}
