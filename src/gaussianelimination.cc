/*
 *  $Header$
 */

#include <Config.hh>
#include <gaussianelimination.hh>
#include <iostream>
#undef GAUSSIAN_DEBUG

/**
 * Pick the most well conditioned co-efficient from the matrix.
 *
 * Scan along the given row in the square matrix of given size
 * starting from the given start column looking for the best
 * co-efficient to pivot on.  This is either the largest value or
 * (preferentially) 1.
 *
 * Returns the column number (from zero) of the best co-efficient.
 */
static inline int find_best_coefficient(double** vals, int size, int row, int startcolumn) {
  double max = fabs(vals[startcolumn][row]);
  int maxcol = startcolumn;
  if (max == 1) { 
    return startcolumn;
  }
  for(int i=startcolumn+1;i<size;i++) {
    if (vals[i][row] == 1) {
      return i;
    }
    
    if (fabs(vals[i][row]) > max) {
      max = fabs(vals[i][row]);
      maxcol = i;
    }    
  }

#ifdef GAUSSIAN_DEBUG
  if (max==0) {
    std::cout << "Singular matrix!" << std::endl;
  }
#endif

  return maxcol;
}

/**
 * Swap the columns c1 and c2.
 */
static inline void swap_columns(double** vals, int c1, int c2) {
  double* p = vals[c1];
  vals[c1] = vals[c2];
  vals[c2] = p;
}

/**
 * Swap the rows r1 and r2.
 */
static inline void swap_rows(double** vals, int r1, int r2,int size) {
  for(int i=0;i<size;i++) {
    double t = vals[i][r1];
    vals[i][r1] = vals[i][r2];
    vals[i][r2] = t;
  }
}

/**
 * Scale the selected row (indexed from 0) in the given matrix.
 *
 * Multiply each element in the selected row by the given factor
 * starting from column startcolumn and continuing until column index = size.
 */
static inline void scale_row(double** vals, int size, int row, double factor, int startcolumn) {
  for(int i=startcolumn;i<size;i++) {
    vals[i][row] *= factor;
  }
}

/**
 * Subtract row r2 from row r1 and store the result in row r1.
 *
 * Subtract factor times each element in row r2 from row r1 and store
 * the result in r1.  Start from column startcolumn and continue until
 * column index=size.  i.e. r1 -= factor*r2
 */
static inline void subtract_row(double** vals, int size, int r1, int r2, double factor, int startcolumn) {
  for(int i=startcolumn;i<size;i++) {
    vals[i][r1] -= factor*vals[i][r2];
  }
}

/**
 * Print out the column major matrix.
 */
static void print(double** vals, int rows,int cols) {
  std::cout << std::endl << "[";
  for(int i=0;i<rows;i++) {
    for(int j=0;j<cols;j++) {
      std::cout << vals[j][i] << "\t";
    }
    std::cout << ";" << std::endl;
  }
  std::cout << std::endl;
}


void solve_simultaneous(double* X, double** A, double* R, int size) {

  // create alphas which we will store the result in.  When we swap
  // columns we also need to swap the corresponding values in R -
  // we'll do this by swapping the pointers in alphas around instead
  // so that the caller gets R back in the right order
  double* alphas[size];
  for(int i=0;i<size;i++) {
    alphas[i] = R+i;
  }

  for(int i=0;i<size;i++) {
#ifdef GAUSSIAN_DEBUG
    std::cout << "-------" << std::endl;
    std::cout << "Row " << i << std::endl;
    print(A,size,size);
    std::cout << std::endl;
    for(int k=0;k<size;k++) {
      std::cout << X[k]<<";"<<std::endl;
    }
#endif

    // find the column index of the best pivot.
    int bestcolumn = find_best_coefficient(A,size,i,i);
#ifdef GAUSSIAN_DEBUG
    std::cout << "Best Column is " << bestcolumn << std::endl;
#endif

    // swap the columns and the pointers accordingly
    swap_columns(A,i,bestcolumn);
    swap_columns(alphas,i,bestcolumn);
    
    // scale row so that it has a one on the leading diagonal
    double bestcoeff = A[i][i];
#ifdef GAUSSIAN_DEBUG
    std::cout << "Best Coeff is " << bestcoeff << std::endl;
    std::cout << "Scale row "<<i<< " by "<<(1/bestcoeff)<< std::endl;
#endif
    if (bestcoeff != 1) {
      scale_row(A,size,i,1/bestcoeff,i); // divide row i by bestcoeff
      X[i]/=bestcoeff;
    }

    // subtract some scale factor of this row from the remaining ones
    // to zero out the lower triangle of the matrix
    for(int j=i+1;j<size;j++) {
#ifdef GAUSSIAN_DEBUG
      std::cout << "Subtract "<<A[i][j]<<" times row " << i << " from row "<<j<<std::endl;
#endif
      // order is important here - subtract row will alter A[i][j]
      X[j]-= A[i][j]*X[i]; // perform the same subtraction on the X co-effs
      subtract_row(A,size,j,i,A[i][j],i); // this does rowj -= A[i][j]*rowi starting from col i
    }
  }

  // now A is an upper triangular matrix (zeros in lower triangle)
  // with leading diagonal of 1. So back substitute for alphas
  //
  // ( 1 a b c ) ( r1 )   ( x1 )
  // ( 0 1 d e ) ( r2 ) = ( x2 )
  // ( 0 0 1 f ) ( r3 )   ( x3 )
  // ( 0 0 0 1 ) ( r4 )   ( x4 )
  //
  // r4 = x4
  // r3+f*r4 = x3
  // r2+d*r3+e*r4 = x2
  // r1+a*r2+b*r3+c*r4 = x4

  for(int i=size-1;i>=0;i--) {
    *alphas[i] = X[i];
    for(int j=i+1;j<size;j++) {
      *alphas[i] -= A[j][i] * *alphas[j];
    }
  }

  // now our result is stored in R !
  // and...we're done
  
}


void invert_matrix(double** A, double** B, int size) {
  for(int i=0;i<size;i++) {
    for(int j=0;j<size;j++) {
      B[i][j] = i==j?1:0;
    }
  }
  predivide(A,B,size,size);

}


void predivide(double** A, double** B, int size, int cols) {
  for(int i=0;i<size;i++) {
#ifdef GAUSSIAN_DEBUG
    std::cout << "-------" << std::endl;
    std::cout << "Row " << i << std::endl;
    print(A,size,size);
    std::cout << std::endl;
    print(B,size,cols);
#endif

    // find the column index of the best pivot.
    int bestcolumn = find_best_coefficient(A,size,i,i);
#ifdef GAUSSIAN_DEBUG
    std::cout << "Best Column is " << bestcolumn << std::endl;
#endif

    // swap the columns and the pointers accordingly
    //swap_columns(A,i,bestcolumn);

    //    swap_rows(B,i,bestcolumn,cols);
    

    // scale row so that it has a one on the leading diagonal
    double bestcoeff = A[i][i];
#ifdef GAUSSIAN_DEBUG
    std::cout << "-------" << std::endl;
    std::cout << "After swapping: " << i << std::endl;
    print(A,size,size);
    std::cout << std::endl;
    print(B,size,cols);
    std::cout << "Best Coeff is " << bestcoeff << std::endl;
    std::cout << "Scale row "<<i<< " by "<<(1/bestcoeff)<< std::endl;
#endif
    if (bestcoeff != 1) {
      scale_row(A,size,i,1/bestcoeff,i); // divide row i by bestcoeff
      scale_row(B,cols,i,1/bestcoeff,0); // divide row i in B by bestcoeff - do the whole row
    }

#ifdef GAUSSIAN_DEBUG
    std::cout << "After scaling" << std::endl;
    print(A,size,size);
    std::cout << std::endl;
    print(B,size,cols);
#endif

    // subtract some scale factor of this row from the remaining ones
    // to zero out the lower triangle of the matrix
    for(int j=0;j<size;j++) {
      if (j != i) {
#ifdef GAUSSIAN_DEBUG
	std::cout << "Subtract "<<A[i][j]<<" times row " << i << " from row "<<j<<std::endl;
#endif
	// the order of these is important - we overwrite A[i][j] in the second one.
	subtract_row(B,cols,j,i,A[i][j],0); // this does rowj -= A[i][j]*rowi - do the whole row

	// subract_row is call by value and so, even though we
	// overwrite A[i][j] as the first thing we do in the function
	// the rest of it works.
	subtract_row(A,size,j,i,A[i][j],i); // this does rowj -= A[i][j]*rowi starting from col i
      }
    }
  }

#ifdef GAUSSIAN_DEBUG
  std::cout << "--------RESULT--------" << std::endl;
  print(A,size,size);
  print(B,size,cols);
#endif
  // now A is an identity matrix and B contains inv(A)*B.
}
