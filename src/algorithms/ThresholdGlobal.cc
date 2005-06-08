/**
 * $Header$
 */

#include <total/algorithms/ThresholdGlobal.hh>

namespace Total {
  ThresholdGlobal::ThresholdGlobal(int threshold) : m_threshold(threshold) {}
  
  bool ThresholdGlobal::operator()(const Image& image, MonochromeImage& dest) const {
    const int width = image.GetWidth();
    const int height = image.GetHeight();
    for(int i=0;i<height;++i) {
      const unsigned char* data_pointer = image.GetRow(i);
      for(int j=0;j<width;++j) {
	const unsigned char pixel = *data_pointer;
	dest.SetPixel(j,i, pixel > m_threshold);
	++data_pointer;
      }
    }
    return true;
  }
};
