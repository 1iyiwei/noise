/*******Test ranks of 100,000 6x8 binary matrices**************
 *******Each row a byte from a RNG, overlapping rows*************/

#include "header.h"
#include "macro.h"

/* define a binary matrix */
typedef struct binmatrix {

  uniform *row;
  int no_row, no_col;
  unsigned long mask;

} binmatrix;

/*print the title*/
void rnk_ttl(char *fn, char *test)
{
 if( test[0]=='6' ){
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|This is the BINARY RANK TEST for 6x8 matrices.  From each of |");
  puts("\t|six random 32-bit integers from the generator under test, a  |");
  puts("\t|specified byte is chosen, and the resulting six bytes form a |");
  puts("\t|6x8 binary matrix whose rank is determined.  That rank can be|");
  puts("\t|from 0 to 6, but ranks 0,1,2,3 are rare; their counts are    |");
  puts("\t|pooled with those for rank 4. Ranks are found for 100,000    |");
  puts("\t|random matrices, and a chi-square test is performed on       |");
  puts("\t|counts for ranks 6,5 and (0,...,4) (pooled together).        |");
  puts("\t|-------------------------------------------------------------|\n");
 }

 if( test[1]=='1' ){
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|This is the BINARY RANK TEST for 31x31 matrices. The leftmost|");
  puts("\t|31 bits of 31 random integers from the test sequence are used|");
  puts("\t|to form a 31x31 binary matrix over the field {0,1}. The rank |");
  puts("\t|is determined. That rank can be from 0 to 31, but ranks< 28  |");
  puts("\t|are rare, and their counts are pooled with those for rank 28.|");
  puts("\t|Ranks are found for 40,000 such random matrices and a chisqu-|");
  puts("\t|are test is performed on counts for ranks 31,30,28 and <=28. |");
  puts("\t|-------------------------------------------------------------|");
 }

 if( test[1]=='2' ){
  puts("\n\t|-------------------------------------------------------------|");
  puts("\t|This is the BINARY RANK TEST for 32x32 matrices. A random 32x|");
  puts("\t|32 binary matrix is formed, each row a 32-bit random integer.|");
  puts("\t|The rank is determined. That rank can be from 0 to 32, ranks |");
  puts("\t|less than 29 are rare, and their counts are pooled with those|");
  puts("\t|for rank 29.  Ranks are found for 40,000 such random matrices|");
  puts("\t|and a chisquare test is performed on counts for ranks  32,31,|");
  puts("\t|30 and <=29.                                                 |");
  puts("\t|-------------------------------------------------------------|");
 }

  printf("\t\tRank test for binary matrices (%s) from %s\n", test, fn);

  return;
}

/*compute the rank of a binary matrix*/
int rkbm(binmatrix bm)
{
  register int i, j, k, rt=0;
  int rank=0;
  uniform tmp;

  for(k=0; k<bm.no_row; ++k){
    i=k;

    while( ( (bm.row[i]>>rt)&1 ) == 0 ){
      ++i;

      if( i<bm.no_row ){
        continue;
      }
      else {
        ++rt;
        if( rt<bm.no_col ){
          i=k;
          continue;
        }
      }

      return rank;
    }

    ++rank;
    if( i!=k ){
      tmp=bm.row[i];
      bm.row[i]=bm.row[k];
      bm.row[k]=tmp;
    }

    for(j=i+1; j<bm.no_row; ++j){
      if( ( (bm.row[j] >> rt) & 1 ) == 0 ) continue;
      else  bm.row[j] ^= bm.row[k]; 
    }

    ++rt;
  }

  return rank;  
} 

/* perform the test and calculate test-stat */
real rnk_stat(char *fn, counter no_mtr, char *test, int rt)
{
  real p6[]={.009443, 0.217439, 0.773118};
  real  p30[]={.0052854502, .1283502644, .5775761902, .2887880952};

  binmatrix bm;
  register counter i, j;
  register int cls, llim;
  char cat[2][4]={ {"r<="}, {"r="} }; 
  counter *f;
 
  short df=3;
  real pvalue, *p;
  real Ef, chsq=0, tmp;

  switch(test[1]){
    case '1': bm.no_row=bm.no_col=31;bm.mask=pow(2,31)-1;llim=28;p=p30;break;
    case '2': bm.no_row=bm.no_col=32;bm.mask=pow(2,32)-1;llim=29; p=p30; break;
    default: {
       df=2;
       bm.no_row=6;
       bm.no_col=8; 
       bm.mask=pow(2,8)-1;
       llim=4;
       p=p6;
       printf("\n\t\t\t      bits %2d to %2d\n", 25-rt, 32-rt);
       break;
    }
  }

  bm.row=(uniform*)malloc(bm.no_row*sizeof(uniform));

  f=(counter*)calloc( (df+1), sizeof(counter));

  puts("\n\tRANK\tOBSERVED\tEXPECTED\t(O-E)^2/E\tSUM\n");

  for( i=1; i<=no_mtr; ++i ){

   /* get the rows of a matrix */
    for(j=0; j<bm.no_row; ++j){
      switch(test[1]){ 
        case '1': bm.row[j]=( uni(fn)>>1 ); break;
        case '2': bm.row[j]=uni(fn); break;
        default: bm.row[j]=( uni(fn)>>rt ); break;
      }
      bm.row[j] &= bm.mask;
    } 
    
    cls=rkbm(bm);
    cls=MAX(llim,cls)-llim;
    ++f[cls];
  }

 /* compute chi-square */
  for(i=0; i<=df; ++i){
    Ef=no_mtr* ( *(p+i) );
    tmp=(f[i]-Ef)*(f[i]-Ef)/Ef;
    chsq+=tmp;
    printf("\t%s%d\t%-12d\t%-12.1f", cat[MIN(1,i)], i+llim, f[i], Ef);
    printf("\t%-12.3f\t%-12.3f\n", tmp, chsq);
  }
  uni("close");

/*  cfree(f, (df+1), sizeof(counter));*/
  free(bm.row);

  pvalue=1-Chisq( df, chsq );
  printf("\n\t\tchi-square = %.3f with df = %d;", chsq, df);
  printf("  p-value = %.3f\n", pvalue);

  printf("\t--------------------------------------------------------------\n"); 

  return pvalue; 
}

/* type "6x8", "31x31" or "32x32" (including the quotation mark) to call 
   each test.*/
real binrnk(char *filename, char *test)
{
  counter no_matrices=100000;
  short rt=24, not6x8=strncmp(test, "6 x 8", 1);
  real *p;
  int p_count = 0;
  real final_p_value;
  
  if( not6x8 ){
    rt=0;
    no_matrices=40000;
  } 
  
  rnk_ttl(filename, test);

  p=(real*)malloc( (rt+1)*sizeof(real) );

  do{
    p[p_count++]=rnk_stat(filename, no_matrices, test, rt);
    --rt; 
  }while(rt>=0);

  if( not6x8 ) return p[0];

  puts("\t    TEST SUMMARY, 25 tests on 100,000 random 6x8 matrices");
  puts("\t    These should be 25 uniform [0,1] random variates:");

  for(rt=24; rt>=0; --rt){
    if( (rt+1)%5==0 ) puts(" ");
    printf("\t%-12.6f", p[rt]);
  }
    
  printf("\n\t\tThe KS test for those 25 supposed UNI's yields\n");
  printf("\t\t\tKS p-value = %.6f\n", KStest(p,25));   

  final_p_value = KStest(p,25);
  free(p);
 
  return final_p_value;
}
    
/*main()
{
  binrnk("binc", "6 x 8");
  binrnk("binc", "32 x 32");

  return;
}*/
