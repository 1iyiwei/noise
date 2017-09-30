/*
 * MD5.cpp
 *
 * Li-Yi Wei
 * 7/21/2005
 *
 */

#include "CgMD5.hpp" 
#include <iostream>

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

#if 1
    _program = cgCreateProgramFromFile(_context, CG_SOURCE, "md5.cg", _fragmentProfile, "fragment", 0);
#else
    _program = cgCreateProgram(_context, CG_SOURCE, _FRAGMENT_PROGRAM, _fragmentProfile, "fragment", 0);
#endif
    
#if 0
    cout << "---- Cg Program Begin ---- " << endl;
    cout << _FRAGMENT_PROGRAM;
    cout << "---- Cg Program End ---- " << endl;
#endif
    
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
    
char CgMD5::_FRAGMENT_PROGRAM[] =
"\n"
"struct FragmentInput\n"
"{\n"
" float4 tex : TEX0;\n"
" float4 col : COL0;\n"
"};\n"
"struct FragmentOutput\n"
"{\n"
" float4 col : COL;\n"
"};\n"
"\n"
"FragmentOutput fragment(FragmentInput input)\n"
"{\n"
" FragmentOutput output;\n"
" float4 result = float4(0, 1, 0, 1);\n"
" if(input.tex.y > input.tex.x * input.tex.x) \n"
" { \n"
"    result = float4(1, 0, 0, 1);\n"
" } \n"
#if 1
" else \n"
" { \n"
"    result = float4(0, 0, 1, 1);\n"
" } \n"
#endif
" \n"
" output.col = result;\n"
" return output;\n"
"}\n";
