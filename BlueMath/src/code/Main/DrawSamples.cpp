/*
  DrawSamples.cpp

  Li-Yi Wei
  6/18/2007

  11/02/2010
  input mode

  4/01/2014
  add optional gradient-file-name and gradient_scale

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cstring>

#include "Utility.hpp"
#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "Random.hpp"

#include "LloydRelaxation.hpp" // for 2D only

int Main(int argc, char **argv)
{
    const int x_idx = 1;
    const int y_idx = 1 - x_idx;

    int min_argc = 8;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name[,gradient-file-name] output-file-name dimension radius-for-fig/image-size-for-ppm (voronoi if radius <= 0 for fig, and even/odd of the last digit indicates the absence/presence of tile boundary) border-for-fig/color-scheme-for-ppm (fig: 0 for no boundary, 1 for boundary, -2 for inverse color and no boundary, -1 for inverse color and boundary, ppm: 0 for white-dot-over-black-background, 1 vice versa) domain_size (dimension numbers, positive/negative for toroidal/none boundary condition) [gradient_scale] optional: color_palette_file" << endl;
        return 1;
    }

    int arg_ctr = 0;
    string input_file_name = argv[++arg_ctr];
    string input_gradient_file_name("");
    if (input_file_name.find(',') != string::npos)
    {
        size_t pos = input_file_name.find(',');
        input_gradient_file_name = input_file_name.substr(pos+1, input_file_name.length()-pos);
        input_file_name.resize(pos);
    }

    const string output_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const string radius_string = argv[++arg_ctr];
    const float input_radius = atof(radius_string.c_str());
    const int image_size = static_cast<int>(input_radius);
    const int color_scheme = atoi(argv[++arg_ctr]);
    const float domain_size_y = atof(argv[++arg_ctr]);
    const float domain_size_x = atof(argv[++arg_ctr]);

    vector<float> region(4);
    region[2] = 0;
    region[3] = fabs(domain_size_y);
    region[0] = 0;
    region[1] = fabs(domain_size_x);

    if(dimension != 2)
    {
        cerr << "currently only works for 2D" << endl;
        return 1;
    }

    cout << "read in samples " << endl;

    vector<Sample> samples;
    if(! Utility::ReadSamples(dimension, input_file_name, samples))
    {
        cerr << "cannot read samples from " << input_file_name << endl;
        return 1;
    }

    cout << "total " << samples.size() << " samples read" << endl;

    vector<Sample> gradients;
    float gradient_scale = 0.f;
    if (!input_gradient_file_name.empty())
    {
        if (arg_ctr+1 > argc)
        {
            cerr << "The scale of the gradient vector is not set" << endl;
            return 1;
        }
        gradient_scale = atof(argv[++arg_ctr]);
        cout << "read in gradients " << endl;

        if(! Utility::ReadSamples(dimension, input_gradient_file_name, gradients))
        {
            cerr << "cannot read gradients from " << input_gradient_file_name << endl;
            return 1;
        }
        cout << "total " << gradients.size() << " gradients read" << endl;

        if (gradients.size() != samples.size())
        {
            cerr << "Mismatch in the number of gradients and samples" << endl;
            return 1;
        }

        // convert gradient into offseted samples
        for (size_t i = 0; i < gradients.size(); ++i)
        {
            gradients[i].coordinate[0] = samples[i].coordinate[0] + gradient_scale*gradients[i].coordinate[0];
            gradients[i].coordinate[1] = samples[i].coordinate[1] + gradient_scale*gradients[i].coordinate[1];
        }
    }

    // color palette
    const string color_palette_file_name = (arg_ctr+1 < argc ? argv[++arg_ctr] : "");
    int num_colors = 0;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if((samples[i].id+1) > num_colors)
        {
            num_colors = (samples[i].id+1);
        }
    }

    if (!gradients.empty())
    {
        num_colors *= 2;
    }

    Random::InitRandomNumberGenerator();

    vector<FrameBuffer::P3> palette;
    {
        string message = Utility::BuildPalette(num_colors, color_palette_file_name, palette);

        if(message != "")
        {
            cerr << message << endl;
            return 1;
        }
    }

    // draw samples

    const float target_radius = 0.1;
    const float radius = (fabs(input_radius) <= 0 ? target_radius : fabs(input_radius));

    if((output_file_name.find(".fig") != string::npos) || (output_file_name.find(".svg") != string::npos))
    {
        vector<FrameBuffer::L2F> locations(samples.size());
        // convert sample to locations
        for(unsigned int i = 0; i < locations.size(); i++)
        {
            locations[i].color_index = samples[i].id;
            locations[i].x = samples[i].coordinate[x_idx]*target_radius/radius;
            locations[i].y = samples[i].coordinate[y_idx]*target_radius/radius;
        }
        //vector<FrameBuffer::L2F> offseted_locations;
        if (!gradients.empty())
        {
            //offseted_locations.resize(gradients.size());
            for (size_t i = 0; i < gradients.size(); ++i)
            {
                FrameBuffer::L2F offseted_location;
                offseted_location.color_index = samples[i].id + num_colors/2;
                offseted_location.x = gradients[i].coordinate[x_idx]*target_radius/radius;
                offseted_location.y = gradients[i].coordinate[y_idx]*target_radius/radius;
                locations.push_back(offseted_location);
            }
        }

        // ensure consistent scaling
        for(unsigned int i = 0; i < region.size(); i++)
        {
            region[i] *= target_radius/radius;
        }

        if(input_radius > 0)
        {
            // draw circles
            if(output_file_name.find(".fig") != string::npos)
            {
                if(! FrameBuffer::WriteFIG(locations, palette, region, target_radius, (color_scheme%2 != 0), (color_scheme < 0), output_file_name))
                {
                    cerr << "error in writing samples for " + output_file_name << endl;
                    return 1;
                }
            }
            else
            {
                if(! FrameBuffer::WriteSVG(locations, palette, region, target_radius, (color_scheme%2 != 0), (color_scheme < 0), output_file_name))
                {
                    cerr << "error in writing samples for " + output_file_name << endl;
                    return 1;
                }
            }
        }
        else
        {
            // generate Voronoi
            vector<Delaunay::Point2D> points(samples.size());

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

            const LloydRelaxation::BoundaryCondition voronoi_boundary_condition = (domain_size_y > 0 ? DelaunayMesh::TOROIDAL : DelaunayMesh::NONE);

            const string message = LloydRelaxation::Voronoi(region[0], region[1], region[2], region[3], voronoi_boundary_condition, p_points, regions);

            if(message != "")
            {
                cerr << "error in generating Voronoi " << message << endl;
                return 1;
            }

            //// Vector of circles to be drawn, only include the largest empty circle for now
            vector<FrameBuffer::Circle> circles;
            //{
            //    double x, y, r;
            //    LloydRelaxation::LargestEmptyCircle(region[0], region[1], region[2], region[3], regions, x, y, r);
            //    FrameBuffer::Circle lec;
            //    lec.x = x;
            //    lec.y = y;
            //    lec.radius = r;
            //    lec.color_index = 0;
            //    circles.push_back(lec);
            //}

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

            if(!FrameBuffer::WriteFIG(locations, tiles, circles, palette, region, target_radius, tile_boundary, (color_scheme%2 != 0), (color_scheme < 0), output_file_name))
            {
                cerr << "error in writing tiles " << endl;
                return 1;
            }
        }

        return 0;
    }
    else if(output_file_name.find(".ppm") != string::npos)
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
        cerr << "Error : " << e.Message() <<endl;
        return 1;
    }
}
