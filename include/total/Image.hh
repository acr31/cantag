/**
 * $Header$
 */
#ifndef IMAGE_GUARD
#define IMAGE_GUARD

#include <tripover/Config.hh>
#include <tripover/Socket.hh>

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
private:
  bool m_from_header;  // set to true if this image is created to
		       // point to an existing buffer of data
  uchar* m_contents;   
  bool m_free_contents;   // if true then should m_contents be free'd
  uchar* m_row_ptr;
  IplImage* m_image; 
public:

  Image();
  Image(int width, int height);
  Image(int width, int height, uchar* contents);
  
  /**
   * Construct an instance of this image from the network socket given
   */
  Image(Socket& socket);
  Image(const Image& c);
  Image(char* filename);
  ~Image();

  inline unsigned char* GetRow(unsigned int y) {
    return (uchar*)(y*m_image->widthStep+m_image->imageData);
  }

  inline const unsigned char* GetRow(unsigned int y) const {
    return (const uchar*)(y*m_image->widthStep+m_image->imageData);
  }

  inline unsigned int GetWidthStep() {
    return m_image->widthStep;
  }

  inline unsigned char* GetDataPointer() {
    return (uchar*)m_image->imageData;
  }

  inline const unsigned char* GetDataPointer() const {
    return (const uchar*)m_image->imageData;
  }
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

  inline unsigned char SampleNoCheck(unsigned int x,unsigned int y) const {
    return ((uchar*)(m_image->imageData + m_image->widthStep*y))[x];
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

  /**
   * Plot a point in the image with the given colour.  x and y are
   * bounds checked; out of range values will be ignored.
   */ 
  inline void DrawPixel(int x,int y, unsigned char colour) {
    if ((x >= 0) && (x < m_image->width) &&
	(y >= 0) && (y < m_image->height)) {
      ((uchar*)(m_image->imageData+m_image->widthStep*y))[x] = colour;
    }
  }

  inline void DrawPixelNoCheck(int x,int y, unsigned char colour) {
    ((uchar*)(m_image->imageData+m_image->widthStep*y))[x] = colour;
  }

  /**
   * Plot a point in the image with the given colour.  x and y are
   * bounds checked; out of range values will be ignored.
   */ 
  inline void DrawPixel(unsigned int x,unsigned int y, unsigned char colour) {
    if ((x < m_image->width) &&
	(y < m_image->height)) {
      ((uchar*)(m_image->imageData+m_image->widthStep*y))[x] = colour;
    }
  }
  
  /**
   * Plot a point in the image with the given colour.  x and y are
   * rounded to the nearest pixel and bounds checked; out of range
   * values will be ignored.
   */ 
  inline void DrawPixel(float x,float y, unsigned char colour) {
    DrawPixel(cvRound(x),cvRound(y),colour);
  }
   
  /**
   * Draw a line of given thickness from (x0,y0) to (x1,y1).  Current
   * method is an antialiased line and thickess is ignored.
   */
  inline void DrawLine(int x0,int y0, int x1,int y1, unsigned char colour, unsigned int thickness) {
    cvLine(m_image,cvPoint(x0,y0),cvPoint(x1,y1),colour,0);
  }

  /**
   * Round x0,y0,x1,y1 to the nearest integer and draw a line of given
   * thickness from (x0,y0) to (x1,y1).  Current method is an
   * antialiased line and thickess is ignored.
   */
  inline void DrawLine(float x0,float y0, float x1,float y1, unsigned char colour, unsigned int thickness) {
    DrawLine(cvRound(x0),cvRound(y0),cvRound(x1),cvRound(y1),colour,thickness);
  }
  
  /**
   * Draw a point of given size at (x,y).
   */
  inline void DrawPoint(int x,int y, unsigned char colour, unsigned int size) {
    cvLine(m_image,cvPoint(x,y),cvPoint(x,y),colour,size);
  }

  /**
   * Round x and y and draw a point at (x,y)
   */
  inline void DrawPoint(float x,float y, unsigned char colour, unsigned int size) {
    DrawPoint(cvRound(x),cvRound(y),colour,size);
  }

  /**
   * Draw a polygon.  points is a pointer to an array of integers,
   * which are x and y co-ordinates consecutivley. i.e. points[2i] is
   * an x co-ordinate and points[2i+1] is the coresponding y
   * co-ordinate.  The polygon is currently drawn antialiased and the
   * thickness is ignored.
   */
  inline void DrawPolygon(int* points, int numpoints, unsigned char colour, unsigned int thickness) {
    CvPoint pts[numpoints];
    for(int i=0;i<numpoints;i++) {
      pts[i] = cvPoint(points[2*i],points[2*i+1]);
    }
    int npts[] = {numpoints};
    CvPoint* ppts = pts;
    //cvPolyLineAA(m_image,&ppts,npts,1,1,colour,0);
        cvPolyLine(m_image,&ppts,npts,1,1,colour,1,8);
  }

  /**
   * Draw a polygon by rounding the elements of points to the nearest
   * integer.
   */
  inline void DrawPolygon(float* points, int numpoints, unsigned char colour, unsigned int thickness) {
    CvPoint pts[numpoints];
    for(int i=0;i<numpoints;i++) {
      pts[i] = cvPoint(cvRound(points[2*i]),cvRound(points[2*i+1]));
    }
    int npts[] = {numpoints};
    CvPoint* ppts = pts;
    cvPolyLine(m_image,&ppts,npts,1,1,colour,0);
  }
  
  /**
   * Draw a filled polygon.  points is a pointer to an array of
   * integers which are x and y co-ordinates consecutively
   * i.e. points[2i] is x coord and points[2i+1] is corresponding y
   * coord.
   */
  inline void DrawFilledPolygon(int* points, int numpoints, unsigned char colour) {
    CvPoint pts[numpoints];
    for(int i=0;i<numpoints;i++) {
      pts[i] = cvPoint(points[2*i],points[2*i+1]);
    }
    cvFillConvexPoly(m_image,pts,numpoints,colour);
    int npts[] = {numpoints};
    CvPoint* ppts = pts;
    cvPolyLine(m_image,&ppts,npts,1,1,colour,0);
  }

  /**
   * Draw a filled polygon by rounding the given points to the nearest
   * integer.
   */
  inline void DrawFilledPolygon(float* points, int numpoints, unsigned char colour) {
    CvPoint pts[numpoints];
    for(int i=0;i<numpoints;i++) {
      pts[i] = cvPoint(cvRound(points[2*i]),cvRound(points[2*i+1]));
    }
    cvFillConvexPoly(m_image,pts,numpoints,colour);
    int npts[] = {numpoints};
    CvPoint* ppts = pts;
    cvPolyLine(m_image,&ppts,npts,1,1,colour,0);
  }

  /**
   * Draw a quadtangle from (x0,y0) to (x1,y1) to (x2,y2) to (x3,y3).
   */
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
  
  /**
   * Draw a quadtangle from (x0,y0) to (x1,y1) to (x2,y2) to (x3,y3).
   */
  inline void DrawQuadTangle(int x0, int y0,
			     int x1, int y1,
			     int x2, int y2,
			     int x3, int y3,
			     unsigned char colour,
			     unsigned int thickness) {
    int pts[] = { x0,y0,
		  x1,y1,
		  x2,y2,
		  x3,y3 };
    DrawPolygon(pts,4,colour,thickness);
  }
  
  /**
   * Save the current image to disk. The file type is inferred from
   * the filename currently only bmp and jpg are supported.
   */
  inline void Save(const char* filename) const {
    cvSaveImage(filename,m_image);
  }

  /**
   * Draw a circle centred on (x0,y0) with given radius.
   */
  inline void DrawCircle(int x0, int y0, int radius, unsigned char colour, unsigned int thickness) {
    cvCircle(m_image,cvPoint(x0,y0),radius,colour,thickness);
  }

  /**
   * Round x0 and y0 to the nearest integer and draw a circle centered
   * on them.
   */
  inline void DrawCircle(float x0, float y0, int radius, unsigned char colour, unsigned int thickness) {
    DrawCircle(cvRound(x0),cvRound(y0),radius,colour,thickness);
  }

  /**
   * Draw a filled circle centred on x0,y0
   */
  inline void DrawFilledCircle(int x0, int y0, int radius, unsigned char colour) {
    DrawSector(x0,y0,radius,0,2*PI,colour);
  }

  /**
   * Draw a filled circle centred on x0,y0, the radius is rounded to the nearest integer
   */
  inline void DrawFilledCircle(int x0, int y0, float radius, unsigned char colour) {
    DrawSector(x0,y0,cvRound(radius),0,2*PI,colour);
  }

  /**
   * Draw a filled circle rounding x0,y0 and radius to the nearest integers
   */
  inline void DrawFilledCircle(float x0, float y0, float radius, unsigned char colour) {
    DrawSector(cvRound(x0),cvRound(y0),cvRound(radius),0,2*PI,colour);
  }

  /**
   * Draw a sector of the circle centred on x0,y0 and radius. The
   * sector starts at angle start_radians and ends at end_radians.
   * Angles are measured from the horizontal, increasing in an
   * anti-clockwise direction.
   */
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
    cvPolyLine(m_image,&ppts,npts,1,1,colour,0);
  }

  /**
   * Round the radius to the nearest integer before drawing the sector.
   */
  inline void DrawSector(int x0, int y0, float radius, float start_radians, float end_radians, unsigned char colour) {
    DrawSector(x0,y0,cvRound(radius),start_radians,end_radians,colour);
  }

  /**
   * Round x0,y0 and the radius to the nearest integer before drawing the sector.
   */
  inline void DrawSector(float x0, float y0, float radius, float start_radians, float end_radians, unsigned char colour) {
    DrawSector(cvRound(x0),cvRound(y0),cvRound(radius),start_radians,end_radians,colour);
  }
  
  /**
   * Return the width of the image
   */
  inline int GetWidth() const { return m_image->width; }

  /**
   * Return the height of the image
   */
  inline int GetHeight() const { return m_image->height; }

  /**
   * Set the image region of interest.  Various algorithms will apply
   * only to this region.
   */
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
  unsigned char GlobalThreshold(unsigned char threshold);

  /**
   * Apply an adaptive threshold to the image.  For each pixel a
   * region of window_size pixels around it is averaged.  If the pixel
   * is greater than this average+offset then it is changed to 0 and
   * otherwise changed to 1.  Note that this method is also inverting
   * the image in addition to binarizing it.
   */
  void AdaptiveThreshold(const unsigned int window_size, const unsigned char offset);
  void AdaptiveThreshold2(unsigned int window_size, unsigned char offset);

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

  /**
   * Draw an ellipse with centre x,y and given width and height - note
   * width and height are twice the size of the major and minor axes
   * they represent.  angle_radians is the angle between the horizontal
   * and the axis given by width
   */
  void DrawEllipseArc(float xc, float yc, float width, float height, float angle_radians, float start_angle, float end_angle, int color, int thickness);
  
  void DrawEllipse(float xc, float yc, float width, float height, float angle_radians, int color, int thickness);

  /**
   * Send this image over the network using the socket proffered.
   * Return the number of bytes written
   */
  int Save(Socket& socket) const;

private:
  int AdaptiveWidthStep(int moving_average,int* previous_line,unsigned int i, unsigned int j,unsigned int s, int t);
  void ellipse_polygon_approx(CvPoint* points, int startindex, int length, float xc, float yc, float width, float height,  float angle_radians, int color, int thickness, float start_angle);

};



#endif//IMAGE_GUARD
