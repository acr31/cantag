#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <iostream>

#include "Config.hh"
#include "TripOuterTag.hh"
#include "TripOriginalCoder.hh"
#include "Drawing.hh"

#undef FILENAME
#define FILENAME "drawtriptag.cc"

int
main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << argv[0] << " [size] [code] [filename]"<<std::endl;
    exit(-1);
  }

  int width = atoi(argv[1]);
  long long value = atoll(argv[2]);

  PROGRESS("Drawing tag with width: " << width << ", code: " << value);

  int imagewidth = 600;


  CvSize size;
  size.width=imagewidth;
  size.height=imagewidth;

  IplImage *image = cvCreateImage(size, IPL_DEPTH_8U,1);
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width;j++) {
      ((uchar*)(image->imageData + image->widthStep*i))[j] = (uchar)255;
    }
  }

  Location2D l(imagewidth/2,
	       imagewidth/2,
	       width,
	       width,
	       PI);
  
  TripOuterTag<TripOriginalCoder<2,16,2>,2,16,24,2,10> t;

  t.Draw2D(image,&l,value, 0, 255);

  cvSaveImage(argv[3],image);
  cvReleaseImage(&image);
}


