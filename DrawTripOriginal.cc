#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <iostream>

#include "TripOriginalParams.hh"

#define WHITE 255
#define BLACK 0


bool
allset(long long value, int sector) {
  bool result = 1;
  for(int i=0;i<RING_COUNT;i++) {
    result = result && (value & 1<<sector+i*SECTOR_COUNT);
  }
  return result;
}


int
main(int argc, char* argv[]) {

  if (argc != 4) {
    std::cout << argv[0] << " [size] [code] [filename]"<<std::endl;
    exit(-1);
  }

  std::cout << argv[0] << std::endl;
  std::cout << "Number of rings: " << RING_COUNT << std::endl;
  std::cout << "Number of sectors per ring: " << SECTOR_COUNT << std::endl;

  int width = atoi(argv[1]);
  long long value = atoll(argv[2]);

  /* value must have a sync sector at the begining and then no other
     sectors which are both set */
  if (!allset(value,0)) {
    std::cout << "!!! Code must contain synchronization sector (all set) for first sector" << std::endl;
    exit(-1);
  }

  for(int i=1;i<SECTOR_COUNT;i++) {
    if (allset(value,i)) {
      std::cout << "!!! Code must not contain any fully set sectors excluding the synchronization sector" << std::endl;
      exit(-1);
    }
  }

  CvSize size;
  size.width=width;
  size.height=width;

  IplImage *image = cvCreateImage(size, IPL_DEPTH_8U,1);
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width;j++) {
      ((uchar*)(image->imageData + image->widthStep*i))[j] = (uchar)255;
    }
  }

  double r = width / 2 / radii_outer[RING_COUNT-1];

  CvPoint p;
  p.x=width/2;
  p.y=width/2;

  CvSize s;
  
  /* cvEllipse doesn't work if the start angle is greater than the end
     angle or if the angles are negative */

  for(int j=RING_COUNT-1;j>=0;j--) {
    s.width=(int)(radii_outer[j]*r);
    s.height=(int)(radii_outer[j]*r);
    
    for(int i=0;i<SECTOR_COUNT-1;i++) {
      double a1 = sector_angles[i]/M_PI*180;
      double a2 = sector_angles[i+1]/M_PI*180;
      if (a1<0) { a1+=360; }
      if (a2<0) { a2+=360; }
      cvEllipse(image,p,s,0,
		360-a2,360-a1,
		(value & 1) ? BLACK : WHITE, -1);
      value >>= 1;
    }

    double a1 = sector_angles[SECTOR_COUNT-1]/M_PI*180;
    double a2 = sector_angles[0]/M_PI*180;
    if (a1<0) { a1+=360; }
    if (a2<0) { a2+=360; }
    if (a2<a1) { a2+=360; }

    cvEllipse(image,p,s,0,
	      360-a2,360-a1,
	      (value & 1) ? BLACK : WHITE, -1);
    value >>= 1;
    cvCircle(image,p,(int)(radii_inner[j]*r),255,-1);
  }

  cvCircle(image,p,(int)r,BLACK,-1);
  cvCircle(image,p,(int)(0.6*r),WHITE,-1);
  cvCircle(image,p,(int)(0.2*r),BLACK,-1);

  cvSaveImage(argv[3],image);
  std::cout << "Done.  Tag written to "<<argv[3]<<std::endl;
}


