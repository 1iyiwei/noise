/*
 * MD5.cpp
 *
 * Li-Yi Wei
 * 7/21/2005
 *
 */

#include "CgMD5.hpp"

#include <iostream>
#include <sstream>
using namespace std;

CgMD5::CgMD5(void) throw(Exception)
{
    cgSetErrorCallback(CheckError);
    
    _context = cgCreateContext();
    
    if(! _context)
    {
        throw Exception("null context");
    }

    _fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(_fragmentProfile);

    const string program = GenerateProgram();   

#if 0
    cout << program.c_str() << endl;
#endif
   
    _program = cgCreateProgram(_context, CG_SOURCE, program.c_str(), _fragmentProfile, "fragment", 0);
  
#if 0
    cout<<"---- PROGRAM BEGIN ----"<<endl;
    cout<<cgGetProgramString(_program, CG_COMPILED_PROGRAM);
    cout<<"---- PROGRAM END ----"<<endl;
#endif
    
    if(_program)
    {
        cgGLLoadProgram(_program);

        CheckError();
    }

    _key = cgGetNamedParameter(_program, "key");
    _res = cgGetNamedParameter(_program, "res");
    
    // enable stuff
    cgGLEnableProfile(_fragmentProfile);
    
    cgGLBindProgram(_program);
}

CgMD5::~CgMD5(void)
{
    cgGLDisableProfile(_fragmentProfile);
    
    cgDestroyContext(_context);
}

void CgMD5::CheckError(void) 
{
    CGerror error = cgGetError();

    if(error != CG_NO_ERROR)
    {
        throw Exception(cgGetErrorString(error));
    }
}

void CgMD5::SetSize(const int height, const int width)
{
    cgSetParameter2f(_res, width, height);
}

void CgMD5::SetKey(const unsigned int key)
{
    cgSetParameter1i(_key, key);
}

void CgMD5::Enable(void) const
{
    cgGLEnableProfile(_fragmentProfile);
    
    cgGLBindProgram(_program);
}

void CgMD5::Disable(void) const
{
    cgGLDisableProfile(_fragmentProfile);
}

string CgMD5::GenerateUniformRandom01Function(void)
{
    ostringstream output_program;

    output_program <<
        "unsigned int leftRotate(in unsigned int x, in unsigned int n)\n"
"{\n"
"    unsigned int t =  ( ((x) << (n)) | ((x) >> (32u-n)) );\n"
"    return t;\n"
"}\n"
"\n"
"unsigned int F(unsigned int x, unsigned int y, unsigned int z)\n"
"{\n"
"    unsigned int t;\n"
"    t = ( (x & y) | ((~x) & z) );\n"
"    return t;\n"
"}\n"
"\n"
"unsigned int G(unsigned int x, unsigned int y, unsigned int z)\n"
"{\n"
"    unsigned int t;\n"
"    t = ( (x & z) | ((~z) & y) );\n"
"    return t;\n"
"}\n"
"\n"
"unsigned int H(unsigned int x, unsigned int y, unsigned int z)\n"
"{\n"
"    unsigned int t;\n"
"    t = ( (x ^ y ^ z) );\n"
"    return t;\n"
"}\n"
"\n"
"unsigned int I(unsigned int x, unsigned int y, unsigned int z)\n"
"{\n"
"    unsigned int t;\n"
"    t = ( y ^ ((x) | (~z)));\n"
"    return t;\n"
"}\n"
"\n"
"void FF(inout uvec4 td, int i, inout uvec4 Fr, float p, in unsigned int data[16])\n"
"{\n"
"    unsigned int Ft = F(td.y,td.z,td.w);\n"
"    unsigned int r = Fr.x;\n"
"    Fr = Fr.yzwx;\n"
"	\n"
"    float t = sin(float(i)) * p;\n"
"    unsigned int trigFunc = unsigned int(floor(t));\n"
"    td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[i]), r );\n"
"    td = td.yzwx;\n"
"}\n"
"\n"
"void GG(inout uvec4 td, int i, inout uvec4 Gr, float p, in unsigned int data[16])\n"
"{\n"
"    unsigned int Ft = G(td.y,td.z,td.w);\n"
"    i = (5*i+1) %16;\n"
"    unsigned int r = Gr.x;\n"
"    Gr = Gr.yzwx;\n"
"    \n"
"    float t = sin(float(i)) * p;\n"
"    unsigned int trigFunc = unsigned int(floor(t));\n"
"    td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[i]), r );\n"
"    td = td.yzwx;\n"
"}\n"
"\n"
"void HH(inout uvec4 td, int i, inout uvec4 Hr, float p, in unsigned int data[16])\n"
"{\n"
"    unsigned int Ft = H(td.y,td.z,td.w);\n"
"    i = (3*i+5) %16;\n"
"    unsigned int r = Hr.x;\n"
"    Hr = Hr.yzwx;\n"
"    \n"
"    float t = sin(float(i)) * p;\n"
"    unsigned int  trigFunc = unsigned int(floor(t));\n"
"    td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[i]), r );\n"
"    td = td.yzwx;\n"
"}\n"
"\n"
"void II(inout uvec4 td, int i, inout uvec4 Ir, float p, in unsigned int data[16])\n"
"{\n"
"    unsigned int Ft = I(td.y,td.z,td.w);\n"
"    i = (7*i) %16;\n"
"    unsigned int r = Ir.x;\n"
"    Ir = Ir.yzwx;\n"
"    \n"
"    float t = sin(float(i)) * p;\n"
"    unsigned int trigFunc = unsigned int(floor(t));\n"
"    td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[i]), r );\n"
"    td = td.yzwx;\n"
"}\n"
"\n"
"\n"
"void initABCD(inout unsigned int A, inout unsigned int B, inout unsigned int C, inout unsigned int D)\n"
"{\n"
"    A = 0x67452301u;\n"
"    B = 0xefcdab89u;\n"
"    C = 0x98badcfeu;\n"
"    D = 0x10325476u;\n"
"}\n"
"\n"
"void setupInput(float2 absCoord, int key, inout unsigned int input[16])\n"
"{\n"
"    unsigned int inputLen = 128u;  \n"
"    \n"
"\n"
"    input[0] = unsigned int(floor(absCoord.x));\n"
"    input[1] = unsigned int(floor(absCoord.y));\n"
"\n"
"    \n"
"    input[0] = input[0] ^ unsigned int(key);\n"
"    input[1] = input[1] ^ unsigned int(key);\n"
"    \n"
"    input[2] = unsigned int(key);\n"
"    input[3] = unsigned int(key);\n"
"	\n"
"    input[4] = 0x80000000u;\n"
"\n"
"    \n"
"    \n"
"    for(int i=5; i<16; i++)\n"
"        input[i] = 0u;\n"
"        \n"
"    \n"
"    input[14] = 0u;\n"
"    input[15] = inputLen;\n"
"}\n"
"\n"
"void MD5opt(in unsigned int data[16], inout uvec4 digest)\n"
"{\n"
"    unsigned int A,B,C,D, Ft, Gt;\n"
"\n"
"    \n"
"    digest = uvec4(0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u);\n"
"    uvec4 td = digest;\n"
"    float p = pow(2.0,32.0);\n"
"    float t;\n"
"	\n"
"    \n"
"    \n"
"    unsigned int i; \n"
"    unsigned int index;\n"
"    unsigned int trigFunc;\n"
"    \n"
"    uvec4 Fr = uvec4(7u,12u, 17u,22u);  \n"
"    uvec4 Gr = uvec4(5u, 9u, 14u,20u);\n"
"    uvec4 Hr = uvec4(4u,11u, 16u,23u);\n"
"    uvec4 Ir = uvec4(6u,10u, 15u,21u);\n"
"    unsigned int r;\n"
"	\n"
"    \n"
"    for(i=0u; i<64u; i++)\n"
"    {\n"
"        if( i >= 0u && i < 16u)\n"
"        {\n"
"            \n"
"            Ft = F(td.y,td.z,td.w);\n"
"            Gt = i;\n"
"            r = Fr.x;\n"
"            Fr = Fr.yzwx;\n"
"        }\n"
"        else if (i >= 16u && i < 32u)\n"
"        {\n"
"            \n"
"            Ft = G(td.y,td.z,td.w);\n"
"            Gt = 5u * i + 1u;\n"
"            Gt = Gt % 16u;\n"
"            r = Gr.x;\n"
"            Gr = Gr.yzwx;\n"
"        }\n"
"        else if (i >= 32u && i < 48u)\n"
"        {\n"
"            \n"
"            Ft = H(td.y,td.z,td.w);\n"
"            Gt = 3u*i + 5u;\n"
"            Gt = Gt %16u;\n"
"            r = Hr.x;\n"
"            Hr = Hr.yzwx;\n"
"        }\n"
"        else\n"
"        {\n"
"            \n"
"            Ft = I(td.y,td.z,td.w);\n"
"            Gt = 7u*i;\n"
"            Gt = Gt % 16u;\n"
"            r = Ir.x;\n"
"            Ir = Ir.yzwx;\n"
"        }\n"
"        \n"
"        t = sin(float(i)) * p;\n"
"        trigFunc = unsigned int(floor(t));\n"
"        td.x = td.y + leftRotate( (td.x + Ft + trigFunc +data[int(Gt)]), r );\n"
"        td = td.yzwx;\n"
"    }\n"
"	\n"
"    digest += td;\n"
"\n"
"}\n"
"\n"
"void MD5unroll(in unsigned int data[16], inout uvec4 digest)\n"
"{\n"
"    unsigned int Ft, Gt;\n"
"    \n"
"    \n"
"    digest = uvec4(0x67452301u, 0xefcdab89u, 0x98badcfeu, 0x10325476u);\n"
"    uvec4 td = digest;\n"
"    float p = pow(2.0,32.0);\n"
"    \n"
"    uvec4 Fr = uvec4(7u,12u, 17u,22u);  \n"
"    uvec4 Gr = uvec4(5u, 9u, 14u,20u);\n"
"    uvec4 Hr = uvec4(4u,11u, 16u,23u);\n"
"    uvec4 Ir = uvec4(6u,10u, 15u,21u);\n"
"	\n"
"    FF(td, 0,Fr,p,data);\n"
"    FF(td, 1,Fr,p,data);\n"
"    FF(td, 2,Fr,p,data);\n"
"    FF(td, 3,Fr,p,data);\n"
"    FF(td, 4,Fr,p,data);\n"
"    FF(td, 5,Fr,p,data);\n"
"    FF(td, 6,Fr,p,data);\n"
"    FF(td, 7,Fr,p,data);\n"
"    FF(td, 8,Fr,p,data);\n"
"    FF(td, 9,Fr,p,data);\n"
"    FF(td,10,Fr,p,data);\n"
"    FF(td,11,Fr,p,data);\n"
"    FF(td,12,Fr,p,data);\n"
"    FF(td,13,Fr,p,data);\n"
"    FF(td,14,Fr,p,data);\n"
"    FF(td,15,Fr,p,data);\n"
"    \n"
"    GG(td,16,Gr,p,data);\n"
"    GG(td,17,Gr,p,data);\n"
"    GG(td,18,Gr,p,data);\n"
"    GG(td,19,Gr,p,data);\n"
"    GG(td,20,Gr,p,data);\n"
"    GG(td,21,Gr,p,data);\n"
"    GG(td,22,Gr,p,data);\n"
"    GG(td,23,Gr,p,data);\n"
"    GG(td,24,Gr,p,data);\n"
"    GG(td,25,Gr,p,data);\n"
"    GG(td,26,Gr,p,data);\n"
"    GG(td,27,Gr,p,data);\n"
"    GG(td,28,Gr,p,data);\n"
"    GG(td,29,Gr,p,data);\n"
"    GG(td,30,Gr,p,data);\n"
"    GG(td,31,Gr,p,data);\n"
"    \n"
"    HH(td,32,Hr,p,data);\n"
"    HH(td,33,Hr,p,data);\n"
"    HH(td,34,Hr,p,data);\n"
"    HH(td,35,Hr,p,data);\n"
"    HH(td,36,Hr,p,data);\n"
"    HH(td,37,Hr,p,data);\n"
"    HH(td,38,Hr,p,data);\n"
"    HH(td,39,Hr,p,data);\n"
"    HH(td,40,Hr,p,data);\n"
"    HH(td,41,Hr,p,data);\n"
"    HH(td,42,Hr,p,data);\n"
"    HH(td,43,Hr,p,data);\n"
"    HH(td,44,Hr,p,data);\n"
"    HH(td,45,Hr,p,data);\n"
"    HH(td,46,Hr,p,data);\n"
"    HH(td,47,Hr,p,data);\n"
"    \n"
"    II(td,48,Ir,p,data);\n"
"    II(td,49,Ir,p,data);\n"
"    II(td,50,Ir,p,data);\n"
"    II(td,51,Ir,p,data);\n"
"    II(td,52,Ir,p,data);\n"
"    II(td,53,Ir,p,data);\n"
"    II(td,54,Ir,p,data);\n"
"    II(td,55,Ir,p,data);\n"
"    II(td,56,Ir,p,data);\n"
"    II(td,57,Ir,p,data);\n"
"    II(td,58,Ir,p,data);\n"
"    II(td,59,Ir,p,data);\n"
"    II(td,60,Ir,p,data);\n"
"    II(td,61,Ir,p,data);\n"
"    II(td,62,Ir,p,data);\n"
"    II(td,63,Ir,p,data);\n"
"	\n"
"    digest += td;\n"
"}\n"
"\n"
"\n"
"float convertToFloatStanley(unsigned int value)\n"
"{\n"
"    unsigned int signbit = (value & 0x80000000u) >> 31u;\n"
"    unsigned int mantissa = (value & 0x7fffff00u) >> 8u;\n"
"    unsigned int exponent = (value & 0x000000ffu);\n"
"    \n"
"    float fVal;\n"
"    fVal = float(mantissa) / pow(2.0,23.0) + 1;\n"
"    \n"
"    fVal *= 1;\n"
"	\n"
"    \n"
"    return fVal;\n"
"}\n"
"\n"
"float ConvertToFloat01(unsigned int value)\n"
"{\n"
"    \n"
"\n"
"    float result = intBitsToFloat(0x3f800000u | (value & 0x007fffffu)) - 1.0;\n"
"    \n"
"    return result;\n"
"}\n"
"\n"
"float4 ConvertToFloat01(uvec4 value)\n"
"{\n"
"    float4 result;\n"
"\n"
"    result.x = ConvertToFloat01(value.x);\n"
"    result.y = ConvertToFloat01(value.y);\n"
"    result.z = ConvertToFloat01(value.z);\n"
"    result.w = ConvertToFloat01(value.w);\n"
"\n"
"    return result;\n"
"}\n"
"\n"
"float4 UniformRandom01(float2 input, uniform int key)\n"
"{\n"
"    unsigned int input_padded[16];\n"
"    setupInput(input, key, input_padded);\n"
"\n"
"    uvec4 digest;\n"
"    MD5unroll(input_padded, digest);\n"
"\n"
"    return ConvertToFloat01(digest);\n"
        "}\n";
    
    return output_program.str();
}

string CgMD5::GenerateProgram(void)
{
    ostringstream output_program;

    output_program << GenerateUniformRandom01Function() << endl;

    output_program <<
        
"struct FragmentInput\n"
"{\n"
"    float4 tex : TEX0;\n"
"    float4 col : COL0;\n"
"};\n"
"\n"
"struct FragmentOutput\n"
"{\n"
"    float4 col : COL;\n"
"};\n"
"\n"
"FragmentOutput fragment(FragmentInput input, uniform int key, uniform float2 res)\n"
"{\n"
"    FragmentOutput output;\n"
"    output.col = UniformRandom01(input.tex.xy * res, key);\n"
"\n"
"    return output;\n"
"}\n"
        "\n";
    
    return output_program.str();
}
