/*
  DrawSamples.cpp

  Li-Yi Wei
  6/18/2007

*/


#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "Sample.hpp"
#include "LloydRelaxation.hpp" // for 2D only
#include "FrameBuffer.hpp"
#include "Random.hpp"

vector<FrameBuffer::P3> BuildPalette(const int num_colors, const string color_palette_file_name)
{
    vector<FrameBuffer::P3> palette(num_colors);

    const FrameBuffer::P3 fixed_palette[] = 
        {
            {255, 0, 0}, {0, 153, 0}, {0, 0, 255},
            {0, 255, 255}, {255, 0, 255}, {255, 255, 0}
        };

    vector<FrameBuffer::P3> given_palette;

    if(color_palette_file_name == "")
    {
        for(int i = 0; i < sizeof(fixed_palette)/sizeof(FrameBuffer::P3); i++)
        {
            given_palette.push_back(fixed_palette[i]);
        }
    }
    else
    {
        ifstream input(color_palette_file_name.c_str());

        if(! input)
        {
            cerr << "cannot read from " << color_palette_file_name << endl;
        }
        else
        {
            FrameBuffer::P3 color;
            while(input.good())
            {
                input >> color.r;
                input >> color.g;
                input >> color.b;

                if(input.good()) given_palette.push_back(color);
            }
        }
    }

    // cerr << "given color: " << given_palette.size() << ", total color: " << num_colors << endl;

    for(unsigned int i = 0; i < palette.size(); i++)
    {
        if(i < given_palette.size())
        {
            palette[i] = given_palette[i];
        }
        else
        {
            palette[i].r = static_cast<int>(floor(256*Random::UniformRandom()));
            palette[i].g = static_cast<int>(floor(256*Random::UniformRandom()));
            palette[i].b = static_cast<int>(floor(256*Random::UniformRandom()));
        }
    }

    return palette;
}

int Main(int argc, char **argv)
{
    int min_argc = 6;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name output-file-name dimension radius-for-fig/image-size-for-ppm (voronoi if radius <= 0 for fig, and even/odd of the last digit indicates the absence/presence of tile boundary) border-for-fig/color-scheme-for-ppm (fig: 0 for no boundary, 1 for boundary, -2 for inverse color and no boundary, -1 for inverse color and boundary, ppm: 0 for white-dot-over-black-background, 1 vice versa) optional: domain_size (dimension numbers) color_palette_file" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const string radius_string = argv[++argCtr];
    const float input_radius = atof(radius_string.c_str());
    const int image_size = static_cast<int>(input_radius);
    const int color_scheme = atoi(argv[++argCtr]);
    vector<float> region(4);
    region[2] = 0;
    region[3] = argCtr+1 < argc ? atof(argv[++argCtr]) : -1;
    region[0] = 0;
    region[1] = argCtr+1 < argc ? atof(argv[++argCtr]) : -1;

    const string color_palette_file_name = (argCtr+1 < argc ? argv[++argCtr] : "");

    if(dimension != 2)
    {
        cerr << "currently only works for 2D" << endl;
        return 1;
    }
    
    ifstream input(input_file_name); // read in samples
    cout << "read in samples " << endl;
    
    vector<Sample> samples;
    
    if(!input)
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
            input >> sample.id;

            for(int i = 0; i < dimension; i++)
            {
                input >> sample.coordinate[i];
            }
            
            if(input.good() && (sample.id >= 0))
            {
                samples.push_back(sample);
            }
        }
    }

    cout << "total " << samples.size() << " samples read" << endl;
    
    // color palette
    int num_colors = 0;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if((samples[i].id+1) > num_colors)
        {
            num_colors = (samples[i].id+1);
        }
    }

    Random::InitRandomNumberGenerator();

    const vector<FrameBuffer::P3> palette = BuildPalette(num_colors, color_palette_file_name);

    // draw samples
    if(strstr(output_file_name, ".fig"))
    {
        vector<FrameBuffer::L2F> locations(samples.size());

        const float target_radius = 0.1;
        const float radius = (fabs(input_radius) <= 0 ? target_radius : fabs(input_radius));
        
        for(unsigned int i = 0; i < locations.size(); i++)
        {
            locations[i].color_index = samples[i].id;
            locations[i].x = samples[i].coordinate[1]*target_radius/radius;
            locations[i].y = samples[i].coordinate[0]*target_radius/radius;
        }

        // ensure consistent scaling
        for(unsigned int i = 0; i < region.size(); i++)
        {
            region[i] *= target_radius/radius;
        }
        
        if(input_radius > 0)
        {
            // draw circles
            if(! FrameBuffer::WriteFIG(locations, palette, region, target_radius, (color_scheme%2 != 0), (color_scheme < 0), output_file_name))
            {
                cerr << "error in writing samples " << endl;
                return 1;
            }
        }
        else
        {
            // generate Voronoi
            vector<Delaunay::Point2D> points(locations.size());

            if(points.size() != samples.size())
            {
                throw Exception("points.size() != samples.size()");
            }

            for(unsigned int i = 0; i < points.size(); i++)
            {
                points[i].id = samples[i].id;
                points[i].x = locations[i].x;
                points[i].y = locations[i].y;
            }
            
            vector<const Delaunay::Point2D *> p_points(points.size());
            for(unsigned int i = 0; i < p_points.size(); i++)
            {
                p_points[i] = &points[i];
            }

            vector<LloydRelaxation::VoronoiRegion> regions;

            const string message = LloydRelaxation::Voronoi(region[0], region[1], region[2], region[3], DelaunayMesh::TOROIDAL, p_points, regions);

            if(message != "")
            {
                cerr << "error in generating Voronoi " << message << endl;
                return 1;
            }

            vector<FrameBuffer::Tile> tiles(regions.size());

            if(tiles.size() != regions.size())
            {
                throw Exception("weird size mismatch");
            }

            for(unsigned int i = 0; i < tiles.size(); i++)
            {
                tiles[i].color_index = regions[i].center.id;
                tiles[i].vertices = vector<FrameBuffer::L2F>(regions[i].ring.size());
                for(unsigned int j = 0; j < tiles[i].vertices.size(); j++)
                {
                    tiles[i].vertices[j].x = regions[i].ring[j].x;
                    tiles[i].vertices[j].y = regions[i].ring[j].y;
                }
            }

            // draw Voronoi
            const char last_digit = radius_string[radius_string.size()-1];
            const int tile_boundary = atoi(&last_digit);

            if(!FrameBuffer::WriteFIG(tiles, palette, region, tile_boundary, (color_scheme%2 != 0), (color_scheme < 0), output_file_name))
            {
                cerr << "error in writing tiles " << endl;
                return 1;
            }
        }

        return 0;
    }
    else if(strstr(output_file_name, ".ppm"))
    {
        if(image_size <= 0)
        {
            cerr << "wrong image size" << endl;
            return 1;
        }
        
        cout << "snap point samples to output image" << endl;

        vector<int> output_size(2, image_size);
        Array<FrameBuffer::P3> output_image(output_size);

        FrameBuffer::P3 background_color;
        background_color.r = background_color.g = background_color.b = 255*(color_scheme > 0);
    
        {
            vector<int> output_index(2, 0);

            for(unsigned int i = 0; i < samples.size(); i++)
            {
                int row = static_cast<int>(floor(samples[i].coordinate[0] * image_size));
                if(row >= image_size) row = image_size-1;
                int col = static_cast<int>(floor(samples[i].coordinate[1] * image_size));
                if(col >= image_size) col = image_size-1;

                output_index[0] = row; output_index[1] = col;
                // output_image[row][col].r = output_image[row][col].g = output_image[row][col].b = 255*(color_scheme <= 0);
                output_image.Put(output_index, palette[samples[i].id]);
            }
        
            if(! FrameBuffer::WritePPM(output_image, 255, output_file_name))
            {
                throw Exception("error in writing file!");
            }
        }
    }
    else
    {
        cerr << "unknown output format" << endl;
        return 1;
    }
    
    // done
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return Main(argc, argv);
    }
    catch(Exception e)
    {
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
