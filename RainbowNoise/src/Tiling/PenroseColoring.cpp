/*
  PenroseColoring.cpp

  generate 3-coloring via hacking the code [Ostromoukhov et al. SIGGRAPH 2004]

  Li-Yi Wei
  10/05/2008

*/


#include <iostream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <time.h>

#include "quasisampler_prototype.h"
#include "Exception.hpp"
#include "Random.hpp"
#include "FrameBuffer.hpp"

class PenroseSampler : public Quasisampler
{
public:
    PenroseSampler(const double domain_width, const double domain_height, const double correction_amount, const unsigned uniform_field_value) : Quasisampler(domain_width, domain_height, correction_amount), _uniform_field_value(uniform_field_value) {}
     
    unsigned getImportanceAt(Point2D point) {return _uniform_field_value;}

protected:
    const unsigned _uniform_field_value;
};

double Det(const Point2D & p1, const Point2D & p2, const Point2D & p3)
{
    return (p1.x*p2.y - p2.x*p1.y + p2.x*p3.y - p3.x*p2.y + p3.x*p1.y - p1.x*p3.y);
}

int InsideRhomb(const Point2D & query, const Quasisampler::TileNode & tile)
{
    vector<Point2D> points;
    points.push_back(tile.getP1());
    points.push_back(tile.getP4());
    points.push_back(tile.getP2());
    points.push_back(tile.getP3());

    int inside = 1;

    for(unsigned int i = 0; i < points.size(); i++)
    {
        if(Det(query, points[i], points[(i+1)%points.size()]) <= 0)
        {
            inside = 0; break;
        }
    }

    return inside;
}

int Main(int argc, char **argv)
{
    int min_argc = 5;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " domain_width domain_height uniform_field_value num_iterations (for colorization) [optional: tile_fig_file]" << endl;
        return 1;
    }

    int argCtr = 0;
    const double domain_width = atof(argv[++argCtr]);
    const double domain_height = atof(argv[++argCtr]);
    const double correction_amount = 0.0; // atof(argv[++argCtr]);
    const unsigned uniform_field_value = atoi(argv[++argCtr]);
    const double delta = 0.01;
    const int num_iterations = atoi(argv[++argCtr]);
    const int fill_corner = 0; // not useful, as the corner tiles are too big
    const int fill_by_flip = 1;
    const char * tile_fig_file_name = (argCtr+1 < argc ? argv[++argCtr] : 0);

    PenroseSampler sampler(domain_width, domain_height, correction_amount, uniform_field_value);

    sampler.buildAdaptiveSubdivision();

    cerr << "build neighbor graph" << endl;
    sampler.buildNeighborGraph(delta);

    cerr << "colorize" << endl;
    Random::InitRandomNumberGenerator(time(0));

    const unsigned num_colors = 3;
    const double flip_probability = 0.9;
    const int total_iterations = sampler.colorize(num_colors, flip_probability, num_iterations);

    // output results
    const vector<Point2D> rhombs = sampler.getRhombPoints();

    for(unsigned int i = 0; i < rhombs.size(); i++)
    {
        cout << rhombs[i].color << " " << rhombs[i].x << " " << rhombs[i].y << endl;
    }

    cerr << "total_iterations: " << total_iterations << endl;

#if 0
    const vector<Point2D> samples = sampler.getSamplingPoints(1);

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        cout << num_colors << " " << samples[i].x << " " << samples[i].y << endl;
    }
#endif

    // fill corner by flip in the y direction
    if(fill_by_flip)
    {
        cerr << "fill_by_flip" << endl;

        const vector<Quasisampler::TileNode *> rhombs = sampler.getLeafNodes();
        vector<Quasisampler::TileNode *> smallest_rhombs;

        double min_scale = 2.0*max(domain_width, domain_height);
        for(unsigned int i = 0; i < rhombs.size(); i++)
        {
            min_scale = min(min_scale, rhombs[i]->getScale());
        }

        for(unsigned int i = 0; i < rhombs.size(); i++)
        {
            if(rhombs[i]->getScale() <= min_scale)
            {
                smallest_rhombs.push_back(rhombs[i]);
            }
        }
        
        vector<Point2D> points = sampler.getRhombPoints();

        for(unsigned int i = 0; i < points.size(); i++)
        {
            int inside_any_rhomb = 0;

            points[i].y = domain_height - points[i].y;

            for(unsigned int j = 0; j < smallest_rhombs.size(); j++)
            {
                if(InsideRhomb(points[i], *smallest_rhombs[j]))
                {
                    inside_any_rhomb = 1; break;
                }
            }

            if(!inside_any_rhomb)
            {
                cout << points[i].color << " " << points[i].x << " " << points[i].y << endl;
            }
        }
    }

    // fill corner
    if(fill_corner)
    {
        const vector<Quasisampler::TileNode *> rhombs = sampler.getLeafNodes();

        double min_scale = 2.0*max(domain_width, domain_height);
        for(unsigned int i = 0; i < rhombs.size(); i++)
        {
            min_scale = min(min_scale, rhombs[i]->getScale());
        }

        for(unsigned int i = 0; i < rhombs.size(); i++)
        {
            if(rhombs[i]->getScale() > min_scale)
            {
                // random coloring
                Point2D fake = rhombs[i]->getPRhomb();
                // fake.color = static_cast<unsigned>(floor(Uniform::UniformRandom()*num_colors))%num_colors;

                if((fake.x >= 0) && (fake.x < domain_width) && (fake.y >= 0) && (fake.y < domain_height))
                {
                    cout << fake.color << " " << fake.x << " " << fake.y << endl;
                }
            }
        }
    }

    // average area
    double average_rhomb_area = 0;
    unsigned num_rhombs = 0;
    {
        const vector<Quasisampler::TileNode *> rhombs = sampler.getLeafNodes();

        double min_scale = 2.0*max(domain_width, domain_height);
        for(unsigned int i = 0; i < rhombs.size(); i++)
        {
            min_scale = min(min_scale, rhombs[i]->getScale());
        }
       
        for(unsigned int i = 0; i < rhombs.size(); i++)
        {
            if(rhombs[i]->getScale() > min_scale) continue;
            
            average_rhomb_area += rhombs[i]->rhombArea();
            num_rhombs++;
        }
    }

    cerr << "average rhomb area: " << average_rhomb_area/num_rhombs << endl;

    if(tile_fig_file_name)
    {
        const vector<Quasisampler::TileNode *> rhombs = sampler.getLeafNodes();

        double min_scale = 2.0*max(domain_width, domain_height);
        for(unsigned int i = 0; i < rhombs.size(); i++)
        {
            min_scale = min(min_scale, rhombs[i]->getScale());
        }

        vector<FrameBuffer::Tile> tiles;
        for(unsigned int i = 0; i < rhombs.size(); i++)
        {
            if(rhombs[i]->getScale() > min_scale) continue;

            FrameBuffer::Tile tile;

            tile.color_index = rhombs[i]->getColor();

            tile.vertices = vector<FrameBuffer::L2F>(4);

            tile.vertices[0].x = rhombs[i]->getP1().x;
            tile.vertices[0].y = rhombs[i]->getP1().y;
            tile.vertices[1].x = rhombs[i]->getP4().x;
            tile.vertices[1].y = rhombs[i]->getP4().y;
            tile.vertices[2].x = rhombs[i]->getP2().x;
            tile.vertices[2].y = rhombs[i]->getP2().y;
            tile.vertices[3].x = rhombs[i]->getP3().x;
            tile.vertices[3].y = rhombs[i]->getP3().y;

            int inside_count = 0;
            for(unsigned int j = 0; j < tile.vertices.size(); j++)
            {
                if((tile.vertices[j].x >= 0) && (tile.vertices[j].x <= domain_width) && (tile.vertices[j].y >= 0) && (tile.vertices[j].y <= domain_height))
                {
                    inside_count++;
                }
            }

            if(inside_count >= 0) tiles.push_back(tile);
        }

        vector<FrameBuffer::P3> palette;
        const FrameBuffer::P3 red = {255, 0, 0};
        const FrameBuffer::P3 green = {0, 255, 0};
        const FrameBuffer::P3 blue = {0, 0, 255};
        palette.push_back(red);
        palette.push_back(green);
        palette.push_back(blue);
        for(unsigned int i = palette.size(); i < num_colors; i++)
        {
            FrameBuffer::P3 color;
            color.r = static_cast<int>(floor(256*Random::UniformRandom()))%255;
            color.g = static_cast<int>(floor(256*Random::UniformRandom()))%255;
            color.b = static_cast<int>(floor(256*Random::UniformRandom()))%255;
            palette.push_back(color);
        }

        vector<float> region(4);
        region[0] = 0; region[1] = domain_width;
        region[2] = 0; region[3] = domain_height;

        const int tile_boundary = 1;
        const int region_boundary = 1;
        const int color_scheme = 0;

        if(! FrameBuffer::WriteFIG(tiles, palette, region, tile_boundary, region_boundary, color_scheme, tile_fig_file_name))
        {
            cerr << "cannot write " << tile_fig_file_name << endl;
        }
    }

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
