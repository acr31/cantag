/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/23 18:18:12  acr31
 * added Matrix Tag and a test program and the beginning of the CRC coder
 *
 */
#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <iostream>

#include "Config.hh"
#include "Drawing.hh"
#include "TripOriginalCoder.hh"
#include "GF4Coder.hh"
#include "CRCCoder.hh"
#include "MatrixTag.hh"
#include "GrayScaleFileImageSource.hh"
#include "adaptivethreshold.hh"
#include "findrectangles.hh"
#include "Rectangle2D.hh"

#undef FILENAME
#define FILENAME "triptest3.cc"

int
main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << argv[0] << " [tagsize] [code] [codingsize] " <<std::endl;
    exit(-1);
  }

  int size = atoi(argv[1]);
  long long code = atoll(argv[2]);
  int codesize = atoi(argv[3]);

  PROGRESS("Drawing tag with width: " << size << ", code: " << code);

  int imagewidth = (int)(size+10);
  if (imagewidth < 600) { imagewidth = 600; }

  IplImage *image = cvCreateImage(cvSize(imagewidth,imagewidth), IPL_DEPTH_8U,1);
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width;j++) {
      ((uchar*)(image->imageData + image->widthStep*i))[j] = (uchar)255;
    }
  }

  Rectangle2D l(imagewidth/2 - size/2,
		imagewidth/2 - size/2,
		imagewidth/2 + size/2,
		imagewidth/2 - size/2,
		imagewidth/2 + size/2,
		imagewidth/2 + size/2,
		imagewidth/2 - size/2,
		imagewidth/2 + size/2);

  MatrixTag<CRCCoder > t(codesize);

  t.Draw2D(image,&l,code, 0, 255);

  std::cout << "Drawn "<< std::endl;
  cvSaveImage("output.jpg",image);
  std::cout << "Drawn "<< std::endl;
    
  cvReleaseImage(&image);

  //  cvWaitKey(1);
  
  /*
  GrayScaleFileImageSource im("output.jpg");
  Image* buf = im.GetBuffer();
  im.Next();
  AdaptiveThreshold(buf);
  std::vector<Ellipse2DChain*> ellipses;
  FindEllipses(buf,10,1000,4,4,0.01,0.001,&ellipses);

  for(std::vector<Ellipse2DChain*>::const_iterator step = ellipses.begin();step!=ellipses.end();step++) {
    if ((*step)->nextchain != NULL) {
      std::cout << "Tag:" << t.Decode(buf, (*step)->current ) << std::endl;
      std::cout << *((*step)->current) << std::endl;
      Location3D *r=  PoseFromCircle((*step)->current,1.0);
      std::cout << *r << std::endl;
      delete(r);
      
    }
    }*/
}


