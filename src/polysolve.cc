/**
 * Copyright 2004 Alastair R. Beresford.
 *  Use characteristic polynomial to calculate eigenvalues for
 *  three-by-three matrix.
 *
 * $Header$
 */

#include <polysolve.hh>
#include <cmath>
#include <cassert>
#include <gaussianelimination.hh>

/**
 * Only solves a polynomial if three roots exist
 * Returns 1 if found, 0 if failed
 * Using "Cardano's formula"
 * details at http://mathworld.wolfram.com/CubicFormula.html
 */
static inline int solve_poly3(double *x, double *res) {
  double a2 = x[2]/x[3];
  double a1 = x[1]/x[3];
  double a0 = x[0]/x[3];

  double q = (3*a1 -a2*a2)/9;
  double r = (9*a1*a2-27*a0-2*a2*a2*a2)/54;
  double d = q*q*q+r*r;
  
  if (d>0)
    return 0;

  double theta = acos(r/sqrt(-1*q*q*q));

  res[0] = 2*sqrt(-1*q)*cos(theta/3)-a2/3;
  res[1] = 2*sqrt(-1*q)*cos((theta+2*M_PI)/3)-a2/3;
  res[2] = 2*sqrt(-1*q)*cos((theta+4*M_PI)/3)-a2/3;
  
  return 1;
}

/** 
 * Custom routine for determining eigenvalues and vectors of a 3x3 matrix
 *
 *  Since matrix is less than 5x5, we can use Cardano's formula to determine
 *  the characteristic polynomial of the matrix, the roots of which are the
 *  eigenvalues of the matrix. The eigenvectors can be found by substituting
 *  the eigenvalues back into the matrix, and using Gaussian elimination.
 */
bool eigensolve(double a, double b, double c,
                double d, double e, double f,
                double g, double h, double i,
                double *eigenvects, double* eigenvals) {
  
  double x[4];

  //characteristic polynomial for 3-by-3 matrix
  //maths derived by noting that we want det(A-xI)
  //and then leaving x undefined, so determinant is a 
  //cubic equation in terms of x.
  x[3] = -1;
  x[2] = a+e+i;
  x[1] = c*g + b*d - e*i - f*h - a*(e+i);
  x[0] = e*i*a + f*h*a + f*g*b + c*d*h - b*d*i - c*g*e;

  //solve cubic to determine eigenvalues
  if (!solve_poly3(x,eigenvals)) {
    return 0;
  }

  //for each eigenvalue, find its vector
  for(int j=0; j<3; j++) {
    //feed the eigenvalue back into the matrix (i.e.substitute for x)
    //then use Gaussian elimination to solve for eigenvectors
    //NOTE: since every eigenvector can be scaled by an arbitary
    //      constant, we assume that the x component is == 1
    //      Therefore we only have to solve for y and z components
    double** B;
    double row2[2];
    double row3[2];
    double* pB[2];
    pB[0]=row2;
    pB[1]=row3;
    B = pB;

    //we need to assume one component (x,y,z) == 1
    //initially,so we assume x==1, however, if x is zero, we get
    //y == z == nan, so we need to iterate through the other
    //two cases (namely, that y==1, then z==1). This will terminate,
    //since an eigenvector cannot be (0,0,0).
    int v=0;
    double R[2];
    do {
      //this would look a lot nicer, if we got passed double** instead
      //of individual elements of the array! Should re-define function
      //declaration in main code and then alter this code here.
      double X[2];
      switch (v) { 
      case 0:
	B[0][0] = e-eigenvals[j];
	B[0][1] = f;
	B[1][0] = h;
	B[1][1] = i-eigenvals[j];
	X[0] = -1*d; X[1] = -1*g;
	break;
      case 1:
	B[0][0] = g;
	B[0][1] = i-eigenvals[j];
	B[1][0] = a-eigenvals[j];
	B[1][1] = c;
	X[0] = -1*h; X[1] = -1*b;
	break;
      case 2:
	B[0][0] = a-eigenvals[j];
	B[0][1] = b;
	B[1][0] = d;
	B[1][1] = e-eigenvals[j];
	X[0] = -1*c; X[1] = -1*f;
	break;
      default:
	assert(0); //should never get here!
      }

      solve_simultaneous(X,B,R,2);

      eigenvects[3*j+v] = 1; //i.e. component v is assumed to be 1
      eigenvects[3*j+(v+1)%3] = R[0];
      eigenvects[3*j+(v+2)%3] = R[1];

      v++;
    } while(isnan(R[0]) ||  isnan(R[1]));
  }

  return 1;
}

void eigensolve(double a, double b, double f,
		/*    */  double c, double d,
		/*               */ double e,
		double* eigenvects,
		double* eigenvals) {
  assert(eigensolve(a,b,f,b,c,d,f,d,e,eigenvects,eigenvals));
}

/* 
int main(void) {

  double A[3][3] = {{0,2,-1},{1,1,0},{-1,0,1}}; 

  double a = A[0][0];
  double b = A[0][1];
  double c = A[0][2];
  double d = A[1][0];
  double e = A[1][1];
  double f = A[1][2];
  double g = A[2][0];
  double h = A[2][1];
  double i = A[2][2];

  double eigenvals[3] = {-42,-42,-42};
  double eigenvects[9] = {-42,-42, -42, -42, -42, -42, -42, -42, -42};

  assert(eigensolve(a,b,c,d,e,f,g,h,i,eigenvects,eigenvals));
  //eigensolve_sym(a,b,c,e,f,i,eigenvects,eigenvals);
  for (int i=0;i<3;i++)
    printf("Solution %d: %lf, (%lf,%lf,%lf)\n",i,eigenvals[i],
	   eigenvects[3*i],eigenvects[3*i+1],eigenvects[3*i+2]);

  return 0;
}
*/
