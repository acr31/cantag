/**
 * $Header$
 *
 * $Log$
 * Revision 1.4  2004/01/24 19:29:23  acr31
 * removed ellipsetoxy and put the project method in Ellipse2D objects
 *
 * Revision 1.3  2004/01/23 18:18:11  acr31
 * added Matrix Tag and a test program and the beginning of the CRC coder
 *
 * Revision 1.2  2004/01/21 11:55:07  acr31
 * added keywords for substitution
 *
 */
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

inline int SampleImage(Image *image,float x, float y) {
  return (int)(((uchar*)(image->imageData + image->widthStep*(int)y))[(int)x]);
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

inline void DrawFilledQuadTangle(Image *image, 
				 float x0, float y0,
				 float x1, float y1,
				 float x2, float y2,
				 float x3, float y3,
				 int color) {
  PROGRESS("Drawing filled quad tangle ("<<x0<<","<<y0<<") ("<<x1<<","<<y1<<") ("<<x2<<","<<y2<<") ("<<x3<<","<<y3<<") Colour " << color);
  CvPoint p[4] = { cvPoint((int)x0,(int)y0),
		   cvPoint((int)x1,(int)y1),
		   cvPoint((int)x2,(int)y2),
		   cvPoint((int)x3,(int)y3) };
  cvFillConvexPoly(image,p,4,color);
}
			   
#endif//DRAWING_GUARD
