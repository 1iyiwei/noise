/*
  CgPoisson.hpp

  use GPU for Poisson disk sampling

  Li-Yi Wei
  
*/

#ifndef _CG_POISSON_HPP
#define _CG_POISSON_HPP

#include "CgSampling.hpp"

class CgPoisson : public CgSampling
{
public:
    // the highest resolution of the initialized mipmap (tip) from a CPU
    // the real computation will be conducted at twice the resolution
    // (per dimension) of this initial value texture
    // the dimension information is implicit in clique_size.size()
    CgPoisson(const TextureSize & texture_size, const float cell_size, const int clique_size, const vector< vector<int> > & neighbor_offsets, const DistanceField::Boundary boundary_condition, const NeighborOffsetStorage neighbor_offset_storage, const vector<int> & initial_noise_keys, const TextureSize & r_value_texture_size);
    ~CgPoisson(void);

    // only pixels with coordinates%clique_size == current_phase will compute
    // the rest will simply copy
    // current_phase is logically a concatenation width+height of TextureSize
    void Update(const vector<int> & current_phase);

    // returns the updated texture id
    // if the input texture id is valid, will use it and replace content
    // return 0 if error
    static vector<GLuint> GenerateInitTexture(const TextureSize & texture_size, const float cell_size, const int clique_size, const vector<const Sample *> & samples, const vector<GLuint> & init_texture_id);

protected:
    static GLuint GenerateNeighborOffsetTexture(const vector< vector<int> > & flattened_neighbor_offsets, const GLuint neighbor_offset_texture_id, const vector<int> & offset_texture_size);
    
    // flattening functions
    static int FlattenNeighborOffsets(const TextureSize & texture_size, const int clique_size, const vector< vector<int> > & input, vector< vector<int> > & output);

    // it appears that I need N-D to 2D transformation for CPU preprocess
    // but 2D to N-D transform for runtime Cg GPU
    static int FlattenIntegerCoordinate(const TextureSize & texture_size, const vector<int> & input, vector<int> & output);
    
    // make sure this function is equivalent to the one above
    static string GenerateParentIndexString(const TextureSize & parent_texture_size, const int clique_size);

    // make sure this function is exact inverse of the flatten function above
    static string GenerateCellIndexNDString(const TextureSize & texture_size, const int clique_size);
    
    // if num_neighbors.size() == 1, it means NEIGHBOR_OFFSET_PROGRAM_CONSTANT
    // if num_neighbors.size() == 1, it means NEIGHBOR_OFFSET_TEXTURE
    static string GenerateMainProgram(const TextureSize & texture_size, const int clique_size, const vector<int> & num_neighbors, const DistanceField::Boundary boundary_condition, const NoiseOption noise_option, const TextureSize & r_value_texture_size);

    static string GenerateInitProgram(const TextureSize & texture_size, const int clique_size);

protected:
    vector<CGparameter> _current_phases;
    
    CGparameter _neighbor_offsets; // handles both constant and texture case
    GLuint _neighbor_offset_texture; // when neighbor offsets are not stored in constants
};

#endif
