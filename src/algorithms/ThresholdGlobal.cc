/**
 * $Header$
 */

#include <total/algorithms/ThresholdGlobal.hh>

namespace Total {
  ThresholdGlobal::ThresholdGlobal(int threshold) : m_threshold(threshold) {}
  
  bool ThresholdGlobal::operator()(const Image<Colour::Grey>& image, MonochromeImage& dest) const {
    const int width = image.GetWidth();
    const int height = image.GetHeight();
    for(int i=0;i<height;++i) {
      const unsigned char* data_pointer = image.GetRow(i);
      for(int j=0;j<width;++j) {
	const unsigned char pixel = *data_pointer;
	dest.SetPixel(j,i, pixel < m_threshold);
	++data_pointer;
      }
    }

    for(int i=0;i<height;++i) {
      dest.SetPixel(0,i,false);
      dest.SetPixel(width-1,i,false);
      if (i == 0 || i == height-1) {
	for(int j=0;j<width-1;++j) {
	  dest.SetPixel(j,i,false);
	}
      }
    }
    return true;
  }
};
