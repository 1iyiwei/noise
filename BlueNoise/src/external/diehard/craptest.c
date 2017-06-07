#include "header.h"
#include "macro.h"

real craptest(char *filename)
{
  const counter no_games=200000;

  register counter  i, win, no_win=0, no_throw;
  register int out_1st, out_nxt; 
  counter f[21]; 
  real Ef[21], sum, mean, std, t, pvalue_w, pvalue_th;

  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|This the CRAPS TEST.  It plays 200,000 games of craps, counts|");
  puts("\t|the number of wins and the number of throws necessary to end |");
  puts("\t|each game.  The number of wins should be (very close to) a   |");
  puts("\t|normal with mean 200000p and variance 200000p(1-p), and      |");
  puts("\t|p=244/495.  Throws necessary to complete the game can vary   |");
  puts("\t|from 1 to infinity, but counts for all>21 are lumped with 21.|");
  puts("\t|A chi-square test is made on the no.-of-throws cell counts.  |");
  puts("\t|Each 32-bit integer from the test file provides the value for|");
  puts("\t|the throw of a die, by floating to [0,1), multiplying by 6   |");
  puts("\t|and taking 1 plus the integer part of the result.            |");
  puts("\t|-------------------------------------------------------------|\n");

   /*compute the expected frequencies*/
  sum=Ef[0]=1/3.;
  for(i=1; i<20; ++i){
    f[i]=0;
    Ef[i]=(27*pow(27/36.,i-1)+40*pow(26/36.,i-1)+55*pow(25/36.,i-1))/648;
    sum+=Ef[i];
  }
  Ef[20]=1.-sum;
  f[0]=f[20]=0;

     /*playing the games*/
#define DIE   1 + (int)( 6* (uni(filename)/UNIMAX) )
#define GAME_OVER  {++f[MIN(20,no_throw)]; no_win+=win;}

  for(i=1; i<=no_games; ++i){
    out_1st=DIE+DIE;
    no_throw=0;

    if(out_1st==7 || out_1st==11){ 
      win=1;
      GAME_OVER 
      continue;
    }
      
    if( out_1st==2 || out_1st==3 || out_1st==12){ 
      win=0;
      GAME_OVER
      continue;
    }

    for( ; ; ){
      out_nxt=DIE+DIE;
      ++no_throw;
      if(out_nxt==7){
        win=0;
        GAME_OVER
        break;
      }
   
      if(out_nxt==out_1st){
        win=1;
        GAME_OVER
        break;
      }
    }
  }
  uni("close");

  mean=244*no_games/495.;
  std=sqrt(mean*251/495.);
  t=(no_win-mean)/std;

  printf("\t\tRESULTS OF CRAPS TEST FOR %s \n", filename);
  printf("\tNo. of wins:  Observed\tExpected\n");
  printf("\t                 %d        %f\n", no_win, mean);

  pvalue_w=1-Phi(t);
  printf("\t\tz-score=%6.3f, pvalue=%7.5f\n", t, pvalue_w);

  printf("\n\tAnalysis of Throws-per-Game:\n");

  printf("\n\tThrows\tObserved\tExpected\tChisq\t Sum of (O-E)^2/E\n");


 /* compute chi-square */
  sum=0;
  for(i=0; i<21; ++i){
    mean=no_games*Ef[i];
    t=(f[i]-mean)*(f[i]-mean)/mean;
    sum+=t;     
    printf("\t%d\t%d\t\t%.1f",i+1, f[i], mean);
    printf("\t\t%.3f\t\t%.3f\n", t, sum);
  }

  pvalue_th=1-Chisq(20,sum);
  printf("\n\tChisq=%7.2f for 20 degrees of freedom, ", sum);
  printf("p=%8.5f\n", pvalue_th);

  printf("\n\t\tSUMMARY of craptest on %s\n", filename);
  printf("\t p-value for no. of wins: %.6f\n", pvalue_w);
  printf("\t p-value for throws/game: %.6f\n", pvalue_th);
  printf("\t_____________________________________________________________\n\n");

  return pvalue_w;
}      

/*main()
{
  char ch;

  craptest("binc");

  return;
}*/
