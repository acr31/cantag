#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <iostream>

#include "CircularOuterParams.hh"

int
main(int argc, char* argv[]) {

  if (argc != 4) {
    std::cout << argv[0] << " [size] [code] [filename]"<<std::endl;
  }

  int width = atoi(argv[1]);
  unsigned int value = (unsigned int)atol(argv[2]);
  CvSize size;
  size.width=width;
  size.height=width;

  IplImage *image = cvCreateImage(size, IPL_DEPTH_8U,1);
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width;j++) {
      ((uchar*)(image->imageData + image->widthStep*i))[j] = (uchar)255;
    }
  }

  double r = width / 2 / radii[RING_COUNT-1];

  CvPoint p;
  p.x=width/2;
  p.y=width/2;

  CvSize s;
  
  for(int j=RING_COUNT-1;j>=0;j--) {
    s.width=(int)(radii_outer[j]*r);
    s.height=(int)(radii_outer[j]*r);
    
    for(int i=0;i<SECTOR_COUNT-1;i++) {
      double a1 = sector_angles[i]/M_PI*180;
      double a2 = sector_angles[i+1]/M_PI*180;
      if (a1<0) { a1+=360; }
      if (a2<0) { a2+=360; }
      
      cvEllipse(image,p,s,0,
		a1,a2,
		(value & 1) ? 0 : 255, -1);
      value >>= 1;
    }

    double a1 = sector_angles[SECTOR_COUNT-1]/M_PI*180;
    double a2 = sector_angles[0]/M_PI*180;
    if (a1<0) { a1+=360; }
    if (a2<0) { a2+=360; }
    if (a2<a1) { a2+=360; }

    cvEllipse(image,p,s,0,
	      a1,a2,
	      (value & 1) ? 0 : 255, -1);
    value >>= 1;
    cvCircle(image,p,(int)(radii_inner[j]*r),255,-1);
  }


  cvCircle(image,p,(int)r,0,-1);
  cvCircle(image,p,(int)(0.6*r),255,-1);
  cvCircle(image,p,(int)(0.2*r),0,-1);

  cvSaveImage(argv[3],image);
}
