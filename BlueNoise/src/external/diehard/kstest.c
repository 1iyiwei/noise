#include "header.h"
#include "macro.h"

/*This test is based on a modified Kolmogorov-Smirnov method.
  The test-statistic is (FN(X)-X)**2/(X*(1-X)) (Anderson-Darling)
  where X is a uniform under null hypothesis. FN(X) is the empirical 
  distribution of X.*/

/*c.d.f of Anderson-Darling statistic (a quick algorithm)*/
real AD(real z)
{
  if( z<.01 ) return 0;

  if( z<=2 ){
    return 2*exp(-1.2337/z)*(1+z/8-.04958*z*z/(1.325+z))/sqrt(z);
  }

  if( z<=4 ) return 1-.6621361*exp(-1.091638*z)-.95095*exp(-2.005138*z); 

  if( 4<z ) return 1-.4938691*exp(-1.050321*z)-.5946335*exp(-1.527198*z);

  return -1;       /*error indicator*/
} 

real spline(real x, int n)
{
  real tmp=ABS(10*x+.5-n);

  if( n<7 ){
    if( tmp>1.5 ) return 0;
    if( tmp<=.5 ) return 1.5-2*tmp*tmp;
    else return 2.25-tmp*(3-tmp);
  }

  else switch(n){
         case 7:{ 
           if( x<=.8 || x>=1 ) return 0;
           else return 100*(x-.9)*(x-.9)-1;
         }
         case 8:{ 
           if( x<=0 || x>=.05 ) return 0;
           if( x<=.01 ) return -100*x;
           else return 25*(x-.05);
         } 
         case 9:{ 
           if( x<=.98 || x>=1 ) return 0;
           else return .1-10*ABS(x-.99);
         }
         default: break;
       }

  return -100000;  /*error indicator*/
} 

real KStest( real *x, int dim)
{
  int fcmpr(const void *u1, const void *u2);

  int L[10][8]={ {40,46,37,34,27,24,20,20},
                 {88,59,43,37,29,27,20,22},
                 {92,63,48,41,30,30,25,24},
                 {82,59,42,37,26,28,26,22},
                 {62,48,33,30,23,23,22,18},
                 {49,34,22,20,16,17,17,12},
                 {17,17, 7, 8, 4, 7, 5, 1},
                 {40,18,19,14,16,13,10, 9},
                 {59,20,10, 4, 1, 1, 0,-1},
                 {41,43,36,112,15,95,32,58} };
  int i, m=MIN(dim-2, 8)-1;
  real pvalue, tmp, sum=0, z=-dim*dim, epsilon=pow(10, -20);

  qsort(x,dim,sizeof(double),fcmpr);

  for(i=0; i<dim; ++i){
    tmp=x[i]*(1-x[dim-1-i]);
    tmp=MAX(epsilon,tmp);
    z-=(2*i+1)*log(tmp);
  }

  z/=dim;
  pvalue=1-AD(z);

  /*for(i=0; i<10; ++i) sum+=L[i][m]*sp(p, i)*.0001;

  if( dim>10 ) sum*=10./dim;
 
  return p+sum;???*/

  return pvalue;
}




 
