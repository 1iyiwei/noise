/*
  CgSampling.hpp

  the base class for all Poisson disk sampling subclasses

  Li-Yi Wei
  11/21/2007

*/

#ifndef _CG_SAMPLING_HPP
#define _CG_SAMPLING_HPP

// #define _STATIC_CONTEXT_FOR_cgGetLastListing

#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "Sample.hpp"
#include "DistanceField.hpp"
#include "framebufferObject.h"

class CgSampling
{
public:
    // this data structure encodes the packing of multi-dimensional data
    // into a 2D texture map
    // specifically, dimension = height.size() + width.size()
    // the texture tile size is height[height.size()-1] x width[width.size()-1]
    // and the tile arrangement is specified by other values in height/width
    // the original dimension vector == width concatenate height
    struct TextureSize
    {
    public:
        TextureSize(void);
        TextureSize(const vector<int> & input);
        TextureSize(const vector<int> & height, const vector<int> & width);
        
        int Dimension(void) const;
        int Size(const int which_dimension) const;
        
        int TotalHeight(void) const;
        int TotalWidth(void) const;
        //int TileHeight(void) const;
        //int TileWidth(void) const;

        operator vector<int>(void) const;

        // assign a single value to all elements
        void Clear(const int value);
        
    public:
        vector<int> height;
        vector<int> width;
    };

    typedef enum {NEIGHBOR_OFFSET_PROGRAM_CONSTANT, NEIGHBOR_OFFSET_TEXTURE} NeighborOffsetStorage;

    typedef enum {UNIFORM, ADAPTIVE} DistanceFieldOption;

    typedef enum {TEXTURE_NOISE, MD5_NOISE} NoiseOption;
    
public:
    CgSampling(const TextureSize & texture_size, const int clique_size, const vector<int> & initial_noise_keys);
    virtual ~CgSampling(void) = 0;

    // initialization stuff
    void Initialize(const vector<GLuint> & initial_value_textures);
    void SetMinimumDistance(const float r_value);
    void SetMinimumDistance(const GLuint r_value_texture);
    void SetWhiteNoise(const vector<GLuint> & white_noise_textures);
    
    // mostly for debug and visualization purposes
    void Display(void) const;
    void GetColor(Array<FrameBuffer::AF> & pixels);
    void GetColor(Array<FrameBuffer::P3> & pixels);

    // obtain intermediate or final results
    vector<GLuint> GetResultTexture(void) const;
    void GetSamples(vector<const Sample *> & samples);
    
    // required number of MRT
    int RequiredDrawBuffers(void) const;
    static int RequiredDrawBuffers(const int dimension);

    // texture size stuff
    void GetTextureSize(TextureSize & answer) const;
    static int ConvertTextureSize(const vector<int> & input, TextureSize & output);

    // texture generation
    static GLuint GenerateWhiteNoise(const TextureSize & texture_size, const GLuint white_noise_texture_id);
    static vector<GLuint> GenerateWhiteNoise(const TextureSize & texture_size, const vector<GLuint> & white_noise_texture_id);
    static GLuint GenerateRValueTexture(const TextureSize & texture_size, const Array<float> & data, const GLuint r_value_texture_id);
 
    // hash function
    static int Hash(const int input);
    
protected:
    static GLuint GenerateRenderTexture(const TextureSize & texture_size, const GLuint render_texture_id);

    static vector<int> NeighborOffsetTextureSize(const int num_neighbor_offsets);
    
    static int FlattenIntegerCoordinate(const TextureSize & texture_size, const vector<int> & input, vector<int> & output);
    
    static int UnFlattenIntegerCoordinate(const TextureSize & texture_size, const vector<int> & input, vector<int> & output);

    static string GenerateFlattenString(const TextureSize & texture_size, const string & input_variable, const string & output_variable);
    static string GenerateUnFlattenString(const TextureSize & texture_size, const string & input_variable, const string & output_variable);
    
    static string GenerateComponentChar(const int which_dimension);
    
    static string GenerateCopyProgram(const TextureSize & texture_size);
    
    static string GenerateDisplayProgram(void);
    
    static string GetGLError(void);
    static void CheckError(void);

    void RenderQuad(void) const;
    
protected:
    const TextureSize _texture_size;

#ifdef _STATIC_CONTEXT_FOR_cgGetLastListing
    static CGcontext _context;
#else
    CGcontext _context;
#endif
    
    CGprofile _fragment_profile;

    CGprogram _program_copy;
    vector<CGparameter> _copy_textures;

    CGprogram _program_main;
    CGparameter _r_value;
    CGparameter _r_value_texture;
    CGparameter _cell_size_main;
    CGparameter _clique_size_main;
    vector<CGparameter> _noise_textures;
    vector<CGparameter> _poisson_textures;

    CGprogram _program_init;
    CGparameter _cell_size_init;
    vector<CGparameter> _init_textures;

    CGprogram _program_display;
    CGparameter _display_texture;
    
    bool _ping_pong; // indicating the texture source at current pass
    vector<GLuint> _textures; // the two textures for ping-pong times # of MRT
    
    FramebufferObject _fbo;
    GLint _current_draw_buffer;

    const int _clique_size;
    
    // for MD5 noise
    const NoiseOption _noise_option;
    vector<int> _initial_keys;
    vector<CGparameter> _keys;
};

#endif
