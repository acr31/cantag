/*
  Copyright (C) 2004 Andrew C. Rice
  Copyright (C) 2005 Alastair R. Beresford

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */
#ifndef IMAGE_GUARD
#define IMAGE_GUARD

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
}

#include <fstream>
#include <list>
#include <iostream>

#ifdef HAVE_BOOST_RANDOM
# include <boost/random.hpp>
#endif

#include <cantag/Config.hh>
#include <cantag/Socket.hh>
#include <cantag/SpeedMath.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/entities/Entity.hh>
#include <cmath>
#include <cassert>

#define COLOUR_BLACK ((unsigned char)0)
#define COLOUR_WHITE ((unsigned char)255)

namespace Cantag {


  /*
   * Overview of the Image class
   * 
   * The image class supports various different image formats
   *
   * In order to achieve good performance from functions such as DrawPixel()
   * we don't want the overhead of a virtual method call. Therefore we cannot use
   * the standard "base class with inherited classes" to specialise for different image formats
   *
   * Instead we do something slightly more complicated...
   *
   * 1) Define a new namespace, called "Colour" which holds both an enumeration for the supported
   * image formats, as well as a const array declaring the size of each pixel in this format.
   *
   * 2) Declare a protected base class called ImageBase which holds data and functions common to all 
   * image formats.
   *
   * 3) Derive partially specialised, templated, classes which inherit the ImageBase and 
   * which support functions which require knowledge of the type of image being supported.
   *
   * 4) Define a final templated "Image" class which inherits from the specialised classes and defines
   * algorithms which function in a general way.
   * (i.e. these are algorithms which depend on specialised routines but which are not
   * in and of themselves specialised). 
   * *** Note: These functions cannot go into the base class, since they depend on the routines 
   * ***       expressed in the specialised classes.
   *
   * This means that users of the library can use it as if there exists one single templated 
   * Image class, however we get code sharing easily, and functions which cannot be called 
   * for a particular image format do not exist (and therefore receive a compile time warning).
   * 
   */

  namespace Colour {
    enum Type {RGB=0,Grey=1,Mono=2};
  }

  /**
   * A wrapper object for an image
   */
  class ImageBase : public Entity {

  protected:
    unsigned int m_width;
    unsigned int m_height;
    unsigned char* m_contents;
    bool m_free_contents;
    unsigned int m_width_step;


    //FIXME: legacy and should probably be removed
    bool m_binary;

    /**
     * Constructs new image using pre-allocated storage
     */
    ImageBase(int width, int height, int width_step, unsigned char* contents, bool own) :
      m_width(width), m_height(height), m_contents(contents), 
      m_free_contents(own), m_width_step(width_step), m_binary(false) {SetValid(true);}
    ImageBase() :
      m_width(0), m_height(0), m_contents(0), 
      m_free_contents(false), m_width_step(0), m_binary(false) {SetValid(false);}

    ImageBase(const ImageBase& image) : m_width(image.m_width), m_height(image.m_height), m_contents(new unsigned char[image.m_width_step*image.m_height]), m_free_contents(true), m_width_step(image.m_width_step),m_binary(false) { 
      SetValid(true); 
      memcpy(m_contents,image.m_contents,image.m_width_step*image.m_height);
    }


    ~ImageBase() {
      if (m_free_contents) {
	delete[] m_contents;
      }    
    }

  public:

    /**
     * Return the width of the image
     */
    inline int GetWidth() const { return m_width; }

    /**
     * Return the height of the image
     */
    inline int GetHeight() const { return m_height; }


    /**
     * Return the number of bytes per line of an image
     */ 
    inline int GetWidthStep() const { return m_width_step; }
  };

  /**
   * Empty unspecialised class to enable specialisation through templates
   */
  template<Colour::Type ColType> class ImageSpecialise : public ImageBase {};

  /**
   * Functions whose body is specific to RGB images are placed in here.
   */
  template<> class ImageSpecialise<Colour::RGB> : public ImageBase { 
  protected:
    ImageSpecialise(int w,int h): ImageBase(w,h,w*Bpp,new unsigned char[Bpp*w*h],false) {
      memset(m_contents,255,w*h*Bpp);
    }

    ImageSpecialise(int w, int h, int w_step, unsigned char* c) : ImageBase(w, h, w_step, c, false) {}

    ImageSpecialise(const ImageSpecialise<Colour::RGB>& image) : ImageBase(image) {}

    ImageSpecialise() : ImageBase() {}
  public:
    /**
     * Number of bytes per pixel
     */
    static const unsigned int Bpp=3;

    /**
     * Write a pixel to the image without a bounds check
     */
    inline void DrawPixelNoCheck(int x,int y, unsigned int colour) {
      *(m_contents+(m_width_step*y+x)*Bpp) =   (unsigned char) ((colour >> 16) & 0xff);
      *(m_contents+(m_width_step*y+x)*Bpp+1) = (unsigned char) ((colour >> 8)  & 0xff);
      *(m_contents+(m_width_step*y+x)*Bpp+2) = (unsigned char) (colour         & 0xff);
    }

    /**
     * Reads a pixel from the image without a bounds check
     */
    inline unsigned int SampleNoCheck(unsigned int x,unsigned int y) const {
      unsigned char r = *(m_contents+m_width_step*y*Bpp);
      unsigned char g = *(m_contents+m_width_step*y*Bpp+1);
      unsigned char b = *(m_contents+m_width_step*y*Bpp+2);
      int col = r; 
      col <<= 8; 
      col ^= g; 
      col <<= 8; 
      col ^= b;
      return col;
    }
  
    /**
     * Return a (non-const) pointer to the yth row of this image
     * Remember that this is raw interface to the underlying data structure
     * - Check number of bytes per pixel with "Bpp" member variable
     */
    inline unsigned char* GetRow(unsigned int y) {
      return Bpp*y*m_width_step+m_contents;
    }

    /**
     * Return a const pointer to the yth row of this image
     * Remember that this is raw interface to the underlying data structure
     * - Check number of bytes per pixel with "Bpp" member variable
     */
    inline const unsigned char* GetRow(unsigned int y) const {
      return Bpp*y*m_width_step+m_contents;
    }
    
    void Load(const char* filename);
    void Save(const char* filename) const;
  };

  /**
   * Functions whose body is specific to Grey images are placed in here.
   */
  template<> class ImageSpecialise<Colour::Grey> : public ImageBase { 
  protected:
    ImageSpecialise(int w,int h): ImageBase(w,h,w,new unsigned char[w*h],true) {
      memset(m_contents,255,w*h*Bpp);
    }

    ImageSpecialise(int w, int h, int w_step, unsigned char* c) : ImageBase(w, h, w_step, c, false) {}

    ImageSpecialise(const ImageSpecialise<Colour::Grey>& image) : ImageBase(image) {};

    ImageSpecialise() : ImageBase() {};

  public:
     /**
     * Number of bytes per pixel
     */
    static const unsigned int Bpp=1;

    /**
     * Write a pixel to the image without a bounds check
     */
    inline void DrawPixelNoCheck(int x,int y, unsigned int colour) {
      *(m_contents+(m_width_step*y+x)*Bpp) = (unsigned char) (colour & 0xff);
    }

    /**
     * Return a (non-const) pointer to the yth row of this image
     * Remember that this is raw interface to the underlying data structure
     * - Check number of bytes per pixel with "Bpp" member variable
     */
    inline unsigned char* GetRow(unsigned int y) {
      return Bpp*y*m_width_step+m_contents;
    }

    /**
     * Return a const pointer to the yth row of this image
     * Remember that this is raw interface to the underlying data structure
     * - Check number of bytes per pixel with "Bpp" member variable
     */
    inline const unsigned char* GetRow(unsigned int y) const {
      return Bpp*y*m_width_step+m_contents;
    }

    /**
     * Reads a pixel from the image without a bounds check
     */
    inline unsigned int SampleNoCheck(unsigned int x,unsigned int y) const {
      return (unsigned int) (m_contents+m_width_step*y*Bpp);
    }

    void Load(const char* filename);
    void Save(const char* filename) const;
  };

  /**
   * Functions whose body is non-specific to image type, but rely on
   * functions which are specialised, go in here.
   */
  template<Colour::Type ColType> class Image : public ImageSpecialise<ColType> {
  private:
  /**
   * Sadly gcc4 is too stupid to infer that parent class variables & methods exist
   * so we have to manually tell the compiler where to find the names; 
   * typedef makes this manual defn easier (well, at least more succinct).
   */
  typedef ImageSpecialise<ColType> s;

  public:
    Image() : ImageSpecialise<ColType>() {}
    Image(int width, int height) : ImageSpecialise<ColType>(width,height) {}
    Image(int width, int height, int width_step, unsigned char* contents) : ImageSpecialise<ColType>(width, height, width_step, contents) {}
    Image(const Image<ColType>& image) : ImageSpecialise<ColType>(image) {};

    /**
     * Attempts to construct a new image, reading a file from disk
     * Throws exception if file doesn't exist, or format is not understandable
     */
    Image(char* filename) {s::Load(filename); };

    /**
     * Constructs an image from a MonochromeImage
     * This function is deprecated. Please remove as soon as dependancies are gone
     */
    Image(const MonochromeImage& mono, const char blackval); 

    /**
     * Read the pixel at the given x and y co-ordinates. Includes a
     * bounds check---returns 0 if out of range.
     */
    inline unsigned char Sample(unsigned int x, unsigned int y) const {
      return (x < s::m_width && y < s::m_height) ? s::SampleNoCheck(x,y) : 0;
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
     * Bitwise AND this mask with every pixel
     */
    void Mask(unsigned char mask);

    /**
     * Plot a point in the image with the given colour.  x and y are
     * bounds checked; out of range values will be ignored.
     */ 
    inline void DrawPixel(int x,int y, unsigned int colour) {
      if ((x >= 0) && (x < (int)s::m_width) &&
	  (y >= 0) && (y < (int)s::m_height)) {
	s::DrawPixelNoCheck(x,y,colour);
      }
    }

    void SeedFill(int x, int y,unsigned int colour);

    /**
     * Plot a point in the image with the given colour.  x and y are
     * bounds checked; out of range values will be ignored.
     */ 
    inline void DrawPixel(unsigned int x,unsigned int y, unsigned int colour) {
      if ((x < s::m_width) &&
	  (y < s::m_height)) {
	s::DrawPixelNoCheck(x,y,colour);
      }
    }
  
    /**
     * Plot a point in the image with the given colour.  x and y are
     * rounded to the nearest pixel and bounds checked; out of range
     * values will be ignored.
     */ 
    inline void DrawPixel(float x,float y, unsigned int colour) {
      DrawPixel(Round(x),Round(y),colour);
    }
   
    /**
     * Draw a line of given thickness from (x0,y0) to (x1,y1).  
     *
     * \todo thickness is ignored
     *
     * \todo doesn't draw a line!
     */
    void DrawLine(int x0,int y0, int x1,int y1, unsigned int colour, unsigned int thickness);

    /**
     * Round x0,y0,x1,y1 to the nearest integer and draw a line of given
     * thickness from (x0,y0) to (x1,y1).  
     */
    inline void DrawLine(float x0,float y0, float x1,float y1, unsigned int colour, unsigned int thickness) {
      DrawLine(Round(x0),Round(y0),Round(x1),Round(y1),colour,thickness);
    }
  
    /**
     * Draw a point of given size at (x,y).
     *
     * \todo size is ignored
     */
    inline void DrawPoint(int x,int y, unsigned int colour, unsigned int size) {
      DrawPixel(x,y,colour);
      DrawPixel(x+1,y,colour);
      DrawPixel(x,y+1,colour);
      DrawPixel(x-1,y,colour);
      DrawPixel(x,y-1,colour);
    }

    /**
     * Round x and y and draw a point at (x,y)
     */
    inline void DrawPoint(float x,float y, unsigned int colour, unsigned int size) {
      DrawPoint(Round(x),Round(y),colour,size);
    }

    /**
     * Draw a polygon.  points is a pointer to vector of floats
     * which are x and y co-ordinates consecutivley.
     */
    void DrawPolygon(const std::vector<float>& points, unsigned int colour, unsigned int thickness);

    /**
     * Draw a polygon.  points is a pointer to an array of integers,
     * which are x and y co-ordinates consecutivley. i.e. points[2i] is
     * an x co-ordinate and points[2i+1] is the coresponding y
     * co-ordinate.  
     */
    void DrawPolygon(int* points, int numpoints, unsigned int colour, unsigned int thickness);

    /**
     * Draw a polygon by rounding the elements of points to the nearest
     * integer.
     */
    void DrawPolygon(float* points, int numpoints, unsigned int colour, unsigned int thickness);
  
    /**
     * Draw a filled polygon.  points is a pointer to an array of
     * integers which are x and y co-ordinates consecutively
     * i.e. points[2i] is x coord and points[2i+1] is corresponding y
     * coord.
     */
    void DrawFilledPolygon(int* points, int numpoints, unsigned int colour);

    void DrawFilledPolygon(const std::vector<float>& points, unsigned int colour);
    /**
     * Draw a filled polygon by rounding the given points to the nearest
     * integer.
     */
    void DrawFilledPolygon(float* points, int numpoints, unsigned int colour);


    /**
     * Draw a quadtangle from (x0,y0) to (x1,y1) to (x2,y2) to (x3,y3).
     */
    inline void DrawFilledQuadTangle(int x0, int y0,
				     int x1, int y1,
				     int x2, int y2,
				     int x3, int y3,
				     unsigned int colour) {
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
			       unsigned int colour,
			       unsigned int thickness) {
      int pts[] = { x0,y0,x1,y1, x2,y2, x3,y3 };
      DrawPolygon(pts,4,colour,thickness);
    }
  
    /**
     * Draw a circle centred on (x0,y0) with given radius.
     */
    inline void DrawCircle(int x0, int y0, int radius, unsigned int colour, unsigned int thickness) {
      DrawEllipse(x0,y0,2*radius,2*radius,0,colour,thickness);
    }

    /**
     * Round x0 and y0 to the nearest integer and draw a circle centered
     * on them.
     */
    inline void DrawCircle(float x0, float y0, int radius, unsigned int colour, unsigned int thickness) {
      DrawEllipse(x0,y0,(float)(2*radius),(float)(2*radius),0,colour,thickness);
    }

    /**
     * Draw a filled circle centred on x0,y0
     */
    inline void DrawFilledCircle(int x0, int y0, int radius, unsigned int colour) {
      DrawEllipse(x0,y0,2*radius,2*radius,0,colour,-1);
    }

    /**
     * Draw a filled circle centred on x0,y0
     */
    inline void DrawFilledCircle(int x0, int y0, float radius, unsigned int colour) {
      DrawEllipse((float)x0,(float)y0,2*radius,2*radius,0,colour,-1);
    }

    /**
     * Draw a filled circle centred on x0,y0
     */
    inline void DrawFilledCircle(float x0, float y0, float radius, unsigned int colour) {
      DrawEllipse(x0,y0,2*radius,2*radius,0,colour,-1);
    }

    /**
     * Draw a sector of the circle centred on x0,y0 and radius. The
     * sector starts at angle start_radians and ends at end_radians.
     * Angles are measured from the horizontal, increasing in an
     * anti-clockwise direction.
     */
    inline void DrawSector(int x0, int y0, int radius, float start_radians, float end_radians, unsigned int colour) {
      DrawEllipseArc(x0,y0,2*radius,2*radius,0,start_radians,end_radians,colour,-1);
    }

    /**
     * Round the radius to the nearest integer before drawing the sector.
     */
    inline void DrawSector(int x0, int y0, float radius, float start_radians, float end_radians, unsigned int colour) {
      DrawSector(x0,y0,Round(radius),start_radians,end_radians,colour);
    }

    /**
     * Round x0,y0 and the radius to the nearest integer before drawing the sector.
     */
    inline void DrawSector(float x0, float y0, float radius, float start_radians, float end_radians, unsigned int colour) {
      DrawSector(Round(x0),Round(y0),Round(radius),start_radians,end_radians,colour);
    }
  
    /**
     * Resize this image to the dimensions of the argument image and write the result into it
     */ 
    //void Resize(ImageBase& image) const;

    /**
     * Apply a global threshold to binarize the image. All pixels with
     * intensity greater than the threshold will be set to 0 and all
     * pixels less than the threshold will be set to 1.  In particular,
     * this method inverts as well as binarizes.
     */
    //void GlobalThreshold(unsigned int threshold);

    /**
     * Apply an adaptive threshold to the image.  For each pixel a
     * region of window_size pixels around it is averaged.  If the pixel
     * is greater than this average+offset then it is changed to 0 and
     * otherwise changed to 1.  Note that this method is also inverting
     * the image in addition to binarizing it.
     */
    //void AdaptiveThreshold(const unsigned int window_size, const unsigned char offset);

    /**
     * Apply a homogonous transform based image segmentation technique.
     * Takes the log of each image pixel,scales it and applies an edge
     * detect filter.  Suitable for finding edges but not for reading the tag.
     */
    //void HomogenousTransform();

    /**
     * Apply noise to the image as if it had been acquired through a CCD array
     */
    //void AddNoise(float mean, float stddev);

    /**
     * Draw an ellipse with centre x,y and given width and height - note
     * width and height are twice the size of the major and minor axes
     * they represent.  angle_radians is the angle between the horizontal
     * and the axis given by width
     */
    void DrawEllipseArc(int xc, int yc, int width, int height, float angle_radians, float start_angle, float end_angle, unsigned int color, int thickness);

    void DrawEllipseArc(float xc, float yc, float width, float height, float angle_radians, float start_angle, float end_angle, unsigned int color, int thickness);
  
    void DrawEllipse(float xc, float yc, float width, float height, float angle_radians, unsigned int color, int thickness);

    /**
     * Send this image over the network using the socket proffered.
     * Return the number of bytes written
     */
    //int Save(Socket& socket) const;

  private:
    int AdaptiveWidthStep(int moving_average,int* previous_line,unsigned int i, unsigned int j,unsigned int s, int t);
    void ellipse_polygon_approx(float* points, int startindex, int length, float xc, float yc, float width, float height,  float angle_radians, unsigned int color, int thickness, float start_angle);
    void ScanLineFill(float* points,int numpoints,unsigned int colour);

    class Edge {
    public:
      int miny;
      int maxy;
      float x;
      int intx;
      float invslope;
    
      Edge(float x0,float y0, float x1, float y1) :
	miny(Round(y0<=y1?y0:y1)),
	maxy(Round(y0<=y1?y1:y0)),
	x(y0<=y1?x0:x1),
	intx(Round(x)),
	invslope( (x0-x1)/(y0-y1) ) {}  
    
      bool operator<(const Edge& a) const {
	return (miny == a.miny) ? intx < a.intx : miny < a.miny;
      }
    };

    struct EdgePtrSort {
      bool operator()(const Edge* a, const Edge* b) {
	return (a->miny == b->miny) ? a->intx < b->intx : a->miny < b->miny;
      }
    };

    struct EdgePtrSortX {
      bool operator()(const Edge* a, const Edge* b) {
	return a->intx < b->intx;
      }
    };
  };


   template<Colour::Type ColType> void Image<ColType>::ConvertScale(float scalefactor, int offset) {
    /**
     * Sadly gcc4 is too stupid to infer that parent class variables & methods exist
     * so we have to manually tell the compiler where to find the names; 
     * typedef makes this manual defn easier (well, at least more succinct).
     */
    typedef ImageSpecialise<ColType> s;

    s::m_binary = false;
    for(int i=0;i<s::GetHeight();++i) {
      unsigned char* ptr = s::GetRow(i);
      for(int j=0;j<s::GetWidth();++j) {
	unsigned char value = *ptr;
	*ptr = Round(value*scalefactor)+offset;
	ptr++;
      }
    }
  }

   template<Colour::Type ColType> void Image<ColType>::Mask(unsigned char mask) {
    /**
     * Sadly gcc4 is too stupid to infer that parent class variables & methods exist
     * so we have to manually tell the compiler where to find the names; 
     * typedef makes this manual defn easier (well, at least more succinct).
     */
    typedef ImageSpecialise<ColType> s;

    s::m_binary = mask == 1;
    for(int i=0;i<s::GetHeight();++i) {
      unsigned char* ptr = s::GetRow(i);
      for(int j=0;j<s::GetWidth();++j) {
	*ptr &= mask;
	++ptr;
      }
    }
  }

#define STEPSIZE 0.01f
   template<Colour::Type ColType> void Image<ColType>::ellipse_polygon_approx(float* points, int startindex, int length, float xc, float yc, float width, float height,  float angle_radians, unsigned int color, int thickness, float start_angle) {
    /**
     * The parametric equation for an ellipse
     *
     * x = xc + a*cos(angle_radians)*cos(t) + b*sin(angle_radians)*sin(t)
     * y = yc - a*sin(angle_radians)*cos(t) + b*cos(angle_radians)*sin(t)
     *
     * a = width/2; b=height/2
     * angle_radians is the angle between the axis given by width and the horizontal
     *
     */

    float cosa = DCOS(8,angle_radians); // DCOS (later)
    float sina = DSIN(8,angle_radians); // DSINE (later)
    float a = width/2;
    float b = height/2;

    float currentAngle = start_angle;
    for(int i=2*startindex;i<2*(length+startindex);i+=2) {
      float cost = cos(currentAngle); // DCOS (later)
      float sint = sin(currentAngle); // DSINE (later)

      points[i] = Round(xc + a*cosa*cost + b*sina*sint);
      points[i+1] = Round(yc + a*sina*cost + b*cosa*sint);
      currentAngle += STEPSIZE;
    }

    int numvertices = startindex+length;
    if (thickness > 0) {
      DrawPolygon(points,numvertices,color,thickness);
    }
    else {
      DrawFilledPolygon(points,numvertices,color);
    }
  }

   template<Colour::Type ColType> void Image<ColType>::DrawEllipse(float xc, float yc, 
			  float width, float height, 
			  float angle_radians, 
			  unsigned int color, int thickness) {
    assert(thickness > 0 || thickness == -1);

    int numsteps = (int)(2.f*M_PI/STEPSIZE); 

    float points[numsteps*2];
    ellipse_polygon_approx(points,0, numsteps, xc, yc ,width, height, angle_radians, color, thickness, 0);
  }

  /*
   Image(const Image& c) : s::m_width(c.s::m_width), s::m_height(c.s::m_height), m_contents(new unsigned char[c.s::m_width_step*c.s::m_height]), m_free_contents(true), s::m_width_step(c.s::m_width_step), s::m_binary(c.s::m_binary) {
    memcpy(m_contents,c.m_contents,s::m_width_step*s::m_height);
  };
  */

  template<Colour::Type ColType> Image<ColType>::Image(const MonochromeImage& mono, const char blackval) : 
    ImageSpecialise<ColType>(mono.GetWidth(),mono.GetHeight()) {

    /**
     * Sadly gcc4 is too stupid to infer that parent class variables & methods exist
     * so we have to manually tell the compiler where to find the names; 
     * typedef makes this manual defn easier (well, at least more succinct).
     */
    typedef ImageSpecialise<ColType> s;

    for(unsigned int i=0;i<s::m_height;++i) {
      unsigned char* data_pointer = s::GetRow(i);
      for(unsigned int j=0;j<s::m_width;++j) {
	for(unsigned int k=0;k<s::Bpp; k++) {
	  *data_pointer = mono.GetPixel(j,i) ? blackval : 0;
	  ++data_pointer;
	}
      }
    }
  };
   
  template<Colour::Type ColType> void Image<ColType>::DrawEllipseArc(int xc, int yc, 
				 int  width, int height, 
				 float angle_radians, 
				 float start_angle, float end_angle, unsigned int color, int thickness) {
    DrawEllipseArc((float)xc,(float)yc,(float)width,(float)height,angle_radians,start_angle,end_angle,color,thickness);
  }

  template<Colour::Type ColType> void Image<ColType>::DrawEllipseArc(float xc, float yc, 
				 float width, float height, 
				 float angle_radians, 
				 float start_angle, float end_angle, unsigned int color, int thickness) {
    assert(thickness > 0 || thickness == -1);
    
    int numsteps = (int)(((float)(end_angle - start_angle))/STEPSIZE)+1; // add one to get to the edge of the sector
    // add in an extra point (the centre) if we are drawing a slice
    float points[numsteps*2+2];
    points[0] = Round(xc);
    points[1] = Round(yc);
    ellipse_polygon_approx(points, 1, numsteps, xc, yc ,width, height, angle_radians, color, thickness, start_angle); 
  }

   template<Colour::Type ColType> void Image<ColType>::DrawLine(int x0,int y0, int x1,int y1, unsigned int colour, unsigned int thickness) {
    if (x1 < x0 || (x0 == x1 && y1 < y0)) {
      DrawLine(x1,y1,x0,y0,colour,thickness);
    }
    else {
      int dy = y1-y0;
      int dx = x1-x0;
      int a = y1-y0;
      int b = -(x1-x0);
      int c = x1*y0-x0*y1;
      int x = x0;
      int y = y0;
      DrawPixel(x,y,colour);

      if (dy > 0 && dy >= dx) { // NE or N
	int d = (int)((a*(x+0.5)+b*(y+1)+c));
	while (y < y1-1) {
	  ++y;
	  if (d>0) {
	    d+=b;
	  }
	  else {
	    d+=a+b;	  
	    ++x;
	  }
	  DrawPixel(x,y,colour);
	}
      }
      else if (dy > 0 && dx > dy) { // E or NE
	int d = (int)((a*(x+1)+b*(y+0.5)+c));
	while (x < x1) {
	  ++x;
	  if (d<0) {
	    d+= a;
	  }
	  else {
	    d+=a+b;
	    ++y;
	  }
	  DrawPixel(x,y,colour);
	}
      }
      else if (dy <= 0 && dx > -dy) { // E or SE
	int d = (int)((a*(x+1)+b*(y-0.5)+c));
	while ( x < x1-1) {
	  ++x;
	  if (d>0) {
	    d+=a;
	  }
	  else {
	    d+=a-b;
	    --y;
	  }
	  DrawPixel(x,y,colour);	  
	}
      }
      else { // SE or S
	int d = (int)((a*(x+0.5)+b*(y-1)+c));
	while (y > y1) {
	  --y;
	  if (d<0) {
	    d-=b;	  
	  }
	  else {
	    d+=a-b;
	    ++x;
	  }
	  DrawPixel(x,y,colour);
	}
      }
    }
  }

   template<Colour::Type ColType> void Image<ColType>::DrawPolygon(float* points, int numpoints, unsigned int colour, unsigned int thickness) {
    for(int i=2;i<2*numpoints;i+=2) {
      DrawLine(points[i-2],points[i-1],points[i],points[i+1],colour,thickness);
    }  
    DrawLine(points[2*numpoints-2],points[2*numpoints-1],points[0],points[1],colour,thickness);

  }

   template<Colour::Type ColType> void Image<ColType>::DrawPolygon(const std::vector<float>& points, unsigned int colour, unsigned int thickness) {
    
    if (points.size() < 4 )
      return;

    for(unsigned int i=2;i<points.size();i+=2) {
      DrawLine(points[i-2],points[i-1],points[i],points[i+1],colour,thickness);
    }
    DrawLine(points[points.size()-2],points[points.size()-1],points[0],points[1],colour,thickness);
  }

   template<Colour::Type ColType> void Image<ColType>::DrawFilledPolygon(const std::vector<float>& points, unsigned int colour) {
    float fpoints[points.size()];
    for(unsigned int i=0;i<points.size();++i) {
      fpoints[i] = points[i];
    }
    ScanLineFill(fpoints,points.size()/2,colour);
  }

   template<Colour::Type ColType> void Image<ColType>::DrawFilledPolygon(int* points, int numpoints, unsigned int colour) {
    float fpoints[numpoints*2];
    for(int i=0;i<numpoints*2;++i) {
      fpoints[i] = points[i];
    }
    ScanLineFill(fpoints,numpoints,colour);
  }

   template<Colour::Type ColType> void Image<ColType>::DrawFilledPolygon(float* points, int numpoints, unsigned int colour) {
    ScanLineFill(points,numpoints,colour);
  }

   template<Colour::Type ColType> void Image<ColType>::ScanLineFill(float* points, int numpoints, unsigned int colour) {

    // build the edge list
    std::list<Edge*> edge_list;
    for(int i=0;i<2*numpoints-2;i+=2) {
      if (points[i+1] != points[i+3]) {
	//      std::cout << "Adding " << points[i] << " " << points[i+1] << " " << points[i+2] << " " << points[i+3] << std::endl;
	edge_list.push_back(new Edge(points[i],points[i+1],points[i+2],points[i+3]));
      }
    }

    if (edge_list.size() == 0) {
      return;
    }

    if (points[1] != points[2*numpoints-1]) {
      //std::cout << "Adding " << points[2*numpoints-2] << " " << points[2*numpoints-1] << " " << points[0] << " " << points[1] << std::endl;
      edge_list.push_back(new Edge(points[2*numpoints-2],points[2*numpoints-1],points[0],points[1]));    
    }
    edge_list.sort(EdgePtrSort());
  
    // initialise scanline
    int scanline = edge_list.front()->miny;
  
    // initialise active edge list
    std::list<Edge*> active_edges;

    while(edge_list.size() > 0 || active_edges.size() > 0) {
      // initialise parity
      bool parity = false;
      typename std::list<Edge*>::iterator i = edge_list.begin();
      while( i != edge_list.end() && (*i)->miny == scanline ) {
	active_edges.push_back(*i);
	i = edge_list.erase(i);
      }
      active_edges.sort(EdgePtrSortX());

      for (typename std::list<Edge*>::iterator j = active_edges.begin();
	   j != active_edges.end(); ++j ) {
      }
    
      // draw the current scan line
      int currentx = active_edges.front()->intx;
      typename std::list<Edge*>::iterator j = active_edges.begin();
      while(j != active_edges.end()) {
	if ( (*j)->intx == currentx ) {
	  //	DrawPixel(currentx,scanline,colour);
	  parity ^= true;
	  ++j;
	}
	else {
	  ++currentx;
	}
	if (parity) {
	  DrawPixel(currentx,scanline,colour);
	}
      }

      // increment scanline
      ++scanline;
    
      // remove edges with maxy == scanline from the active list
      // update the x values for the others
      for(typename std::list<Edge*>::iterator k = active_edges.begin(); k!=active_edges.end();) {
	if ( (*k)->maxy == scanline ) {
	  delete *k;
	  k = active_edges.erase(k);
	}
	else {
	  Edge* val = *k;
	  val->x += val->invslope;
	  val->intx = Round(val->x);
	  ++k;
	}
      }

      /*
	if (active_edges.size()==1) {
	int miny = active_edges.front()->miny;
	int maxy = active_edges.front()->maxy;
	std::cout << "Edges:" << miny << "->" << maxy <<" inside? " << scanline << std::endl;
      }
      */
    }

  }

  /*
   * Recursive fill.  OK, so it's not pretty, but
   * we don't need high performance and it works fine!
   *
   * Rob Harle <rkh23@cam.ac.uk>
   */
   template<Colour::Type ColType> void Image<ColType>::SeedFill(int x, int y,unsigned int colour) {

    /**
     * Sadly gcc4 is too stupid to infer that parent class variables & methods exist
     * so we have to manually tell the compiler where to find the names; 
     * typedef makes this manual defn easier (well, at least more succinct).
     */
    typedef ImageSpecialise<ColType> s;

    if (x<0 || y<0 || x >=(int)s::m_width || y >= (int)s::m_height) return;
    if (Sample(x,y)==colour) return;
    else {
      DrawPixel(x,y,colour);
      SeedFill(x+1,y,colour);
      SeedFill(x-1,y,colour);
      SeedFill(x,y+1,colour);
      SeedFill(x,y-1,colour);
    }
  }




   template<Colour::Type ColType> void Image<ColType>::DrawPolygon(int* points, int numpoints, unsigned int colour, unsigned int thickness) {
    for(int i=2;i<2*numpoints;i+=2) {
      DrawLine(points[i-2],points[i-1],points[i],points[i+1],colour,thickness);
    }
    DrawLine(points[2*numpoints-2],points[2*numpoints-1],points[0],points[1],colour,thickness);
  }



};
//#else

//#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
//#include <Magick++.h>
//#endif
//
//namespace Cantag {
//  template<Colour::Type ColType> class Image;
//}
#endif//IMAGE_GUARD
