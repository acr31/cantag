#ifndef TRIP_OUTER_TAG_GUARD
#define TRIP_OUTER_TAG_GUARD

#include <vector>
#include <cv.h>

#include "Config.hh"
#include "Tag.hh"
#include "Drawing.hh"
#include "ellipsetoxy.hh"
#include "TripOriginalCoder.hh"

#undef FILENAME
#define FILENAME "TripOuterTag.hh"
/**
 * A circular tag with the same encoding as a trip tag but the bullseye is outside the data
 *
 * RING_COUNT = number of data rings
 * SECTOR_COUNT = total number of sectors 
 * SYNC_COUNT = total number of positions to try when looking for a sync sector
 * CHECKSUM_COUNT = number of checksum sectors
 * DATA_RING_OFFSET = the width of the bullseye is ths many times the gap between the bullseye and the first data ring
 */
template<class C = TripOriginalCoder<2,16,2>, // Coder class
	 int RING_COUNT = 2,
	 int SECTOR_COUNT = 16,
	 int SYNC_COUNT = 24,
	 int CHECKSUM_COUNT = 2,
	 int DATA_RING_OFFSET = 10>
class TripOuterTag : virtual Tag  {
private:
  C m_coder;

public:
  TripOuterTag() : m_coder() {};

  void Draw2D(Image *image, Location2D *l, unsigned long code, int black, int white) {
    PROGRESS("Draw2D called for ellipse centre (" << l->m_x << "," << l->m_y<< "), size ("<<l->m_width<<","<<l->m_height<<"), code "<<code);
    
    DrawFilledEllipse(image, l->m_x, l->m_y, l->m_width, l->m_height,l->m_angle_radians, black);

    DrawFilledEllipse(image, l->m_x, l->m_y, l->m_width*0.6, l->m_height*0.6,l->m_angle_radians,white);

    // the width of each data ring
    // 1-1/DATA_RING_OFFSET gives the outer radius of the outer ring
    // We then have to fit RING_COUNT rings in that range
    // So each ring is (1-1/DATA_RING_OFFSET)/RING_COUNT wide

    float ring_width = (0.6-1/(float)DATA_RING_OFFSET)/(float)(RING_COUNT+1);
    PROGRESS("Ring width is "<<ring_width);
    for(int i=RING_COUNT-1;i>=0;i--) {
      m_coder.Set(code);
      for(int j=0;j<SECTOR_COUNT;j++) {
	unsigned int value = m_coder.NextChunk();
	int colour = ((value & (1<<i)) == (1<<i)) ? black : white;

	DrawFilledEllipse(image,
			  l->m_x,
			  l->m_y,
			  l->m_width*ring_width*(float)(i+1),
			  l->m_height*ring_width*(float)(i+1),
			  l->m_angle_radians,
			  (float)j*2*PI/SECTOR_COUNT,
			  ((float)j+1)*2*PI/SECTOR_COUNT,
			  colour);
      }      
    }
  }

  unsigned long Decode(Image *image, Location2D *l) {
    m_coder.Reset();
    
    // loop round reading chunks and passing them to the decoder

    // if it returns false on any insert then we might be misaligned
    // so rotate a little bit round and try again

    // if it throws an InvalidSymbol exception then we stop trying

    // if we read a full 360 degrees then we stop and ask it for the
    // code

    float sync_step = 2*PI/SYNC_COUNT;
    float sector_step = 2*PI/SECTOR_COUNT;
    float ring_width = (0.6-1/(float)DATA_RING_OFFSET)/(float)(RING_COUNT+1);

    
#ifdef IMAGE_DEBUG
    int debug_attempt=0;
    Image* debug0 = cvCloneImage(image);
    cvConvertScale(debug0,debug0,0.5,128);    
#endif

    for(int i=0;i<SYNC_COUNT;i++) {
      bool valid = 1;
      for(int j=0;j<SECTOR_COUNT;j++) {
	// read a chunk by sampling each ring and shifting and adding
	unsigned int chunk =0;
	for(int k=RING_COUNT-1;k>=0;k--) {
	  float sample_width = ring_width*k + ring_width/2;
	  chunk = chunk << 1;
	  int x;
	  int y;
	  EllipseToXY(l->m_x,
		      l->m_y,
		      l->m_width,
		      l->m_height,
		      l->m_angle_radians,
		      sync_step*i+sector_step*j,
		      sample_width,
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
	 

#endif//TRIP_OUTER_TAG_GUARD
