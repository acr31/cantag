#include <Config.hh>
#include <Drawing.hh>
#include <opencv/cv.h>

static void print(double* array, int rows, int cols) {
  std::cout << "-----------------------------------" << std::endl;
  std::cout << "[ ";
  for(int i=0;i<rows;i++) {
    for(int j=0;j<cols;j++) {
      std::cout << array[i*cols+j] << "\t";
    }
    std::cout << ";" << std::endl;
  }
  std::cout << "]";
}

Ellipse2D fitellipse(float* points, int numpoints) {

  //  for(int i=0;i<numpoints*2;i++) {
  //    std::cout << "  " << points[i] << " " << points[i+1] << ";" << std::endl;
  //  }

  //  print(points,numpoints,2);

  double d1[numpoints*3];
  double d2[numpoints*3];

  int pointer = 0;
  for(int i=0;i<numpoints*2;i+=2) {
    d1[pointer] = points[i]*points[i];
    d2[pointer++] = points[i];

    d1[pointer] = points[i]*points[i+1];
    d2[pointer++] = points[i+1];

    d1[pointer] = points[i+1]*points[i+1];
    d2[pointer++] = 1;
  }
  
  //  print(d1,numpoints,3);
  //  print(d2,numpoints,3);

  CvMat D1;
  CvMat D2;
  cvInitMatHeader(&D1,numpoints,3,CV_64F,d1);
  cvInitMatHeader(&D2,numpoints,3,CV_64F,d2);

  double s1[9];
  double s2[9];
  double s3[9];

  CvMat S1;
  CvMat S2;
  CvMat S3;

  cvInitMatHeader(&S1,3,3,CV_64F,s1);
  cvInitMatHeader(&S2,3,3,CV_64F,s2);
  cvInitMatHeader(&S3,3,3,CV_64F,s3);
  
  cvMulTransposed(&D1,&S1,1); // this does S1 = D1' * D1 
  cvGEMM(&D1,&D2,1,NULL,1,&S2, CV_GEMM_A_T); // this does S2 = D1' * D2
  cvMulTransposed(&D2,&S3,1); // this does S3 = D2' * D2

  //  print(s1,3,3);
  //  print(s2,3,3);
  //  print(s3,3,3);

  double invs3[9];
  CvMat invS3;
  cvInitMatHeader(&invS3,3,3,CV_64F,invs3);
  cvInvert(&S3,&invS3,CV_LU);
  
  //  print(invs3,3,3);
  
  CvMat T;
  double t[9];
  cvInitMatHeader(&T,3,3,CV_64F,t);
  cvGEMM(&invS3,&S2,-1,NULL,1,&T,CV_GEMM_B_T); // this does T = -inv(S3)*S2'

  //  print(t,3,3);

  double m[9];
  CvMat M;
  cvInitMatHeader(&M,3,3,CV_64F,m);
  cvMatMulAdd(&S2,&T,&S1,&M);// this does M = S1 + S2 * T
  
  //  print(m,3,3);

  double m2[9];
  CvMat M2;
  cvInitMatHeader(&M2,3,3,CV_64F,m2);

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

  //  print(m2,3,3);

  double eigvals[3];
  double eigvects[9];
  CvMat Eigvals;
  CvMat Eigvects;
  cvInitMatHeader(&Eigvals,3,1,CV_64F,eigvals);
  cvInitMatHeader(&Eigvects,3,3,CV_64F,eigvects);

  cvEigenVV(&M2,&Eigvects,&Eigvals,pow(10,-15));

  //  print(eigvals,3,1);
  //  print(eigvects,3,3);

  
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
