/**
 * Copyright 2004 Alastair R. Beresford.
 *  Use characteristic polynomial to calculate eigenvalues for
 *  three-by-three matrix.
 *
 * $Header$
 */

#ifndef POLYSOLVE_GUARD
#define POLYSOLVE_GUARD

#include <Config.hh>

bool eigensolve(double a, double b, double c,
                double d, double e, double f,
                double g, double h, double i,
		double *eigenvects, double* eigenvals);
  
void eigensolve(double a, double b, double f,
		/*    */  double c, double d,
		/*               */ double e,
		double* eigenvects,
		double* eigenvals);

#endif//POLYSOLVE_GUARD
