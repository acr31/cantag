/**
 * $Header$
 *
 * $Log$
 * Revision 1.4  2004/01/23 11:57:09  acr31
 * moved Location2D to Ellipse2D in preparation for Square Tags
 *
 * Revision 1.3  2004/01/21 13:41:36  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 * Revision 1.2  2004/01/21 11:55:09  acr31
 * added keywords for substitution
 *
 */

#include "posefromcircle.hh"

#undef FILENAME
#define FILENAME "posefromcircle.cc"

Location3D*
PoseFromCircle(const Ellipse2D *l, double radius) {
  PROGRESS("Angle: " << l->m_angle_radians);
  PROGRESS("Centre: " <<l->m_x << " " << l->m_y);
  PROGRESS("Axes: " << l->m_width << " " << l->m_height);
 
  float sintheta = sin(l->m_angle_radians);
  float costheta = cos(l->m_angle_radians);
  float x0 = l->m_x;
  float y0 = l->m_y;
  float asq = l->m_width * l->m_width * 0.25;
  float bsq = l->m_height * l->m_height *0.25;

  float sinthetasq = sintheta*sintheta;
  float costhetasq = costheta*costheta;
  
  float x0sq = x0*x0;
  float y0sq = y0*y0;

  float coeffa = 1/asq*sinthetasq + 1/bsq*costhetasq;
  float coeffb = 2/asq*sintheta*costheta - 2/bsq*sintheta*costheta;
  float coeffc = 1/asq*costhetasq + 1/bsq*sinthetasq;
  float coeffd = -2/asq*sinthetasq*x0 - 2/asq*sintheta*costheta*y0 + 2/bsq*sintheta*costheta*y0 - 2/bsq*costhetasq*x0;
  float coeffe = -2/asq*sintheta*x0*costheta - 2/asq*costhetasq*y0 - 2/bsq*sinthetasq*y0 + 2/bsq*sintheta*x0*costheta;
  float coefff = 1/asq*sinthetasq*x0sq + 2/asq*sintheta*x0*costheta*y0 + 1/asq*costhetasq*y0sq + 1/bsq*sinthetasq*y0sq - 2/bsq*sintheta*x0*costheta*y0 + 1/bsq*costhetasq*x0sq-1;
                                                                                     
  PROGRESS("CoeffA: " << coeffa);
  PROGRESS("CoeffB: " << coeffb);
  PROGRESS("CoeffC: " << coeffc);
  PROGRESS("CoeffD: " << coeffd);
  PROGRESS("CoeffE: " << coeffe);
  PROGRESS("CoeffF: " << coefff);


  float Cvals[] = {coeffa, coeffb/2, coeffd/2,
		   coeffb/2, coeffc, coeffe/2, 
		   coeffd/2, coeffe/2, coefff};
  
  CvMat C;
  CvMat eigvect;
  CvMat eigval;

  float eigvects[9];
  float eigvals[3];

  cvInitMatHeader(&C,3,3,CV_64F,Cvals);
  cvInitMatHeader(&eigvect,3,3,CV_64F,eigvects);
  cvInitMatHeader(&eigval,3,1,CV_64F,eigvals);
  
  cvEigenVV(&C,&eigvect,&eigval,pow(10,-15));


  PROGRESS("Eigen Vectors: " << eigvects[0] << " " << eigvects[1] << " " << eigvects[2]);
  PROGRESS("               " << eigvects[3] << " " << eigvects[4] << " " << eigvects[5]);
  PROGRESS("               " << eigvects[6] << " " << eigvects[7] << " " << eigvects[8]);

  PROGRESS("Eigen Values:  " << eigvals[0] << " " << eigvals[1] << " " << eigvals[2]);


  /* 
     eigvect = ( <- e3 -> )
               ( <- e2 -> )
	       ( <- e1 -> )

     eigval =  (v3 v2 v1)

     v1 < v2 < v2

     So we need to normalise each eigenvector and swap eigvect around to be this

     neweigvect = ( ^  ^  ^ )
                  ( e1 e2 e3)
		  ( v  v  v )

    i.e.

    ( 0 1 2 )  ->   ( 6 3 0 ) 
    ( 3 4 5 )       ( 7 4 1 )
    ( 6 7 8 )       ( 8 5 2 )
  */
  
  float dete1 = sqrt(eigvects[6]*eigvects[6] + 
		      eigvects[7]*eigvects[7] + 
		      eigvects[8]*eigvects[8]);

  float dete2 = sqrt(eigvects[3]*eigvects[3] + 
		      eigvects[4]*eigvects[4] +
		      eigvects[5]*eigvects[5]);

  float dete3 = sqrt(eigvects[0]*eigvects[0] +
		      eigvects[1]*eigvects[1] +
		      eigvects[2]*eigvects[2]);

  float t = eigvects[0];
  eigvects[0] = eigvects[6]/dete1;
  eigvects[6] = eigvects[8]/dete1;
  eigvects[8] = eigvects[2]/dete3;
  eigvects[2] = t/dete3;
  t = eigvects[1];
  eigvects[1] = eigvects[3]/dete2;
  eigvects[3] = eigvects[7]/dete1;
  eigvects[7] = eigvects[5]/dete2;
  eigvects[5] = t/dete3;
  eigvects[4]/=dete2;


  PROGRESS("Rotation 1   : " << eigvects[0] << " " << eigvects[1] << " " << eigvects[2]);
  PROGRESS("               " << eigvects[3] << " " << eigvects[4] << " " << eigvects[5]);
  PROGRESS("               " << eigvects[6] << " " << eigvects[7] << " " << eigvects[8]);



  /* the second transformation  - remember eigenvalues are in reverse order*/
  float cc = ( eigvals[0] - eigvals[1] ) / ( eigvals[0] - eigvals[2]);
  float s  = ( eigvals[1] - eigvals[2] ) / ( eigvals[0] - eigvals[2]);
  if (fabs(cc) < 0.0001) {
    cc = 0;
  }
  if (fabs(s) < 0.0001) {
    s = 0;
  }

  cc = sqrt(cc);
  s = sqrt(s);


  float R2vals[] = { cc,0,s,
		      0 ,1,0,
		      -s,0,cc};
  CvMat R2;

  cvInitMatHeader(&R2,3,3,CV_64F,R2vals);


  PROGRESS("Rotation 2   : " << R2vals[0] << " " << R2vals[1] << " " << R2vals[2]);
  PROGRESS("               " << R2vals[3] << " " << R2vals[4] << " " << R2vals[5]);
  PROGRESS("               " << R2vals[6] << " " << R2vals[7] << " " << R2vals[8]);


  /* remember eigenvalues are in reverse order */
  float dist = -eigvals[1]*eigvals[1]*radius/eigvals[2]/eigvals[0];
  if (fabs(dist) < 0.0001) { dist = 0.0; }
  dist = sqrt(dist);

  float alpha = (eigvals[0]-eigvals[1])*(eigvals[1]-eigvals[2])*dist*dist / eigvals[1]/eigvals[1];
  if (fabs(alpha) < 0.0001) { alpha = 0.0; }
  alpha = sqrt(alpha);
  
  cvMatMulAdd(&eigvect,&R2,NULL,&C);


  PROGRESS("Dist:  " << dist);
  PROGRESS("Alpha: " << alpha);

  PROGRESS("Combined     : " << Cvals[0] << " " << Cvals[1] << " " << Cvals[2]);
  PROGRESS("               " << Cvals[3] << " " << Cvals[4] << " " << Cvals[5]);
  PROGRESS("               " << Cvals[6] << " " << Cvals[7] << " " << Cvals[8]);


  float pvals[] = { alpha,0,dist };
  CvMat p;
  cvInitMatHeader(&p,3,1,CV_64F,pvals);

  cvMatMulAdd(&C,&p,NULL,&p);
  float sx = pvals[0];
  float sy = pvals[1];
  float sz = pvals[2];

  pvals[0] = 0;
  pvals[1] = 0;
  pvals[2] = 1;
  cvMatMulAdd(&C,&p,NULL,&p);

  return new Location3D(sx,sy,sz,pvals[0],pvals[1],pvals[2]);
}
