#include <Config.hh>
#include <GaussianElimination.hh>
#include <GaussianMatrix.hh>

#undef FILENAME
#define FILENAME "GaussianElimination.cc"

#undef GAUSSIAN_DEBUG

void GaussianElimination(float* X, float** A, float* R, int size) {
  float* alphas[size];
  for(int i=0;i<size;i++) {
    alphas[i] = R+i;
  }

  GaussianMatrix m(A,size);

  for(int i=0;i<size;i++) {
#ifdef GAUSSIAN_DEBUG
    std::cout << "-------" << std::endl;
    std::cout << "Row " << i << std::endl;
    m.Print();
    std::cout << std::endl;
    for(int k=0;k<size;k++) {
      std::cout << X[k]<<";"<<std::endl;
    }
#endif

    int bestcolumn = m.FindBestCoefficient(i,i);
#ifdef GAUSSIAN_DEBUG
    std::cout << "Best Column is " << bestcolumn << std::endl;
#endif

    m.SwapColumns(i,bestcolumn);
    // swap the pointers to the final results accordingly
    float* t = alphas[i];
    alphas[i] = alphas[bestcolumn];
    alphas[bestcolumn] = t;
    
    // scale row so that it has a one on the leading diagonal
    float bestcoeff = A[i][i];
#ifdef GAUSSIAN_DEBUG
    std::cout << "Best Coeff is " << bestcoeff << std::endl;
    std::cout << "Scale row "<<i<< " by "<<(1/bestcoeff)<< std::endl;
#endif
    m.ScaleRow(i,1/bestcoeff,i);
    X[i]/=bestcoeff;

    // subtract some scale factor of this row from the remaining ones
    // to zero out the lower triangle of the matrix
    for(int j=i+1;j<size;j++) {
#ifdef GAUSSIAN_DEBUG
      std::cout << "Subtract "<<A[i][j]<<" times row " << i << " from row "<<j<<std::endl;
#endif
      X[j]-= A[i][j]*X[i]; // perform the same subtraction on the X co-effs
      m.SubtractRow(i,j,A[i][j],i);

    }
  }

  // now A is an upper triangular matrix (zeros in lower triangle)
  // with leading diagonal of 1
  for(int i=size-1;i>=0;i--) {
    *alphas[i] = X[i];
    for(int j=i+1;j<size;j++) {
      *alphas[i] -= A[j][i] * *alphas[j];
    }
  }

  // now our result is stored in R !

//  for(int i=0;i<size;i++) {
  //    std::cout << *alphas[i] << std::endl;
  //  }

  // and...we're done
  
}
