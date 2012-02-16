/*
 * FrameBuffer.cpp
 *
 * Li-Yi Wei
 *
 */

#pragma warning (disable:4786)
#include <algorithm>
using namespace std;

#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "Random.hpp"

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

int FrameBuffer::ReadPGM(const char * file_name,
                         Array<P2> & pixels,
                         int & maximum_value)
{
    ifstream input(file_name);

    if(!input)
    {
        return 0;
    }
    else
    {
        // check magic number
        string magic;
        
        input >> magic;
        if(magic != "P2") return 0;
        
        // height, width, maximum
        int height, width;
        input >> width >> height >> maximum_value;
        if(input.bad()) return 0;

        // pixels
        vector<int> size_spec(2);
        size_spec[0] = height; size_spec[1] = width;
        pixels = Array<P2>(size_spec);

        vector<int> coord(2);
        P2 pixel;
        //for(int row = height-1; row >= 0; row--)
        for(int row = 0; row < height; row++)
        {
            for(int col = 0; col < width; col++)
            {
                coord[0] = row; coord[1] = col;
                input >> pixel;
          
                pixels.Put(coord, pixel);
                if(input.bad()) return 0;
            }
        }
    }

    return 1;
}

int FrameBuffer::WritePGM(const Array<P2> & pixels,
                          const int maximum_value,
                          const char * file_name)
{
    if(pixels.Dimension() != 2)
    {
        return 0;
    }
    
    ofstream output(file_name);

    if(!output)
    {
        return 0;
    }
    else
    {
        // magic number
        output << "P2" << endl;

        const int height = pixels.Size(0);
        if(height <= 0)
        {
            return 0;
        }

        const int width = pixels.Size(1);
        
        // header
        output << width << " " << height << " " << maximum_value << endl;

        // content
        vector<int> coord(2);
        P2 pixel;
        //for(int i = height-1; i >= 0; i--)
        for(int i = 0; i < height; i++)
            for(int j = 0; j < width; j++)
            {
                coord[0] = i; coord[1] = j;
                if(! pixels.Get(coord, pixel))
                {
                    return 0;
                }
                
                output << pixel << endl;
            }
        
        return 1;
    }
}

int FrameBuffer::ReadPPM(const char * file_name,
                         Array<P3> & pixels,
                         int & maximum_value)
{
    ifstream input(file_name);

    if(!input)
    {
        return 0;
    }
    else
    {
        // check magic number
        string magic;
        
        input >> magic;
        if(magic != "P3") return 0;
        
        // height, width, maximum
        int height, width;
        input >> width >> height >> maximum_value;
        if(input.bad()) return 0;

        // pixels
        vector<int> size_spec(2);
        size_spec[0] = height; size_spec[1] = width;
        pixels = Array<P3>(size_spec);

        vector<int> coord(2);
        P3 pixel;
        //for(int row = height-1; row >= 0; row--)
        for(int row = 0; row < height; row++)
        {
            for(int col = 0; col < width; col++)
            {
                coord[0] = row; coord[1] = col;
                input >> pixel.r;
                input >> pixel.g;
                input >> pixel.b;

                pixels.Put(coord, pixel);
                if(input.bad()) return 0;
            }
        }
    }

    return 1;
}

int FrameBuffer::WritePPM(const Array<P3> & pixels,
                          const int maximum_value,
                          const char * file_name)
{
    if(pixels.Dimension() != 2)
    {
        return 0;
    }
    
    ofstream output(file_name);

    if(!output)
    {
        return 0;
    }
    else
    {
        // magic number
        output << "P3" << endl;

        const int height = pixels.Size(0);
        if(height <= 0)
        {
            return 0;
        }

        const int width = pixels.Size(1);
        
        // header
        output << width << " " << height << " " << maximum_value << endl;

        // content
        vector<int> coord(2);
        P3 pixel;
        //for(int i = height-1; i >= 0; i--)
        for(int i = 0; i < height; i++)
            for(int j = 0; j < width; j++)
            {
                coord[0] = i; coord[1] = j;
                if(! pixels.Get(coord, pixel))
                {
                    return 0;
                }
                
                output << pixel.r << " " << pixel.g << " " <<  pixel.b << endl;
            }
        
        return 1;
    }
}

int FrameBuffer::ReadPFM(const char * file_name,
                         Array<PF> & pixels)
{
    ifstream input(file_name);

    if(!input)
    {
        return 0;
    }
    else
    {
        // check magic number
        string magic;
        
        input >> magic;
        if(magic != "PF") return 0;
        
        // dimension
        int dimension;
        input >> dimension;
        if(input.bad()) return 0;
        if(dimension <= 0) return 0;
        
        // sizes
        vector<int> sizes(dimension);
        int total_num_pixels = 1;
        {
            for(unsigned int i = 0; i < sizes.size(); i++)
            {
                input >> sizes[i];
                if(input.bad()) return 0;
                if(sizes[i] <= 0) return 0;

                total_num_pixels *= sizes[i];
            }
        }
        
        // pixels
        vector<PF> pixels_read(total_num_pixels);

        for(unsigned int i = 0; i < pixels_read.size(); i++)
        {
                input >> pixels_read[i].r;
                input >> pixels_read[i].g;
                input >> pixels_read[i].b;
                if(input.bad()) return 0;
        }
        
        pixels = Array<PF>(sizes);
        if(! pixels.Put(pixels_read))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

int FrameBuffer::WritePFM(const Array<PF> & pixels,
                          const char * file_name)
{
    ofstream output(file_name);

    if(!output)
    {
        return 0;
    }
    else
    {
        // magic number
        output << "PF" << endl;

        // header
        output << pixels.Dimension() << endl;

        {
            for(int i = 0; i < pixels.Dimension(); i++)
            {
                output << pixels.Size(i) << " ";
            }
            output << endl;
        }
        
        // content
        vector<PF> pixels_write;
        if(! pixels.Get(pixels_write))
        {
            return 0;
        }
        
        for(unsigned int i = 0; i < pixels_write.size(); i++)
        {
            output << pixels_write[i].r << " " << pixels_write[i].g << " " <<  pixels_write[i].b << endl;
        }
        
        return 1;
    }
}

int FrameBuffer::WriteFIG(const vector<L2F> & samples, const vector<P3> & palette, const vector<float> & region, const float radius, const int boundary, const int color_scheme, const char *file_name)
{
    vector<Tile> tiles;

    return WriteFIG(samples, tiles, palette, region, radius, boundary, boundary, color_scheme, file_name);
}

int FrameBuffer::WriteFIG(const vector<Tile> & tiles, const vector<P3> & palette, const vector<float> & region, const int tile_boundary, const int region_boundary, const int color_scheme, const char *file_name)
{
    vector<L2F> samples;

    return WriteFIG(samples, tiles, palette, region, 0, tile_boundary, region_boundary, color_scheme, file_name);
}

int FrameBuffer::WriteFIG(const vector<L2F> & samples, const vector<Tile> & tiles, const vector<P3> & palette, const vector<float> & region, const float radius, const int tile_boundary, const int region_boundary, const int color_scheme, const char *file_name)
{
    ofstream output(file_name);

    if(region.size() != 4)
    {
        return 0;
    }
    
    if((color_scheme < 0) || (color_scheme > 1))
    {
        return 0;
    }
    
    if(!output)
    {
        return 0;
    }
    else
    {
        // from http://www.xfig.org/userman/fig-format.html
        const int units = 1200;
        const int point_unit = 75;
        
        output << "#FIG 3.2" << endl;
        output << "Landscape" << endl;
        output << "Center" << endl;
        output << "Inches" << endl;
        output << "Letter" << endl;
        output << "100.00" << endl;
        output << "Single" << endl;
        output << "-2" << endl;
        output << units << " 2" << endl;

        // custom colors
        for(unsigned int i = 0; i < palette.size(); i++)
        {
            output << 0 << " " << 32+i << " #" << FigColorString(palette[i]) << endl;
        }

        float x_min = 1.0/sin(0.0);
        float x_max = -1.0/sin(0.0);
        float y_min = 1.0/sin(0.0);
        float y_max = -1.0/sin(0.0);
       
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            // ellipse
            output << 1 << " ";

            // circle defined by radius
            output << 3 << " ";
            
            // line type
            output << 0 << " ";
            
            // thickness
            output << 0 << " ";
            
            // pen color black
            output << 7*color_scheme << " ";
            
            // fill color custom
            output << 32+samples[i].color_index << " ";

            // depth
            // output << 50 << " ";
            // output << (static_cast<int>(floor(Random::UniformRandom()*palette.size()))%palette.size()) + 50 - (palette.size()-1)/2<< " ";
            const int depth_mean = 50;
            const int depth_var = min(depth_mean, static_cast<int>(palette.size()));
            output << (static_cast<int>(floor(Random::UniformRandom()*depth_var))%depth_var) + depth_mean - (depth_var-1)/2 << " ";

            // line style default
            output << -1 << " ";

            // area fill full saturation
            output << 20 << " ";

            // style value
            output << 0.0 << " ";

            // direction
            output << 1 << " ";

            // angle of the x-axis
            output << 0.0 << " ";

            // center x, y
            const float center_x = floor(samples[i].x*units+0.5);
            const float center_y = floor(samples[i].y*units+0.5);

            output << center_x << " " << center_y << " ";

            if(center_x < x_min) x_min = center_x;
            if(center_x > x_max) x_max = center_x;
            if(center_y < y_min) y_min = center_y;
            if(center_y > y_max) y_max = center_y;
            
            // radius x, y
            const float radius_value = floor(radius*units+0.5);
            output << radius_value << " " << radius_value << " ";

            // the first point entered
            output << center_x << " " << center_y << " ";
            
            // the last point entered
            output << center_x+point_unit << " " << center_y+point_unit << " ";
            // done
            output << endl;
        }

        for(unsigned int i = 0; i < tiles.size(); i++)
        { 
            if(tiles[i].vertices.size() <= 0) continue;

            // polyline
            output << 2 << " ";

            // polygon
            output << 3 << " ";

            // line style
            output << 0 << " ";

            // thickness
            output << max(1, tile_boundary) << " ";

            const int fill_color_index = 32 + tiles[i].color_index;

            // pen color black
            output << (tile_boundary ? 7*color_scheme : fill_color_index)<< " ";

            // fill color
            output << fill_color_index << " ";

            // depth
            output << _FIG_MAX_DEPTH << " ";

            // pen style
            output << -1 << " ";

            // area fill
            output << 20 << " ";

            // style value
            output << 0.0 << " ";

            // joint style
            output << 1 << " ";

            // cap style
            output << 1 << " ";

            // radiius
            output << -1 << " ";

            // forward arrow
            output << 0 << " ";

            // backward arrow
            output << 0 << " ";

            // npoints
            output << tiles[i].vertices.size()+1 << " " << endl;

            // the corners with the first one repeated
            output << "\t ";
            for(unsigned int j = 0; j <= tiles[i].vertices.size(); j++)
            {
                output << static_cast<int>(floor(tiles[i].vertices[j%tiles[i].vertices.size()].x*units+0.5)) << " " << static_cast<int>(floor(tiles[i].vertices[j%tiles[i].vertices.size()].y*units+0.5)) << " ";
            }

            output << endl;
        }

        if((region[0] < region[1]) && (region[2] < region[3]))
        {
            x_min = region[0]*units;
            x_max = region[1]*units;
            y_min = region[2]*units;
            y_max = region[3]*units;
        }
        
        {
            const float radius_value = floor(radius*units+0.5);
            x_min -= radius_value;
            x_max += radius_value;
            y_min -= radius_value;
            y_max += radius_value;
        }

        if(region_boundary || (!region_boundary && (color_scheme == 1)))
        {
            // polyline
            output << 2 << " ";

            // box
            output << 2 << " ";

            // line style
            output << 0 << " ";

            // thickness
            output << region_boundary << " ";

            // pen color black
            output << 7*color_scheme << " ";

            // fill color white
            output << 7*(1-color_scheme) << " ";

            // depth
            output << (tiles.size() <= 0 ? _FIG_MAX_DEPTH : _FIG_MAX_DEPTH-1)<< " ";
            
            // pen style
            output << -1 << " ";

            // area fill
            output << (tiles.size() <= 0 ? 20 : -1)<< " ";

            // style value
            output << 0.0 << " ";

            // joint style
            output << 0 << " ";

            // cap style
            output << 0 << " ";

            // radiius
            output << -1 << " ";

            // forward arrow
            output << 0 << " ";

            // backward arrow
            output << 0 << " ";

            // npoints
            output << 5 << " " << endl;

            // the 4 corners with the first one repeated
            output << "\t ";
            output << floor(x_min) << " " << floor(y_min) << " ";
            output << ceil(x_max) << " " << floor(y_min) << " ";
            output << ceil(x_max) << " " << ceil(y_max) << " ";
            output << floor(x_min) << " " << ceil(y_max) << " ";
            output << floor(x_min) << " " << floor(y_min) << " ";

            output << endl;
        }

        return 1;
    }
}

int FrameBuffer::WriteFIG(const vector<L2F> & samples, const vector<Picture> & palette, const vector<float> & region, const int boundary, const int color_scheme, const char *file_name)
{
    if(region.size() != 4)
    {
        return 0;
    }
    
    if((color_scheme < 0) || (color_scheme > 1))
    {
        return 0;
    }
    
    ofstream output(file_name);

    if(!output)
    {
        return 0;
    }
    else
    {
        // from http://www.xfig.org/userman/fig-format.html
        const int units = 1200;
        const int point_unit = 75;
        
        output << "#FIG 3.2" << endl;
        output << "Landscape" << endl;
        output << "Center" << endl;
        output << "Inches" << endl;
        output << "Letter" << endl;
        output << "100.00" << endl;
        output << "Single" << endl;
        output << "-2" << endl;
        output << units << " 2" << endl;

        vector<L2F> vertices(4);

        for(unsigned int i = 0; i < samples.size(); i++)
        {
            const Picture & picture = palette[samples[i].color_index%palette.size()];

            for(unsigned int j = 0; j < vertices.size(); j++)
            {
                vertices[j].x = samples[i].x + ((j%3 != 0) - 0.5)*picture.width;
                vertices[j].y = samples[i].y + ((j/2) - 0.5)*picture.height;
            }

            // polyline
            output << 2 << " ";

            // imported picture bounding box
            output << 5 << " ";

            // line style
            output << 0 << " ";

            // thickness
            output << 1 << " ";

            // pen color black
            output << 0 << " ";

            // fill color white
            output << -1 << " ";

            // depth
            // output << (static_cast<int>(floor(Random::UniformRandom()*palette.size()))%palette.size()) + 50 - (palette.size()-1)/2 << " ";
            const int depth_mean = 50;
            const int depth_var = min(depth_mean, static_cast<int>(palette.size()));
            output << (static_cast<int>(floor(Random::UniformRandom()*depth_var))%depth_var) + depth_mean - (depth_var-1)/2 << " ";

            // pen style
            output << -1 << " ";

            // area fill
            output << -1 << " ";

            // style value
            output << 0.0 << " ";

            // joint style
            output << 0 << " ";

            // cap style
            output << 0 << " ";

            // radiius
            output << -1 << " ";

            // forward arrow
            output << 0 << " ";

            // backward arrow
            output << 0 << " ";

            // npoints
            output << (vertices.size() + 1) << " " << endl;

            // flipped
            output << "\t ";
            output << 0 << " ";

            // file name
            output << picture.name << " " << endl;

            // the 4 corners with the first one repeated
            output << "\t ";
            for(unsigned int j = 0; j <= vertices.size(); j++)
            {
                output << floor(vertices[j%vertices.size()].x*units+0.5) << " " << floor(vertices[j%vertices.size()].y*units+0.5) << " ";
            }

            output << endl;
        }
    }

    return 1;
}

string FrameBuffer::FigColorString(const P3 input)
{
    ostringstream output;
    output.width(2); output.fill('0');
    output << hex << (input.r%256);
    output.width(2); output.fill('0');
    output << hex << (input.g%256);
    output.width(2); output.fill('0');
    output << hex << (input.b%256);
    
    return output.str();
}

const int FrameBuffer::_FIG_MAX_DEPTH = 999;
