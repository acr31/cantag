/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/23 22:38:53  acr31
 * testing of square tags
 *
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
  try {
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
  
    cvSaveImage("output.jpg",image);
    cvReleaseImage(&image);

    GrayScaleFileImageSource im("output.jpg");
    Image* buf = im.GetBuffer();
    im.Next();
    AdaptiveThreshold(buf);


    std::vector<Rectangle2DChain*> rectangles;
    FindRectangles(buf,4,4,&rectangles);
    for(std::vector<Rectangle2DChain*>::const_iterator step = rectangles.begin();step!=rectangles.end();step++) {
      std::cout << "Tag:" << t.Decode(buf, (*step)->current ) << std::endl;
      std::cout << *((*step)->current) << std::endl;
    }
  }
  catch (Exception &e) {
    std::cout << e << std::endl;
  }
}
