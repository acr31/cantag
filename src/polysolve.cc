/**
 * Copyright 2004 Alastair R. Beresford.
 *  Use characteristic polynomial to calculate eigenvalues for
 *  three-by-three matrix.
 *
 * $Header$
 */
#include <tripover/polysolve.hh>
#include <cmath>
#include <cassert>
#include <tripover/gaussianelimination.hh>

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

  double q = (a2*a2-3*a1)/9;
  double r = (2*a2*a2*a2-9*a1*a2+27*a0)/54;

  //printf("poly=%lf,%lf,%lf\n",a2,a1,a0);
  //printf("test is %d (r=%e,q=%e)\n", (r*r)>(q*q*q),r,q);
  if ((r*r)>(q*q*q))
    return 0;
  
  double theta = acos(r/sqrt(q*q*q));

  res[0] = -2*sqrt(q)*cos(theta/3)-a2/3;
  res[1] = 0;
  res[2] = 0;
  res[3] = 0;
  res[4] = -2*sqrt(q)*cos((theta+2*M_PI)/3)-a2/3;
  res[5] = 0;
  res[6] = 0;
  res[7] = 0;
  res[8] = -2*sqrt(q)*cos((theta-2*M_PI)/3)-a2/3;

  //printf("res=%lf,%lf,%lf\n",res[0],res[1],res[2]);
  return 1;
}

/** Test the eigen vector/value to see if we are close 
 *  to being correct or not
 */
bool check_eigen(double a, double b, double c,
		 double d, double e, double f,
		 double g, double h, double i,
		 double v1,double v2,double v3,
		 double val) {
  double u1, u2, u3;
  
  //  printf("Test: %lf,%lf,%lf\n",v1,v2,v3);
  if (isnan(v1) || isnan(v2) || isnan(v3))
    return 0;
  
  u1 = a*v1+b*v2+c*v3 - v1*val;
  u2 = d*v1+e*v2+f*v3 - v2*val;
  u3 = g*v1+h*v2+i*v3 - v3*val;
  
  //  printf("   Sol: %lf,%lf,%lf\n",u1,u2,u3);
  //accuracy required for the eigenvector
  double test = 0.1;
  if (fabs(u1)<test && fabs(u2)<test && fabs(u3)<test)
    return 1;
  else
    return 0;
}

/** Check the eigenvectors and values for consistency
 *
 */
void check_eigen_all(double a, double b, double c,
                double d, double e, double f,
                double g, double h, double i,
		 double *vects, double* vals) {
  
  for (int j=0; j<3; j++) {
    double v1,v2,v3;
    v1 = a*vects[j+0]+b*vects[j+3]+c*vects[j+6] - vects[j]*vals[3*j+j];
    v2 = d*vects[j+0]+e*vects[j+3]+f*vects[j+6] - vects[j+3]*vals[3*j+j];
    v3 = g*vects[j+0]+h*vects[j+3]+i*vects[j+6] - vects[j+6]*vals[3*j+j];

    //    printf("Sol:%d, %lf,%lf,%lf\n",j,v1,v2,v3);
  }
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

  double A[3][3] = {{a,b,c},{d,e,f},{g,h,i}};

  //http://mathworld.wolfram.com/CharacteristicPolynomial.html
  //
  //-x*x*x + Tr(A)*x*x +\sum_ij[0.5*(a_ij*a_ij - a_ii*a_jj)(1-d_ij)*x +det(A)]
  //where d_ij is the Kronecker Delta (d_ij=0 for i !=j or d_ij=1 for i==j)
  x[3] = -1;
  x[2] = a+e+i; //Tr(A)
  double total = 0;
  for (int j=0; j<3; j++)
    for (int k=0; k<3; k++)
      if (j != k)
	total += 0.5*(A[j][k]*A[k][j]-A[j][j]*A[k][k]);
  x[1] = total;
  x[0] = (a*e*i+b*f*g+c*d*h)-(a*f*h+b*d*i+c*e*g); //det(A)

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
    double vect[3], val;
    do {
      //this would look a lot nicer, if we got passed double** instead
      //of individual elements of the array! Should re-define function
      //declaration in main code and then alter this code here.
      double X[2];
      switch (v) { 
      case 0:
	B[0][0] = e-eigenvals[j*4];
	B[0][1] = f;
	B[1][0] = h;
	B[1][1] = i-eigenvals[j*4];
	X[0] = -1*d; X[1] = -1*g;
	break;
      case 1:
	B[0][0] = g;
	B[0][1] = i-eigenvals[j*4];
	B[1][0] = a-eigenvals[j*4];
	B[1][1] = c;
	X[0] = -1*h; X[1] = -1*b;
	break;
      case 2:
	B[0][0] = a-eigenvals[j*4];
	B[0][1] = b;
	B[1][0] = d;
	B[1][1] = e-eigenvals[j*4];
	X[0] = -1*c; X[1] = -1*f;
	break;
      default:
	//assert(0); //should never get here!
	return false;
      }

      solve_simultaneous(X,B,R,2);

      vect[v] = eigenvects[j+3*v] = 1; //i.e. component v is assumed to be 1
      vect[(v+1)%3] = eigenvects[j+3*((v+1)%3)] = R[0];
      vect[(v+2)%3] = eigenvects[j+3*((v+2)%3)] = R[1];
      val = eigenvals[j*4];
      v++;
      //      printf("** %d ",v-1);
    } while(!check_eigen(a,b,c,d,e,f,g,h,i,vect[0],vect[1],vect[2],val));
  }

  check_eigen_all(a,b,c,d,e,f,g,h,i,eigenvects,eigenvals);

  // normalize
  double mods[3] = {0};
  for(int j=0;j<3;++j) {
    for(int i=0;i<3;++i) {
      mods[i] += eigenvects[3*j+i] * eigenvects[3*j+i];
    }
  }
  for(int i=0;i<3;++i) {
    mods[i] = sqrt(mods[i]);
  }
  for(int j=0;j<3;++j) {
    for(int i=0;i<3;++i) {
      eigenvects[3*j+i] /= mods[i];
    }
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


/** For non-symmetric matrix, balance the matrix to improve our  
 *  estimate of eigenvalues. Idea is that magnitude of the ith row
 *  should be as close as possible to the magnitude of the ith column
 *  (making power of two adjustments to each does not effect eigenvals)
 *
 *  Note that a symmetric matrix is unnaffected by this routine
 *
 * Note: eigenvalues are not effected by balancing, but eigenvects are
 * At the moment, I'm not sure how to correct eigenvects, so do use yet!
 */
void balance_matrix(double A[3][3]) {
  
  double row,col, scale;
  int adjrow;

  for(int i=0;i<3;i++) {
    for (int j=0;j<3;j++) {
      if (i != j) {
	row += fabs(A[i][j]);
	col += fabs(A[j][i]);
      }
    }  
    //find power of two which most closely adjusts row and col
    //vals so that their magnitudes are equal
    //    printf("i=%d,row=%lf,col=%lf\n",i,row,col);
    if (row != 0 && col != 0) {
      scale = 1;
      adjrow = -1;
      if (row > 2*col) {
	adjrow = 1;
	while (row>2*col) {
	  row /= 2;
	  scale *= 2;
	}
      } 
      else if (col > 2*row) {
	adjrow = 0;
	while (col>2*row) {
	  col /= 2;
	  scale *= 2;
	}
      }
      //      printf("i=%d,adjrow=%d,scale=%lf\n",i,adjrow,scale);
      if (adjrow != -1 && scale != 1.0) { //worth doing adjustment
	for (int j=0; j<3; j++) {
	  if (i != j) {
	    if (adjrow) {
	      A[i][j] /= scale;
	      A[j][i] *= scale;
	    }
	    else {
	      A[i][j] *= scale;
	      A[j][i] /= scale;
	    }
	  }
	}
      }
    }
    //  printf("loop A=\n");
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      //      printf(" %lf ",A[i][j]);
    }
    //    printf("\n");
  }

  }
}

/*
int main(void) {

  //double A[3][3] = {{0,2,-1},{1,1,0},{-1,0,1}}; 
  //double A[3][3] = {{0,8,-1},{0.25,1,0},{-1,0,1}};
  //double A[3][3] = {{0.04986984158961, 0.04240513051812, 0.03726377761915},
  //		    {-0.20456834060045, -0.13168871992875, -0.08480724456028},
  //		    {0.20491659383226, 0.10228744767860, 0.04987361967856}};
  //  double A[3][3] = {{0.0066505931317806243896, 5.587231852114200592 
  //  	     , 6.396405552513897419},
  //  	    {5.587231852114200592, 1.1654572351835668087
  // 	     , 3.9870005915872752666},
  //  	    {6.396405552513897419, 3.9870005915872752666
  //  	     , 2.2472502430900931358}};
  //  double A[3][3] = {{-54745956.051440485,468045.05687609315,58324912.849545352},
  //		    {-937341.55267617106,-96460403.091143966,-936090.11372616887},
  //		    {51473932.982628867,468670.77630506456,-54745956.051475354}};
  double A[3][3] = {{-5.4746e+07,468045,5.83249e+07},
		    {-937342,-9.64604e+07,-936090},
		    {5.14739e+07,468671,-5.4746e+07}}     ;
  //double A[3][3] = {{0.0499, 0.0424, 0.0373},
  //{-0.2046, -0.1317, -0.0848},
  //{0.2049, 0.1023, 0.0499}};

  
  printf("A=\n");
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      printf(" %lf ",A[i][j]);
    }
    printf("\n");
  }
  //balance_matrix(A);
  printf("A=\n");
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      printf(" %lf ",A[i][j]);
    }
    printf("\n");
  }
  

  double a = A[0][0];
  double b = A[0][1];
  double c = A[0][2];
  double d = A[1][0];
  double e = A[1][1];
  double f = A[1][2];
  double g = A[2][0];
  double h = A[2][1];
  double i = A[2][2];

  double eigenvals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  double eigenvects[9] = {-42,-42, -42, -42, -42, -42, -42, -42, -42};

  assert(eigensolve(a,b,c,d,e,f,g,h,i,eigenvects,eigenvals));
  //eigensolve_sym(a,b,c,e,f,i,eigenvects,eigenvals);
  for (int i=0;i<3;i++)
    printf("Solution %d: %lf, (%lf,%lf,%lf)\n",i,eigenvals[3*i+i],
	   eigenvects[i],eigenvects[3+i],eigenvects[6+i]);

  //check output of matlab
  //double vals[3] = {0.0297,-0.0382,-0.0235};
//  double vects[9] = {0.0086, -0.0210,  0.0192, 0.0088, 0.0125, -0.0350, 0.0062, -0.0200, 0.0105};

  //check_eigen(a,b,c,d,e,f,g,h,i,vects,vals);

  return 0;
}
*/
