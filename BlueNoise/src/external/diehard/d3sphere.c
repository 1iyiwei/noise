#include "header.h"
#include "macro.h"

real sphere(char *filename)
{
  const counter no_obs=20, no_pts=4000, edge=1000;
  const real ratio=edge/UNIMAX;

  int fcmpr(const void *u1, const void *u2);

  register counter i, j, k;
  real d, dmin;
  real *x, *y, *z, *p, r3, pvalue;
   
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|             THE 3DSPHERES TEST                              |");
  puts("\t|Choose  4000 random points in a cube of edge 1000.  At each  |");
  puts("\t|point, center a sphere large enough to reach the next closest|");
  puts("\t|point. Then the volume of the smallest such sphere is (very  |");
  puts("\t|close to) exponentially distributed with mean 120pi/3.  Thus |");
  puts("\t|the radius cubed is exponential with mean 30. (The mean is   |");
  puts("\t|obtained by extensive simulation).  The 3DSPHERES test gener-|");
  puts("\t|ates 4000 such spheres 20 times.  Each min radius cubed leads|");
  puts("\t|to a uniform variable by means of 1-exp(-r^3/30.), then a    |");
  puts("\t| KSTEST is done on the 20 p-values.                          |");
  puts("\t|-------------------------------------------------------------|\n");

  printf("\t\t    The 3DSPHERES test for file %s\n\n", filename);
  printf("\t\tsample no\tr^3\t\tequiv. uni.\n");

  x=(real*)malloc(no_pts*sizeof(real));   
  y=(real*)malloc(no_pts*sizeof(real));   
  z=(real*)malloc(no_pts*sizeof(real));  
  p=(real*)malloc(no_obs*sizeof(real));    

  for(i=1; i<=no_obs; ++i){ 
      dmin=10000000.;

    for(j=0; j<no_pts; ++j){
      x[j]=ratio*uni(filename);
      y[j]=ratio*uni(filename);
      z[j]=ratio*uni(filename);
    }
    qsort(x, no_pts, sizeof(real), fcmpr);

    /* to compare with fortran counter part
    for(j=0; j<no_pts; ++j){
      y[j]=ratio*uni(filename);
      z[j]=ratio*uni(filename);
    }*/

    for(j=0; j<no_pts-1; ++j){
      for(k=j+1; k<no_pts; ++k){
         d=(x[k]-x[j])*(x[k]-x[j]);

         if(d>=dmin) break;
         d+=(y[k]-y[j])*(y[k]-y[j])+(z[k]-z[j])*(z[k]-z[j]);
         dmin=MIN(dmin, d);
      }
    }

      r3=dmin*sqrt(dmin);
      p[i-1]=1-exp(-MIN(r3/30., 20));
      printf("\t\t   %d\t\t%.3f\t\t%f\n", i, r3, p[i-1]);
  }
  uni("close");

  free(x); free(y); free(z); 
  
  puts("\t--------------------------------------------------------------");
  pvalue=KStest(p,no_obs);
  printf("\t\tp-value for KS test on those %d p-values: %f", no_obs, pvalue);
  puts("\n");
    
  free(p);

  return pvalue;
}

/*main()
{
  sphere("binc");

  return;
}*/
