/**
 * Eigenvector solving routines.
 *
 * Copyright Chris Cain <cbc20@cam.ac.uk> 
 *
 * $Header$
 */
#include <cmath>

#include <eigenvv.hh>
#include <opencv/cv.h>
#undef EIGEN_DEBUG

#ifdef EIGEN_DEBUG
#include <iostream>
#endif

void eigensolve(double a, double b, double f,
		/*    */  double c, double d,
		/*               */ double e,
		double* eigenvects,
		double* eigenvals) {

  double m[] = { a, b, f,
		 b, c, d,
		 f, d, e };
  CvMat M;
  cvInitMatHeader(&M,3,3,CV_64F,m);

  double vects[9];
  double vals[3];
  CvMat Vects;
  CvMat Vals;
  cvInitMatHeader(&Vects,3,3,CV_64F,vects);
  cvInitMatHeader(&Vals,3,1,CV_64F,vals);
  
  cvEigenVV(&M,&Vects,&Vals,0.0000000000000000001);

  eigenvals[0] = vals[0];   eigenvals[1] = 0;        eigenvals[2] = 0;
  eigenvals[3] = 0;         eigenvals[4] = vals[1];  eigenvals[5] = 0;
  eigenvals[6] = 0;         eigenvals[7] = 0;        eigenvals[8] = vals[2];

  eigenvects[0] = vects[0];   eigenvects[1] = vects[3];   eigenvects[2] = vects[6]; 
  eigenvects[3] = vects[1];   eigenvects[4] = vects[4];   eigenvects[5] = vects[7]; 
  eigenvects[6] = vects[2];   eigenvects[7] = vects[5];   eigenvects[8] = vects[8]; 

  /*

  
  // Matrix is:

  // a b f
  // b c d
  // f d e

  double t,x,y,z,w,q11,q12,q13,q21,q22,q23,q31,q32,q33,shift;
  int count,cnt,iters,max;

#ifdef EIGEN_DEBUG
  std::cout << "Original Matrix m = [" << std::endl;
  std::cout << " "<<a<<" "<<b<<" "<< f << ";" << std::endl;
  std::cout << " "<<b<<" "<<c<<" "<< d << ";" << std::endl;
  std::cout << " "<<f<<" "<<d<<" "<< e << "]" << std::endl;
#endif

  // Reduce to tridiagonal form
  
  t = sqrt(b*b+f*f);
  z = b/t;
  w = f/t;
  
  b =  t;
  t = -c*w+d*z; // t stores bottom d
  c =  c*z+d*w;
  f =  d*z+e*w; // f stores right hand d
  e = -d*w+e*z;
  
  c =  c*z+f*w;
  d =  t*z+e*w;
  e = -t*w+e*z;
  
#ifdef EIGEN_DEBUG
  std::cout << "First e = "<< e << std::endl;
#endif  

  q11 = 1.0; q12 = 0.0; q13 = 0.0;
  q21 = 0.0; q22 = z;   q23 = -w;
  q31 = 0.0; q32 = w;   q33 = z;      
  
  iters = 0;
  
  while((fabs(d)>fabs(c)*1e-9)&&(iters<100))
    {
      shift = e;
      a -= shift;
      c -= shift;
      e -= shift;
      
      t = sqrt(a*a+b*b);
      y = b/t;
      x = a/t;
      
      a = t;
      
      t = b;
      b =  t*x+c*y;
      c = -t*y+c*x;
      
      // Delay adjusting d
      
      t = sqrt(c*c+d*d);
      z = c/t;
      w = d/t;
      
      c = t;
      
      // Now adjust d
      
      t = d*x;
      
      d =  z*t+w*e;
      e = -w*t+z*e;
      
      // Now do QR step
      
      a = a*x+b*y;
      b = c*y;
      c = c*x;
      
      c = c*z+d*w;
      d = e*w;
      e = e*z;
      
      a += shift;
      c += shift;
      e += shift;
      
      // Update matrix of eigenvectors
      
      t   =  q11;
      q11 =  x*t+y*q12;
      q12 = -y*t+x*q12;
      
      t   =  q21;
      q21 =  x*t+y*q22;
      q22 = -y*t+x*q22;
      
      t   =  q31;
      q31 =  x*t+y*q32;
      q32 = -y*t+x*q32;
      
      t   =  q12;
      q12 =  z*t+w*q13;
      q13 = -w*t+z*q13;
      
      t   =  q22;
      q22 =  z*t+w*q23;
      q23 = -w*t+z*q23;
      
      t   =  q32;
      q32 =  z*t+w*q33;
      q33 = -w*t+z*q33;
      
#ifdef EIGEN_DEBUG
      std::cout << "m = [" << std::endl;
      std::cout << " "<<a<<" "<<b<<" "<< 0.0 << ";" << std::endl;
      std::cout << " "<<b<<" "<<c<<" "<< d << ";" << std::endl;
      std::cout << " "<<0.0<<" "<<d<<" "<< e << "]" << std::endl;
#endif
      count++;
      iters++;
      
    }
      
  if(iters==100)
    {
#ifdef EIGEN_DEBUG
      std::cout << "FAIL! m = [" << std::endl;
      std::cout << " "<<a<<" "<<b<<" "<< 0.0 << ";" << std::endl;
      std::cout << " "<<b<<" "<<c<<" "<< d << ";" << std::endl;
      std::cout << " "<<0.0<<" "<<d<<" "<< e << "]" << std::endl;
#endif
    }	  
  else
    {
      // Find rotation for last two evals
      
      z = sqrt((a-c)*(a-c)/4+b*b);
      w = sqrt(1-(b/z)*(b/z));  // z>=b, so not bad sqrt
      x = sqrt((1+w)/2);        // w<=1, so x<=1
      y = sqrt((1-w)/2);        // w<=1, so not bad sqrt
      if ((a-c)*b < 0) y=-y;

      // Eigenvalues finally

      t = (a+c)/2;
      a = t + z;
      c = t - z;

      // Update eigenvectors

      t   =  q11;
      q11 =  x*t+y*q12;
      q12 = -y*t+x*q12;

      t   =  q21;
      q21 =  x*t+y*q22;
      q22 = -y*t+x*q22;

      t   =  q31;
      q31 =  x*t+y*q32;
      q32 = -y*t+x*q32;

#ifdef EIGEN_DEBUG
      std::cout << "Det = "<< (a*c*e) << std::endl;
      std::cout << "Trace =  "<< (a+c+e)  << std::endl;
      std::cout << "Iterations = "<< iters << std::endl;

      std::cout << "Evals " << a << " " << c << " " << e << std::endl;
      std::cout << "Evecs " << q11 << " " << q12 << " " << q13 << std::endl;
      std::cout << "      " << q21 << " " << q22 << " " << q23 << std::endl;
      std::cout << "      " << q31 << " " << q32 << " " << q33 << std::endl;

#endif
      eigenvects[0] = q11;  eigenvects[1] = q12;  eigenvects[2] = q13;
      eigenvects[3] = q21;  eigenvects[4] = q22;  eigenvects[5] = q23;
      eigenvects[6] = q31;  eigenvects[7] = q32;  eigenvects[8] = q33;

      eigenvals[0] = a;     eigenvals[1] = 0;     eigenvals[2] = 0;
      eigenvals[3] = 0;     eigenvals[4] = c;     eigenvals[5] = 0;
      eigenvals[6] = 0;     eigenvals[7] = 0;     eigenvals[8] = e;
      }*/
}  
void eigensolve(double a, double b, double c,
		double d, double e, double f,
		double g, double h, double i,
		double *eigenvects, double* eigenvals) {
  // Matrix is:

  // a b c
  // d e f
  // g h i

  double t,x,y,z,w,eta,v1,v2,v3,w2,w3;
  double q11,q12,q13,q21,q22,q23,q31,q32,q33;
  int count,cnt,iters,max;
  v1 =0;
  v2=0;
  v3 =0;
  count=0;

#ifdef EIGEN_DEBUG
  std::cout << "Original Matrix m = " << std::endl;
  std::cout << "["<<a<<" "<<b<<" "<< c << ";" << std::endl;
  std::cout << " "<<d<<" "<<e<<" "<< f << ";" << std::endl;
  std::cout << " "<<g<<" "<<h<<" "<< h << "]" << std::endl;
#endif

  // Reduce to Hessenberg form
  
  t = sqrt(d*d+g*g);
  if(t>0)
    {
      z = d/t;
      w = g/t;
    }
  else
    {
      z = 1;
      w = 0;
    }
  
  // Premultiply
  
  d = t;
  g = 0;

  t = e;
  e =  z*t+w*h;
  h = -w*t+z*h;

  t = f;
  f =  z*t+w*i;
  i = -w*t+z*i;

  // Postmultiply

  t = b;
  b =  t*z+c*w;
  c = -t*w+c*z;

  t = e;
  e =  t*z+f*w;
  f = -t*w+f*z;

  t = h;
  h =  t*z+i*w;
  i = -t*w+i*z;

  // Adjust Q

  q11 = 1.0; q12 = 0.0; q13 = 0.0;
  q21 = 0.0; q22 = z;   q23 = -w;
  q31 = 0.0; q32 = w;   q33 = z;      
  
  iters = 0;
  
  while((fabs(d)>1e-12*(fabs(a)+fabs(e)))
	&&(fabs(h)>1e-12*(fabs(e)+fabs(i))))
    {
      // Calulate reflection

      t = e*i-f*h; 
      v1 = a*(a-e-i)+(t+b*d);
      v2 = d*(a-i);
      v3 = d*h;

      //v2 = a-i;
      //v1 = ((a-e)*v2-f*h)/(b+d);
      //v3 = h;

      //      eta = fabs(v1);
      //      if (fabs(v2)>eta) eta = fabs(v2);
      //      if (fabs(v3)>eta) eta = fabs(v3);
      
      //      v1 /= eta;
      //      v2 /= eta;
      //      v3 /= eta;

      t = sqrt(v1*v1+v2*v2+v3*v3);
      if(v1<0) t = -t;
      
      v1 += t;

      //w1 = 1;
      w2 = v2/v1;
      w3 = v3/v1;

      v1 /= t;
      v2 /= t;
      v3 /= t;
      
      //t *= eta;
      
      // Premultiply

      t = a + w2*d + w3*g;
      a -= v1*t;
      d -= v2*t;
      g -= v3*t;
      
      t = b + w2*e + w3*h;
      b -= v1*t;
      e -= v2*t;
      h -= v3*t;
            
      t = c + w2*f + w3*i;
      c -= v1*t;
      f -= v2*t;
      i -= v3*t;

      // Postmultiply

      t = a + b*w2 + c*w3;
      a -= v1*t;
      b -= v2*t;
      c -= v3*t;
      
      t = d + e*w2 + f*w3;
      d -= v1*t;
      e -= v2*t;
      f -= v3*t;
      
      t = g + h*w2 + i*w3;
      g -= v1*t;
      h -= v2*t;
      i -= v3*t;
            
      // Postmultiply eigenvectors

      t = q11 + w2*q12 + w3*q13;
      q11 -= v1*t;
      q12 -= v2*t;
      q13 -= v3*t;
      
      t = q21 + w2*q22 + w3*q23;
      q21 -= v1*t;
      q22 -= v2*t;
      q23 -= v3*t;
            
      t = q31 + w2*q32 + w3*q33;
      q31 -= v1*t;
      q32 -= v2*t;
      q33 -= v3*t;

      // Reduce to Hessenberg form
      
      t = sqrt(d*d+g*g);
      if(t>0)
	{
	  z = d/t;
	  w = g/t;
	}
      else
	{
	  z = 1;
	  w = 0;
	}
      
      // Premultiply
      
      d = t;
      g = 0;
      
      t = e;
      e =  z*t+w*h;
      h = -w*t+z*h;
      
      t = f;
      f =  z*t+w*i;
      i = -w*t+z*i;
      
      // Postmultiply
      
      t = b;
      b =  t*z+c*w;
      c = -t*w+c*z;
      
      t = e;
      e =  t*z+f*w;
      f = -t*w+f*z;
      
      t = h;
      h =  t*z+i*w;
      i = -t*w+i*z;

      // Postmultiply Eigenvectors by transpose

      t = q12;
      q12 =  z*t+w*q13;
      q13 = -w*t+z*q13;
      
      t = q22;
      q22 =  z*t+w*q23;
      q23 = -w*t+z*q23;
      
      t = q32;
      q32 =  z*t+w*q33;
      q33 = -w*t+z*q33;
      
      //      printf("\n%+25.18g %+25.18g %+25.18g\n%+25.18g %+25.18g %+25.18g\n%+25.18g %+25.18g %+25.18g\n",a,b,c,d,e,f,g,h,i);

      //      printf("\nTrace %+25.18g\nDet   %+25.18g\n",a+e+i,a*(e*i-f*h)-b*(d*i-f*g)+c*(d*h-e*g));
      //      printf("\n-------------------------------------------------------------------------------\n");
      count++;
      iters++;
      
    }
  
  if(iters==100)
    {
#ifdef EIGEN_DEBUG
      std::cout << "Failed to reduce matrix = " << std::endl;
      std::cout << "["<<a<<" "<<b<<" "<< c << ";" << std::endl;
      std::cout << " "<<d<<" "<<e<<" "<< f << ";" << std::endl;
      std::cout << " "<<g<<" "<<h<<" "<< i << "]" << std::endl;
#endif
    }	  
  else
    {
#ifdef EIGEN_DEBUG
      std::cout << "Reduced Matrix = " << std::endl;
      std::cout << "["<<a<<" "<<b<<" "<< c << ";" << std::endl;
      std::cout << " "<<d<<" "<<e<<" "<< f << ";" << std::endl;
      std::cout << " "<<g<<" "<<h<<" "<< i << "]" << std::endl;
#endif
      double m11,m12,m13,m21,m22,m23,m31,m32,m33;
      // Calculate eigenvectors for reduced matrix

      if (fabs(h)<=1e-12*(fabs(e)+fabs(i)))
	{
	  t = (a+e)*(a+e)-4*(a*e-b*d);

	  if(t>=0)
	    {
	      t = sqrt(t);
	      v1 = (a+e+t)/2;
	      v2 = (a+e-t)/2;
	      v3 = i;

	      m11 = -b;
	      m21 = a-v1;
	      m31 = 0;

	      m12 = -b;
	      m22 = a-v2;
	      m32 = 0;

	      m13 = b*f-c*(e-v3);
	      m23 = c*d-f*(a-v3);
	      m33 = (a-v3)*(e-v3)-b*d;
	    }
	}
      else if(fabs(d)<=1e-12*(fabs(a)+fabs(e)))
	{
	  t = (e+i)*(e+i)-4*(e*i-f*h);

	  if(t>=0)
	    {
	      t  = sqrt(t);
	      v1 = a;
	      v2 = (e+i+t)/2;
	      v3 = (e+i-t)/2;

	      m11 = 1;
	      m21 = 0;
	      m31 = 0;

	      m12 = b*f-c*(e-v2);
	      m22 = -f*(a-v2);
	      m32 = (e-v2)*(a-v2);

	      m13 = b*f-c*(e-v3);
	      m23 = -f*(a-v3);
	      m33 = (e-v3)*(a-v3);
	    }
	}
      else {
	m11 =0;
	m21 =0;
	m31 =0;
	m12 =0;
	m22 =0;
	m32 =0;
	m13 =0;
	m23 =0;
	m33 =0;
      }

      if (t<0) {
#ifdef EIGEN_DEBUG      
	std::cout << "Complex evals, ignore the following" << std::endl;
#endif
	return;
      }

      // Calculate eigenvectors for original matrix

      x = q11*m11+q12*m21+q13*m31;
      y = q11*m12+q12*m22+q13*m32;
      z = q11*m13+q12*m23+q13*m33;
      q11 = x;
      q12 = y;
      q13 = z;

      x = q21*m11+q22*m21+q23*m31;
      y = q21*m12+q22*m22+q23*m32;
      z = q21*m13+q22*m23+q23*m33;
      q21 = x;
      q22 = y;
      q23 = z;

      x = q31*m11+q32*m21+q33*m31;
      y = q31*m12+q32*m22+q33*m32;
      z = q31*m13+q32*m23+q33*m33;
      q31 = x;
      q32 = y;
      q33 = z;

      t = sqrt(q11*q11+q21*q21+q31*q31);
      q11 /= t;
      q21 /= t;
      q31 /= t;

      t = sqrt(q12*q12+q22*q22+q32*q32);
      q12 /= t;
      q22 /= t;
      q32 /= t;

      t = sqrt(q13*q13+q23*q23+q33*q33);
      q13 /= t;
      q23 /= t;
      q33 /= t;
      
#ifdef EIGEN_DEBUG
      std::cout << "Evals = "<<v1 << " " << v2 << " " << v3 << std::endl;
      std::cout << "Det = " << (v1*v2*v3) << std::endl;
      std::cout << "Trace = " << (v1+v2+v3) << std::endl;
      std::cout << "Iterations = " << iters << std::endl;
      std::cout << "Evecs of reduced matrix:" << std::endl;
      std::cout << "["<<m11<<" "<<m12<<" "<<m13<<" "<<";"<<std::endl;
      std::cout << " "<<m21<<" "<<m22<<" "<<m23<<" "<<";"<<std::endl;
      std::cout << " "<<m31<<" "<<m32<<" "<<m33<<"]"<<std::endl;

      std::cout << "Evecs of original matrix:" << std::endl;
      std::cout << "["<<q11<<" "<<q12<<" "<<q13<<";"<<std::endl;
      std::cout << " "<<q21<<" "<<q22<<" "<<q23<<";"<<std::endl;
      std::cout << " "<<q31<<" "<<q32<<" "<<q33<<"]"<<std::endl;
#endif 

      eigenvects[0] = q11;   eigenvects[1] = q12;   eigenvects[2] = q13;
      eigenvects[3] = q21;   eigenvects[4] = q22;   eigenvects[5] = q23;
      eigenvects[6] = q31;   eigenvects[7] = q32;   eigenvects[8] = q33;

      eigenvals[0] = v1;     eigenvals[1] = 0;     eigenvals[2] = 0;
      eigenvals[3] = 0;      eigenvals[4] = v2;    eigenvals[5] = 0;
      eigenvals[6] = 0;      eigenvals[7] = 0;     eigenvals[8] = v3;
    }   
}



