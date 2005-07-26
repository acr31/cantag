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
 * $Header$
 */

#include <total/algorithms/FitEllipseLS.hh>
#include <total/gaussianelimination.hh>
#include <total/polysolve.hh>
#include <total/SpeedMath.hh>

#undef ELLIPSE_DEBUG_DUMP_POINTS
#undef ELLIPSE_DEBUG

namespace Total {

  bool FitEllipseLS::operator()(const ContourEntity& contour, ShapeEntity<Ellipse>& shape) const {
    const std::vector<float>& points = contour.GetPoints();

    int numpoints = points.size()/2;
    if (numpoints < 6) {
      return false;
    }

#ifdef ELLIPSE_DEBUG_DUMP_POINTS
    for(int i=0;i<numpoints*2;i+=2) {
      PROGRESS(points[i] << " " << points[i+1] <<";");
    }
#endif
  
    // Compute

    // S1 = D1' * D1
    // S2 = D1' * D2
    // S3 = D2' * D2

    // D1 is [ x.^2 , x.*y, y.^2 ]
    // d1(i,0) = points[2*i]*points[2*i]
    // d1(i,1) = points[2*i]*points[2*i+1]
    // d1(i,2) = points[2*i+1]*points[2*i+1]

    // D2 is [x,y,1]
    // d2(i,0) = points[2*i]
    // d2(i,1) = points[2*i+1]
    // d2(i,2) = 1

    double s1[9] = {0};
    double s2[9] = {0};
    double s30[3] = {0};
    double s31[3] = {0};
    double s32[3] = {0};
    double* s3[] = { s30,s31,s32};  // store s3 in row major format so we can use it for gaussian elimination

    for(std::vector<float>::const_iterator i = points.begin();
	i != points.end();
	++i) {
      float x = *i;
      ++i;
      float y = *i;
      double d1col[] = {x*x, x*y, y*y};
      double d2col[] = {x,y,1};
    
      for(int k=0;k<3;k++) {
	for(int l=0;l<3;l++) {
	  s1[k*3+l] += d1col[k] * d1col[l];
	  s2[k*3+l] += d1col[k] * d2col[l];
	  s3[k][l] += d2col[k] * d2col[l];
	}
      }
    }

#ifdef ELLIPSE_DEBUG
    print("S1",s1,3,3);
    print("S2",s2,3,3);
    print("S3",s3,3,3);
#endif

    // check the determinant of S3 is non-zero - if we have a straight line for example then this is zero
    /*
    
    | a b c | 
    | d e f |  = a(ei-hf) - b(di-fg) + c(dh-eg)
    | g h i | 
    */
    float determinant = 
      s3[0][0]*(s3[1][1]*s3[2][2]-s3[1][2]*s3[2][1]) -
      s3[1][0]*(s3[0][1]*s3[2][2]-s3[2][1]*s3[0][2]) +
      s3[2][0]*(s3[0][1]*s3[1][2]-s3[1][1]*s3[0][2]);
  
#ifdef ELLIPSE_DEBUG
    PROGRESS("Determinant of S3 " << determinant);
#endif    

    if (fabs(determinant) < 1e-20) { 
#ifdef ELLIPSE_DEBUG
      PROGRESS("Determinant of S3 is zero - no fit");
#endif
      return false;
    }

    // Compute

    // T = -inv(S3) * S2'
  
    // this is row major representation => this is -S2 transpose.
    double t0[] = { -s2[0], -s2[3], -s2[6]};
    double t1[] = { -s2[1], -s2[4], -s2[7]};
    double t2[] = { -s2[2], -s2[5], -s2[8]};
    double* t[] = { t0,t1,t2 };
  
    predivide(s3,t,3,3);


#ifdef ELLIPSE_DEBUG
    print("T",t,3,3);
#endif
    // Compute

    // M = S1 + S2 * T
    // leave the result in S1

    for(int i=0;i<3;i++) {
      for(int j=0;j<3;j++) {
	for(int k=0;k<3;k++) {
	  s1[i*3+j] += s2[i*3+k] * t[k][j]; // ith row, kth column of s2 * kth row,jth column of t
	}
      }
    }

#ifdef ELLIPSE_DEBUG
    print("M",s1,3,3);
#endif

    // premultiply M by inv(C1)
    //
    // ( M31/2 M32/2 M33/2 )
    // ( -M21  -M22  -M23  )
    // ( M11/2 M12/2 M13/2 )
    // and find eigenvectors

#ifdef ELLIPSE_DEBUG
    double m2[9];
    m2[0] = s1[6]/2;
    m2[1] = s1[7]/2;
    m2[2] = s1[8]/2;

    m2[3] = -s1[3];
    m2[4] = -s1[4];
    m2[5] = -s1[5];
  
    m2[6] = s1[0]/2;
    m2[7] = s1[1]/2;
    m2[8] = s1[2]/2;
    print("M2",m2,3,3);
#endif

    double eigvals[9];
    double eigvects[9];
    if (!eigensolve(s1[6]/2,s1[7]/2,s1[8]/2,
		    -s1[3],-s1[4],-s1[5],
		    s1[0]/2,s1[1]/2,s1[2]/2,
		    eigvects,
		    eigvals)) {
#ifdef ELLIPSE_DEBUG
      PROGRESS("Failed to solve eigenvectors of the non-symmetric matrix.  Fit failed.");
#endif
      return false;
    }


#ifdef ELLIPSE_DEBUG
    print("Eigvals",eigvals,3,3);
    print("Eigvects",eigvects,3,3);
#endif

    //  for(int i=0;i<9;i++) {
    //  eigvects[i] = -eigvects[i];
    // }

    for(int i = 0; i < 3; i++) {
      if (4*eigvects[i]*eigvects[i+6]-eigvects[i+3]*eigvects[i+3] >= 0.0) {
	
	Ellipse* e = new Ellipse(eigvects[i],
				 eigvects[i+3],
				 eigvects[i+6],
				 t[0][0]*eigvects[i]+t[0][1]*eigvects[i+3]+t[0][2]*eigvects[i+6],
				 t[1][0]*eigvects[i]+t[1][1]*eigvects[i+3]+t[1][2]*eigvects[i+6],
				 t[2][0]*eigvects[i]+t[2][1]*eigvects[i+3]+t[2][2]*eigvects[i+6]);	

#ifdef ELLIPSE_DEBUG
	PROGRESS("Fitted ellipse: a="<< 
		 eigvects[i]   <<","<<
		 eigvects[i+3] <<","<<
		 eigvects[i+6] <<","<<
		 t[0][0]*eigvects[i]+t[0][1]*eigvects[i+3]+t[0][2]*eigvects[i+6] <<","<<
		 t[1][0]*eigvects[i]+t[1][1]*eigvects[i+3]+t[1][2]*eigvects[i+6] <<","<<
		 t[2][0]*eigvects[i]+t[2][1]*eigvects[i+3]+t[2][2]*eigvects[i+6]);	
#endif      

	if (e->CheckError(points)) {
	  shape.m_shapeDetails = e;
	  shape.m_shapeFitted = true;
	  return true;
	}
	else {
	  shape.m_shapeFitted = false;
	  delete e;
	}
      }
    }
    return false;
  }
}
