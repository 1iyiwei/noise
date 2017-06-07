
#include "header.h"
#include "macro.h"

real park(char *filename)
{
  const counter no_obs=10, side=100;
  const counter no_trials=12000;
  const real ratio=side/UNIMAX, mu=3523, sigma=21.9;

  register counter no_succ, j, k;
  real xtmp, ytmp;
  real *x, *y, mean, var, z, *p, pvalue; 
  counter sum=0, ss=0, i;

  puts("\t|-------------------------------------------------------------|");
  puts("\t|              THIS IS A PARKING LOT TEST                     |");
  puts("\t|In a square of side 100, randomly \"park\" a car---a circle of |");
  puts("\t|radius 1.   Then try to park a 2nd, a 3rd, and so on, each   |");
  puts("\t|time parking \"by ear\".  That is, if an attempt to park a car |");
  puts("\t|causes a crash with one already parked, try again at a new   |");
  puts("\t|random location. (To avoid path problems, consider parking   |");
  puts("\t|helicopters rather than cars.)   Each attempt leads to either|");
  puts("\t|a crash or a success, the latter followed by an increment to |");
  puts("\t|the list of cars already parked. If we plot n: the number of |");
  puts("\t|attempts, versus k: the number successfully parked, we get a |");
  puts("\t|curve that should be similar to those provided by a perfect  |");
  puts("\t|random number generator.  Theory for the behavior of such a  |");
  puts("\t|random curve seems beyond reach, and as graphics displays are|");
  puts("\t|not available for this battery of tests, a simple characteriz|");
  puts("\t|ation of the random experiment is used: k, the number of cars|");
  puts("\t|successfully parked after n=12,000 attempts. Simulation shows|");
  puts("\t|that k should average 3523 with sigma 21.9 and is very close |");
  puts("\t|to normally distributed.  Thus (k-3523)/21.9 should be a st- |");
  puts("\t|andard normal variable, which, converted to a uniform varia- |");
  puts("\t|ble, provides input to a KSTEST based on a sample of 10.     |");
  puts("\t|-------------------------------------------------------------|\n");
  
  printf("\t\tCDPARK: result of %d tests on file %s\n", no_obs, filename);
  printf("\t  (Of %d tries, the average no. of successes", no_trials);
  printf(" should be \n\t   %.1f with sigma=%.1f)\n\n", mu, sigma);
  printf("\t   No. succeses\t\tz-score\t\tp-value\n");

  p=(real*)malloc(no_obs*sizeof(real));
  x=(real*)malloc(no_trials*sizeof(real));
  y=(real*)malloc(no_trials*sizeof(real));

#define PARK  { xtmp=ratio*uni(filename); ytmp=ratio*uni(filename);}
#define CRASH   (ABS(x[k]-xtmp)<=1 && ABS(y[k]-ytmp)<=1)

  for(i=1; i<=no_obs; ++i){     
    PARK                         /* the first parking */
    x[0]=xtmp;
    y[0]=ytmp;
    no_succ=1;

    for(j=0; j<no_trials; ++j){
      PARK                       /* succesive parking */
      for(k=0; ; ){
        if( CRASH  ){
          break;
        }

        ++k;
        if(k<no_succ){
          continue; 
        }

        x[no_succ]=xtmp;
        y[no_succ]=ytmp;
        ++no_succ;
        break;
      }
    }

    sum+=no_succ;
    ss+=no_succ*no_succ;
  
    z=(no_succ-mu)/sigma;
    p[i-1]=1-Phi(z);

    printf("\t\t%d\t\t% .4f\t\t%f\n", no_succ, z, p[i-1]);
  }
  uni("close");

  free(p); free(x); free(y);

  mean=(real)sum/no_obs;
  var=(real)ss/no_obs-mean*mean;

  printf("\t  Square side=%d, avg. no. parked=%.2f", side, mean);
  printf(" sample std.=%.2f\n", sqrt(var)); 
 
  pvalue=KStest(p, no_obs);
  printf("\t     p-value of the KSTEST for those %d", no_obs); 
  printf(" p-values: %f\n\n", pvalue);
    
  return pvalue;
}

/*main()
{
  park("binc");

  return;
}*/
