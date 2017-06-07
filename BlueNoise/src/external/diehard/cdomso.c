#include "header.h"

/*print the title*/
void monky_ttl(char *fn, char *test)
{
 switch( test[1] ){
    case 'P':{
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|        OPSO means Overlapping-Pairs-Sparse-Occupancy        |");
  puts("\t|The OPSO test considers 2-letter words from an alphabet of   |");
  puts("\t|1024 letters.  Each letter is determined by a specified ten  |");
  puts("\t|bits from a 32-bit integer in the sequence to be tested. OPSO|");
  puts("\t|generates  2^21 (overlapping) 2-letter words  (from 2^21+1   |");
  puts("\t|\"keystrokes\")  and counts the number of missing words---that |");
  puts("\t|is 2-letter words which do not appear in the entire sequence.|");
  puts("\t|That count should be very close to normally distributed with |");
  puts("\t|mean 141,909, sigma 290. Thus (missingwrds-141909)/290 should|");
  puts("\t|be a standard normal variable. The OPSO test takes 32 bits at|");
  puts("\t|a time from the test file and uses a designated set of ten   |");
  puts("\t|consecutive bits. It then restarts the file for the next de- |");
  puts("\t|signated 10 bits, and so on.                                 |");
  puts("\t|------------------------------------------------------------ |\n");
  break;
    }

    case 'Q':{
  puts("\n\t|------------------------------------------------------------ |");
  puts("\t|    OQSO means Overlapping-Quadruples-Sparse-Occupancy       |");
  puts("\t|  The test OQSO is similar, except that it considers 4-letter|");
  puts("\t|words from an alphabet of 32 letters, each letter determined |");
  puts("\t|by a designated string of 5 consecutive bits from the test   |");
  puts("\t|file, elements of which are assumed 32-bit random integers.  |");
  puts("\t|The mean number of missing words in a sequence of 2^21 four- |");
  puts("\t|letter words,  (2^21+3 \"keystrokes\"), is again 141909, with  |");
  puts("\t|sigma = 295.  The mean is based on theory; sigma comes from  |");
  puts("\t|extensive simulation.                                        |");
  puts("\t|------------------------------------------------------------ |\n");
  break;
    }

    case 'N':{
  puts("\n\t|------------------------------------------------------------ |");    puts("\t|    The DNA test considers an alphabet of 4 letters: C,G,A,T,|");
  puts("\t|determined by two designated bits in the sequence of random  |");
  puts("\t|integers being tested.  It considers 10-letter words, so that|");
  puts("\t|as in OPSO and OQSO, there are 2^20 possible words, and the  |");
  puts("\t|mean number of missing words from a string of 2^21  (over-   |");
  puts("\t|lapping)  10-letter  words (2^21+9 \"keystrokes\") is 141909.  |");
  puts("\t|The standard deviation sigma=339 was determined as for OQSO  |");
  puts("\t|by simulation.  (Sigma for OPSO, 290, is the true value (to  |");
  puts("\t|three places), not determined by simulation.                 |");
  puts("\t|------------------------------------------------------------ |\n");
  break;
    }
 
    default: puts("Wrong specification!!!"); exit(0);
  }

    printf("\t\t\t   %s test for file %s\n\n", test, fn);
    printf("\tBits used\tNo. missing words");
    printf("\tz-score\t\tp-value\n");

  return;
}

/* get a word from specific bits */
unsigned long get_w(char *fn, short bits_pl, int rt)
{
  static int flag=-1, ltrs_pw;
  static unsigned long wd, maskltr;
  short i;

  wd <<= bits_pl;

  if(flag!=rt){
    flag=rt;
    maskltr=pow(2,bits_pl)-1;
    ltrs_pw=20/bits_pl;

    for(i=1; i<ltrs_pw; ++i){
      wd+=( (uni(fn)>>rt) & maskltr ) ;
      wd <<= bits_pl;
    }
  }

  wd+=( (uni(fn)>>rt) & maskltr ) ;

 /* 1048575=2**20-1 */
  return (wd & 1048575); 
}


real monky_stat(char *filename, char *test, int no_tests)
{
  const real bits_pw=20, mean=pow(2, bits_pw)*exp(-2);
  const counter dim=pow(2, bits_pw-5);

  register unsigned long j, u, l, wd;

  unsigned long maskbit[32];
  counter i, k, rt=0, bits_pl, no_wds=pow(2, bits_pw+1);
  counter no_mswds, *wds;

  real std, z;

  real p_values[35];
  int p_count = 0;
  
  switch(test[1]){
    case 'I': rt=33; break;
    case 'P': bits_pl=10; std=290; break;
    case 'Q': bits_pl=5;  std=295; break;
    case 'N': bits_pl=2;  std=339; break; 
    default  : break;
  }

  for(j=0; j<32; ++j){
    maskbit[j]=pow(2,j);
  }

  wds=(counter*)malloc(dim*sizeof(counter));

  do{
    for(i=1; i<=no_tests; ++i){
      for(j=0; j<dim; ++j){
        wds[j]=0;  
      }
     
      for(j=1; j<=no_wds; ++j){
        wd = get_w(filename, bits_pl, rt);
        l = wd & 31;
        u = wd >> 5; 
        wds[u] |= maskbit[l];
      }
  
    /*count no. of empty cells (=no. missing words)*/
      no_mswds=0;
      for(j=0; j<dim; ++j){
        for(k=0; k<32; ++k){
          if( (wds[j] & maskbit[k]) == 0 ) ++no_mswds;
        }
      }

      z=(no_mswds-mean)/std;
      printf("\t%d to %d  \t\t%d ", 33-rt-bits_pl, 32-rt, no_mswds);
      printf("\t\t% .4f\t\t%f\n", z, 1-Phi(z));
      p_values[p_count++] = 1-Phi(z);
    }
 
    uni("close");
    ++rt;
  }while(rt<=32-bits_pl);

  free(wds);

  printf("\t------------------------------");
  printf("-----------------------------------\n");
  
  return KStest(p_values, p_count);
}

/* type "OPSO" for OPSO test and so on */
real monky(char *filename, char *test)
{
  const int no_tests=1;

  counter no_wds;

  monky_ttl(filename, test);

  return monky_stat(filename, test, no_tests);

}


