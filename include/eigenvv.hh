/**
 * Eigenvector solving routines.
 *
 * Copyright Chris Cain <cbc20@cam.ac.uk> 
 *
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/02/13 21:47:36  acr31
 * work on ellipse fitting
 *
 */

#ifndef EIGENVV_GUARD
#define EIGENVV_GUARD

#include <Config.hh>


/**
 * Find the eigenvalus and eigenvectors of the symmetric 3x3 matrix.
 * The matrix is of the format [a b c; b d e; c e f].  The resulting
 * eigenvector and eigenvalue arrays must contain 9 elements, they
 * will be filled in row major order.
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
 */
void eigensolve(double m11, double m12, double m13,
		double m21, double m22, double m23,
		double m31, double m32, double m33,
		double *eigenvecs, double* eigenvals);

#endif//EIGENVV_GUARD
