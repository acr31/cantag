/**
 * $Header$
 *
 * $Log$
 * Revision 1.4  2004/01/28 17:19:35  acr31
 * providing my own implementation of draw ellipse
 *
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


void DrawEllipse(Image *image,float xc, float yc,float width, float height, float angle_radians, int color, int thickness);
void DrawEllipseArc(Image *image,float xc, float yc,float width, float height, float angle_radians, float start_angle, float end_angle, int color, int thickness);

inline void DrawEllipse(Image *image, Ellipse2D *ellipse, float start_angle, float end_angle, int colour, int thickness) {
  DrawEllipseArc(image,ellipse->m_x,ellipse->m_y,ellipse->m_width,ellipse->m_height,ellipse->m_angle_radians,start_angle,end_angle,colour,thickness);
}

inline void DrawEllipse(Image *image, Ellipse2D *ellipse, int colour, int thickness) {
  DrawEllipse(image,ellipse->m_x,ellipse->m_y,ellipse->m_width,ellipse->m_height,ellipse->m_angle_radians,colour,thickness);
}

inline void DrawFilledEllipse(Image *image, Ellipse2D *ellipse, float start_angle, float end_angle, int colour) {
  DrawEllipseArc(image,ellipse->m_x,ellipse->m_y,ellipse->m_width,ellipse->m_height,ellipse->m_angle_radians,start_angle,end_angle,colour,-1);
}

inline void DrawFilledEllipse(Image *image, Ellipse2D *ellipse, int colour) {
  DrawEllipse(image,ellipse->m_x,ellipse->m_y,ellipse->m_width,ellipse->m_height,ellipse->m_angle_radians,colour,-1);
}



inline void DrawFilledEllipse(Image *image, float x, float y, float width, float height, float angle_radians, int color) {
  DrawEllipse(image,x,y,width, height, angle_radians,color,-1);
}

inline void DrawFilledEllipse(Image *image, float x, float y, float width, float height, float angle_radians, float start_angle, float end_angle, int color) {
  DrawEllipseArc(image,x,y,width, height, angle_radians,start_angle,end_angle,color,-1);
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
  Image* result =  cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
  cvConvertScale(result,result,-1,255);
  return result;
}

inline Image* CreateRGBImage(int width, int height) {
  Image* result =  cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);
  cvConvertScale(result,result,-1,CV_RGB(255,255,255));
  return result;
}

inline void FreeImage(Image** image) {
  cvReleaseImage(image);
}

inline void SaveImage(const char* filename, Image* image) {
  cvSaveImage(filename,image);
}
#endif//DRAWING_GUARD
