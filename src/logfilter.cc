#include <Config.hh>
#include <Drawing.hh>
#include <logfilter.hh>

#include <cmath>
#include <opencv/cv.h>

void LogFilter(Image *image) {

  // take the log of each pixel in the image
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width;j++) {
      uchar* ptr = (uchar*)(image->imageData + i * image->widthStep + j);
      double result = 45.985904 * log((double)*ptr + 1);
      *ptr = (uchar)result;
    }
  }
  cvSaveImage("debug-save1.jpg",image);

  cvCanny(image,image,128,128,3);

  //  Image* dest = cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_16S,1);
  // then run a sobel 3x3 edge filter
  /*
  cvSobel(image,dest,2,2,3);
  cvSaveImage("debug-save2.jpg",dest);

  for(int i=0;i<dest->height;i++) {
    for(int j=0;j<dest->width;j++) {
      short v = ((short*)(dest->imageData+i*dest->widthStep))[j];
      // convert to unsigned char by shifting right 8 bits and then adding 128
      v  = v>>8;
      v+=128;
      ((uchar*)(image->imageData+i*image->widthStep))[j] = (uchar)v;
    }
  }

  cvReleaseImage(&dest);*/
  cvSaveImage("debug-save2.jpg",image);
}
