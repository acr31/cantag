#include <cstdio>

#include <moreau-basis.hh>
extern int eigen(
           int     vec,           /* switch for computing evectors ...*/
           int     ortho,         /* orthogonal Hessenberg reduction? */
           int     ev_norm,       /* normalize Eigenvectors? .........*/
           int     n,             /* size of matrix ..................*/
           REAL ** mat,           /* input matrix ....................*/
           REAL ** eivec,         /* Eigenvectors ....................*/
           REAL  * valre,         /* real parts of eigenvalues .......*/
           REAL  * valim,         /* imaginary parts of eigenvalues ..*/
           int   * cnt            /* Iteration counter ...............*/
	   );

int main(int argc, char* argv[]) {

  REAL** A = new REAL*[3];
  REAL** eigvec = new REAL*[3];
  REAL* valre = new REAL[3];
  REAL* valim = new REAL[3];
  for(int i=0;i<3;++i) {
    A[i] = new REAL[3];
    eigvec[i] = new REAL[3];
  }
        
      
 
  A[0][0] = 0.70207100000000;   A[0][1] = 0.00072535500000;   A[0][2] = 0.00059334000000; 
  A[1][0] = 0.00072535500000;   A[1][1] = 0.71210600000000;   A[1][2] = 0.00058756500000; 
  A[2][0] = 0.00059334000000;   A[2][1] = 0.00058756500000;   A[2][2] = -0.00105470000000; 
  
  int* cnt = new int[3];

  int res = eigen(1,1,1,3,A,eigvec,valre,valim,cnt);

  printf("Result = %d\n",res);
  for(int i=0;i<3;++i) {
    for(int j=0;j<3;++j) {
      printf("%f ",eigvec[i][j]);
    }
    printf("\n");
  }

  for(int i=0;i<3;++i) {
    printf("%f + %f\n",valre[i],valim[i]);
  }
    
}
