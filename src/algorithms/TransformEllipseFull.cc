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

#include <cantag/algorithms/TransformEllipseFull.hh>
#include <cantag/polysolve.hh>
#include <cantag/SpeedMath.hh>

namespace Cantag {

  bool TransformEllipseFull::operator()(const ShapeEntity<Ellipse>& source, TransformEntity& dest) const {    
    if (!source.m_shapeFitted) { return false; }
    const Ellipse& ellipse = *source.m_shapeDetails;
    float a = ellipse.GetA();
    float b = ellipse.GetB();
    float c = ellipse.GetC();
    float d = ellipse.GetD();
    float e = ellipse.GetE();
    float f = ellipse.GetF();
  
#ifdef ELLIPSE_TRANSFORM_DEBUG
    PROGRESS("Ellipse params (a-f) are ["<<
	     a << "," <<
	     b << "," <<
	     c << "," <<
	     d << "," <<
	     e << "," <<
	     f << "];");
#endif
    
    double eigvects[9];
    double eigvals[9];

    /**
     * Solve eigenvectors of ( a     b/2    d/2 )
     *                       ( b/2   c      e/2 )
     *                       ( d/2   e/2    f   )
     */
    if (!eigensolve(a,b/2, d/2,
		    c,e/2,
		    f,
		    eigvects, eigvals)) {
#ifdef ELLIPSE_TRANSFORM_DEBUG
      PROGRESS("EIGENFAIL! Failed to find eigenvectors of symmetric matrix");
      PROGRESS(a << " " << b/2 << " " << d/2 << " " << c << " " << e/2 << " " << f);
#endif
      return false;
    }

#ifdef ELLIPSE_TRANSFORM_DEBUG
    PROGRESS("Eigen Vectors: e=[" << eigvects[0] << "," << eigvects[1] << "," << eigvects[2] << ";");
    PROGRESS("                  " << eigvects[3] << "," << eigvects[4] << "," << eigvects[5] << ";");
    PROGRESS("                  " << eigvects[6] << "," << eigvects[7] << "," << eigvects[8] << "];");

    PROGRESS("Eigen Values: v=[" << eigvals[0] << "," << eigvals[1] << "," << eigvals[2] << ";");
    PROGRESS("                 " << eigvals[3] << "," << eigvals[4] << "," << eigvals[5] << ";");
    PROGRESS("                 " << eigvals[6] << "," << eigvals[7] << "," << eigvals[8] << "];");
#endif

    // if more than one of the eigenvalues is less than one then
    // multiply them all by -1.  We can do this because the ellipse
    // equation is only defined up to a scale factor.  We do this
    // because the y axis eigenvalue must have larger magnitude than the
    // x axis value in order for our next rotation to make sense.
    int count = 0;
    for(int i=0;i<3;i++) {
      if (eigvals[4*i] < 0) { ++count; }
    }
    if (count > 1) {
#ifdef ELLIPSE_TRANSFORM_DEBUG
      PROGRESS("Two of the eigenvalues are less than zero - reversing ellipse equation");    
      PROGRESS("Eigen Values: v=[" << eigvals[0] << "," << eigvals[1] << "," << eigvals[2] << ";");
      PROGRESS("                 " << eigvals[3] << "," << eigvals[4] << "," << eigvals[5] << ";");
      PROGRESS("                 " << eigvals[6] << "," << eigvals[7] << "," << eigvals[8] << "];");
#endif
      for(int i=0;i<3;i++) {
	eigvals[4*i]*=-1;
      }
    }



    // bubble sort the vectors (based on their eigenvalue)...I'm so embarresed ;-)
    for(int i=0;i<4;i++) {
      for(int j=1;j<3;j++) {
	if (eigvals[j*4-4] < eigvals[j*4]) {
	  // swap the vector
	  for(int k=0;k<3;k++) {
	    double t = eigvects[j-1+k*3];
	    eigvects[j-1+k*3] = eigvects[j+k*3];
	    eigvects[j+k*3] = t;	  
	  }

	  // swap the value
	  double t = eigvals[j*4-4];
	  eigvals[j*4-4] = eigvals[j*4];
	  eigvals[j*4]=t;
	}
      }
    }

    // make sure the normal vector will point in the right direction
    if (eigvects[8] < 0) {
      eigvects[2]*=-1;
      eigvects[5]*=-1;
      eigvects[8]*=-1;
    }

    // make sure that our eigenvectors do not include any reflections
    float determinant = 
      eigvects[0]*eigvects[4]*eigvects[8]
      - eigvects[0]*eigvects[5]*eigvects[7]
      - eigvects[1]*eigvects[3]*eigvects[8] 
      + eigvects[1]*eigvects[5]*eigvects[6] 
      + eigvects[2]*eigvects[3]*eigvects[7]
      - eigvects[2]*eigvects[4]*eigvects[6];
#ifdef ELLIPSE_TRANSFORM_DEBUG
    PROGRESS("Determinant = " << determinant);
#endif

    if (determinant < 0) {
      // it is enough to swap one of the vectors to make the determinant positive. 
      eigvects[0] *= -1;
      eigvects[3] *= -1;
      eigvects[6] *= -1;
#ifdef ELLIPSE_TRANSFORM_DEBUG
      determinant = 
	eigvects[0]*eigvects[4]*eigvects[8]
	- eigvects[0]*eigvects[5]*eigvects[7]
	- eigvects[1]*eigvects[3]*eigvects[8] 
	+ eigvects[1]*eigvects[5]*eigvects[6] 
	+ eigvects[2]*eigvects[3]*eigvects[7]
	- eigvects[2]*eigvects[4]*eigvects[6];
      PROGRESS("Swap x eigvect. Det = " << determinant);
#endif
        
    }

    double r1[] = { eigvects[0], eigvects[1], eigvects[2], 0,
		    eigvects[3], eigvects[4], eigvects[5], 0,
		    eigvects[6], eigvects[7], eigvects[8], 0,
		    0,           0,           0,           1};

#ifdef ELLIPSE_TRANSFORM_DEBUG
    PROGRESS("Rotation 1: r1=[ " << eigvects[0] << "," << eigvects[1] << "," << eigvects[2] << ";");
    PROGRESS("                 " << eigvects[3] << "," << eigvects[4] << "," << eigvects[5] << ";");
    PROGRESS("                 " << eigvects[6] << "," << eigvects[7] << "," << eigvects[8] << "];");

    PROGRESS("Sorted Eigen Values: sev=[ " << eigvals[0] << "," << eigvals[1] << "," << eigvals[2] << ";");
    PROGRESS("                           " << eigvals[3] << "," << eigvals[4] << "," << eigvals[5] << ";");
    PROGRESS("                           " << eigvals[6] << "," << eigvals[7] << "," << eigvals[8] << "];");

#endif
      
    double denom = ( eigvals[8] - eigvals[0] );
    double cossq = ( eigvals[8] - eigvals[4] ) / denom;
    double sinsq = ( eigvals[4] - eigvals[0] ) / denom;
    double pmsin = sqrt(sinsq);
    double pmcos = sqrt(cossq);

 

    // here is our first ambiguity choice point.  We need to decide plus or minus theta

    double r2c1[] = { pmcos, 0, -pmsin, 0,
		      0,     1, 0,     0, 
		      pmsin, 0, pmcos, 0,
		      0,     0, 0,     1 };
  
    double r2c2[] = { pmcos, 0, pmsin, 0,
		      0,     1, 0,     0, 
		      -pmsin, 0, pmcos, 0,
		      0,     0, 0,     1 };

#ifdef ELLIPSE_TRANSFORM_DEBUG  
    PROGRESS("Rotation 2(1): r2c1=[" << r2c1[0] << "," << r2c1[1] << "," << r2c1[2] << "," << r2c1[3] << ";");
    PROGRESS("                " << r2c1[4] << "," << r2c1[5] << "," << r2c1[6] << "," << r2c1[7] << ";");
    PROGRESS("                " << r2c1[8] << "," << r2c1[9] << "," << r2c1[10] << "," << r2c1[11] << ";");
    PROGRESS("                " << r2c1[12] << "," << r2c1[13] << "," << r2c1[14] << "," << r2c1[15] << "];");

    PROGRESS("Rotation 2(2): r2c2=[" << r2c2[0] << "," << r2c2[1] << "," << r2c2[2] << "," << r2c2[3] << ";");
    PROGRESS("                " << r2c2[4] << "," << r2c2[5] << "," << r2c2[6] << "," << r2c2[7] << ";");
    PROGRESS("                " << r2c2[8] << "," << r2c2[9] << "," << r2c2[10] << "," << r2c2[11] << ";");
    PROGRESS("                " << r2c2[12] << "," << r2c2[13] << "," << r2c2[14] << "," << r2c2[15] << "];");
#endif

    // now build the transformation matrix that goes from the unit circle to the 3d circle
    // this is a translation in x to align on the axis  premultiplied by
    // a scale factor premultiplied by
    // rotation r2 premuliplied by
    // rotation r1
  
    // apply the relevant translation - we have to choose again here based on our choice of theta
    double tx = sqrt((eigvals[4]-eigvals[0])*(eigvals[8]-eigvals[4]))/eigvals[4];


#ifdef ELLIPSE_TRANSFORM_DEBUG
    PROGRESS("Translation tx = "<<tx);
#endif
    // and another choice based on theta
    double scale = sqrt(-eigvals[0]*eigvals[8]/eigvals[4]/eigvals[4]);


#ifdef ELLIPSE_TRANSFORM_DEBUG
    PROGRESS("Scale factor " << scale);
#endif
  
    double transc1[] = {1,0,0,tx/scale,
			0,1,0,0,
			0,0,1,1/scale,
			0,0,0,1};
  
    double transc2[] = {1,0,0,-tx/scale,
			0,1,0,0,
			0,0,1,1/scale,
			0,0,0,1};



    
#ifdef ELLIPSE_TRANSFORM_DEBUG  
    PROGRESS("transc1=[" << transc1[0] << "," << transc1[1] << "," << transc1[2] << "," << transc1[3] << ";");
    PROGRESS("                 " << transc1[4] << "," << transc1[5] << "," << transc1[6] << "," << transc1[7] << ";");
    PROGRESS("                 " << transc1[8] << "," << transc1[9] << "," << transc1[10] << "," << transc1[11] << ";");
    PROGRESS("                 " << transc1[12] << "," << transc1[13] << "," << transc1[14] << "," << transc1[15] << ";");

    PROGRESS("transc2: mt2=[" << transc2[0] << "," << transc2[1] << "," << transc2[2] << "," << transc2[3] << ";");
    PROGRESS("                 " << transc2[4] << "," << transc2[5] << "," << transc2[6] << "," << transc2[7] << ";");
    PROGRESS("                 " << transc2[8] << "," << transc2[9] << "," << transc2[10] << "," << transc2[11] << ";");
    PROGRESS("                 " << transc2[12] << "," << transc2[13] << "," << transc2[14] << "," << transc2[15] << ";");
#endif


    double rtotc1[16];
    double rtotc2[16];
    // premultiply by r2
    for(int row=0;row<4;row++) {
      for(int col=0;col<4;col++) {
	rtotc1[row*4+col] = 0;
	rtotc2[row*4+col] = 0;
	for(int k=0;k<4;k++) {
	  rtotc1[row*4+col] += r2c1[row*4+k] * transc1[k*4+col];
	  rtotc2[row*4+col] += r2c2[row*4+k] * transc2[k*4+col];
	}
      }
    }

    Transform* t1 = new Transform(1.f);
    Transform* t2 = new Transform(1.f);
 
    dest.GetTransforms().push_back(t1);
    dest.GetTransforms().push_back(t2);
   
    // premultiply by r1
    for(int row=0;row<4;row++) {
      for(int col=0;col<4;col++) {
	(*t1)[row*4+col] = 0;
	(*t2)[row*4+col] = 0;
	for(int k=0;k<4;k++) {
	  (*t1)[row*4+col] += r1[row*4+k] * rtotc1[k*4+col];
	  (*t2)[row*4+col] += r1[row*4+k] * rtotc2[k*4+col];
	}
      }
    }

#ifdef ELLIPSE_TRANSFORM_DEBUG  
    PROGRESS("M_Transform: mt=[" << (*t1)[0] << "," << (*t1)[1] << "," << (*t1)[2] << "," << (*t1)[3] << ";");
    PROGRESS("                 " << (*t1)[4] << "," << (*t1)[5] << "," << (*t1)[6] << "," << (*t1)[7] << ";");
    PROGRESS("                 " << (*t1)[8] << "," << (*t1)[9] << "," << (*t1)[10] << "," << (*t1)[11] << ";");
    PROGRESS("                 " << (*t1)[12] << "," << (*t1)[13] << "," << (*t1)[14] << "," << (*t1)[15] << ";");

    PROGRESS("M_Transform: m(*t2)=[" << (*t2)[0] << "," << (*t2)[1] << "," << (*t2)[2] << "," << (*t2)[3] << ";");
    PROGRESS("                 " << (*t2)[4] << "," << (*t2)[5] << "," << (*t2)[6] << "," << (*t2)[7] << ";");
    PROGRESS("                 " << (*t2)[8] << "," << (*t2)[9] << "," << (*t2)[10] << "," << (*t2)[11] << ";");
    PROGRESS("                 " << (*t2)[12] << "," << (*t2)[13] << "," << (*t2)[14] << "," << (*t2)[15] << ";");
#endif

    return true;
  }

}
