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

  CvSize size;
  size.width=width;
  size.height=width;

  IplImage *image = cvCreateImage(size, IPL_DEPTH_8U,1);
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width;j++) {
      ((uchar*)(image->imageData + image->widthStep*i))[j] = (uchar)255;
    }
  }

  CvPoint p;
  p.x=width/2;
  p.y=width/2;

  double r = width / 2 / radii_outer[RING_COUNT-1];
  CvSize s;
  s.width=r;
  s.height=r;


  CvBox2D box;
  box.center=p;
  box.size=s;
  box.angle=0.0;
  
  TripOriginalTag t(box,value);
  t.Synthesize(image);

  cvSaveImage(argv[3],image);

  std::cout << "Done.  Tag written to "<<argv[3]<<std::endl;

  cvReleaseImage(&image);
}


