/**
 * Provides gaussian elimination functionality for square matrices
 *
 *  $Header$
 *
 *  $Log$
 *  Revision 1.1  2004/02/10 18:12:43  acr31
 *  moved gaussian elimination functionality around a bit
 *
 */

#include <Config.hh>
#include <gaussianelimination.hh>

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
static inline int find_best_coefficient(float** vals, int size, int row, int startcolumn) {
  float max = fabs(vals[startcolumn][row]);
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
static inline void swap_columns(float** vals, int c1, int c2) {
  float* p = vals[c1];
  vals[c1] = vals[c2];
  vals[c2] = p;
}

/**
 * Scale the selected row (indexed from 0) in the given matrix.
 *
 * Multiply each element in the selected row by the given factor
 * starting from column startcolumn and continuing until column index = size.
 */
static inline void scale_row(float** vals, int size, int row, float factor, int startcolumn) {
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
static inline void subtract_row(float** vals, int size, int r1, int r2, float factor, int startcolumn) {
  for(int i=startcolumn;i<size;i++) {
    vals[i][r1] -= factor*vals[i][r2];
  }
}

/**
 * Print out the matrix.
 */
static void print(float** vals, int size) {
  std::cout << std::endl << "[";
  for(int i=0;i<size;i++) {
    for(int j=0;j<size;j++) {
      std::cout << vals[j][i] << "\t";
    }
    std::cout << ";" << std::endl;
  }
  std::cout << std::endl;
}

/**
 * Solve the equation A*R = X for R.
 *
 * A is a square matrix stored as an array of pointers to arrays
 * holding the _columns_ of the matrix.  R is an vector of matching
 * height to A which will contain the results of the elimination.  X
 * is a vector of matching height to A that holds the RHS of the
 * equation.  This function works by reducing A to an upper triangular
 * matrix (i.e. the lower triangle is all zeros) by Gaussian
 * Elimination and then back substituting to extract R.  The contents
 * of A, X and R will all be modified by this function.  The cost of
 * this reducing each row is N comparisons (to find the best pivot),
 * two pointer swaps (to swap the pivot into the best position), N+1
 * divisions (to divide through by the pivot), size-N lots of N+1
 * subtractions and multiplications (to zero out the lower corner of
 * the matrix).  This cost is incurred for each row and then an
 * additional N*N/2 subtractions and multiplications to back
 * substitute the final values.
 */
void solve_simultaneous(float* X, float** A, float* R, int size) {

  // create alphas which we will store the result in.  When we swap
  // columns we also need to swap the corresponding values in R -
  // we'll do this by swapping the pointers in alphas around instead
  // so that the caller gets R back in the right order
  float* alphas[size];
  for(int i=0;i<size;i++) {
    alphas[i] = R+i;
  }

  for(int i=0;i<size;i++) {
#ifdef GAUSSIAN_DEBUG
    std::cout << "-------" << std::endl;
    std::cout << "Row " << i << std::endl;
    print(A,size);
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
    float bestcoeff = A[i][i];
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

/**
 * Invert matrix A and store the result in R.
 *
 * This function performs a full gaussian elimination on A to reduce
 * it to an identity matrix.  Simultaneously solves Ar(i)=b(i) for
 * r(i) where b(i) is the ith column of a appropriatly sized identity
 * matrix.  The inverse of A is then the matrix made up with columns
 * r(i). This function overwrites the contents of A and R (the
 * result). The matrix A is expected to be an array of arrays
 * containing _columns_.  The result R is an array of arrays
 * containing _rows_ of the result.
 *
 */
void invert_matrix(float** A, float** R, int size) {
}
