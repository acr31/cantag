/**
 * Copyright 2004 Alastair R. Beresford.
 *  Use characteristic polynomial to calculate eigenvalues for
 *  three-by-three matrix.
 *
 * $Header$
 */

#ifndef POLYSOLVE_GUARD
#define POLYSOLVE_GUARD

#include <tripover/Config.hh>

bool eigensolve(double a, double b, double c,
                double d, double e, double f,
                double g, double h, double i,
		double *eigenvects, double* eigenvals);
  
/**
 * Find the eigenvalues and eigenvectors of the symmetric 3x3 matrix.
 * The matrix is of the format [a b c; b d e; c e f].  The resulting
 * eigenvector and eigenvalue arrays must contain 9 elements, they
 * will be filled in row major order.
 */
void eigensolve(double a, double b, double f,
		/*    */  double c, double d,
		/*               */ double e,
		double* eigenvects,
		double* eigenvals);

#endif//POLYSOLVE_GUARD
