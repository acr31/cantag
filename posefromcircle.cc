#include "posefromcircle.hh"

#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

void
PoseFromCircle(const CvBox2D &ellipse, double radius, CvPoint3D32f* unit_normal, CvPoint3D32f* centre) {

#ifdef DEBUG
  std::cout << "Angle: " <<ellipse.angle << std::endl;
  std::cout << "Centre: " <<ellipse.center.x << " " << ellipse.center.y << std::endl;
  std::cout << "Axes: " <<ellipse.size.width << " " << ellipse.size.height << std::endl;
#endif
 
  double sintheta = sin(ellipse.angle/180*CV_PI);
  double costheta = cos(ellipse.angle/180*CV_PI);
  double x0 = ellipse.center.x;
  double y0 = ellipse.center.y;
  double asq = ellipse.size.width * ellipse.size.width;
  double bsq = ellipse.size.height *ellipse.size.height;;

  double sinthetasq = sintheta*sintheta;
  double costhetasq = costheta*costheta;
  
  double x0sq = x0*x0;
  double y0sq = y0*y0;

  double coeffa = 1/asq*sinthetasq + 1/bsq*costhetasq;
  double coeffb = 2/asq*sintheta*costheta - 2/bsq*sintheta*costheta;
  double coeffc = 1/asq*costhetasq + 1/bsq*sinthetasq;
  double coeffd = -2/asq*sinthetasq*x0 - 2/asq*sintheta*costheta*y0 + 2/bsq*sintheta*costheta*y0 - 2/bsq*costhetasq*x0;
  double coeffe = -2/asq*sintheta*x0*costheta - 2/asq*costhetasq*y0 - 2/bsq*sinthetasq*y0 + 2/bsq*sintheta*x0*costheta;
  double coefff = 1/asq*sinthetasq*x0sq + 2/asq*sintheta*x0*costheta*y0 + 1/asq*costhetasq*y0sq + 1/bsq*sinthetasq*y0sq - 2/bsq*sintheta*x0*costheta*y0 + 1/bsq*costhetasq*x0sq-1;
                                                                                     

#ifdef DEBUG
  std::cout << "CoeffA: " << coeffa << std::endl;
  std::cout << "CoeffB: " << coeffb << std::endl;
  std::cout << "CoeffC: " << coeffc << std::endl;
  std::cout << "CoeffD: " << coeffd << std::endl;
  std::cout << "CoeffE: " << coeffe << std::endl;
  std::cout << "CoeffF: " << coefff << std::endl;
#endif

  double Cvals[] = {coeffa, coeffb/2, coeffd/2,
		   coeffb/2, coeffc, coeffe/2, 
		   coeffd/2, coeffe/2, coefff};
  
  CvMat C;
  CvMat eigvect;
  CvMat eigval;

  double eigvects[9];
  double eigvals[3];

  cvInitMatHeader(&C,3,3,CV_64F,Cvals);
  cvInitMatHeader(&eigvect,3,3,CV_64F,eigvects);
  cvInitMatHeader(&eigval,3,1,CV_64F,eigvals);
  
  cvEigenVV(&C,&eigvect,&eigval,pow(10,-15));

#ifdef DEBUG
  std::cout << "Eigen Vectors: " << eigvects[0] << " " << eigvects[1] << " " << eigvects[2] << std::endl;
  std::cout << "               " << eigvects[3] << " " << eigvects[4] << " " << eigvects[5] << std::endl;
  std::cout << "               " << eigvects[6] << " " << eigvects[7] << " " << eigvects[8] << std::endl;

  std::cout << "Eigen Values:  " << eigvals[0] << " " << eigvals[1] << " " << eigvals[2] << std::endl;
#endif

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
  
  double dete1 = sqrt(eigvects[6]*eigvects[6] + 
		      eigvects[7]*eigvects[7] + 
		      eigvects[8]*eigvects[8]);

  double dete2 = sqrt(eigvects[3]*eigvects[3] + 
		      eigvects[4]*eigvects[4] +
		      eigvects[5]*eigvects[5]);

  double dete3 = sqrt(eigvects[0]*eigvects[0] +
		      eigvects[1]*eigvects[1] +
		      eigvects[2]*eigvects[2]);

  double t = eigvects[0];
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

#ifdef DEBUG
  std::cout << "Rotation 1   : " << eigvects[0] << " " << eigvects[1] << " " << eigvects[2] << std::endl;
  std::cout << "               " << eigvects[3] << " " << eigvects[4] << " " << eigvects[5] << std::endl;
  std::cout << "               " << eigvects[6] << " " << eigvects[7] << " " << eigvects[8] << std::endl;
#endif


  /* the second transformation  - remember eigenvalues are in reverse order*/
  double cc = ( eigvals[0] - eigvals[1] ) / ( eigvals[0] - eigvals[2]);
  double s  = ( eigvals[1] - eigvals[2] ) / ( eigvals[0] - eigvals[2]);
  if (fabs(cc) < 0.0001) {
    cc = 0;
  }
  if (fabs(s) < 0.0001) {
    s = 0;
  }

  cc = sqrt(cc);
  s = sqrt(s);


  double R2vals[] = { cc,0,s,
		      0 ,1,0,
		      -s,0,cc};
  CvMat R2;

  cvInitMatHeader(&R2,3,3,CV_64F,R2vals);

#ifdef DEBUG
  std::cout << "Rotation 2   : " << R2vals[0] << " " << R2vals[1] << " " << R2vals[2] << std::endl;
  std::cout << "               " << R2vals[3] << " " << R2vals[4] << " " << R2vals[5] << std::endl;
  std::cout << "               " << R2vals[6] << " " << R2vals[7] << " " << R2vals[8] << std::endl;
#endif

  /* remember eigenvalues are in reverse order */
  double dist = -eigvals[1]*eigvals[1]*radius/eigvals[2]/eigvals[0];
  if (fabs(dist) < 0.0001) { dist = 0.0; }
  dist = sqrt(dist);

  double alpha = (eigvals[0]-eigvals[1])*(eigvals[1]-eigvals[2])*dist*dist / eigvals[1]/eigvals[1];
  if (fabs(alpha) < 0.0001) { alpha = 0.0; }
  alpha = sqrt(alpha);
  
  cvMatMulAdd(&eigvect,&R2,NULL,&C);

#ifdef DEBUG
  std::cout << "Dist:  " << dist << std::endl;
  std::cout << "Alpha: " << alpha << std::endl;

  std::cout << "Combined     : " << Cvals[0] << " " << Cvals[1] << " " << Cvals[2] << std::endl;
  std::cout << "               " << Cvals[3] << " " << Cvals[4] << " " << Cvals[5] << std::endl;
  std::cout << "               " << Cvals[6] << " " << Cvals[7] << " " << Cvals[8] << std::endl;
#endif

  double pvals[] = { alpha,0,dist };
  CvMat p;
  cvInitMatHeader(&p,3,1,CV_64F,pvals);

  cvMatMulAdd(&C,&p,NULL,&p);
  centre->x = pvals[0];
  centre->y = pvals[1];
  centre->z = pvals[2];

  pvals[0] = 0;
  pvals[1] = 0;
  pvals[2] = 1;
  cvMatMulAdd(&C,&p,NULL,&p);
  unit_normal->x = pvals[0];
  unit_normal->y = pvals[1];
  unit_normal->z = pvals[2];
}
