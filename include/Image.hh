/**
 * $Header$
 */
#ifndef IMAGE_GUARD
#define IMAGE_GUARD

#include <Config.hh>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cmath>
#define COLOUR_BLACK ((unsigned char)0)
#define COLOUR_WHITE ((unsigned char)255)
#define PI CV_PI

/**
 * A wrapper object for OpenCv's image
 */
class Image {
  
public:
  IplImage* m_image;
  Image(int width, int height);
  Image(const Image& c);
  Image(char* filename);
  ~Image();

  /**
   * Read the pixel at the given x and y co-ordinates. Includes a
   * bounds check---returns 0 if out of range.
   */
  inline unsigned char Sample(unsigned int x, unsigned int y) const {
    if ((x < m_image->width) &&
	(y < m_image->height)) {
      return ((uchar*)(m_image->imageData + m_image->widthStep*y))[x];
    }
    else {
      return 0;
    }
  }

  /**
   * Read the pixel at the given x and y co-ordinates. Includes a
   * bounds check---returns 0 if out of range.
   */
  inline unsigned char Sample(int x, int y) const {
    if ((x >= 0) && (x < m_image->width) &&
	(y >= 0) && (y < m_image->height)) {
      return ((uchar*)(m_image->imageData + m_image->widthStep*y))[x];
    }
    else {
      return 0;
    }
  }

  /**
   * Round the floating point values to the nearest pixel and then
   * sample the image at that point.
   */
  inline unsigned char Sample(float x, float y) const {
    return Sample(cvRound(x),cvRound(y));
  }

  /**
   * Mutltiply every point in the image by scalefactor and add on the
   * offset.
   */
  inline void ConvertScale(float scalefactor, int offset) {
    cvConvertScale(m_image,m_image,scalefactor,offset);
  }
  
  /**
   * Resize the image and store the result in the target image
   */
  inline void Resize(Image& dest) const {
    cvResize(m_image,dest.m_image);
  }

  inline void DrawPixel(int x,int y, unsigned char colour) {
    if ((x >= 0) && (x < m_image->width) &&
	(y >= 0) && (y < m_image->height)) {
      ((uchar*)(m_image->imageData+m_image->widthStep*y))[x] = colour;
    }
  }

  inline void DrawPixel(unsigned int x,unsigned int y, unsigned char colour) {
    if ((x < m_image->width) &&
	(y < m_image->height)) {
      ((uchar*)(m_image->imageData+m_image->widthStep*y))[x] = colour;
    }
  }


  inline void DrawPixel(float x,float y, unsigned char colour) {
    DrawPixel(cvRound(x),cvRound(y),colour);
  }
   
  inline void DrawLine(int x0,int y0, int x1,int y1, unsigned char colour, unsigned int thickness) {
    cvLineAA(m_image,cvPoint(x0,y0),cvPoint(x1,y1),colour,0);
  }

  inline void DrawLine(float x0,float y0, float x1,float y1, unsigned char colour, unsigned int thickness) {
    DrawLine(cvRound(x0),cvRound(y0),cvRound(x1),cvRound(y1),colour,thickness);
  }
  
  inline void DrawPoint(int x,int y, unsigned char colour, unsigned int size) {
    cvLine(m_image,cvPoint(x,y),cvPoint(x,y),colour,size);
  }

  inline void DrawPoint(float x,float y, unsigned char colour, unsigned int size) {
    DrawPoint(cvRound(x),cvRound(y),colour,size);
  }

  inline void DrawPolygon(int* points, int numpoints, unsigned char colour, unsigned int thickness) {
    CvPoint pts[numpoints];
    for(int i=0;i<numpoints;i++) {
      pts[i] = cvPoint(points[2*i],points[2*i+1]);
    }
    int npts[] = {numpoints};
    CvPoint* ppts = pts;
    cvPolyLineAA(m_image,&ppts,npts,1,1,colour,0);
  }

  inline void DrawPolygon(float* points, int numpoints, unsigned char colour, unsigned int thickness) {
    CvPoint pts[numpoints];
    for(int i=0;i<numpoints;i++) {
      pts[i] = cvPoint(cvRound(points[2*i]),cvRound(points[2*i+1]));
    }
    int npts[] = {numpoints};
    CvPoint* ppts = pts;
    cvPolyLineAA(m_image,&ppts,npts,1,1,colour,0);
  }
  
  inline void DrawFilledPolygon(int* points, int numpoints, unsigned char colour) {
    CvPoint pts[numpoints];
    for(int i=0;i<numpoints;i++) {
      pts[i] = cvPoint(points[2*i],points[2*i+1]);
    }
    cvFillConvexPoly(m_image,pts,numpoints,colour);
    int npts[] = {numpoints};
    CvPoint* ppts = pts;
    cvPolyLineAA(m_image,&ppts,npts,1,1,colour,0);
  }

  inline void DrawFilledPolygon(float* points, int numpoints, unsigned char colour) {
    CvPoint pts[numpoints];
    for(int i=0;i<numpoints;i++) {
      pts[i] = cvPoint(cvRound(points[2*i]),cvRound(points[2*i+1]));
    }
    cvFillConvexPoly(m_image,pts,numpoints,colour);
    int npts[] = {numpoints};
    CvPoint* ppts = pts;
    cvPolyLineAA(m_image,&ppts,npts,1,1,colour,0);
  }

  inline void DrawFilledQuadTangle(int x0, int y0,
				   int x1, int y1,
				   int x2, int y2,
				   int x3, int y3,
				   unsigned char colour) {
    int pts[] = { x0,y0,
		  x1,y1,
		  x2,y2,
		  x3,y3 };
    DrawFilledPolygon(pts,4,colour);
  }

  inline void Save(const char* filename) const {
    cvSaveImage(filename,m_image);
  }

  inline void DrawCircle(int x0, int y0, int radius, unsigned char colour, unsigned int thickness) {
    cvCircle(m_image,cvPoint(x0,y0),radius,colour,thickness);
  }

  inline void DrawCircle(float x0, float y0, int radius, unsigned char colour, unsigned int thickness) {
    DrawCircle(cvRound(x0),cvRound(y0),radius,colour,thickness);
  }

  inline void DrawFilledCircle(int x0, int y0, int radius, unsigned char colour) {
    //    cvCircle(m_image,cvPoint(x0,y0),radius,colour,-1);
    DrawSector(x0,y0,radius,0,2*PI,colour);
  }

  inline void DrawFilledCircle(int x0, int y0, float radius, unsigned char colour) {
    //    DrawFilledCircle(x0,y0,cvRound(radius),colour);
    DrawSector(x0,y0,cvRound(radius),0,2*PI,colour);
  }

  inline void DrawFilledCircle(float x0, float y0, float radius, unsigned char colour) {
    //    DrawFilledCircle(cvRound(x0),cvRound(y0),cvRound(radius),colour);
    DrawSector(cvRound(x0),cvRound(y0),cvRound(radius),0,2*PI,colour);
  }

  inline void DrawSector(int x0, int y0, int radius, float start_radians, float end_radians, unsigned char colour) {
    int numsteps = (int)(100/fabs(start_radians - end_radians)*2*PI);
    CvPoint pts[numsteps+1];
    for(int i=0;i<numsteps;i++) {
      float theta = start_radians + (end_radians - start_radians)*(float)i/(float)(numsteps-1);
      pts[i] = cvPoint(x0 + cvRound(radius*cos(theta)), y0 + cvRound(radius*sin(theta)));
    }
    pts[numsteps] = cvPoint(x0,y0);
    cvFillConvexPoly(m_image,pts,numsteps+1,colour);
    int npts[] = {numsteps+1};
    CvPoint* ppts = pts;
    cvPolyLineAA(m_image,&ppts,npts,1,1,colour,0);
  }

  inline void DrawSector(int x0, int y0, float radius, float start_radians, float end_radians, unsigned char colour) {
    DrawSector(x0,y0,cvRound(radius),start_radians,end_radians,colour);
  }

  inline void DrawSector(float x0, float y0, float radius, float start_radians, float end_radians, unsigned char colour) {
    DrawSector(cvRound(x0),cvRound(y0),cvRound(radius),start_radians,end_radians,colour);
  }

  inline int GetWidth() const { return m_image->width; }
  inline int GetHeight() const { return m_image->height; }

  inline void SetROI(int x0, int y0, int x1, int y1) {
    assert(x0<x1 && y0<y1);
    cvSetImageROI(m_image,cvRect(x0,y0,x1-x0,y1-y0));
  }

  /**
   * Apply a global threshold to binarize the image. All pixels with
   * intensity greater than the threshold will be set to 0 and all
   * pixels less than the threshold will be set to 1.  In particular,
   * this method inverts as well as binarizes.
   */
  void GlobalThreshold(unsigned char threshold);

  /**
   * Apply an adaptive threshold to the image.  For each pixel a
   * region of window_size pixels around it is averaged.  If the pixel
   * is greater than this average+offset then it is changed to 0 and
   * otherwise changed to 1.  Note that this method is also inverting
   * the image in addition to binarizing it.
   */
  void AdaptiveThreshold(unsigned int window_size, unsigned char offset);

  /**
   * Apply a homogonous transform based image segmentation technique.
   * Takes the log of each image pixel,scales it and applies an edge
   * detect filter.  Suitable for finding edges but not for reading the tag.
   */
  void HomogenousTransform();

  /**
   * Apply noise to the image as if it had been acquired through a CCD array
   */
  void AddNoise(float mean, float stddev);


private:
  void AdaptiveWidthStep(float* moving_average,float* previous_line,unsigned int i, unsigned int j,unsigned int s, float t);
};


#endif//IMAGE_GUARD
