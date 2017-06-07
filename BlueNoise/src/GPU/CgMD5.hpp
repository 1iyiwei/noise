/*
 * CgMD5.hpp
 *
 * Cg implementation of my MD5 noise
 *
 * Li-Yi Wei
 * 12/16/2007
 *
 */

#pragma warning (disable:4290)

#ifndef _CG_MD5_HPP
#define _CG_MD5_HPP

#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "Exception.hpp"

class CgMD5
{
public:
    CgMD5(void) throw(Exception);
    ~CgMD5(void);

    void SetSize(const int height, const int width);
    void SetKey(const unsigned int key);
    
    void Enable(void) const;
    void Disable(void) const;

    static string GenerateUniformRandom01Function(void);
    
protected:
    static string GenerateProgram(void);
    
    static void CheckError(void);
    
protected:
    CGcontext _context;
    CGprogram _program;
    CGprofile _fragmentProfile;

    CGparameter _key;
    CGparameter _res;
};

#endif
