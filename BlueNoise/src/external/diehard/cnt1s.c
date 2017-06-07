#include "header.h"

typedef enum {A,B,C,D,E} letters;

/*convert a byte to a letter*/
letters b2l(int byte)
{
  register int cnt1s=0, tmp, j;

    for(j=0; j<8; ++j){
     tmp=(byte>>j) & 1;
      if( tmp==1 ) ++cnt1s;
    }

    if(cnt1s<3) return A;
     
    switch( cnt1s ){
      case 3: return B;
      case 4: return C;
      case 5: return D;
      default: break;
    }
  
  return E;
}

/* get a byte from a stream of bytes */
int getb(char *filename, int rt)
{
  static short rest=0;
  static uniform wd;

  if(rest==0){
    wd=uni(filename);
    rest=4;
  }

  --rest;

  return ( (wd >> rest*8) & 255 );    /*255=pow(2,8)-1*/
}

/* get a specific byte of a random integer */ 
int getsb(char *filename, int rt)
{
  return ( (uni(filename) >> rt) & 255 );
}

/* count the 1's in the sequence of bytes */
real cnt_stat(char *fn, counter no_wds, char *test, int rshft)
{
  const real mean=2500, std=sqrt(5000);
  const real prob[]={37/256.,56/256.,70/256.,56/256.,37/256.};

  int (*byte)(char*, int);

  register int wd;
  register counter i; 

  short j, s;
  counter *f, f4[625], f5[3125];
  real Ef, chsq=0, z;
  
  if( test[1]=='p' ){
    byte=getsb;
    printf("\t%d to %d  ", 25-rshft,32-rshft);
  }
  else{
    byte=getb;
    printf("\t\t");
  }

  for(i=0; i<625; ++i){
    f4[i]=0;
    f5[i]=0;
  }

  for(i=625; i<3125; ++i){
    f5[i]=0;
  }

#define BYTE  ( byte(fn, rshft) )
  wd=(625*b2l(BYTE)+125*b2l(BYTE)+25*b2l(BYTE)+5*b2l(BYTE)+b2l(BYTE)); 

  for(i=1; i<no_wds; ++i){   
    wd %= 625;                      /*Erase leftmost letter of w*/
    ++f4[wd];

    wd=5*wd+b2l(BYTE);              /*Shift wd left, add new letter*/
    ++f5[wd];
  }

/* compute Q5-Q4, where Q4,Q5=sum(obs-exp)**2/exp */
  for(s=4; s<=5; ++s){
    int ltrspwd, wdspos;
    letters ltr;

    switch(s){
      case 4: wdspos=625; f=f4; ltrspwd=4; break;
      case 5: wdspos=3125; f=f5; chsq=-chsq; ltrspwd=5; break;
    }
  
    for(i=0; i<wdspos; ++i){
      Ef=no_wds;
      wd=i;
 
      for(j=0; j<ltrspwd; ++j){
        ltr=wd%5;
        Ef*=prob[ltr];
        wd/=5; 
      }

      chsq+=( *(f+i)-Ef )*( *(f+i)-Ef )/Ef;
    } 
  }
  uni("close");
    
  z=(chsq-mean)/std;
  printf("\t%.2f\t\t% .3f\t\t%f\n", chsq, z, 1-Phi(z));

#if 0
  return chsq;
#else
  return 1-Phi(z);
#endif
}

/*print the title*/
void cnt_ttl(char *fn, char *test)
{
 if( strncmp(test, "specific", 2 )==0){
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|    This is the COUNT-THE-1''s TEST for specific bytes.      |");
  puts("\t|Consider the file under test as a stream of 32-bit integers. |");
  puts("\t|From each integer, a specific byte is chosen , say the left- |");
  puts("\t|most: bits 1 to 8. Each byte can contain from 0 to 8 1''s,   |");
  puts("\t|with probabilitie 1,8,28,56,70,56,28,8,1 over 256.  Now let  |");
  puts("\t|the specified bytes from successive integers provide a string|");
  puts("\t|of (overlapping) 5-letter words, each \"letter\" taking values |");
  puts("\t|A,B,C,D,E. The letters are determined  by the number of 1''s,|");
  puts("\t|in that byte: 0,1,or 2 ---> A, 3 ---> B, 4 ---> C, 5 ---> D, |");
  puts("\t|and  6,7 or 8 ---> E.  Thus we have a monkey at a typewriter |");
  puts("\t|hitting five keys with with various probabilities: 37,56,70, |");
  puts("\t|56,37 over 256. There are 5^5 possible 5-letter words, and   |");
  puts("\t|from a string of 256,000 (overlapping) 5-letter words, counts|");
  puts("\t|are made on the frequencies for each word. The quadratic form|");
  puts("\t|in the weak inverse of the covariance matrix of the cell     |");
  puts("\t|counts provides a chisquare test: Q5-Q4, the difference of   |");
  puts("\t|the naive Pearson  sums of (OBS-EXP)^2/EXP on counts for 5-  |");
  puts("\t|and 4-letter cell  counts.                                   |");
  puts("\t|-------------------------------------------------------------|\n");

  printf("\t\tTest results for specific bytes from %s\n", fn);
 }

 else{
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|    This is the COUNT-THE-1''s TEST on a stream of bytes.    |");
  puts("\t|Consider the file under test as a stream of bytes (four per  |");
  puts("\t|32 bit integer).  Each byte can contain from 0 to 8 1''s,    |");
  puts("\t|with probabilities 1,8,28,56,70,56,28,8,1 over 256.  Now let |");
  puts("\t|the stream of bytes provide a string of overlapping  5-letter|");
  puts("\t|words, each \"letter\" taking values A,B,C,D,E. The letters are|");
  puts("\t|determined by the number of 1''s in a byte: 0,1,or 2 yield A,|");
  puts("\t|3 yields B, 4 yields C, 5 yields D and 6,7 or 8 yield E. Thus|");
  puts("\t|we have a monkey at a typewriter hitting five keys with vari-|");
  puts("\t|ous probabilities (37,56,70,56,37 over 256).  There are 5^5  |");
  puts("\t|possible 5-letter words, and from a string of 256,000 (over- |");
  puts("\t|lapping) 5-letter words, counts are made on the frequencies  |");
  puts("\t|for each word.   The quadratic form in the weak inverse of   |");
  puts("\t|the covariance matrix of the cell counts provides a chisquare|");
  puts("\t|test: Q5-Q4, the difference of the naive Pearson sums of     |");
  puts("\t|(OBS-EXP)^2/EXP on counts for 5- and 4-letter cell counts.   |");
  puts("\t|-------------------------------------------------------------|\n");
  printf("\t\tTest result for the byte stream from %s\n", fn);
 }

 return;
}

/* type "stream" (use small case and don't forget the quotation mark) to test
   a stream of bytes. type "specific" to test specific bytes */
real cnt1s(char *filename, char *test)
{
    int rt=24;
    real p_values[rt+1];
    int p_count = 0;
  counter no_wds=256000; 

  char *s="bits used";

  if(strncmp(test, "stream", 2)==0){
    rt=-1;
    s="\t";
    no_wds=2560000;
  }

  cnt_ttl(filename, test);
      
  printf("\t  (Degrees of freedom: 5^4-5^3=2500; sample size: %d)\n\n", no_wds);
  printf("\t%s\tchisquare\tz-score\t\tp-value\n", s);

  do{
    p_values[p_count++] = cnt_stat(filename, no_wds, test, rt);
    --rt;  
  }while(rt>=0);

  return (p_count <= 1 ? p_values[0] : KStest(p_values, p_count));
}

/*main()
{

  cnt1s("binc", "stream");
  cnt1s("binc", "specific");

  return;
}*/
