#include <Config.hh>
#include <Drawing.hh>
#include <opencv/cv.h>

static void print(const char* label, double* array, int rows, int cols) {
  std::cout << "-----------------------------------" << std::endl;
  std::cout << label << "= [ ";
  for(int i=0;i<rows;i++) {
    for(int j=0;j<cols;j++) {
      std::cout << array[i*cols+j] << "\t";
    }
    std::cout << ";" << std::endl;
  }
  std::cout << "]" << std::endl;
}

Ellipse2D fitellipse(float* points, int numpoints) {

  for(int i=0;i<numpoints*2;i+=2) {
    std::cout << points[i] << " " << points[i+1] << " ; ";
  }
  std::cout << std::endl;

  double d1[numpoints*3];
  double d2[numpoints*3];
  
  // Compute

  // D1 = [ x.^2 , x.*y , y.^2 ]
  // D2 = [ x , y , 1]

  int pointer = 0;
  for(int i=0;i<numpoints*2;i+=2) {
    d1[pointer] = points[i]*points[i];
    d2[pointer++] = points[i];

    d1[pointer] = points[i]*points[i+1];
    d2[pointer++] = points[i+1];

    d1[pointer] = points[i+1]*points[i+1];
    d2[pointer++] = 1;
  }
  
  print("D1",d1,numpoints,3);
  print("D2",d2,numpoints,3);


  double s1[9];
  double s2[9];
  double s3[9];

  // Compute

  // S1 = D1' * D1
  // S2 = D1' * D2
  // S3 = D2' * D2

  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      s1[i*3+j] = d1[i]*d1[j];
      s2[i*3+j] = d1[i]*d2[j];
      s3[i*3+j] = d2[i]*d2[j];
      for(int k=1;k<numpoints;k++) {
	s1[i*3+j] += d1[i+k*3] * d1[j+k*3];
	s2[i*3+j] += d1[i+k*3] * d2[j+k*3];
	s3[i*3+j] += d2[i+k*3] * d2[j+k*3];
      }
    }
  }

  print("S1",s1,3,3);
  print("S2",s2,3,3);
  print("S3",s3,3,3);

  // Compute
  
  // S3Inv = inv(S3)
  
  double s3inv[9];
  CvMat S3;
  CvMat S3Inv;
  cvInitMatHeader(&S3,3,3,CV_64F,s3);
  cvInitMatHeader(&S3Inv,3,3,CV_64F,s3inv);
  cvInvert(&S3,&S3Inv,CV_LU);
  // do gaussian eliminiation with S3*x1= [1;0;0], S3*x2=[0;1;0], S3*x3=[0;0;1]

  print("S3Inv",s3inv,3,3);

  // Compute

  // T = -inv(S3) * S2'

  double t[9];
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      t[i*3+j] = -s3inv[i*3]*s2[j*3];
      for(int k=1;k<3;k++) {
	t[i*3+j] -= s3inv[i*3+k] * s2[j*3+k]; // <-cunning
      }
    }
  }
  print("T",t,3,3);

  // Compute

  // M = S1 + S2 * T

  double m[9];
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      m[i*3+j] = s1[i*3+j];
      for(int k=0;k<3;k++) {
	m[i*3+j] += s2[i*3+k] * t[k*3+j];
      }
    }
  }
  print("M",m,3,3);

  double m2[9];
  // premultiply M by inv(C1)
  //
  // ( M31/2 M32/2 M33/2 )
  // ( -M21  -M22  -M23  )
  // ( M11/2 M12/2 M13/2 )
  m2[0] = m[6]/2;
  m2[1] = m[7]/2;
  m2[2] = m[8]/2;

  m2[3] = -m[3];
  m2[4] = -m[4];
  m2[5] = -m[5];
  
  m2[6] = m[0]/2;
  m2[7] = m[1]/2;
  m2[8] = m[2]/2;

  print("M2",m2,3,3);


  CvMat M2;
  cvInitMatHeader(&M2,3,3,CV_64F,m2);

  double eigvals[3];
  double eigvects[9];
  CvMat Eigvals;
  CvMat Eigvects;
  cvInitMatHeader(&Eigvals,3,1,CV_64F,eigvals);
  cvInitMatHeader(&Eigvects,3,3,CV_64F,eigvects);

  cvEigenVV(&M2,&Eigvects,&Eigvals,pow(10,-30));
  print("Eigvals",eigvals,1,3);
  print("Eigvects",eigvects,3,3);

  for(int i = 0; i < 9; i+=3) {
    if (4*eigvects[i]*eigvects[i+2]-eigvects[i+1]*eigvects[i+1] >= 0.0) {
      PROGRESS("Fitted ellipse: a="<<
	       eigvects[i] << "," <<
	       eigvects[i+1]<< "," <<
	       eigvects[i+2]<< "," <<
	       t[0]*eigvects[i]+t[1]*eigvects[i+1]+t[2]*eigvects[i+2]<< "," <<
	       t[3]*eigvects[i]+t[4]*eigvects[i+1]+t[5]*eigvects[i+2]<< "," <<
	       t[6]*eigvects[i]+t[7]*eigvects[i+1]+t[8]*eigvects[i+2]);
      return Ellipse2D(eigvects[i],
		       eigvects[i+1],
		       eigvects[i+2],
		       t[0]*eigvects[i]+t[1]*eigvects[i+1]+t[2]*eigvects[i+2],
		       t[3]*eigvects[i]+t[4]*eigvects[i+1]+t[5]*eigvects[i+2],
		       t[6]*eigvects[i]+t[7]*eigvects[i+1]+t[8]*eigvects[i+2]);
      
    }
    
  }
}
