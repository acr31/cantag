#ifndef ADAPTIVE_THRESHOLD_GUARD
#define ADAPTIVE_THRESHOLD_GUARD

#include <Drawing.hh>
#include <ImageFilter.hh>

class AdaptiveThreshold : public virtual ImageFilter {
private:
  Image* m_image;
  int m_window_size;
  int m_offset;
public:
  AdaptiveThreshold(int window_size,int offset);
  virtual ~AdaptiveThreshold();
  virtual Image* LoadImage(Image* image);
  virtual Image* BinarizeRegion(int x, int y, int width , int height);
};

#endif//ADAPTIVE_THRESHOLD_GUARD
