/*
  CgBlue.hpp

  better implementation of CgPoisson
  I prefer to start from a clean slate for 2 reasons:
  1. The original code of CgPoisson is already quite complex
  2. The original code provides good visualization
  
  Li-Yi Wei
  11/22/2007

*/

#ifndef _CG_BLUE_HPP
#define _CG_BLUE_HPP

#include "CgSampling.hpp"

class CgBlue : public CgSampling
{
public:
    CgBlue(const TextureSize & texture_size, const float cell_size, const int clique_size, const vector< vector<int> > & neighbor_offsets, const DistanceField::Boundary boundary_condition, const NeighborOffsetStorage neighbor_offset_storage, const vector<int> & initial_noise_keys, const TextureSize & r_value_texture_size);
    ~CgBlue(void);
    
    void Update(const vector<int> & current_phase);
    
    // returns the updated texture id
    // if the input texture id is valid, will use it and replace content
    // return 0 if error
    static vector<GLuint> GenerateInitTexture(const TextureSize & texture_size, const float cell_size, const int clique_size, const vector<const Sample *> & samples, const vector<GLuint> & init_texture_id);
    
protected:
    static vector<GLuint> GenerateNeighborOffsetTexture(const vector< vector< vector<int> > > & diced_neighbor_offsets, const vector<GLuint> & neighbor_offset_texture_id, const vector<int> & offset_texture_size);

    static int DiceNeighborOffsets(const TextureSize & texture_size, const vector< vector<int> > & input, vector< vector< vector<int> > > & output);

    static int ComputePhaseGroupSize(const TextureSize & texture_size, const int clique_size, TextureSize & phase_group_size);

    // pack functions between original layout and coherent layout
    // where pixels of the same phase group are sitting together
    static int PackIntegerCoordinate(const TextureSize & phase_group_size, const int clique_size, const vector<int> & input, vector<int> & output);
    static int UnPackIntegerCoordinate(const TextureSize & phase_group_size, const int clique_size, const vector<int> & input, vector<int> & output);
    
    // flattening functions
    // it appears that I need N-D to 2D transformation for CPU preprocess
    // but 2D to N-D transform for runtime Cg GPU
    static int FlattenIntegerCoordinate(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const vector<int> & input, vector<int> & output);

    // inverse function of above, not really used for just for debugging
    static int UnFlattenIntegerCoordinate(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const vector<int> & input, vector<int> & output);

    static int PackFlattenIntegerCoordinate(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const vector<int> & input, vector<int> & output);

    // inverse function of above, not really used for just for debugging
    static int UnFlattenUnPackIntegerCoordinate(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const vector<int> & input, vector<int> & output);

    // the string generators must be equivalent to the numerical cousins above
    static string GeneratePackString(const TextureSize & phase_group_size, const int clique_size, const string & input_variable, const string & output_variable);
    static string GenerateUnPackString(const TextureSize & phase_group_size, const int clique_size, const string & input_variable, const string & output_variable);
    static string GenerateFlattenString(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const string & input_variable, const string & output_variable);
    static string GenerateUnFlattenString(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const string & input_variable, const string & output_variable);

    static string GeneratePackFlattenString(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const string & input_variable, const string & packed_variable, const string & output_variable);
    static string GenerateUnFlattenUnPackString(const TextureSize & texture_size, const TextureSize & phase_group_size, const int clique_size, const string & input_variable, const string & packed_variable, const string & output_variable);

    static string GenerateComponentString(const int dimension);
    
    // if num_neighbors.size() == 1, it means NEIGHBOR_OFFSET_PROGRAM_CONSTANT
    // if num_neighbors.size() == 1, it means NEIGHBOR_OFFSET_TEXTURE
    static string GenerateMainProgram(const TextureSize & texture_size, const int clique_size, const vector<int> & num_neighbors, const DistanceField::Boundary boundary_condition, const NoiseOption noise_option, const TextureSize & r_value_texture_size);

    static string GenerateInitProgram(const TextureSize & texture_size, const int clique_size);

    void RenderQuad(const vector<int> & current_phase) const;

protected:

    vector<CGparameter> _neighbor_offsets;
    vector<GLuint> _neighbor_offset_textures;
};

#endif
