#ifndef DRAWING_GUARD
#define DRAWING_GUARD

#include "Config.hh"
#include <cv.h>

typedef IplImage Image;

#undef FILENAME
#define FILENAME "Drawing.hh"

#define PI CV_PI

inline int SampleImage(Image *image,int x, int y) {
  return (int)(((uchar*)(image->imageData + image->widthStep*y))[x]);
}

inline void DrawEllipse(Image *image, float x, float y, float width, float height, float angle_radians, float start_angle, float end_angle, int color, int thickness) {
  PROGRESS("Drawing ellipse. Centre ("<<x<<","<<y<<") Size ("<<width<<","<<height<<") Angle "<<angle_radians<<" Arc ("<<start_angle<<"->"<<end_angle<<") in Colour "<<color);
  cvEllipse(image,
	    cvPoint(cvRound(x), cvRound(y)), 
	    cvSize( cvRound(width*0.5), cvRound(height*0.5)),
	    angle_radians*180/CV_PI,
	    360-start_angle*180/CV_PI,
	    360-end_angle*180/CV_PI,
	    color,
	    thickness);
}

inline void DrawEllipse(Image *image, float x, float y, float width, float height, float angle_radians, int color, int thickness) {
   DrawEllipse(image, x, y, width, height, angle_radians, 0, 2*CV_PI, color,-1);
}

inline void DrawFilledEllipse(Image *image, float x, float y, float width, float height, float angle_radians, int color) {
  DrawEllipse(image,x,y,width, height, angle_radians, 0,2*CV_PI,color,-1);
}

inline void DrawFilledEllipse(Image *image, float x, float y, float width, float height, float angle_radians, float start_angle, float end_angle, int color) {
  DrawEllipse(image,x,y,width, height, angle_radians,start_angle,end_angle,color,-1);
}

#endif//DRAWING_GUARD
