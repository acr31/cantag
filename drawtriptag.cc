#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <iostream>

#include "TripOriginalTag.hh"

int
main(int argc, char* argv[]) {

  if (argc != 4) {
    std::cout << argv[0] << " [size] [code] [filename]"<<std::endl;
    exit(-1);
  }

  int width = atoi(argv[1]);
  long long value = atoll(argv[2]);

  double imagewidth = width;
  if (width < 300) {
    imagewidth = 300;
  }

  CvSize size;
  size.width=imagewidth;
  size.height=imagewidth;

  IplImage *image = cvCreateImage(size, IPL_DEPTH_8U,1);
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width;j++) {
      ((uchar*)(image->imageData + image->widthStep*i))[j] = (uchar)255;
    }
  }

  CvPoint2D32f p;
  p.x=imagewidth/2;
  p.y=imagewidth/2;

  double r = width / TripOriginalTag<>::TagRadius();

  CvSize2D32f s;
  s.width=r;
  s.height=r;


  CvBox2D box;
  box.center=p;
  box.size=s;
  box.angle=10.0;
  
  TripOriginalTag<> t(box,value);
  t.Synthesize(image,255,0);

  cvSaveImage(argv[3],image);
  cvReleaseImage(&image);
}


