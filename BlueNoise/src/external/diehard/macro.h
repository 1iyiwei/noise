#ifndef _MACRO_H_
#define _MACRO_H_

#ifdef PI
#undef PI
#endif

#define PI      3.141592653589793

#define MAX(a, b)  ( (a)<(b) ? (b):(a) )

#define MIN(a, b)  ( (a)<(b) ? (a):(b) )

#define ABS(x)     ( (x)>0 ? (x):(-(x)) )

#define SIGN(x)    ( (x)>0 ? 1:(-1) )

#endif /*_MACRO_H_*/
