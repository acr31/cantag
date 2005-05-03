/**
 * $Header$
 */

#include <total/algorithms/SimpleEllipseFit.hh>

namespace Total {

  void SimpleEllipseFit::operator()(const ContourEntity* contour, ShapeEntity<Ellipse>* shape) {
    const std::vector<float>& points = contour->points;
    if (points.size()/2 < 6) return;

    float centrex = 0;
    float centrey = 0;
    int count = 0;
    for(std::vector<float>::const_iterator i = points.begin(); i!= points.end(); ++i) {
      centrex += *i;
      ++i;
      centrey += *i;
      count++;
    }
    centrex/=count;
    centrey/=count;

    float majorx = -1;
    float majory = -1;
    float majorlen = 0;
    
    float minorx = -1;
    float minory = -1;
    float minorlen = 1e10;

    for(std::vector<float>::const_iterator i = points.begin(); i!= points.end(); ++i) {
      float x = *i;
      ++i;
      float y = *i;
      float distsq = (centrex - x)*(centrex-x) + (centrey-y)*(centrey-y);
      if (distsq > majorlen) {
	majorx = x;
	majory = y;
	majorlen = distsq;
      }
      
      if (distsq < minorlen) {
	minorx = x;
	minory = y;
	minorlen = distsq;
      }
    }

    assert(majorlen != 0);
    assert(minorlen != 1e10);

    majorlen = sqrt(majorlen);
    minorlen = sqrt(minorlen);

    float theta = atan((majory-centrey)/(majorx-centrex)); // DATAN

    shape->m_shapeDetails = new Ellipse(centrex,centrey,theta,majorlen,minorlen);
    shape->m_shapeFitted = true;
  }
}
