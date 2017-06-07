
#ifndef _HEADER_H_
#define _HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DIM       4096
#define UNIMAX    4294967296.   /*pow(2,32)*/

typedef unsigned int    uniform;    

typedef unsigned long   counter;

typedef double          real;

uniform uni(char *filename);

double Phi(double z);

double Chisq(int df, double chsq);

double Poisson(double lambda, int k);

real KStest(real *x, int dim);

/* tests: all return final aggregate p-value */
real bday(char *filename);
real operm5(char *filename);
real binrnk(char *filename, char *test); 
real bitst(char *filename);
real monky(char *filename, char *test);
real cnt1s(char *filename, char *test);
real park(char *filename);
real mindist(char *filename);
real sphere(char *filename);
real squeez(char *filename);
real osum(char *filename);
real runtest(char *filename);
real craptest(char *filename);

#endif /*_HEADER_H_*/



