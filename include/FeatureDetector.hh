#ifndef FEATURE_DETECTOR_GUARD
#define FEATURE_DETECTOR_GUARD

#include <Config.hh>
#include <Tag.hh>
#include <ImageFilter.hh>
template <class C>
class FeatureDetector : protected virtual Tag<C>, protected virtual ImageFilter {
public:
  virtual void FindFeatures(Image* buffer) =0;
};

#endif//FEATURE_DETECTOR_GUARD
