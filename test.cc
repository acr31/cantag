#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "ConcentricEllipse.hh"
#include "Tag.hh"

int 
main(int argc,char* argv[]) 
{
  /* load image from disk */
  IplImage *img = cvLoadImage(argv[1]);

  /* Convert to grayscale */
  CvSize size;
  size.width=img->width;
  size.height=img->height;
  IplImage *gray = cvCreateImage(size,IPL_DEPTH_8U,1);
  cvCvtColor(img,gray,CV_RGB2GRAY);

  /* Adaptive Threshold using a window size 1/8th of the image size */
  int window_size = gray->width/8;
  window_size+= 1-(window_size %2); /* window size must be an odd number */
  cvAdaptiveThreshold(gray,gray,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,window_size,40);

  std::vector<Tag> result;
    findTags(gray,&result);
 
  for(std::vector<Tag>::const_iterator search = result.begin();search != result.end();search++) {
    std::cout << search->code << std::endl;
  }

  cvSaveImage(argv[2],gray);
  cvReleaseImage(&img);
  cvReleaseImage(&gray);
  return 0;
}
