/**
 * $Header$
 */
#ifndef IMAGE_GUARD
#define IMAGE_GUARD

#include <tripover/Config.hh>
#include <tripover/Socket.hh>

#include <cmath>

#define COLOUR_BLACK ((unsigned char)0)
#define COLOUR_WHITE ((unsigned char)255)
#define PI M_PI

/**
 * A wrapper object for OpenCv's image
 */
class Image {
private:
  unsigned int m_width;
  unsigned int m_height;
  unsigned char* m_contents;
  bool m_free_contents;
  
  unsigned int m_width_step;

  bool m_binary;

  inline int Round(double d) const {
    double t =(d+6755399441055744.0); 
    return *(int*)&t;
  }
  
public:

  /**
   * Construct an empty image object
   */
  Image() : m_free_contents(false) {}

  /**
   * Construct an image with the specified width and height
   */
  Image(int width, int height);

  /**
   * Construct an image using the buffer offered to store data
   */
  Image(int width, int height, int bytes_per_line, unsigned char* contents);
  
  /**
   * Construct an instance of this image from the network socket given
   */
  Image(Socket& socket);

  /**
   * Copy construct this image from the one offered - this will copy
   * the data in the data buffer too.
   */
  Image(const Image& c);

  /**
   * Load the chosen file from disk and attempt to construct an image
   * from it.  Throws an exception if the file cannot be found or
   * opened.
   */
  Image(char* filename);

  /**
   * Destructor will free the data buffer unless the image was created
   * with an external buffer
   */
  ~Image();


  /**
   * Return a (non-const) pointer to the yth row of this image
   */
  inline unsigned char* GetRow(unsigned int y) {
    return y*m_width_step+m_contents;
  }

  /**
   * Return a const pointer to the yth row of this image
   */
  inline const unsigned char* GetRow(unsigned int y) const {
    return y*m_width_step+m_contents;
  }

  /**
   * Return the number of bytes per line for this image
   */
  inline unsigned int GetWidthStep() const{
    return m_width_step;
  }

  /**
   * \todo remove this
   */
  inline unsigned char* GetDataPointer() {
    return m_contents;
  }
  
  /**
   * \todo remove this
   */
  inline const unsigned char* GetDataPointer() const {
    return m_contents;
  }

  /**
   * Read the pixel at the given x and y co-ordinates. Includes a
   * bounds check---returns 0 if out of range.
   */
  inline unsigned char Sample(unsigned int x, unsigned int y) const {
    return (x < m_width && y < m_height) ? GetRow(y)[x] : 0;
  }

  /**
   * Reads a pixel from the image without a bounds check
   */
  inline unsigned char SampleNoCheck(unsigned int x,unsigned int y) const {
    return GetRow(y)[x];
  }

  /**
   * Read the pixel at the given x and y co-ordinates. Includes a
   * bounds check---returns 0 if out of range.
   */
  inline unsigned char Sample(int x, int y) const {
    return (x >= 0 && y>=0) ? Sample( (unsigned int)x, (unsigned int)y ) : 0;
  }
  
  /**
   * Round the floating point values to the nearest pixel and then
   * sample the image at that point.
   */
  inline unsigned char Sample(float x, float y) const {
    return Sample(Round(x),Round(y));
  }

  /**
   * Mutltiply every point in the image by scalefactor and add on the
   * offset.
   */
  void ConvertScale(float scalefactor, int offset);

  /**
   * Plot a point in the image with the given colour.  x and y are
   * bounds checked; out of range values will be ignored.
   */ 
  inline void DrawPixel(int x,int y, unsigned char colour) {
    if ((x >= 0) && (x < m_width) &&
	(y >= 0) && (y < m_height)) {
      DrawPixelNoCheck(x,y,colour);
    }
  }

  inline void DrawPixelNoCheck(int x,int y, unsigned char colour) {
    (m_contents+m_width_step*y)[x] = colour;
  }

  /**
   * Plot a point in the image with the given colour.  x and y are
   * bounds checked; out of range values will be ignored.
   */ 
  inline void DrawPixel(unsigned int x,unsigned int y, unsigned char colour) {
    if ((x < m_width) &&
	(y < m_height)) {
      DrawPixelNoCheck(x,y,colour);
    }
  }
  
  /**
   * Plot a point in the image with the given colour.  x and y are
   * rounded to the nearest pixel and bounds checked; out of range
   * values will be ignored.
   */ 
  inline void DrawPixel(float x,float y, unsigned char colour) {
    DrawPixel(Round(x),Round(y),colour);
  }
   
  /**
   * Draw a line of given thickness from (x0,y0) to (x1,y1).  
   *
   * \todo thickness is ignored
   *
   * \todo doesn't draw a line!
   */
  void DrawLine(int x0,int y0, int x1,int y1, unsigned char colour, unsigned int thickness);

  /**
   * Round x0,y0,x1,y1 to the nearest integer and draw a line of given
   * thickness from (x0,y0) to (x1,y1).  
   */
  inline void DrawLine(float x0,float y0, float x1,float y1, unsigned char colour, unsigned int thickness) {
    DrawLine(Round(x0),Round(y0),Round(x1),Round(y1),colour,thickness);
  }
  
  /**
   * Draw a point of given size at (x,y).
   *
   * \todo size is ignored
   */
  inline void DrawPoint(int x,int y, unsigned char colour, unsigned int size) {
    DrawPixel(x,y,colour);
  }

  /**
   * Round x and y and draw a point at (x,y)
   */
  inline void DrawPoint(float x,float y, unsigned char colour, unsigned int size) {
    DrawPoint(Round(x),Round(y),colour,size);
  }

  /**
   * Draw a polygon.  points is a pointer to an array of integers,
   * which are x and y co-ordinates consecutivley. i.e. points[2i] is
   * an x co-ordinate and points[2i+1] is the coresponding y
   * co-ordinate.  
   */
  void DrawPolygon(int* points, int numpoints, unsigned char colour, unsigned int thickness);

  /**
   * Draw a polygon by rounding the elements of points to the nearest
   * integer.
   */
  void DrawPolygon(float* points, int numpoints, unsigned char colour, unsigned int thickness);
  
  /**
   * Draw a filled polygon.  points is a pointer to an array of
   * integers which are x and y co-ordinates consecutively
   * i.e. points[2i] is x coord and points[2i+1] is corresponding y
   * coord.
   */
  void DrawFilledPolygon(int* points, int numpoints, unsigned char colour);

  /**
   * Draw a filled polygon by rounding the given points to the nearest
   * integer.
   */
  void DrawFilledPolygon(float* points, int numpoints, unsigned char colour);


  /**
   * Draw a quadtangle from (x0,y0) to (x1,y1) to (x2,y2) to (x3,y3).
   */
  inline void DrawFilledQuadTangle(int x0, int y0,
				   int x1, int y1,
				   int x2, int y2,
				   int x3, int y3,
				   unsigned char colour) {
    int pts[] = { x0,y0,x1,y1, x2,y2,x3,y3 };
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
    int pts[] = { x0,y0,x1,y1, x2,y2, x3,y3 };
    DrawPolygon(pts,4,colour,thickness);
  }
  
  /**
   * Save the current image to disk. The file type is inferred from
   * the filename currently only bmp and jpg are supported.
   */
  void Save(const char* filename) const;

  /**
   * Draw a circle centred on (x0,y0) with given radius.
   */
  inline void DrawCircle(int x0, int y0, int radius, unsigned char colour, unsigned int thickness) {
    DrawEllipse(x0,y0,radius,radius,0,colour,thickness);
  }

  /**
   * Round x0 and y0 to the nearest integer and draw a circle centered
   * on them.
   */
  inline void DrawCircle(float x0, float y0, int radius, unsigned char colour, unsigned int thickness) {
    DrawEllipse(x0,y0,radius,radius,0,colour,thickness);
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
    DrawSector(x0,y0,Round(radius),0,2*PI,colour);
  }

  /**
   * Draw a filled circle rounding x0,y0 and radius to the nearest integers
   */
  inline void DrawFilledCircle(float x0, float y0, float radius, unsigned char colour) {
    DrawSector(Round(x0),Round(y0),Round(radius),0,2*PI,colour);
  }

  /**
   * Draw a sector of the circle centred on x0,y0 and radius. The
   * sector starts at angle start_radians and ends at end_radians.
   * Angles are measured from the horizontal, increasing in an
   * anti-clockwise direction.
   */
  inline void DrawSector(int x0, int y0, int radius, float start_radians, float end_radians, unsigned char colour) {
    DrawEllipseArc(x0,y0,radius/2,radius/2,0,start_radians,end_radians,colour,-1);
  }

  /**
   * Round the radius to the nearest integer before drawing the sector.
   */
  inline void DrawSector(int x0, int y0, float radius, float start_radians, float end_radians, unsigned char colour) {
    DrawSector(x0,y0,Round(radius),start_radians,end_radians,colour);
  }

  /**
   * Round x0,y0 and the radius to the nearest integer before drawing the sector.
   */
  inline void DrawSector(float x0, float y0, float radius, float start_radians, float end_radians, unsigned char colour) {
    DrawSector(Round(x0),Round(y0),Round(radius),start_radians,end_radians,colour);
  }
  
  /**
   * Resize this image to the dimensions of the argument image and write the result into it
   */ 
  void Resize(Image& image) const;

  /**
   * Return the width of the image
   */
  inline int GetWidth() const { return m_width; }

  /**
   * Return the height of the image
   */
  inline int GetHeight() const { return m_height; }

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
  void DrawEllipseArc(int xc, int yc, int width, int height, float angle_radians, float start_angle, float end_angle, unsigned char color, int thickness);

  void DrawEllipseArc(float xc, float yc, float width, float height, float angle_radians, float start_angle, float end_angle, unsigned char color, int thickness);
  
  void DrawEllipse(float xc, float yc, float width, float height, float angle_radians, unsigned char color, int thickness);

  /**
   * Send this image over the network using the socket proffered.
   * Return the number of bytes written
   */
  int Save(Socket& socket) const;

private:
  int AdaptiveWidthStep(int moving_average,int* previous_line,unsigned int i, unsigned int j,unsigned int s, int t);
  void ellipse_polygon_approx(float* points, int startindex, int length, float xc, float yc, float width, float height,  float angle_radians, unsigned char color, int thickness, float start_angle);

};



#endif//IMAGE_GUARD
