/**
 * $Header$
 *
 * $Log$
 * Revision 1.3  2004/01/27 18:06:58  acr31
 * changed inheriting classes to inherit publicly from their parents
 *
 * Revision 1.2  2004/01/26 08:55:36  acr31
 * added some new drawing method for Ellipse2D's and Rectangle2D's
 *
 * Revision 1.1  2004/01/25 14:54:36  acr31
 * moved over to automake/autoconf build system
 *
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
#include "Ellipse2D.hh"
#include "Rectangle2D.hh"
#include <opencv/cv.h>

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

/**
 * Draw an ellipse with centre x,y and given width and height - note
 * width and height are twice the size of the major and minor axes
 * they represent.  angle_radians is the angle between the horizontal
 * and the axis given by width
 */
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

inline void DrawEllipse(Image *image, Ellipse2D *ellipse, float start_angle, float end_angle, int colour, int thickness) {
  DrawEllipse(image,ellipse->m_x,ellipse->m_y,ellipse->m_width,ellipse->m_height,ellipse->m_angle_radians,start_angle,end_angle,colour,thickness);
}

inline void DrawEllipse(Image *image, Ellipse2D *ellipse, int colour, int thickness) {
  DrawEllipse(image,ellipse->m_x,ellipse->m_y,ellipse->m_width,ellipse->m_height,ellipse->m_angle_radians,0,2*PI,colour,thickness);
}

inline void DrawFilledEllipse(Image *image, Ellipse2D *ellipse, float start_angle, float end_angle, int colour) {
  DrawEllipse(image,ellipse->m_x,ellipse->m_y,ellipse->m_width,ellipse->m_height,ellipse->m_angle_radians,start_angle,end_angle,colour,-1);
}

inline void DrawFilledEllipse(Image *image, Ellipse2D *ellipse, int colour) {
  DrawEllipse(image,ellipse->m_x,ellipse->m_y,ellipse->m_width,ellipse->m_height,ellipse->m_angle_radians,0,2*PI,colour,-1);
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

inline void DrawFilledQuadTangle(Image* image,
				 Rectangle2D* r,
				 int colour) {
  DrawFilledQuadTangle(image,
		       r->m_x0,r->m_y0,
		       r->m_x1,r->m_y1,
		       r->m_x2,r->m_y2,
		       r->m_x3,r->m_y3,
		       colour);
}


inline Image* CreateImage(int width, int height) {
  return cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
}

inline Image* CreateRGBImage(int width, int height) {
  return cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);
}

inline void FreeImage(Image** image) {
  cvReleaseImage(image);
}

inline void SaveImage(const char* filename, Image* image) {
  cvSaveImage(filename,image);
}
#endif//DRAWING_GUARD
