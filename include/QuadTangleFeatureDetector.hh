#ifndef QUADTANGLE_FEATURE_DETECTOR
#define QUADTANGLE_FEATURE_DETECTOR

#include <Config.hh>
#include <QuadTangle2D.hh>
#include <FeatureDetector.hh>

class QuadTangleFeatureDetector : public virtual FeatureDetector<QuadTangle2D> {

public:
  QuadTangleFeatureDetector();
  virtual void FindFeatures(Image* buf,Camera* camera);

private:
  bool compare(QuadTangle2D *r1, QuadTangle2D *r2);
};

#endif
