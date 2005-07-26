/**
 * $Header$
 */

#ifndef THRESHOLD_GLOBAL_GUARD
#define THRESHOLD_GLOBAL_GUARD

#include <total/Image.hh>
#include <total/MonochromeImage.hh>
#include <total/Function.hh>

namespace Total {

  class ThresholdGlobal : public Function1<Image<Colour::Grey>,MonochromeImage> {
  private:
    int m_threshold;
  public:
    ThresholdGlobal(int threshold);
    bool operator()(const Image<Colour::Grey>& source, MonochromeImage& dest) const;
  };
}

#endif//THRESHOLD_GLOBAL_GUARD
