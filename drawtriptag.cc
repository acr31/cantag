/**
 * $Header$
 *
 * $Log$
 * Revision 1.8  2004/01/23 11:49:54  acr31
 * Finished integrating the GF4 coder - I've ripped out the error correcting stuff for now and its just looking for a match but it doesn't seem to work.  I need to spend some time to understand the division and mod operations in the GF4Poly to progress
 *
 * Revision 1.7  2004/01/23 09:09:07  acr31
 * changes for testing the new gf4 code
 *
 * Revision 1.6  2004/01/21 11:55:09  acr31
 * added keywords for substitution
 *
 */
#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <iostream>

#include "Config.hh"
#include "Drawing.hh"
#include "TripOriginalCoder.hh"
#include "RingTag.hh"
#include "GF4Coder.hh"

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
	       0.4);


  //  RingTag<TripOriginalCoder<2> > t(2, //RINGS
  RingTag<GF4Coder > t(2, //RINGS
				  19, //SECTORS
				  24, // SYNC ANGLES
				  0.3, // BULLSEYE INNER
				  1.2, // BULLSEYE OUTER
				  0.4, // DATA INNER
				  1.1); // DATA OUTER

  t.Draw2D(image,&l,value, 0, 255);

  cvSaveImage(argv[3],image);
  cvReleaseImage(&image);
}


