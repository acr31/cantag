/**
 * Provides gaussian elimination functionality for square matrices
 *
 *  $Header$
 *
 *  $Log$
 *  Revision 1.5  2004/02/13 21:47:40  acr31
 *  work on ellipse fitting
 *
 *  Revision 1.4  2004/02/11 08:23:49  acr31
 *  *** empty log message ***
 *
 *  Revision 1.3  2004/02/10 22:22:03  acr31
 *  added inverting a matrix
 *
 *  Revision 1.2  2004/02/10 22:19:27  acr31
 *  got gaussian elimination working for inverting matrices
 *
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
 * containing _columns_ of the result.
 *
 */
void invert_matrix(double** A, double** B, int size) {
  for(int i=0;i<size;i++) {
    for(int j=0;j<size;j++) {
      B[i][j] = i==j?1:0;
    }
  }
  predivide(A,B,size,size);

}

/**
 * Takes a matrix A and a matrix B and returns inv(A)*B stored in B.
 *
 * A and B be in column major format (array of arrays of columns) and
 * The parameter size is the size of the square matrix A, matrix B has
 * the same number of rows as A and a number of columns given by cols.
 * This function performs a gaussian elimination on A to find x in
 * inv(A)*B = X by solving A*X(i)=B(i) for each column in B
 * simultaneously.  This function destroys the contents of A and B.
 */
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
