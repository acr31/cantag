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

/**
 * Copyright 2004 Alastair R. Beresford.
 *  Use characteristic polynomial to calculate eigenvalues for
 *  three-by-three matrix.
 *
 * $Header$
 */

#ifndef POLYSOLVE_GUARD
#define POLYSOLVE_GUARD

#include <cantag/Config.hh>

namespace Cantag {

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
  bool eigensolve(double a, double b, double f,
		  /*    */  double c, double d,
		  /*               */ double e,
		  double* eigenvects,
		  double* eigenvals);
}
#endif//POLYSOLVE_GUARD
