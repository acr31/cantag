/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/*
 *  $Header$
 */

#include <cantag/Config.hh>
#include <cantag/gaussianelimination.hh>
#include <cantag/SpeedMath.hh>

#ifdef TEXT_DEBUG
# include <iostream>
# undef GAUSSIAN_DEBUG
#endif

namespace Cantag {
  /**
   * Pick the most well conditioned co-efficient from the matrix.
   *
   * Scan along the given column in the square matrix of given size
   * starting from the given start row looking for the best
   * co-efficient to pivot on.  This is either the largest value or
   * (preferentially) 1.
   *
   * Returns the column number (from zero) of the best co-efficient.
   */
  static inline int find_best_coefficient(double** vals, int size, int column, int startrow) {
    double max = abs(vals[startrow][column]);
    int maxrow = startrow;
    if (abs(max-1.0) <= dbl_epsilon()) { 
      return maxrow;
    }
    for(int i=startrow+1;i<size;i++) {
      if (abs(vals[i][column]-1.0) <= dbl_epsilon()) {
	return i;
      }
    
      if (abs(vals[i][column]) > max) {
	max = abs(vals[i][column]);
	maxrow = i;
      }    
    }

#ifdef GAUSSIAN_DEBUG
    if (abs(max) <= dbl_epsilon())) {
      std::cout << "Singular matrix!" << std::endl;
    }
#endif

    return maxrow;
  }

  /**
   * Swap the rows r1 and r2.
   */
  static inline void swap_rows(double** vals, int r1, int r2) {
    double* p = vals[r1];
    vals[r1] = vals[r2];
    vals[r2] = p;
  }

  /**
   * Swap the rows c1 and c2.
   */
// UNUSED
//  static inline void swap_columns(double** vals, int c1, int c2,int size) {
//    for(int i=0;i<size;i++) {
//      double t = vals[i][c1];
//      vals[i][c1] = vals[i][c2];
//      vals[i][c2] = t;
//    }
//  }

  /**
   * Scale the selected row (indexed from 0) in the given matrix.
   *
   * Multiply each element in the selected row by the given factor
   * starting from column startcolumn and continuing until column index = size.
   */
  static inline void scale_row(double** vals, int size, int row, double factor, int startcolumn) {
    for(int i=startcolumn;i<size;i++) {
      vals[row][i] *= factor;
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
      vals[r1][i] -= factor*vals[r2][i];
    }
  }

#ifdef GAUSSIAN_DEBUG
  /**
   * Print out the row major matrix.
   */
  static void print(double** vals, int rows,int cols) {
    std::cout << std::endl << "[";
    for(int i=0;i<rows;i++) {
      for(int j=0;j<cols;j++) {
	std::cout << vals[i][j] << "\t";
      }
      std::cout << ";" << std::endl;
    }
    std::cout << std::endl;
  }
#endif

  void solve_simultaneous(double* X, double** A, double* R, int size) {

    for(int i=0;i<size;i++) {
#ifdef GAUSSIAN_DEBUG
      std::cout << "-------" << std::endl;
      std::cout << "Column " << i << std::endl;
      print(A,size,size);
      std::cout << std::endl;
      for(int k=0;k<size;k++) {
	std::cout << X[k]<<";"<<std::endl;
      }
#endif

      // find the row index of the best pivot.
      int bestrow = find_best_coefficient(A,size,i,i);
#ifdef GAUSSIAN_DEBUG
      std::cout << "Best Row is " << bestrow << std::endl;
#endif

      // swap the rows and the pointers accordingly
      swap_rows(A,i,bestrow);
      double t = X[i];
      X[i] = X[bestrow];
      X[bestrow] = t;

      // scale row so that it has a one on the leading diagonal
      double bestcoeff = A[i][i];
#ifdef GAUSSIAN_DEBUG
      std::cout << "Best Coeff is " << bestcoeff << std::endl;
      std::cout << "Scale row "<<i<< " by "<<(1/bestcoeff)<< std::endl;
#endif
      if (abs(bestcoeff-1.0) > dbl_epsilon()) {
	scale_row(A,size,i,1/bestcoeff,i); // divide row i by bestcoeff
	X[i]/=bestcoeff;
      }

      // subtract some scale factor of this row from the remaining ones
      // to zero out the lower triangle of the matrix
      for(int j=i+1;j<size;j++) {
#ifdef GAUSSIAN_DEBUG
	std::cout << "Subtract "<<A[j][i]<<" times row " << i << " from row "<<j<<std::endl;
#endif
	// order is important here - subtract row will alter A[j][i]
	X[j]-= A[j][i]*X[i]; // perform the same subtraction on the X co-effs
	subtract_row(A,size,j,i,A[j][i],i); // this does rowj -= A[i][j]*rowi starting from col i
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
      R[i] = X[i];
      for(int j=i+1;j<size;j++) {
	R[i] -= A[i][j] * R[j];
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
      int bestrow = find_best_coefficient(A,size,i,i);
#ifdef GAUSSIAN_DEBUG
      std::cout << "Best Row is " << bestrow << std::endl;
#endif

      // swap the columns and the pointers accordingly
      swap_rows(A,i,bestrow);
      swap_rows(B,i,bestrow);
      //    swap_columns(B,i,bestrow,cols);
    
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
      if (abs(bestcoeff-1.0) > dbl_epsilon()) {
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
	  std::cout << "Subtract "<<A[j][i]<<" times row " << i << " from row "<<j<<std::endl;
#endif
	  // the order of these is important - we overwrite A[i][j] in the second one.
	  subtract_row(B,cols,j,i,A[j][i],0); // this does rowj -= A[i][j]*rowi - do the whole row

	  // subract_row is call by value and so, even though we
	  // overwrite A[i][j] as the first thing we do in the function
	  // the rest of it works.
	  subtract_row(A,size,j,i,A[j][i],i); // this does rowj -= A[i][j]*rowi starting from col i
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
}
