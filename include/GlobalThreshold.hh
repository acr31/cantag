#ifndef GLOBAL_THRESHOLD_GUARD
#define GLOBAL_THRESHOLD_GUARD

#include <Drawing.hh>
#include <ImageFilter.hh>

class GlobalThreshold : public virtual ImageFilter {
private:
  Image* m_image;
  int m_threshold;
public:
  GlobalThreshold(int threshold);
  virtual ~GlobalThreshold();
  virtual Image* LoadImage(Image* image);
  virtual Image* BinarizeRegion(int x, int y, int width , int height);
};

#endif//GLOBAL_THRESHOLD_GUARD
