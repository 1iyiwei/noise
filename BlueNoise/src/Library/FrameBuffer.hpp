/*
 * FrameBuffer.hpp
 *
 * some commom frame buffer operations
 *
 * Li-Yi Wei
 *
 */

#ifndef _FRAME_BUFFER_HPP
#define _FRAME_BUFFER_HPP

#include <vector>
using namespace std;

#include "Array.hpp"

class FrameBuffer
{
public:
    struct P3
    {
        int r, g, b;
    };

    static int ReadPPM(const char * file_name,
                       Array<P3> & pixels,
                       int & maximum_value);
    
    static int WritePPM(const Array<P3> & pixels,
                        const int maximum_value,
                        const char * file_name);

    static int ReadPPM(const char * file_name,
                       vector< vector<P3> > & pixels,
                       int & maximum_value);
    
    static int WritePPM(const vector< vector<P3> > & pixels,
                        const int maximum_value,
                        const char * file_name);

    struct P3L2I
    {
        int r, g, b;
        int x, y;
    };
    
    // write out selected pixels from a bitmap into fig format
    // this is useful to avoid missing important details during zoom out
    // region: [x_min x_max y_min y_max] in pixels
    static int WriteFIG(const vector<P3L2I> & samples,
                        const vector<int> & region,
                        const char * file_name);
    
    // struct PF
    union PF // assume monochrome only now
    {
        float r;
        float g;
        float b;
    };
    
    static int ReadPFM(const char * file_name,
                       Array<PF> & pixels);
    
    static int WritePFM(const Array<PF> & pixels,
                        const char * file_name);

    struct L2F
    {
        float x, y;
    };

    // color_scheme: 0 for black over white, 1 vice versa
    // region: [x_min x_max y_min y_max] in float
    static int WriteFIG(const vector<L2F> & samples, const vector<float> & region, const float radius, const int boundary, const int color_scheme, const char *file_name);

    struct AF
    {
        float r, g, b, a;
    };
};

#endif
