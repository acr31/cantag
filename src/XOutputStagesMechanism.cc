/**
 * $Header$
 */

#include <tripover/XOutputStagesMechanism.hh>

extern "C" {
#include <sys/ipc.h>
#include <sys/shm.h>
}

XOutputStagesMechanism::XOutputStagesMechanism(int width, int height, const Camera& camera) :
  XOutputMechanism(width, height, camera)
{}

XOutputStagesMechanism::~XOutputStagesMechanism() {}

void XOutputStagesMechanism::FromImageSource(const Image& image) {
  for (int y=0; y<m_height/2; ++y) {
    const unsigned char* pointer = image.GetRow(2*y);
    char* destptr = m_image->data + m_image->bytes_per_line * y;
    for (int x=0; x<m_width/2; ++x) {
      unsigned char data = *pointer;
      pointer+=2;
      
      unsigned long rPart = data;
      int diff = m_redbits - 8;
      rPart = diff > 0 ? (rPart << diff) : (rPart >> (-diff));
      rPart <<= m_redshift;

      unsigned long gPart = data;
      diff = m_greenbits - 8;
      gPart = diff > 0 ? (gPart << diff) : (gPart >> (-diff));
      gPart <<= m_greenshift;

      unsigned long bPart = data;
      diff = m_bluebits - 8;
      bPart = diff > 0 ? (bPart << diff) : (bPart >> (-diff));
      bPart <<= m_blueshift;      

      unsigned long value = rPart | gPart | bPart;
      for(int i=0;i<m_bytes_per_pixel;++i) {
	*destptr = value & 0xFF;
	destptr++;
	value>>=8;
      }
    }
  }
}

void XOutputStagesMechanism::FromThreshold(const Image& image) {
  const int midpoint = m_bytes_per_pixel * m_width/2;
  for (int y=0; y<m_height/2; ++y) {
    const unsigned char* pointer = image.GetRow(2*y);
    char* destptr = m_image->data + m_image->bytes_per_line * y + midpoint;
    for (int x=0; x<m_width/2; ++x) {
      unsigned char data = *pointer;
      pointer+=2;
      for(int i=0;i<m_bytes_per_pixel;++i) {
	*destptr = data == 0 ? 0xFF : 0;
	destptr++;
      }
    }
  }
}

void XOutputStagesMechanism::FromContourTree(const ContourTree& contours) {
  const int midpoint = m_image->bits_per_pixel/8*m_image->width/2;
  for(int y=m_height/2;y<m_height;++y) {
    memset(m_image->data+m_image->bytes_per_line*y,0xFF,midpoint);
  }
  FromContourTree(contours.GetRootContour());
}

void XOutputStagesMechanism::FromContourTree(const ContourTree::Contour* contour) {
  if (!contour->weeded) {
    for(std::vector<float>::const_iterator i = contour->points.begin();
	i != contour->points.end();
	++i) {
      const float x = *i;
      ++i;
      const float y = *i;
      XPutPixel(m_image,(int)(x/2),(int)(y/2+m_height/2),0);
    }
  }
  for(std::vector<ContourTree::Contour*>::const_iterator i = contour->children.begin();
      i != contour->children.end();
      ++i) {
    FromContourTree(*i);
  }
}

