/* SQUEEZE TEST:  How many iterations of k=k*uni()+1 are required
   to squeeze k down to 1, starting with k=2147483647=2^31-1?
   The exact distribution of the required j is used, with
   a chi-square test based on no_trials=100,000 tries.
   The mean of j is 23.064779, with variance 23.70971151.*/

#include "header.h"
#include "macro.h"

real squeez(char *filename)
{
  const counter no_trials=100000;
  const real ratio=no_trials/1000000., std=sqrt(84);

  register counter i;
  register unsigned long k; 
  register short j;

  counter f[43];
  real Ef[]={21.03,57.79,175.54,467.32,1107.83, 2367.84,
               4609.44,8241.16,13627.81,20968.49,30176.12,40801.97,52042.03,
               62838.28,72056.37,78694.51,82067.55,81919.35,78440.08,72194.12,
               63986.79,54709.31,45198.52,36136.61,28000.28,21055.67,15386.52,
               10940.20,7577.96,5119.56,3377.26,2177.87,1374.39,849.70,515.18,
               306.66, 179.39, 103.24, 58.51, 32.69, 18.03,  9.82, 11.21};
  real tmp, chsq=0;

  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|                 This is the SQUEEZE test                    |");
  puts("\t| Random integers are floated to get uniforms on [0,1). Start-|");
  puts("\t| ing with k=2^31=2147483647, the test finds j, the number of |");
  puts("\t| iterations necessary to reduce k to 1, using the reduction  |");
  puts("\t| k=ceiling(k*U), with U provided by floating integers from   |");
  puts("\t| the file being tested.  Such j''s are found 100,000 times,  |");
  puts("\t| then counts for the number of times j was <=6,7,...,47,>=48 |");
  puts("\t| are used to provide a chi-square test for cell frequencies. |");
  puts("\t|-------------------------------------------------------------|\n");

  printf("\t\t\tRESULTS OF SQUEEZE TEST FOR %s\n\n", filename);
  printf("\t\t    Table of standardized frequency counts\n");
  printf("\t\t(obs-exp)^2/exp  for j=(1,..,6), 7,...,47,(48,...)\n\t" );
 
  for(i=0; i<43; ++i){
    f[i]=0;
    Ef[i]*=ratio;
  }
  
  for(i=1; i<=no_trials; ++i){
    k=2147483647;
    j=0;

    /* squeeze k */
    while( (k!=1)&&(j<48) ){
      k=k*( uni(filename)/UNIMAX )+1; 
      ++j;
    }

    j=MAX(j-6,0);
    ++f[j];
  }
  uni("close");

  /* compute chi-square */
  for(i=0; i<=42; ++i){
    tmp=(f[i]-Ef[i])/sqrt(Ef[i]);
    chsq+=tmp*tmp;
    printf("\t% .1f  ", tmp );
    if((i+1)%6==0) printf("\n\t");
  }

  printf("\n\t\tChi-square with 42 degrees of freedom:%f\n", chsq);
  printf("\t\tz-score=%f, p-value=%8.6f\n",(chsq-42.)/std, 1-Chisq(42,chsq)); 
  printf("\t_____________________________________________________________\n\n");

  return 1-Chisq(42,chsq);
}

/*main()
{
  squeez("binc");

  return;
}*/
