/**
 * $Header$
 *
 * $Log$
 * Revision 1.3  2004/01/21 13:41:37  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 * Revision 1.2  2004/01/21 11:55:10  acr31
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
#include "GrayScaleFileImageSource.hh"
#include "adaptivethreshold.hh"
#include "findellipses.hh"
#include "posefromcircle.hh"
#include "Location2D.hh"
#include "Location3D.hh"

#undef FILENAME
#define FILENAME "drawtriptag.cc"

int
main(int argc, char* argv[]) {
  if (argc != 10) {
    std::cout << argv[0] << " [size] [code] [rings] [sectors] [sync angles] [bullseye inner] [bullseye outer] [data inner] [data outer]"<<std::endl;
    exit(-1);
  }

  int size = atoi(argv[1]);
  long long code = atoll(argv[2]);
  int rings = atoi(argv[3]);
  int sectors = atoi(argv[4]);
  int sync_angles = atoi(argv[5]);
  float bullseye_inner = atof(argv[6]);
  float bullseye_outer = atof(argv[7]);
  float data_inner = atof(argv[8]);
  float data_outer = atof(argv[9]);

  PROGRESS("Drawing tag with width: " << size << ", code: " << code);

  float max = bullseye_inner;
  if (bullseye_outer > max) { max = bullseye_outer; }
  if (data_inner > max) { max = data_inner; }
  if (data_outer > max) { max = data_outer; }

  int imagewidth = (int)(size*max+10);
  if (imagewidth < 600) { imagewidth = 600; }

  IplImage *image = cvCreateImage(cvSize(imagewidth,imagewidth), IPL_DEPTH_8U,1);
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width;j++) {
      ((uchar*)(image->imageData + image->widthStep*i))[j] = (uchar)255;
    }
  }

  Location2D l(imagewidth/2,
	       imagewidth/2,
	       size,
	       size,
	       0.05);

  RingTag<TripOriginalCoder<2> > t(rings, //RINGS
				   sectors, //SECTORS
				   sync_angles, // SYNC ANGLES
				   bullseye_inner, // BULLSEYE INNER
				   bullseye_outer, // BULLSEYE OUTER
				   data_inner, // DATA INNER
				   data_outer); // DATA OUTER

  t.Draw2D(image,&l,code, 0, 255);

  cvSaveImage("output.jpg",image);
  cvReleaseImage(&image);

  GrayScaleFileImageSource im("output.jpg");
  Image* buf = im.GetBuffer();
  im.Next();
  AdaptiveThreshold(buf);
  std::vector<Location2DChain*> ellipses;
  FindEllipses(buf,10,1000,4,4,0.01,0.001,&ellipses);

  for(std::vector<Location2DChain*>::const_iterator step = ellipses.begin();step!=ellipses.end();step++) {
    if ((*step)->nextchain != NULL) {
      std::cout << "Tag:" << t.Decode(buf, (*step)->current ) << std::endl;
      std::cout << *((*step)->current) << std::endl;
      Location3D *r=  PoseFromCircle((*step)->current,1.0);
      std::cout << *r << std::endl;
      delete(r);
      
    }
  }
}


