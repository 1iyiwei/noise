#include "header.h"

real osum(char *filename)
{
  const counter no_obs=10, no_sum=100, no_num=100;
  const real mean=.5*no_num, rstd=sqrt(12);

  counter i, j, k;
  real *x, *y, *p, *pv, tmp, sum, a, b;

  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|            The  OVERLAPPING SUMS test                       |");
  puts("\t|Integers are floated to get a sequence U(1),U(2),... of uni- |");
  puts("\t|form [0,1) variables.  Then overlapping sums,                |");
  puts("\t|  S(1)=U(1)+...+U(100), S2=U(2)+...+U(101),... are formed.   |");
  puts("\t|The S''s are virtually normal with a certain covariance mat- |");
  puts("\t|rix.  A linear transformation of the S''s converts them to a |");
  puts("\t|sequence of independent standard normals, which are converted|");
  puts("\t|to uniform variables for a KSTEST.                           |");
  puts("\t|-------------------------------------------------------------|\n");

  printf("\t\t\tResults of the OSUM test for %s\n\n", filename);
  printf("\t\t\tTest no\t\t\tp-value\n");

  x=(real*)malloc(no_num*sizeof(real));
  y=(real*)malloc(no_num*sizeof(real));
  p=(real*)malloc(no_sum*sizeof(real));
  pv=(real*)malloc(no_obs*sizeof(real));

  for(i=1; i<=no_obs; ++i){
    for(j=0; j<no_sum; ++j){

   /* get a sequence of numbers and add them up */
      sum=0;
      for(k=0; k<no_num; ++k){
        y[k]=uni(filename)/UNIMAX;
        sum+=y[k];
      }

   /* discard the first number in the sequence and get another number
      add up the new sequence of numbers */

      for(k=1; k<no_num; ++k){
        tmp=y[k-1];
        y[k-1]=(sum-mean)*rstd;
        sum-=(tmp-uni(filename)/UNIMAX);
      }
      y[no_num-1]=(sum-mean)*rstd;

      x[0]=y[0]/sqrt(no_num);
 
      x[1]= -x[0]*(no_num-1)/sqrt(2*no_num-1.)+y[1]*sqrt(no_num/(2*no_num-1.));
      x[0]=Phi(x[0]);
      x[1]=Phi(x[1]);

      for(k=2; k<no_num; ++k){
        a=2*no_num+1-k;
        b=2*a-2;
        x[k]=y[0]/sqrt(a*b)-sqrt((a-1)/(b+2))*y[k-1]+y[k]*sqrt(a/b);
        x[k]=Phi(x[k]);
      }

      p[j]=KStest(x, no_num);
    }

    pv[i-1]=KStest(p, no_num);
    printf("\t\t\t  %d \t\t\t%f\n", i, pv[i-1]);
  }
  uni("close");

  printf("\t_____________________________________________________________\n\n");

  free(x); free(y); free(p);

  tmp=KStest(pv, no_obs);
  free(pv);

  printf("\t\tp-value for %d kstests on %d kstests:%f\n", no_obs, no_sum, tmp);

  return tmp;
}

/*main()
{
  osum("binc");

  return;
}*/
