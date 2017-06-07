#include "header.h"
#include "macro.h"

/*count the number of runs and compute the statistics*/
/*Algorithm AS 157 Appl. Statist. (1981) vol. 30, No. 1*/
void udruns(real *x, int length, real *ustat, real *dstat )
{
  register short ru=0, rd=0;
  register counter i;
  counter j, *ucnt, *dcnt;

  register real up;
  real a[6][6]={ {4529.4, 9044.9, 13568., 18091., 22615., 27892.},
                 {9044.9, 18097., 27139., 36187., 45234., 55789.}, 
                 {13568., 27139., 40721., 54281., 67852., 83685.},
                 {18091., 36187., 54281., 72414., 90470., 111580.},
                 {22615., 45234., 67852., 90470., 113262., 139476.},
                 {27892., 55789.,  83685.,111580., 139476., 172860.} };

  real b[]={1./6, 5./24, 11./120, 19./720, 29./5040, 1./840};
 
  if( length < 4000 ){
    puts("Length of the sequence is too short (< 4000)!!!");
    exit(0);
  }

  ucnt=(counter*)calloc(6, sizeof(counter) );
  dcnt=(counter*)calloc(6, sizeof(counter) );

/*   The loop determines the number of runs-up and runs-down of 
     length i for i = 1(1)5 and the number of runs-up and runs-down
     of length greater than or equal to 6. */

  for(i=1; i<length; ++i){ 
    up=x[i]-x[i-1];

/* this does not happen actually. it is included for logic reason */
    if(up==0){
      if( x[i]<=.5 ) up=-1;
      else up=1;
    }

    if( up>0 ){
      ++dcnt[rd];
      rd=0;
      ru=MIN(ru+1, 5);
      continue; 
    }

    if( up<0 ){
      ++ucnt[ru];
      ru=0;
      rd=MIN(rd+1, 5);
      continue;
    }
  }
    
  ++ucnt[ru];
  ++dcnt[rd];

/*calculate the test-stat*/
  *ustat = 0;
  *dstat = 0;
  for(i=0; i<6; ++i){   
    for(j=0; j<6; ++j){   
      *ustat+=( ucnt[i]-length*b[i] ) * ( ucnt[j]-length*b[j] ) * a[i][j];
      *dstat+=( dcnt[i]-length*b[i] ) * ( dcnt[j]-length*b[j] ) * a[i][j];
    }        
  }

  *ustat /= length;
  *dstat /= length;

/*  cfree(ucnt, 6, sizeof(counter));  
  cfree(dcnt, 6, sizeof(counter));
*/
 
  return;
}

real runtest(char *filename)
{
  const counter no_sets=2, no_seqs=10, length=10000;
  const p_count = no_sets*2;
  
  int i, j, k;
  real *x, ustat, dstat, *pu, *pd, pv;

  real p_values[p_count];
  real final_p_value;
  
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|    This is the RUNS test.  It counts runs up, and runs down,|");
  puts("\t|in a sequence of uniform [0,1) variables, obtained by float- |");
  puts("\t|ing the 32-bit integers in the specified file. This example  |");
  puts("\t|shows how runs are counted: .123,.357,.789,.425,.224,.416,.95|");
  puts("\t|contains an up-run of length 3, a down-run of length 2 and an|");
  puts("\t|up-run of (at least) 2, depending on the next values.  The   |");
  puts("\t|covariance matrices for the runs-up and runs-down are well   |");
  puts("\t|known, leading to chisquare tests for quadratic forms in the |");
  puts("\t|weak inverses of the covariance matrices.  Runs are counted  |");
  puts("\t|for sequences of length 10,000.  This is done ten times. Then|");
  puts("\t|another three sets of ten.                                   |");
  puts("\t|-------------------------------------------------------------|\n");
    
  printf("\t\t\tThe RUNS test for file %s\n", filename);
  puts("\t\t(Up and down runs in a sequence of 10000 numbers)");

  x=(real*)malloc(length*sizeof(real));
  pu=(real*)malloc(no_seqs*sizeof(real));
  pd=(real*)malloc(no_seqs*sizeof(real));

  for(i=1; i<=no_sets; ++i){
    for(j=0; j<no_seqs; ++j){
      for(k=0; k<length; ++k){
        x[k]=uni(filename)/UNIMAX; 
      }
       
      udruns(x, length, &ustat, &dstat);
      pu[j]=Chisq(6, ustat);
      pd[j]=Chisq(6, dstat);
    }

    pv=KStest(pu,no_seqs);
    printf("\t\t\t\tSet %d\n", i);
    printf("\t\t runs up; ks test for %d p's: %f\n", no_seqs, pv);
    p_values[(i-1)*2+0] = pv;
    pv=KStest(pd,no_seqs);
    printf("\t\t runs down; ks test for %d p's: %f\n", no_seqs, pv);
    p_values[(i-1)*2+1] = pv;
  }
  uni("close");

  final_p_value = KStest(p_values, p_count);
  
  free(x); free(pu); free(pd);

  return final_p_value;
}

/*main()
{
  runtest("binc");

  return;
}*/
