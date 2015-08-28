/*
  Copyright (C) 2004 Alastair R. Beresford

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

  Email: arb33@cam.ac.uk
*/

/**
 *  Use characteristic polynomial to calculate eigenvalues for
 *  three-by-three matrix.
 */

#include <cantag/polysolve.hh>
#include <cantag/SpeedMath.hh>
#include <cassert>
#include <stdio.h>

namespace Cantag {

#ifdef EIGEN_DEBUG
  void print2x2(double m[2][2]) {
    printf("Debug=\n");
    for (int i=0; i<2; i++) {
      for (int j=0; j<2; j++) {
	printf(" %lf ",m[i][j]);
      }
      printf("\n");
    }
  }
#endif

  /** 
   * Solve simultaneous equations with two unknowns
   * Effectively does Gaussian elimination on a 2x2 matrix; i.e. solve:
   * 
   * |a b||x| _ |i|
   * |c d||y| - |j|
   *
   * to find x and y (given a,b,c,d,i,j)
   */
  static inline void solve_simul2(double A[2][2], double x[2], double r[2]) {


#ifdef EIGEN_DEBUG
    print2x2(A);
    printf("R=%lf %lf\n",r[0],r[1]);
#endif
    //find largest co-efficient first column as pivot point
    if (abs(A[0][0]) > abs(A[1][0])) {
      // y = (aj-ci)/(ad-cb)
      x[1] = (A[0][0]*r[1]-A[1][0]*r[0])/(A[0][0]*A[1][1]-A[1][0]*A[0][1]); 
      // x = (i-by)/a
      x[0] = (r[0]-A[0][1]*x[1])/A[0][0];
    }
    else {
      // y = (ci-aj)/(bc-ad)
      x[1] = (A[1][0]*r[0]-A[0][0]*r[1])/(A[1][0]*A[0][1]-A[0][0]*A[1][1]); 
      // x = (j-dy)/c
      x[0] = (r[1]-A[1][1]*x[1])/A[1][0];
    }
#ifdef EIGEN_DEBUG
    printf("Sim solve: %lf,%lf\n",x[0],x[1]);

    double testi = A[0][0]*x[0]+A[0][1]*x[1]-r[0];
    double testj = A[1][0]*x[0]+A[1][1]*x[1]-r[1];

    printf("Test accuracy of sim solve: %lf %lf\n",testi,testj);
#endif
  } 

  /**
   * Solve all real-valued cubic equations
   * x[3] is coefficient of x**3, x[2] for x**2, and so on
   * res stores the result
   * return value is 0 for failure, 1 for success
   */
  static inline bool solve_poly3(double *x, double *res) {

    double a2 = x[2]/x[3];
    double a1 = x[1]/x[3];
    double a0 = x[0]/x[3];

    double q = (a2*a2-3*a1)/9;
    double r = (2*a2*a2*a2-9*a1*a2+27*a0)/54;

    if ((r*r)>(q*q*q)) {
      return 0;
    }
  
    double theta = acos(r/sqrt(q*q*q));

    res[0] = -2.0*sqrt(q)*cos(theta/3.0)-a2/3.0;
    res[1] = -2.0*sqrt(q)*cos((theta+2.0*DBL_PI)/3.0)-a2/3.0;
    res[2] = -2.0*sqrt(q)*cos((theta-2.0*DBL_PI)/3.0)-a2/3.0;

    return 1;
  }

  /** 
   *  Provide a measure of the accuracy of accuracy of an eigenvalue
   *
   * M is the matrix which is from which the eigenvector and value is calculated
   * vect is an array of the eigenvector components
   * val is the eigenvalue
   * the return value is a Euclidian distance measure of the error
   */
  static inline double accuracy_eigen3(double M[3][3], double vect[3], double val) {
  
#ifdef EIGEN_DEBUG
    printf("  Test accuracy of: (%lf,%lf,%lf),%lf\n",vect[0],vect[1],vect[2],val);
#endif
    if (is_nan(vect[0]) || is_nan(vect[1]) || is_nan(vect[2]))
      return -1;

    //multiply out using eigenvector to measure the error
    double u1, u2, u3;
    u1 = M[0][0]*vect[0]+M[0][1]*vect[1]+M[0][2]*vect[2] - vect[0]*val;
    u2 = M[1][0]*vect[0]+M[1][1]*vect[1]+M[1][2]*vect[2] - vect[1]*val;
    u3 = M[2][0]*vect[0]+M[2][1]*vect[1]+M[2][2]*vect[2] - vect[2]*val;
  
#ifdef EIGEN_DEBUG
    printf("  Accuracy is: (%lf,%lf,%lf) -> %e\n",u1,u2,u3,u1*u1+u2*u2+u3*u3);
#endif

    return u1*u1+u2*u2+u3*u3;
  }

  /** 
   * Routine for determining eigenvalues and vectors of a 3x3 matrix
   *
   * Since matrix is less 3x3, we can use Cardano's formula to determine
   * the characteristic polynomial of the matrix, the roots of which are the
   * eigenvalues of the matrix. The eigenvectors can be found by substituting
   * the eigenvalues back into the matrix, and using simple Gaussian elimination.
   *
   * M is the matrix
   * evects is memory area used to store the resulting eigenvectors
   * evals is the memory area used to store the resulting eigenvalues
   * return value indicates whether this function was successful
   */
  static bool eigensolve3(double M[3][3], double evects[3][3], double evals[3]) {

    //characteristic polynomial of m
    double x[4];

    //double A[3][3] = {{a,b,c},{d,e,f},{g,h,i}};

    //http://mathworld.wolfram.com/CharacteristicPolynomial.html
    //
    //-x*x*x + Tr(A)*x*x +\sum_ij[0.5*(a_ij*a_ij - a_ii*a_jj)(1-d_ij)*x +det(A)]
    //where d_ij is the Kronecker Delta (d_ij=0 for i !=j or d_ij=1 for i==j)
    x[3] = -1;
    x[2] = M[0][0]+M[1][1]+M[2][2]; //Tr(A)
    x[1] = 0;
    for (int j=0; j<3; j++) //\sum_ij[0.5*(a_ij*a_ij - a_ii*a_jj)(1-d_ij)*x
      for (int k=0; k<3; k++)
	if (j != k)
	  x[1] += 0.5*(M[j][k]*M[k][j]-M[j][j]*M[k][k]);
    //det(A):
    x[0] =(M[0][0]*M[1][1]*M[2][2]+M[0][1]*M[1][2]*M[2][0]+M[0][2]*M[1][0]*M[2][1])
      -(M[0][0]*M[1][2]*M[2][1]+M[0][1]*M[1][0]*M[2][2]+M[0][2]*M[1][1]*M[2][0]);

#ifdef EIGEN_DEBUG
    printf("Characteristic Poly: %lf,%lf,%lf\n",x[2],x[1],x[0]);
#endif

    //solve cubic to determine eigenvalues
    if (!solve_poly3(x,evals)) return 0;

#ifdef EIGEN_DEBUG
    printf("Evals %lf %lf %lf\n",evals[0],evals[1],evals[2]);
#endif

    //for each eigenvalue, find the corresponding eigenvector
    for(int j=0; j<3; j++) {

      //we need to assume one component x,y or z == 1
      //initially, we assume x==1, however, if x is zero, we get
      //y == z == nan, so we need to iterate through the other
      //two cases (namely, that y==1, then z==1).
      //Here we check all three cases and select the best one

      double vects[3][3]; //three sets of eigenvects
      double vectsacc[3]; //accuracy of each of the eigenvectors
      for(int k=0; k<3; k++) {
	double curvect[2] = {0,0}; //temp storage of current vect

	//possible to optimise these with modulo arithmetic, but would then be
	//be more or less impossible to understand (and probably not any faster!)
	switch(k) {
	case 0: {
	  double B[2][2] = {{M[1][1]-evals[j],M[1][2]},{M[2][1],M[2][2]-evals[j]}};
	  double r[2] = {-1*M[1][0],-1*M[2][0]};
	  solve_simul2(B,curvect,r);
	  break;
	}
	case 1: {
	  double B[2][2] = {{M[0][0]-evals[j],M[0][2]},{M[2][0],M[2][2]-evals[j]}};
	  double r[2] = {-1*M[0][1],-1*M[2][1]};
	  solve_simul2(B,curvect,r);
	  break;
	}
	case 2: {
	  double B[2][2] = {{M[0][0]-evals[j],M[0][1]},{M[1][0],M[1][1]-evals[j]}};
	  double r[2] = {-1*M[0][2],-1*M[1][2]};
	  solve_simul2(B,curvect,r);
	  break;
	}
	}
      
	{
	  double normalise = 0.0;
	  normalise = sqrt(curvect[0]*curvect[0]+curvect[1]*curvect[1]+1);
	  vects[k][k] = 1/normalise; //component assumed to be = 1
	  vects[k][(k+1)%3] = curvect[k%2]/normalise; 
	  vects[k][(k+2)%3] = curvect[(k+1)%2]/normalise;
	  vectsacc[k] = accuracy_eigen3(M,vects[k],evals[j]);
	}
      }

      //find vector with minimal error
      double minval=vectsacc[0];
      int minnum=0;
      for (int i=1;i<3;i++) {
	if (abs(vectsacc[i]+1) <= dbl_epsilon()) { //i.e. vectsacc[i] doesn't represent a nan
	  if (minval > vectsacc[i] || abs(minval+1) <= dbl_epsilon()) { //minval=-1 return if isnan=1
	    minval = vectsacc[i]; 
	    minnum=i;
	  }
	}
      }
  
#ifdef EIGEN_DEBUG
      printf("Selected %d\n\n",minnum);
#endif

      //set evects to this value
      for(int i=0;i<3;i++)
	evects[j][i]=vects[minnum][i];
    }
    return 1;
  }

  /**
   * Helper function to access eigensolve
   */
  bool eigensolve(double a, double b, double c,
		  double d, double e, double f,
		  double g, double h, double i,
		  double *eigenvects, double* eigenvals) {

    double M[3][3] = {{a,b,c},{d,e,f},{g,h,i}};
    double res[3][3];
    bool ret;
    ret = eigensolve3(M,res,eigenvals);
    if (ret) {
      eigenvals[4] = eigenvals[1];
      eigenvals[8] = eigenvals[2];
      eigenvals[1] = eigenvals[2] = eigenvals[3] = eigenvals[5] = eigenvals[6] = eigenvals[7] = 0;
    
      for (int i=0;i<9;i++) {
	eigenvects[i]=res[i%3][(i/3)];
      }
      return 1;
    } else {
      return 0;
    }
  }

  /**
   * Helper function to access eigensolve
   */
  bool eigensolve(double a, double b, double f,
		  /*    */  double c, double d,
		  /*               */ double e,
		  double* eigenvects,
		  double* eigenvals) {
    return eigensolve(a,b,f,b,c,d,f,d,e,eigenvects,eigenvals);
  }
}

/*Start of DEBUG code, uncomment for testing!*/

/*
void print3x3(double m[3][3]) {
  printf("M=\n");
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      printf(" %lf ",m[i][j]);
    }
    printf("\n");
  }
}


int main(void) {
  double A[3][3] = {{-8990.1523663332627620548009872437,0,2299.4290327826952307077590376139},
		    {-0,-21344.000000024156179279088973999,-0},
		    {38682.24858129024505615234375,0,-8990.152366357091523241251707077}};

  print3x3(A);

  double evects[3][3];
  double evals[3];
  
  Cantag::eigensolve3(A, evects, evals);

  print3x3(evects);
  printf("\nEvals: %lf %lf %lf\n",evals[0],evals[1],evals[2]);
}
*/
