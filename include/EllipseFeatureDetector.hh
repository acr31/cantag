#ifndef ELLIPSE_FEATURE_DETECTOR
#define ELLIPSE_FEATURE_DETECTOR

#include <Config.hh>
#include <Ellipse2D.hh>
#include <FeatureDetector.hh>

class EllipseFeatureDetector : public virtual FeatureDetector<Ellipse2D> {

public:
  EllipseFeatureDetector();
  virtual void FindFeatures(Image* buf,Camera* camera);

private:
  bool compare(Ellipse2D *e1, Ellipse2D *e2);
  bool calcerror(CvBox2D *ellipse, CvPoint2D32f *fpoints, int count);

};

#endif
