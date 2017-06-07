#include "header.h" 

/* determine the state of a permutation */
int state(uniform *u, int dim, int begin) 
{
  uniform perm[5];              /*5 is the dimension of u*/
  unsigned map[60]={39,38,37,36,41,40,54,55,56,57,58,59,49,48,52,53,50,51,42,
                    43,44,45,46,47,33,32,31,30,35,34,12,13,14,15,16,17,29,28,
                    24,25,27,26,21,20,19,18,23,22,2,3,5,4,1,0,10,11,9,8,6,7  };
  register int i, j, index, max;
  int tmp, state=0;

  for(i=0; i<5; ++i){
    index=(begin+i)%5;
    perm[i]=u[index];
  } 

  for(i=4; i>=1; --i){
    max=perm[0];
    index=0;
  
    for(j=1; j<=i; ++j){ 
      if( perm[j]<max ){
        continue;
      }

      max=perm[j];
      index=j;
    }

    state*=(i+1);
    state+=index;

    tmp=perm[i];
    perm[i]=perm[index];
    perm[index]=tmp;
  }   
 
  if( state<60 ){
    state= map[state];
  }

  return state;
}
     
real operm5(char *filename)
{
  const counter no_obs=1000000, no_tests=2, no_trs=1000000;
  const real mean=2*no_trs/120., ratio=no_trs*200000.;

  FILE *infile;

  uniform u[5];
  counter f[120];
  long A[60][60], B[60][60];    /* A, B are part of the cov matrix */
  int i, j, k, begin, st;
  real x[60], y[60], chsq;

  real p_values[no_tests];
  
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|           THE OVERLAPPING 5-PERMUTATION TEST                |");
  puts("\t|This is the OPERM5 test.  It looks at a sequence of one mill-|");
  puts("\t|ion 32-bit random integers.  Each set of five consecutive    |");
  puts("\t|integers can be in one of 120 states, for the 5! possible or-|");
  puts("\t|derings of five numbers.  Thus the 5th, 6th, 7th,...numbers  |");      puts("\t|each provide a state. As many thousands of state transitions |");
  puts("\t|are observed,  cumulative counts are made of the number of   |");
  puts("\t|occurences of each state.  Then the quadratic form in the    |");
  puts("\t|weak inverse of the 120x120 covariance matrix yields a test  |");
  puts("\t|equivalent to the likelihood ratio test that the 120 cell    |");
  puts("\t|counts came from the specified (asymptotically) normal dis-  |");
  puts("\t|tribution with the specified 120x120 covariance matrix (with |");
  puts("\t|rank 99).  This version uses 1,000,000 integers, twice.      |");
  puts("\t|-------------------------------------------------------------|\n");

  printf("\t\t\tOPERM5 test for file \n", filename);
  puts("\t\t  (For samples of 1,000,000 consecutive 5-tuples)\n");

  infile=fopen("operm5.cov", "r");

  if(!infile)
  {
      fprintf(stderr, "cannot open operm5.cov");
      exit(1);
  }
  
  for(i=0; i<60; ++i){
    for(j=i; j<60; ++j){
      fscanf(infile, "%d", &A[i][j]);
      A[j][i]=A[i][j];
    }
  }

  for(i=0; i<60; ++i){
    for(j=i; j<60; ++j){
      fscanf(infile, "%d", &B[i][j]);
      B[j][i]=B[i][j];
    }
  }

  fclose(infile);

  for(i=1; i<=no_tests; ++i){
    for(j=0; j<120; ++j){
      f[j]=0;
    }
           
    for(j=0; j<5; ++j){
      u[j]=uni(filename);
    }

    begin=0;
    for(j=1; j<=no_trs; ++j){
      st=state(u, 5, begin);
      ++f[st];
      u[begin]=uni(filename);
 
      begin=(begin+1)%5;
    }

    chsq=0;

    for(j=0; j<60; ++j){ 
       x[j]=f[j]+f[j+60]-mean;
       y[j]=f[j]-(real)f[j+60];
    }

    for(j=0; j<60; ++j){
      for(k=0; k<60; ++k){
        chsq+=x[j]*A[j][k]*x[k]+y[j]*B[j][k]*y[k];
      }
    }

    chsq/=ratio;

    printf("\t\t\t  sample %d \n", i);
    printf("\tchisquare=%f with df=99; p-value= %f\n",chsq,1-Chisq(99,chsq));
    printf("\t_______________________________________________________________");
    puts("\n");
    
    p_values[i-1] = 1-Chisq(99,chsq);
    /* printf("operm5: p_values[%d] = %f\n", i, p_values[i-1]);*/
  }
  uni("close");

  return KStest(p_values,no_tests);
}
  
/*main()
{
  operm5("binc");

  return;
}*/
