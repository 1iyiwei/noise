
#include "header.h"
#include "macro.h"

/*gamma(z) when 2z is a integer*/
double G(double z)
{
  int tmp=2*z;

  if( tmp!=2*z || z==0 ) printf("Error in calling G(z)!!!");

  switch(tmp){
    case 1: return sqrt(PI);
    case 2: return 1;
    default: break;
  }

  return (z-1)*G(z-1);
}

int ucmpr( const void *i1, const void *i2)
{
  uniform *u1=(uniform*)i1, *u2=(uniform*)i2;
  
  if( *u1<*u2 ) return -1;
  if( *u1==*u2 ) return 0;

  return 1;
}

int fcmpr(const void *f1, const void *f2)
{
  real *u1=(real*)f1, *u2=(real*)f2;

  if( *u1<*u2 ) return -1;
  if( *u1==*u2 ) return 0;

  return 1;
}

/*read in a uniform random number from a file*/
uniform uni(char *filename)
{
  static FILE *infile;

  static char isopen='n';
  static counter count=DIM;
  static uniform uniran[DIM];

 /* uniform fortran[1]; */ 

  if( strcmp(filename, "close")==0 ){
    fclose(infile);
    isopen='n';
    count=DIM;

    return 0;
  }
    
  if( (++count)<DIM ){
    return uniran[count];
  }

  if( isopen=='n' ){
    if( (infile=fopen(filename, "r"))==NULL ){
      printf("can't open file %s!!!\n", filename);
      exit(1);
    }

    isopen='y';
/*fread( fortran, sizeof(uniform), 1 , infile );*/ /*for binary file written*/
                                                   /*by fortran program*/
  }

  fread( uniran, sizeof(uniform), DIM , infile );

  count=0;
/*fread( fortran, sizeof(uniform), 1 , infile );
fread( fortran, sizeof(uniform), 1 , infile );*/

  return uniran[count];
}

/*show the bit-pattern of an integer*/
void showbit(int n)
{
  short int i, bit[32];
  int tmp=n;

  for(i=0; i<32; ++i){
    bit[i]=0;
    if(tmp<0) bit[i]=1;
    tmp*=2;
    printf("%d",bit[i]);
  } 
 
  return;
}
