/*
  BlueGPU.cpp

  the main driver for GPU Poisson disk sampling (Blue noise) generation

  Li-Yi Wei
  11/26/2007

*/

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>

#include "Exception.hpp"
#include "Random.hpp"
#include "KDTree.hpp"
#include "UniformDistanceField.hpp"
#include "RandomCounter.hpp"
#include "PhaseGroup.hpp"

#include "CgBlue.hpp"
#include "framebufferObject.h"
#include "glErrorUtil.h"

#include "Timer.hpp"

#ifdef NDEBUG 
// using this will currently cause run time error during program exit
#define _PRECOMPUTE_WHITE_NOISE
#endif

// #define _LAST_LEVEL_PHASE_GROUP_1

class ExitException : public Exception
{
public:
    ExitException(const string & message);
};

ExitException::ExitException(const string & message) : Exception(message)
{
    // nothing to do
}

struct HouseKeeper
{
    HouseKeeper(void) : p_poisson(0), p_phase_counter(0), dimension(0), min_distance(0), k_number(0), root_cell_size(0), all_phase_complete(1), show_off(1), last_frame_dumped(0) {}
    ~HouseKeeper(void)
        {
            if(p_poisson) delete p_poisson;
            if(p_phase_counter) delete p_phase_counter;
        }

    CgBlue * p_poisson;
    Counter * p_phase_counter;
    int dimension;
    float min_distance;
    int k_number;
    float root_cell_size;
    int clique_size;
    int current_k_number;
    float cell_size;
    float min_cell_size;
#ifdef _PRECOMPUTE_WHITE_NOISE
    vector< vector<GLuint> > white_noise_textures;
#else
    vector<GLuint> white_noise_texture;
#endif
    CgBlue::TextureSize largest_texture_size;
    vector<GLuint> init_texture;
    bool all_phase_complete;
    vector<int> current_phase;
    CgBlue::TextureSize texture_size;
    vector< vector<int> > neighbor_offsets;
    DistanceField::Boundary boundary_condition;
    CgBlue::NeighborOffsetStorage neighbor_offset_storage;
    vector<int> initial_noise_keys;
    CgBlue::TextureSize r_value_texture_size;
    GLuint r_value_texture;
    int show_off;
    int last_frame_dumped; // for image dumping
    Timer timer;
    Timer watch; // for other purposes
    double total_watch_time; // accumulate watch results
};

HouseKeeper house_keeper;

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        throw ExitException("exit");
        break;
        
    default:
        break;
    }
}

void Idle(void)
{
    glutPostRedisplay();
}

void Display(void)
{
#if 0
    {
        cerr << "house_keeper.current_k_number " << house_keeper.current_k_number << ", house_keeper.current_phase ";
        for(unsigned int i = 0; i < house_keeper.current_phase.size(); i++)
        {
            cerr <<  house_keeper.current_phase[i];
        }
        cerr << endl;
    }
#endif
    
    if(! house_keeper.all_phase_complete)
    {
        house_keeper.p_phase_counter->Get(house_keeper.current_phase);
        house_keeper.all_phase_complete = !house_keeper.p_phase_counter->Next();

        house_keeper.p_poisson->Update(house_keeper.current_phase);
        if(0) { // debug
            vector<const Sample *> samples;
            house_keeper.p_poisson->GetSamples(samples);
            
            cout << house_keeper.cell_size << " - " << "total # of samples " << samples.size() << endl;
                
            for(unsigned int i = 0; i < samples.size(); i++)
            {
                if(samples[i])
                {
                    for(unsigned int j = 0; j < samples[i]->coordinate.Dimension(); j++)
                    {
                        cout << "   " << samples[i]->coordinate[j] << " ";
                    }
                    cout << endl;
                }
            }
        }
    }
    else
    {
        house_keeper.p_poisson->GetTextureSize(house_keeper.texture_size);
            
        if(house_keeper.current_k_number >= (house_keeper.k_number-1))
        {
            house_keeper.watch.Stop();

            if((house_keeper.texture_size.TotalWidth() == house_keeper.largest_texture_size.TotalWidth()) && (house_keeper.texture_size.TotalHeight() == house_keeper.largest_texture_size.TotalHeight()))
            {
                cerr << "last level ";
            }
                        
            house_keeper.total_watch_time += house_keeper.watch.ElapsedTime();
            cerr << "watch time " << house_keeper.watch.ElapsedTime() << endl;
        
            if(house_keeper.cell_size <= house_keeper.min_cell_size)
            {
                // done
                throw ExitException("done");
            }
            else
            {
                // go to the next higher resolution
                for(unsigned int i = 0; i < house_keeper.texture_size.height.size(); i++)
                {
                    house_keeper.texture_size.height[i] *= 2;
                }
                for(unsigned int i = 0; i < house_keeper.texture_size.width.size(); i++)
                {
                    house_keeper.texture_size.width[i] *= 2;
                }

                house_keeper.cell_size /= 2;
                house_keeper.init_texture = house_keeper.p_poisson->GetResultTexture();

                glutReshapeWindow(house_keeper.texture_size.TotalWidth(), house_keeper.texture_size.TotalHeight());
            }
        }
        else
        {
            house_keeper.p_phase_counter->Reset();
            house_keeper.current_k_number++;

            if(house_keeper.initial_noise_keys.size() <= 0)
            {
#ifdef _PRECOMPUTE_WHITE_NOISE
                assert((house_keeper.current_k_number >= 0) && (house_keeper.current_k_number < house_keeper.white_noise_textures.size()));
                house_keeper.p_poisson->SetWhiteNoise(house_keeper.white_noise_textures[house_keeper.current_k_number]);
#else
                house_keeper.white_noise_texture = house_keeper.p_poisson->GenerateWhiteNoise(house_keeper.texture_size, house_keeper.white_noise_texture);
                house_keeper.p_poisson->SetWhiteNoise(house_keeper.white_noise_texture);
#endif
            }
            house_keeper.all_phase_complete = 0;
        }
    }

    if(house_keeper.show_off) house_keeper.p_poisson->Display();

    // dump images for display
    //if(house_keeper.current_k_number == (house_keeper.k_number-1))
    if(house_keeper.show_off > 1)
    {
        Array<FrameBuffer::P3> pixels;
        house_keeper.p_poisson->GetColor(pixels);

        ostringstream file_name;
#if 0
        file_name << "poisson_color_s" << house_keeper.texture_size.TotalHeight() << "_k" << house_keeper.current_k_number << "_p";
        for(unsigned int i = 0; i < house_keeper.current_phase.size(); i++)
        {
            file_name << house_keeper.current_phase[i];
        }
#else
        file_name << "poisson_color_" << setw(3) << setfill('0') << house_keeper.last_frame_dumped;
        house_keeper.last_frame_dumped++;
#endif
        file_name << ".ppm";

        FrameBuffer::WritePPM(pixels, 255, file_name.str().c_str());
    }

    // done
    glutSwapBuffers();
}

void Reshape(int width, int height)
{
#if 1
    if((width < house_keeper.texture_size.TotalWidth()) || (height < house_keeper.texture_size.TotalHeight()))
    {
        cerr << "width " << width << ", house_keeper.texture_size.TotalWidth() " << house_keeper.texture_size.TotalWidth() << ", height " << height << ",  house_keeper.texture_size.TotalHeight() " <<  house_keeper.texture_size.TotalHeight() << endl;
        throw Exception("incompatible sizes for reshape");
    }
#endif
    
    glViewport(0, 0, house_keeper.texture_size.TotalWidth(), house_keeper.texture_size.TotalHeight());
    CgBlue * p_old_poisson = house_keeper.p_poisson;

    {
        for(unsigned int i = 0; i < house_keeper.initial_noise_keys.size(); i++)
        {
            house_keeper.initial_noise_keys[i] = CgBlue::Hash(house_keeper.initial_noise_keys[(i+house_keeper.initial_noise_keys.size()-1)%house_keeper.initial_noise_keys.size()]);
        }
    }
    
    house_keeper.p_poisson = new CgBlue(house_keeper.texture_size, house_keeper.cell_size, house_keeper.clique_size, house_keeper.neighbor_offsets, house_keeper.boundary_condition, house_keeper.neighbor_offset_storage, house_keeper.initial_noise_keys, house_keeper.r_value_texture_size);

    house_keeper.p_poisson->Initialize(house_keeper.init_texture);
    // if(house_keeper.texture_size.TotalHeight() >= 8) throw ExitException("debug"); // debug

    if(0) { // debug
            vector<const Sample *> samples;
            house_keeper.p_poisson->GetSamples(samples);
            
            cout << "initialize " << house_keeper.cell_size << " - " << "total # of samples " << samples.size() << endl;
                
            for(unsigned int i = 0; i < samples.size(); i++)
            {
                if(samples[i])
                {
                    for(unsigned int j = 0; j < samples[i]->coordinate.Dimension(); j++)
                    {
                        cout << "   " << samples[i]->coordinate[j] << " ";
                    }
                    cout << endl;
                }
            }
    }
    if(house_keeper.r_value_texture <= 0)
    {
        house_keeper.p_poisson->SetMinimumDistance(house_keeper.min_distance);
    }
    else
    {
        house_keeper.p_poisson->SetMinimumDistance(house_keeper.r_value_texture);
    }
    house_keeper.current_k_number = -1;

#ifdef _LAST_LEVEL_PHASE_GROUP_1
    if((house_keeper.texture_size.TotalWidth() == house_keeper.largest_texture_size.TotalWidth()) && (house_keeper.texture_size.TotalHeight() == house_keeper.largest_texture_size.TotalHeight()))
    {
        cerr << "last level phase group 1 " << endl;
        if(house_keeper.p_phase_counter)
        {
            delete house_keeper.p_phase_counter;
        }
        house_keeper.p_phase_counter = new RandomCounter(house_keeper.dimension, 0, 0);
    }
#endif
    
    if(p_old_poisson) delete p_old_poisson;
    
    house_keeper.watch.Start();
}

CgBlue::TextureSize ComputeLargestTextureSize(const CgBlue::TextureSize & texture_size, const float init_cell_size, const float min_cell_size)
{
    // largest possible texture size
    CgBlue::TextureSize largest_texture_size = texture_size;
    float current_cell_size = init_cell_size;

    while(current_cell_size > min_cell_size)
    {
        current_cell_size /= 2;
        for(unsigned int i = 0; i < largest_texture_size.height.size(); i++)
        {
            largest_texture_size.height[i] *= 2;
        }
        for(unsigned int i = 0; i < largest_texture_size.width.size(); i++)
        {
            largest_texture_size.width[i] *= 2;
        }
    }

    return largest_texture_size;
}

#include "RampDistanceField.hpp"
#include "SampledDistanceField.hpp"

int GetDistanceField(const int dimension, const float min_distance, const string & importance_field_name, const vector<float> & range_min, const vector<float> & range_max, Array<float> & distance_field_data, const CgBlue::TextureSize & texture_size, const float init_cell_size, const float min_cell_size)
{
    if(importance_field_name == "ramp")
    {
        const CgBlue::TextureSize largest_texture_size = ComputeLargestTextureSize(texture_size, init_cell_size, min_cell_size);

        if(largest_texture_size.Dimension() != dimension) return 0;
        
        vector<int> field_size(dimension);

        for(unsigned int i = 0; i < largest_texture_size.height.size(); i++)
        {
            field_size[i] = largest_texture_size.height[i];
        }
        for(unsigned int i = 0; i < largest_texture_size.width.size(); i++)
        {
            field_size[i+largest_texture_size.height.size()] = largest_texture_size.width[i];
        }

        distance_field_data = Array<float>(field_size);

        RampDistanceField ramp_distance_field(range_min, range_max, 2*min_distance);
    
        if(dimension != 2)
        {
            throw Exception("can only handle 2D ramp now!");
        }

        Coordinate coordinate(2);
        
        vector<int> index(dimension);
        for(int i = 0; i < field_size[0]; i++)
        {
            index[0] = i;
            coordinate[0] = range_min[0] + (i+0.5)*(range_max[0] - range_min[0])/field_size[0];
            
            for(int j = 0; j < field_size[1]; j++)
            {
                index[1] = j;
                coordinate[1] = range_min[1] + (j+0.5)*(range_max[1] - range_min[1])/field_size[1];

                const float value = ramp_distance_field.Query(coordinate);

                distance_field_data.Put(index, value);
            }
        }
#if 0
        // debug
        Array<FrameBuffer::PF> pixels(field_size);
        for(int i = 0; i < field_size[0]; i++)
        {
            index[0] = i;
            
            for(int j = 0; j < field_size[1]; j++)
            {
                index[1] = j;

                float value;
                distance_field_data.Get(index, value);

                FrameBuffer::PF pixel;
                pixel.r = pixel.g = pixel.b = value;
                pixels.Put(index, pixel);
            }
        }

        FrameBuffer::WritePFM(pixels, "data/foo.pfm");
#endif
    }
    else
    {
        // right now, it only reads in importance field without any conversion
        throw Exception("image distance field code not yet done!");

        // read in image, recording importance
        Array<FrameBuffer::PF> image;

        if(! FrameBuffer::ReadPFM(importance_field_name.c_str(), image))
        {
            return 0;
        }

        if(image.Dimension() != dimension) return 0;
    
        // convert into gray scale importance
        vector<int> field_size;
        image.Size(field_size);

        distance_field_data = Array<float>(field_size);

        vector<FrameBuffer::PF> input_data;
        vector<float> output_data;

        if(! image.Get(input_data)) return 0;
        if(! distance_field_data.Get(output_data)) return 0;

        assert(input_data.size() == output_data.size());

        for(unsigned int i = 0; i < output_data.size(); i++)
        {
            output_data[i] = input_data[i].g;
        }
    
        if(! distance_field_data.Put(output_data)) return 0;
    }
    
    // done
    return 1;
}

int Main(int argc, char **argv)
{
    // process input arguments
#ifndef _ADAPTIVE_GPU
    const int num_fixed_arguments = 11;
#else
    const int num_fixed_arguments = 12;
#endif
    
    if(argc < num_fixed_arguments)
    {
        cerr << "Usage: " << "dimension r k num_cpu_levels root_cell_size clique_size (if <= 0 default will be used) boundary_condition (0 for none, 1 for toroidal) neighbor_offset_storage (0 for program constant, 1 for texture) noise_option (0 for texture, 1 for md5) show-off (0 for none, 1 for display, 2 for display and image dump)";

#ifdef _ADAPTIVE_GPU
        cerr << " importance_field_name ";
#endif
        
        cerr << " importance_field_size (dimension numbers)" << endl;

        return 1;
    }
    
    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float min_distance = atof(argv[++argCtr]);
    const int k_number = atoi(argv[++argCtr]);
    const int num_cpu_levels = atoi(argv[++argCtr]);
    const PhaseGroup::Order order = PhaseGroup::SEQUENTIAL_RANDOM;
    const float root_cell_size = atof(argv[++argCtr]);
    const int input_clique_size = atoi(argv[++argCtr]);
    const DistanceField::Boundary boundary_condition = static_cast<DistanceField::Boundary>(atoi(argv[++argCtr]));
    const CgBlue::NeighborOffsetStorage neighbor_offset_storage = static_cast<CgBlue::NeighborOffsetStorage>(atoi(argv[++argCtr]));
    const CgBlue::NoiseOption noise_option = static_cast<CgBlue::NoiseOption>(atoi(argv[++argCtr]));
    const int show_off = atoi(argv[++argCtr]);
    
#ifdef _ADAPTIVE_GPU
    const string importance_field_name = argv[++argCtr];
#else
    const string importance_field_name = "";
#endif
    
    if(argc < num_fixed_arguments+dimension)
    {
        cerr << "insufficient number of importance_size arguments" << endl;
        return 1;
    }

    if(dimension > 6)
    {
        cerr << "can only handle up to 6D now!" << endl;
        return 1;
    }

    if(num_cpu_levels <= 0)
    {
        cerr << "need at least one CPU level for initialization" << endl;
        return 1;
    }

    // init stuff
    vector<float> range_min(dimension);
    vector<float> range_max(dimension);
    {
        for(int i = 0; i < dimension; i++)
        {
            range_min[i] = 0.0;
            range_max[i] = atof(argv[++argCtr]); //instead of 1.0
        }
    }

    UniformDistanceField distance_field(range_min, range_max, min_distance);

    KDTree tree(dimension, root_cell_size, distance_field);

    Random::InitRandomNumberGenerator();

    // CPU computation
    
    do
    {
        const int current_level = tree.NumLevels()-1;

        vector<int> num_cells_per_dim(dimension);
        tree.GetNumCellsPerDimension(current_level, num_cells_per_dim);

        {
            cerr << "level: " << current_level << ", grid size: ";

            for(int i = 0; i < dimension; i++)
            {
                cerr << num_cells_per_dim[i] << " ";
            }
            cerr << ", # cells: " << tree.NumCells(current_level) << endl;
        }
        
        vector< vector<KDTree::Cell *> > phase_groups;
        if(! PhaseGroup::Compute(order, tree, current_level, phase_groups))
        {
            cerr << "cannot compute phase groups" << endl;
            return 1;
        }

        cerr << "phase group # " << phase_groups.size() << endl;
        
        const float cell_size = tree.CellSize(current_level);
        vector<int> cell_index(dimension);
        
        for(unsigned int i = 0; i < phase_groups.size(); i++)
            for(unsigned int j = 0; j < phase_groups[i].size(); j++)
            {
                KDTree::Cell * current_cell = phase_groups[i][j];

                if(current_cell)
                {
                    if(! current_cell->GetIndex(cell_index))
                    {
                        cerr << "error in getting cell index" << endl;
                        return 1;
                    }

                    assert(cell_index.size() == dimension);
                    
                    for(int trial = 0; (trial < k_number) && (current_cell->NumSamples() == 0); trial++)
                    {
                        Sample * sample = new Sample(dimension);
                        assert(sample);
                        
                        for(int n = 0; n < dimension; n++)
                        {
                            sample->coordinate[n] = (cell_index[n]+Random::UniformRandom())*cell_size;
                        }

                        if(! current_cell->EccentricConflict(*sample) && distance_field.Inside(sample->coordinate))
                        {
                            if(! current_cell->Add(*sample))
                            {
                                cerr << "cannot add sample" << endl;
                                return 1;
                            }
                        }
                        else
                        {
                            delete sample; sample = 0;
                        }
                    }
                }
            }

        if(tree.NumLevels() >= num_cpu_levels)
        {
            break;
        }

        // debugging info
        if(0)
        {
            vector<const Sample *> samples;

            tree.GetSamples(samples);

            cerr << samples.size() << " samples at level " << current_level << endl;
        }
    }
    while(tree.Subdivide());

    if(0) // debug
    {
        vector<const Sample *> samples;

        tree.GetSamples(samples);

        for(unsigned int i = 0; i < samples.size(); i++)
        {
            cout << "CPU sample";

            for(int j = 0; j < samples[i]->coordinate.Dimension(); j++)
            {
                cout << " " << samples[i]->coordinate[j];
            }
            cout << endl;
        }
    }
    
    // GPU stuff
    // texture_size
    vector<int> tree_texture_size;
    if(! tree.GetNumCellsPerDimension(tree.NumLevels()-1, tree_texture_size))
    {
        cerr << "cannot get the tree texture size" << endl;
        return 1;
    }

    CgBlue::TextureSize texture_size;

    if(! CgBlue::ConvertTextureSize(tree_texture_size, texture_size))
    {
        cerr << "cannot convert from tree texture size to GPU texture size" << endl;
        return 1;
    }
    
    const int height = texture_size.TotalHeight()*static_cast<int>(pow(2.0, texture_size.height.size()*1.0));
    const int width = texture_size.TotalWidth()*static_cast<int>(pow(2.0, texture_size.width.size()*1.0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(Keyboard);
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutIdleFunc(Idle);
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
      
    // Initialize the "OpenGL Extension Wrangler" library
    glewInit();

    string result;

    if(!glewIsSupported( "GL_VERSION_2_0 GL_ARB_fragment_program GL_EXT_framebuffer_object GL_NV_float_buffer" )) 
    {
        ostringstream error_message;

        error_message << "Error: failed to get minimal extensions for demo\n";
        error_message << "This sample requires:\n";
        error_message << "  OpenGL version 2.0\n";
        error_message << "  GL_ARB_fragment_program\n";
        error_message << "  GL_EXT_framebuffer_object" << endl;

        result = error_message.str();

        if(result != "") throw Exception(result);
    }

    {
        vector<const Sample *> samples;
        tree.GetSamples(samples);
        
        const CgBlue::TextureSize init_texture_size(texture_size);

        for(unsigned int i = 0; i < texture_size.height.size(); i++)
        {
            texture_size.height[i] *= 2;
        }
        for(unsigned int i = 0; i < texture_size.width.size(); i++)
        {
            texture_size.width[i] *= 2;
        }

        // cell_size
        const float cell_size = tree.CellSize(tree.NumLevels()-1)/2.0;

        // clique_size
        
        int clique_size = input_clique_size > 0 ? input_clique_size : static_cast<int>(ceil(sqrt(tree.ConflictRadiusSquare())));

        for(unsigned int i = 0; i < init_texture_size.height.size(); i++)
        {
            if(clique_size > init_texture_size.height[i]) clique_size = init_texture_size.height[i];
        }for(unsigned int i = 0; i < init_texture_size.width.size(); i++)
        {
            if(clique_size > init_texture_size.width[i]) clique_size = init_texture_size.width[i];
        }
        
        while(texture_size.TotalHeight()%clique_size)
        {
            clique_size++;
        }

        for(unsigned int i = 0; i < init_texture_size.height.size(); i++)
        {
            if(init_texture_size.height[i]%clique_size)
            {
                cerr << "texture size not an integer multiple of clique_size: init_texture_size.height[" << i << "] " << init_texture_size.height[i] << ", clique_size " << clique_size << endl;
                return 1;
            }
        }
        for(unsigned int i = 0; i < init_texture_size.width.size(); i++)
        {
            if(init_texture_size.width[i]%clique_size)
            {
                cerr << "texture size not an integer multiple of clique_size: init_texture_size.width[" << i << "] " << init_texture_size.width[i] << ", clique_size " << clique_size << endl;
                return 1;
            }
        }
        for(unsigned int i = 0; i < texture_size.height.size(); i++)
        {
            if(texture_size.height[i]%clique_size)
            {
                cerr << "texture size not an integer multiple of clique_size: texture_size.height[" << i << "] " << texture_size.height[i] << ", clique_size " << clique_size << endl;
                return 1;
            }
        }
        for(unsigned int i = 0; i < texture_size.width.size(); i++)
        {
            if(texture_size.width[i]%clique_size)
            {
                cerr << "texture size not an integer multiple of clique_size: texture_size.width[" << i << "] " << texture_size.width[i] << ", clique_size " << clique_size << endl;
                return 1;
            }
        }

        cerr << "clique_size: " << clique_size << endl;
        
        house_keeper.init_texture = vector<GLuint>(CgBlue::RequiredDrawBuffers(dimension));
        for(unsigned int i = 0; i < house_keeper.init_texture.size(); i++)
        {
            house_keeper.init_texture[i] = 0;
        }
        
        cerr << "init texture size: (" << init_texture_size.TotalWidth() << ", " << init_texture_size.TotalHeight() << ")" << endl;

        house_keeper.init_texture = CgBlue::GenerateInitTexture(init_texture_size, tree.CellSize(tree.NumLevels()-1), clique_size, samples, house_keeper.init_texture);

        // neighbor_offsets 
        int max_program_exe_instructions;
        glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_EXEC_INSTRUCTIONS_NV, &max_program_exe_instructions);
        const bool nv40_or_lower = (max_program_exe_instructions <= 65536);

        int conflict_square_radius = tree.ConflictRadiusSquare();
        if(nv40_or_lower && (tree.Dimension() > 4))
        {
            switch(tree.Dimension())
            {
            case 5:
                conflict_square_radius = 16; // magic number obtained by trial/error on 6800
                break;

            case 6:
                conflict_square_radius = 10; // magic number obtained by trial/error on 6800
                break;

            default:
                throw Exception("unhandled dimensionality");
                break;
            }
        }
        
        cerr << "conflict radius square: " << conflict_square_radius << endl;
        OnionLayerCounter counter(tree.Dimension(), conflict_square_radius);
        // OnionLayerCounter counter(tree.Dimension(), tree.ConflictRadiusSquare());
        // OnionLayerCounter counter(tree.Dimension(), min(16, tree.ConflictRadiusSquare())); // debug for 5D

        // OnionLayerCounter counter(tree.Dimension(), min(10, tree.ConflictRadiusSquare())); // debug for 6D
   
        vector<int> value(tree.Dimension());
        counter.Reset();
        do
        {
            counter.Get(value);
            house_keeper.neighbor_offsets.push_back(value);
        }       
        while(counter.Next());
        
        house_keeper.p_phase_counter = new RandomCounter(tree.Dimension(), 0, clique_size-1);

        house_keeper.dimension = dimension;
        house_keeper.min_distance = min_distance;
        house_keeper.k_number = k_number;
        house_keeper.root_cell_size = root_cell_size;
        house_keeper.clique_size = clique_size;
        house_keeper.min_cell_size = house_keeper.min_distance/sqrt(house_keeper.dimension*1.0);
        house_keeper.current_k_number = -1;
        house_keeper.cell_size = cell_size;
        house_keeper.all_phase_complete = 1;
        house_keeper.texture_size = texture_size;
        house_keeper.boundary_condition = boundary_condition;
        house_keeper.neighbor_offset_storage = neighbor_offset_storage;

        if((noise_option == CgBlue::MD5_NOISE) && glewIsSupported("GL_EXT_gpu_shader4"))
        {
            house_keeper.initial_noise_keys = vector<int>(CgBlue::RequiredDrawBuffers(dimension));

            if(house_keeper.initial_noise_keys.size() > 0)
            {
                house_keeper.initial_noise_keys[0] = time(0);
            }
            
            for(unsigned int i = 1; i <  house_keeper.initial_noise_keys.size(); i++)
            {
                house_keeper.initial_noise_keys[i] = CgBlue::Hash(house_keeper.initial_noise_keys[i-1]);
            }
        }
        else
        {
            house_keeper.initial_noise_keys.clear();
        }
        
        vector<int> zero(1); zero[0] = 0;
        if(importance_field_name == "")
        {
            house_keeper.r_value_texture = 0;
        
            house_keeper.r_value_texture_size = CgBlue::TextureSize(zero, zero);
        }
        else
        {
            Array<float> importance_field_data;
            const int is_uniform = !GetDistanceField(dimension, min_distance, importance_field_name, range_min, range_max, importance_field_data, texture_size, cell_size, house_keeper.min_cell_size);
            vector<int> importance_field_size;
            importance_field_data.Size(importance_field_size);
            CgBlue::TextureSize importance_field_texture_size;
            if(!CgBlue::ConvertTextureSize(importance_field_size, importance_field_texture_size))
            {
                throw Exception("CgBlue::ConvertTextureSize(importance_field_size, importance_field_texture_size)");
            }
            const CgBlue::TextureSize r_value_texture_size = is_uniform ? CgBlue::TextureSize(zero, zero) : importance_field_texture_size;

            house_keeper.r_value_texture = CgBlue::GenerateRValueTexture(importance_field_texture_size, importance_field_data, house_keeper.r_value_texture);
            house_keeper.r_value_texture_size = r_value_texture_size;
        }
        
        house_keeper.show_off = show_off;
    }

#ifdef _PRECOMPUTE_WHITE_NOISE
    if(house_keeper.initial_noise_keys.size() <= 0)
    {
        // pre-compute white noise textures

        // largest possible texture size
#if 1
        const CgBlue::TextureSize largest_texture_size = ComputeLargestTextureSize(house_keeper.texture_size, house_keeper.cell_size, house_keeper.min_cell_size);
#else   
        CgBlue::TextureSize largest_texture_size = house_keeper.texture_size;
        float current_cell_size = house_keeper.cell_size;

        while(current_cell_size > house_keeper.min_cell_size)
        {
            current_cell_size /= 2;
            for(unsigned int i = 0; i < largest_texture_size.height.size(); i++)
            {
                largest_texture_size.height[i] *= 2;
            }
            for(unsigned int i = 0; i < largest_texture_size.width.size(); i++)
            {
                largest_texture_size.width[i] *= 2;
            }
        }
#endif
        house_keeper.largest_texture_size = largest_texture_size;
        cerr << "largest texture size: " << largest_texture_size.TotalHeight() << ", " << largest_texture_size.TotalWidth() << endl;
        
        for(int i = 0; i < k_number; i++)
        {
            vector<GLuint> texture_ids;

            for(int j = 0; j < CgBlue::RequiredDrawBuffers(dimension); j++)
            {
                GLuint texture_id = -1;
                texture_id = house_keeper.p_poisson->GenerateWhiteNoise(largest_texture_size, texture_id);
                texture_ids.push_back(texture_id);
            }
            house_keeper.white_noise_textures.push_back(texture_ids);
        }
    }
#else
    if(house_keeper.initial_noise_keys.size() <= 0)
    {
        house_keeper.white_noise_texture = vector<GLuint>(CgBlue::RequiredDrawBuffers(dimension));
        for(unsigned int i = 0; i < house_keeper.white_noise_texture.size(); i++)
        {
            house_keeper.white_noise_texture[i] = 0;
        }
    }
#endif
    
    house_keeper.total_watch_time = 0;
    house_keeper.timer.Start();

    glutMainLoop();

    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return Main(argc, argv);
    }
    catch(ExitException e)
    {
        house_keeper.timer.Stop();

        cerr << "total time " << house_keeper.timer.ElapsedTime() << endl;
        cerr << "total watch time " << house_keeper.total_watch_time << endl;
        
        // dump out results here
        vector<const Sample *> samples;
        house_keeper.p_poisson->GetSamples(samples);

        cerr << "total # of samples " << samples.size() << endl;
                
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            if(samples[i])
            {
                for(unsigned int j = 0; j < samples[i]->coordinate.Dimension(); j++)
                {
                    cout << samples[i]->coordinate[j] << " ";
                }
                cout << endl;
            }
        }

#ifdef _PRECOMPUTE_WHITE_NOISE
        // temporary hack to avoid the annoying popup window after things are done
        throw ExitException("done");
#endif
        
        return 0;
    }
    catch(Exception e)
    {
        cerr << "Error : "<< e.Message() << endl;
        return 1;
    }
}
