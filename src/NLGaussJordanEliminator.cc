/*
  Copyright (C) 2004 Robert K. Harle

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

  Email: robert.harle@achilles.org
  Email: rkh23@cantab.net
*/

#include <total/NLGaussJordanEliminator.hh>
#include <total/NLNLMAPExceptions.hh>

#include <cmath>
#include <iostream>

#define SWAP(x,y) {REAL t=x;x=y;y=t;}

using namespace Total;

/**
 * Solve a set of linear equations
 * y=Ax using gauss Jordan elimination
 * Inputs:  matrix A as a
 *          vector y as y
 * Outputs: Inverse A as a
 *          Vector x as y
 */
int GaussJordanEliminator::Eliminate(REAL **a, int na,
				     REAL **y, int ny)
{

  // This allocation means
  // we don't work in place 
  // but it makes it easier
  // to comprehend
  REAL identity[na][na];

  for (int i=0;i<na;i++) {
    for (int j=0;j<na;j++) {
      if (i!=j) identity[i][j]=0.0;
      else identity[i][j]=1.0;
    }
  }
  
  for (int c=0;c<na;c++) {
    
    // Get the current pivot row
    REAL biggest=-1.0;
    int pivot=-1;
   
    for (int i=c; i<na; i++) {
      if (fabs(a[i][c])> biggest) {
	biggest = a[i][c];
	pivot=i;
      }
    }

    if (pivot!=c) {
      // Have to do a row swap
      for (int j=0;j<na;j++) SWAP(a[pivot][j],a[c][j]);
      for (int j=0;j<ny;j++) SWAP(y[pivot][j],y[c][j]);
      for (int j=0;j<na;j++) SWAP(identity[pivot][j],identity[c][j]);
    }

    // Now we divide the pivot row by the pivot at a[c][c]
    REAL mag = a[c][c];
    if (mag==0.0) throw SingularMatrix();
    if (a[c][c]!=0.0 && a[c][c]!=1.0) {
      for (int j=0;j<na;j++) a[c][j] /= mag;
      for (int j=0;j<ny;j++) y[c][j] /= mag;
      for (int j=0;j<na;j++) identity[c][j] /= mag;
    }

    // Now have to make the rest of the rows zero
    for (int i=0; i<na; i++) {
      if (i!=c) {
	REAL factor = a[i][c];
	for (int j=0; j<na; j++) a[i][j] -= a[c][j]*factor;
	for (int j=0; j<ny; j++) y[i][j] -= y[c][j]*factor;
	for (int j=0; j<na; j++) identity[i][j] -= identity[c][j]*factor;
	}
      }
    }

  // Now copy the answer
  for (int i=0; i<na; i++) {
    for (int j=0; j<na; j++) {
      a[i][j]=identity[i][j];
    }
  }

  return 1;
}
