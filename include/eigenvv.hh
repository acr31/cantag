/**
 * Eigenvector solving routines.
 *
 * Copyright Chris Cain <cbc20@cam.ac.uk> 
 *
 * $Header$
 *
 */

#ifndef EIGENVV_GUARD
#define EIGENVV_GUARD

#include <Config.hh>


/**
 * Find the eigenvalues and eigenvectors of the symmetric 3x3 matrix.
 * The matrix is of the format [a b c; b d e; c e f].  The resulting
 * eigenvector and eigenvalue arrays must contain 9 elements, they
 * will be filled in row major order.
 *
 * \todo regression test.  Generate a diagonal matrix and some eigen
 * vectors. multiply them together and eigen solve.
 *
 * \todo currently doesn't work - I've gone back to the opencv methods
 */ 
void eigensolve(double a,   double b,   double c,
		/*     b,*/ double d,   double e,
		/*     c,          e,*/ double f,
		double* eigenvects,
		double* eigenvals);
/**
 * Find the eigenvalues and eigenvectors of a non-symmetric 3x3
 * matrix. Using the following
 * method:....
 *
 * \todo regression test. Test with symmetric method and then nonsymmetric
 * 
 * \todo find out how this works
 */
bool eigensolve(double m11, double m12, double m13,
		double m21, double m22, double m23,
		double m31, double m32, double m33,
		double *eigenvecs, double* eigenvals);

#endif//EIGENVV_GUARD
