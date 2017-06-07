
/*THE OVERLAPPING 20-tuples TEST  BITSTREAM, 20 BITS PER WORD, N words
 If n=2^22, should be 19205.3 missing 20-letter words, sigma 167.
 If n=2^21, should be 141909 missing 20-letter words, sigma   428.
 If n=2^20, should be 385750 missing 20-letter words, sigma   512 */

#include "header.h"

real bitst(char *filename)
{
  const counter nb_pw=20, power=21, no_obs=20;
  const counter no_bits=pow(2,power-5), no_wds=pow(2,power);
  const counter dim=pow(2,nb_pw-5);
  register counter mask=pow(2,nb_pw)-1; 
  const real mean=pow(2,nb_pw)*exp(-pow(2,(power-nb_pw))), std=428;

  register uniform w20, w32;
  register counter i, j, k, l, u, no_mswds=0; 
  counter bitmask[32], *wds;
  real z;

  real p_values[no_obs];
  
  for(i=0; i<32; ++i){
    bitmask[i]=pow(2,i);
  }

  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|                  THE BITSTREAM TEST                         |");
  puts("\t|The file under test is viewed as a stream of bits. Call them |");
  puts("\t|b1,b2,... .  Consider an alphabet with two \"letters\", 0 and 1|");
  puts("\t|and think of the stream of bits as a succession of 20-letter |");
  puts("\t|\"words\", overlapping.  Thus the first word is b1b2...b20, the|");
  puts("\t|second is b2b3...b21, and so on.  The bitstream test counts  |");
  puts("\t|the number of missing 20-letter (20-bit) words in a string of|");
  puts("\t|2^21 overlapping 20-letter words.  There are 2^20 possible 20|");
  puts("\t|letter words.  For a truly random string of 2^21+19 bits, the|");
  puts("\t|number of missing words j should be (very close to) normally |");
  puts("\t|distributed with mean 141,909 and sigma 428.  Thus           |");
  puts("\t| (j-141909)/428 should be a standard normal variate (z score)|");
  puts("\t|that leads to a uniform [0,1) p value.  The test is repeated |");
  puts("\t|twenty times.                                                |");
  puts("\t|-------------------------------------------------------------|\n");

  printf("\t\tTHE OVERLAPPING 20-TUPLES BITSTREAM  TEST for %s\n", filename);
  printf("\t (%d bits/word, %d words", nb_pw, no_wds);
  printf(" %d bitstreams.", no_obs );
  printf(" No. missing words \n\t  should average %.2f", mean);
  printf(" with sigma=%.2f.)\n", std);
  puts("\t----------------------------------------------------------------");
  printf("\t\t   BITSTREAM test results for %s.\n\n", filename);
  printf("\tBitstream\tNo. missing words\tz-score\t\tp-value\n");

  wds=(counter*)malloc(dim*sizeof(counter));

  w20 = uni(filename);

    /*main loop*/               
  for(i=1; i<=no_obs; ++i){
    for(j=0; j<dim; ++j) wds[j]=0;

    for(j=0; j<no_bits; ++j){
      w32=uni(filename);  
      
      for(k=1; k<=32; ++k){
        w20 <<=1;
        w20 += (w32 & 1);
        w20 &= mask;
        l = w20 & 31;
        u = w20 >> 5; 
        wds[u] |= bitmask[l];
        w32 >>=1;
      }
    }
  
    /*count no. of empty cells (=no. missing words)*/
    no_mswds=0;
    for(j=0; j<dim; ++j){
      for(k=0; k<32; ++k){
        if( (wds[j] & bitmask[k]) == 0 ) ++no_mswds;
       }
    }

    z=(no_mswds-mean)/std;
    printf("\t   %d\t\t%d \t\t\t% .2f\t\t%f\n", i, no_mswds, z, 1-Phi(z));
    p_values[i-1] = 1-Phi(z);
  }

  uni("close");
  free(wds);
  puts("\t----------------------------------------------------------------");

  return KStest(p_values, no_obs);
}

/*main()
{
  bitst("binc");

  return;
}*/
