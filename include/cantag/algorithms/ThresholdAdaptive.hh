/**
 * $Header$
 */

#ifndef THRESHOLD_ADAPTIVE_GUARD
#define THRESHOLD_ADAPTIVE_GUARD

#include <cantag/Image.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Function.hh>

namespace Cantag {

  /**
   * An implementation of Pierre Wellner's Adaptive Thresholding
   *  @TechReport{t:europarc93:wellner,
   *     author       = "Pierre Wellner",
   *     title        = "Adaptive Thresholding for the {D}igital{D}esk",
   *     institution  = "EuroPARC",
   *     year         = "1993",
   *     number       = "EPC-93-110",
   *     comment      = "Nice introduction to global and adaptive thresholding.  Presents an efficient and effective adaptive thresholding technique and demonstrates on lots of example images.",
   *     file         = "ar/ddesk-threshold.pdf"
   *   }
   * 
   * Adapted to use a more efficient calculation for the moving
   * average. the window used is now 2^window_size
   *
   */
  class ThresholdAdaptive : public Function1<Image<Colour::Grey>,MonochromeImage> {
  private:
    int m_window_size;
    int m_offset;
  public:
    ThresholdAdaptive(int window_size, int offset);
    bool operator()(const Image<Colour::Grey>& source, MonochromeImage& dest) const;
  };
}

#endif//THRESHOLD_ADAPTIVE_GUARD
