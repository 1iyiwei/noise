/*
  Nearest2D.cpp


  Li-Yi Wei
  2/7/2013

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>

#include "Utility.hpp"
#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "LloydRelaxation.hpp"
#include "DelaunayProximity2D.hpp"
#include "Random.hpp"

int Main(int argc, char **argv)
{
    int min_argc = 8;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name output-file-name dimension domain_size boundary_condition (0 none, 1 toroidal) dot_radius [color_palette_file]" << endl;

        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const string output_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const float domain_size_y = atof(argv[++arg_ctr]);
    const float domain_size_x = atof(argv[++arg_ctr]);
    const DelaunayProximity2D::BoundaryCondition boundary_condition = static_cast<DelaunayProximity2D::BoundaryCondition>(atoi(argv[++arg_ctr]));
    const float radius = atof(argv[++arg_ctr]);

    const double very_small = 0;
    const int tile_boundary = 1;
    const int region_boundary = 1;
    const int color_scheme = 0;

    const string color_palette_file_name = (arg_ctr+1 < argc ? argv[++arg_ctr] : "");
    
    if(dimension != 2)
    {
        cerr << "dimension != 2" << endl;
        return 1;
    }

    // samples
    vector<Sample> samples;
    if(! Utility::ReadSamples(dimension, input_file_name, samples))
    {
        cerr << "cannot read samples from " << input_file_name << endl;
        return 1;
    }

    // color palette
    int num_colors = 0;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if((samples[i].id+1) > num_colors)
        {
            num_colors = (samples[i].id+1);
        }
    }

    num_colors += 2; // query and answer points

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

    // proximity class
    vector<DelaunayProximity2D::Point2D> points(samples.size());
    for(unsigned int i = 0; i < points.size(); i++)
    {
        points[i].id = samples[i].id;
        points[i].x = samples[i].coordinate[1];
        points[i].y = samples[i].coordinate[0];
    }

    vector<const DelaunayProximity2D::Point2D *> p_points(points.size());
    for(unsigned int i = 0; i < p_points.size(); i++)
    {
        p_points[i] = &points[i];
    }

    const DelaunayProximity2D::Box box(0, domain_size_x, 0, domain_size_y);

    DelaunayProximity2D proximity2D(box, very_small, boundary_condition, p_points);

    // query point
    DelaunayProximity2D::Point2D query, result;

    query.x = Random::UniformRandom()*domain_size_x;
    query.y = Random::UniformRandom()*domain_size_y;

    if(! proximity2D.Nearest(query, result))
    {
        cerr << "cannot query (" << query.x << ", " << query.y << ")" << endl;
        return 1;
    }

    // draw
    {
        vector<FrameBuffer::L2F> locations(samples.size());
        
        for(unsigned int i = 0; i < locations.size(); i++)
        {
            locations[i].color_index = samples[i].id;
            locations[i].x = samples[i].coordinate[1];
            locations[i].y = samples[i].coordinate[0];

            if((locations[i].x == result.x) && (locations[i].y == result.y))
            {
                locations[i].color_index = num_colors - 1;
            }
        }

        FrameBuffer::L2F query_f, result_f;

        query_f.color_index = num_colors - 2;
        query_f.x = query.x;
        query_f.y = query.y;

        result_f.color_index = num_colors - 1;
        result_f.x = result.x;
        result_f.y = result.y;

        locations.push_back(query_f);
        locations.push_back(result_f);

        // generate Voronoi

        vector<LloydRelaxation::VoronoiRegion> regions;

        const string message = LloydRelaxation::Voronoi(box.x_min, box.x_max, box.y_min, box.y_max, boundary_condition, p_points, regions);

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

        vector<float> region(4);
        region[2] = 0;
        region[3] = fabs(domain_size_y);
        region[0] = 0;
        region[1] = fabs(domain_size_x);

        vector<FrameBuffer::Circle> circles;

        // draw Voronoi
        if(!FrameBuffer::WriteFIG(locations, tiles, circles, palette, region, radius, tile_boundary, region_boundary, color_scheme, output_file_name))
        {
            cerr << "error in writing " << output_file_name << endl;
            return 1;
        }
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
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
