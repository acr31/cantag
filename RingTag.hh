/**
 * $Header$
 *
 * $Log$
 * Revision 1.5  2004/01/23 12:05:48  acr31
 * moved Tag to CircularTag in preparation for Squaretag
 *
 * Revision 1.4  2004/01/23 11:57:08  acr31
 * moved Location2D to Ellipse2D in preparation for Square Tags
 *
 * Revision 1.3  2004/01/21 13:41:36  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 * Revision 1.2  2004/01/21 11:52:29  acr31
 * added keywords
 *
 */

#ifndef RING_TAG_GUARD
#define RING_TAG_GUARD

#include "Config.hh"
#include "Drawing.hh"
#include "CircularTag.hh"
#include "Coder.hh"
#include "ellipsetoxy.hh"

#undef FILENAME
#define FILENAME "RingTag.hh"

template<class C>
class RingTag : virtual CircularTag {
private:
  int m_ring_count;
  int m_sector_count;
  int m_sync_count;
  float m_bullseye_inner_radius;
  float m_bullseye_outer_radius;
  float m_data_inner_radius;
  float m_data_outer_radius;
  C m_coder;

  float *m_data_ring_outer_radii;
  float *m_data_ring_centre_radii;
  float *m_sector_angles;
  float *m_sync_angles;

public:
  RingTag(int ring_count,
	  int sector_count,
	  int sync_count,
	  float bullseye_inner_radius,
	  float bullseye_outer_radius,
	  float data_inner_radius,
	  float data_outer_radius) :
    m_ring_count(ring_count),
    m_sector_count(sector_count),
    m_sync_count(sync_count),
    m_bullseye_inner_radius(bullseye_inner_radius),
    m_bullseye_outer_radius(bullseye_outer_radius),
    m_data_inner_radius(data_inner_radius),
    m_data_outer_radius(data_outer_radius),
    m_coder(1<<ring_count,sector_count) {

    assert(bullseye_inner_radius < bullseye_outer_radius);
    assert(data_inner_radius < data_outer_radius);
    assert(sector_count <= sync_count);

    // bullseye_inner_radius < bullseye_outer_radius < data_inner_radius < data_outer_radius
    // data_inner_radius < data_outer_radius < bullseye_inner_radius < bullseye_outer_radius
    // bullseye_inner_radius < data_inner_radius < data_outer_radius < bullseye_outer_radius

    // Populate the radii cache
    // Slice up the range data_inner_radius to data_outer_radius evenly.

    // there are two ways of doing this.

    // 1) give each one an equal amount
    // 2) give them different amounts so that the area of each sector is the same
  
    // Lets give each one an equal amount - first pass
  
    m_data_ring_centre_radii = new float[m_ring_count];
    m_data_ring_outer_radii = new float[m_ring_count];

    float ring_width = (m_data_outer_radius-m_data_inner_radius)/m_ring_count;

    for(int i=0;i<m_ring_count;i++) {
      m_data_ring_outer_radii[i] = m_data_inner_radius + ring_width*(i+1);
      m_data_ring_centre_radii[i] = m_data_ring_outer_radii[i]-ring_width/2;    
    }

    // now the sector angles - go one more than necessary it makes drawing easier ;-)

    m_sector_angles = new float[m_sector_count+1];
    for(int i=0;i<m_sector_count+1;i++) {
      m_sector_angles[i] = 2*PI/m_sector_count *i;
    }
  
    m_sync_angles = new float[m_sync_count+1];
    for(int i=0;i<m_sync_count;i++) {
      m_sync_angles[i] = 2*PI/m_sync_count *i;
    }
  }

  virtual ~RingTag() {
    delete[] m_data_ring_outer_radii;
    delete[] m_data_ring_centre_radii;
    delete[] m_sector_angles;
    delete[] m_sync_angles;
  }

  virtual void Draw2D(Image* image, const Ellipse2D *l, unsigned long code, int black, int white) {
    PROGRESS("Draw2D called for ellipse centre (" << l->m_x << "," << l->m_y<< "), size ("<<l->m_width<<","<<l->m_height<<"), code "<<code);
    
    // Work from the outside inwards

    if (m_bullseye_outer_radius > m_data_outer_radius) {
      PROGRESS("Drawing outer bullseye edge");
      DrawFilledEllipse(image, 
			l->m_x, 
			l->m_y,
			l->m_width*m_bullseye_outer_radius, 
			l->m_height*m_bullseye_outer_radius,
			l->m_angle_radians, 
			black);
    }

    if (m_bullseye_inner_radius > m_data_outer_radius) {
      PROGRESS("Drawing inner bullseye edge");
      DrawFilledEllipse(image, 
			l->m_x, 
			l->m_y,
			l->m_width*m_bullseye_inner_radius, 
			l->m_height*m_bullseye_inner_radius,
			l->m_angle_radians, 
			white);
    }

    PROGRESS("Drawing data rings");
    for(int i=m_ring_count-1;i>=0;i--) {
      m_coder.Set(code);
      for(int j=0;j<m_sector_count;j++) {
	unsigned int value = m_coder.NextChunk();
	int colour = ((value & (1<<i)) == (1<<i)) ? black : white;
	DrawFilledEllipse(image,
			  l->m_x,
			  l->m_y,
			  l->m_width*m_data_ring_outer_radii[i],
			  l->m_height*m_data_ring_outer_radii[i],
			  l->m_angle_radians,
			  m_sector_angles[j],
			  m_sector_angles[j+1],
			  colour);	
      }
    }

    PROGRESS("Blanking out inside of data rings");
    if (m_data_inner_radius != 0) {
      DrawFilledEllipse(image,
			l->m_x,
			l->m_y,
			l->m_width*m_data_inner_radius,
			l->m_height*m_data_inner_radius,
			l->m_angle_radians,
			(m_bullseye_inner_radius < m_data_inner_radius &&
			 m_bullseye_outer_radius > m_data_inner_radius) ? black : white);
    }


    if (m_bullseye_outer_radius < m_data_inner_radius) {
      PROGRESS("Drawing outer bullseye edge");
      DrawFilledEllipse(image, 
			l->m_x, 
			l->m_y,
			l->m_width*m_bullseye_outer_radius, 
			l->m_height*m_bullseye_outer_radius,
			l->m_angle_radians, 
			black);
    }
    
    if (m_bullseye_inner_radius < m_data_inner_radius) {
      PROGRESS("Drawing inner bullseye edge");	   
      DrawFilledEllipse(image, 
			l->m_x, 
			l->m_y,
			l->m_width*m_bullseye_inner_radius, 
			l->m_height*m_bullseye_inner_radius,
			l->m_angle_radians, 
			white);
    }
  }

  virtual unsigned long Decode(Image *image, const Ellipse2D *l) {
    // the location we have here locates the outer ring of the
    // bullseye.  Therefore we will need to scale it by the actual
    // size of the bullseye to hit the data sectors properly.

    m_coder.Reset();
    
    // loop round reading chunks and passing them to the decoder

    // if it returns false on any insert then we might be misaligned
    // so rotate a little bit round and try again

    // if it throws an InvalidSymbol exception then we stop trying

    // if we read a full 360 degrees then we stop and ask it for the
    // code
    
#ifdef IMAGE_DEBUG
    int debug_attempt=0;
    Image* debug0 = cvCloneImage(image);
    cvConvertScale(debug0,debug0,0.5,128);    
#endif

    for(int i=0;i<m_sync_count;i++) {
      bool valid = 1;
      for(int j=0;j<m_sector_count;j++) {
	// read a chunk by sampling each ring and shifting and adding
	unsigned int chunk =0;
	for(int k=m_ring_count-1;k>=0;k--) {
	  chunk = chunk << 1;
	  int x;
	  int y;
	  EllipseToXY(l->m_x,
		      l->m_y,
		      l->m_width/m_bullseye_outer_radius,
		      l->m_height/m_bullseye_outer_radius,
		      l->m_angle_radians,
		      m_sync_angles[i]+m_sector_angles[j],
		      m_data_ring_centre_radii[k],
		      &x,&y);
	  bool sample = SampleImage(image,x,y) < 128;
#ifdef IMAGE_DEBUG
	  cvLine(debug0,cvPoint(x,y),cvPoint(x,y),0,3);
#endif
	  chunk |= (sample ? 1:0);
	}
	try {
	  if (!(m_coder.LoadChunk(chunk))) {
	    valid =0;
	    break;
	  }
	}
	catch (Coder::InvalidSymbol &e) {
#ifdef IMAGE_DEBUG
	  char filename[255];
	  snprintf(filename,255,"debug-decode%d.jpg",debug_attempt++);
	  cvSaveImage(filename,debug0);
	  cvReleaseImage(&debug0);
#endif
	  throw e;
	}
      }

#ifdef IMAGE_DEBUG
      char filename[255];
      snprintf(filename,255,"debug-decode%d.jpg",debug_attempt++);
      cvSaveImage(filename,debug0);
      cvCopyImage(image,debug0);
      cvConvertScale(debug0,debug0,0.5,128);    
#endif
      if (valid) {
#ifdef IMAGE_DEBUG
	cvReleaseImage(&debug0);
#endif
	return m_coder.Decode();
      }
      else {
	// try again with another sync angle
	m_coder.Reset();
      }
    }
  }

};

#endif//RING_TAG_GUARD
