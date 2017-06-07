/* minimum distance^2 between n  random points(x(i),y(i)).
   mean is about .64 for 4000 points in a square of side 1000.
   and .995 for 8000 points in a square of side 10000.
   Since distance^2 is approximately exponential with mean .04,
   1.-exp(-d^2/.04) should be uniform on [0,1).  Thus a KS test.*/

#include "header.h"
#include "macro.h"

struct point {
  real x, y;
};

/*needed for qsort*/
int ptscmpr(const void *p1, const void *p2)
{
  struct point *u1=(struct point*)p1, *u2=(struct point*)p2;
  
  if( u1->y < u2->y ) return -1;
  if( u1->y == u2->y ) return 0;

  return 1;
}

real mindist(char *filename)
{
  const counter no_pts=8000, no_smpl=100, side=10000; 
  const real ratio=10000./UNIMAX;

  struct point *pts; 

  register counter i, j, k;
  real d, dmin;
  real *p, sum, pvalue;

 
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|              THE MINIMUM DISTANCE TEST                      |");
  puts("\t|It does this 100 times:  choose n=8000 random points in a    |");
  puts("\t|square of side 10000.  Find d, the minimum distance between  |");
  puts("\t|the (n^2-n)/2 pairs of points.  If the points are truly inde-|");
  puts("\t|pendent uniform, then d^2, the square of the minimum distance|");
  puts("\t|should be (very close to) exponentially distributed with mean|");
  puts("\t|.995 .  Thus 1-exp(-d^2/.995) should be uniform on [0,1) and |");
  puts("\t|a KSTEST on the resulting 100 values serves as a test of uni-|");
  puts("\t|formity for random points in the square. Test numbers=0 mod 5|");
  puts("\t|are printed but the KSTEST is based on the full set of 100   |");
  puts("\t|random choices of 8000 points in the 10000x10000 square.     |");
  puts("\t|-------------------------------------------------------------|\n");

  printf("\t\tThis is the MINIMUM DISTANCE test for file %s\n\n", filename);
  printf("\tSample no.\t d^2\t\t mean\t\tequiv uni\n");

  pts=(struct point*)malloc(no_pts*sizeof(struct point));  
  p=(real*)malloc(no_smpl*sizeof(real));  

#define GETPT   { pts[j].y=ratio*uni(filename); pts[j].x=ratio*uni(filename); }
  for(i=1; i<=no_smpl; ++i){
    dmin=2*side*side;
    for(j=0; j<no_pts; ++j){
      GETPT
    }    

    qsort(pts, no_pts, sizeof(*pts), ptscmpr);

    /* find the minimum distance */
    for(j=0; j<no_pts-1; ++j){
      for(k=j+1; k<no_pts; ++k){
        d=(pts[k].y-pts[j].y)*(pts[k].y-pts[j].y);
        if( d > dmin ) break;
        else{
          d+=(pts[k].x-pts[j].x)*(pts[k].x-pts[j].x);
          dmin=MIN(dmin, d);
        }
      }
    }

    sum+=dmin;
    p[i-1]=1-exp(-dmin/.995);    /* transforming into U[0,1] */

    if( i%5==0){ 
      printf("\t   %d\t\t%.4f\t\t%.4f\t\t%f\n", i, dmin, sum/i, p[i-1]);
    }
  }
  uni("close");

  free(pts);

  puts("\t--------------------------------------------------------------");
  printf("\tResult of KS test on %d transformed mindist^2's:", no_smpl);
  pvalue=KStest(p,no_smpl);
  printf(" p-value=%f\n\n", pvalue);

  free(p);

  return pvalue;
}

/*main()
{
  mindist("binc");

  return;
}*/
