#include <stdio.h>
#include <math.h>
#include "macro.h"

double G(double z);

/*p.d.f of Standard Normal*/
double phi(double x)
{
  return exp(-x*x/2)/sqrt(2*PI);
}


/*c.d.f of Standard Normal*/
double Phi(double x)
{
  double tmp=x/sqrt(2.);
  tmp=1+erf(tmp); 

  return tmp/2;
}

/*p.d.f of Chi-square*/
double chisq(int df, double x)
{
  return ( pow(x/2, (df-2)/2.)*exp(-x/2)/(2*G(df/2.)) );
}

/*c.d.f of Chi-square*/
double Chisq(int df, double x)
{
  switch(df){
    case 1: return 2*Phi(sqrt(x))-1;
    case 2: return 1-exp(-x/2);
    default: break;
  }

  return ( Chisq(df-2,x)-2*chisq(df,x) );
}

/*p.d.f of Poisson distribution*/
double Poisson(double lambda, int k)
{
  if(k==0) return exp(-lambda);

  return exp(-lambda)*pow(lambda, k)/G(k+1);  
}

/*main()
{
  char ch;
  double x;
  int df;

  do{
    scanf("%d%lf", &df, &x);
    printf("%15.13f\n", chisq(df,x) );
  }while( (ch=getchar())!='n' );

  return;
}*/

