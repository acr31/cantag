/**
 * $Header$
 */

#include <total/algorithms/TransformEllipseLinear.hh>
#include <total/SpeedMath.hh>

namespace Total {

  bool TransformEllipseLinear::operator()(const ShapeEntity<Ellipse>& source, TransformEntity& dest) const {    
    if (!source.m_shapeFitted) { return false; }

    //  wc  hs  0  x0
    //  ws  hc  0  y0
    //  0   0   1  0
    //  0   0   0  1
    
    const Ellipse& ellipse = *source.m_shapeDetails;

    Transform* t = new Transform(1.f);
    dest.GetTransforms().push_back(t);

    (*t)[0] = ellipse.GetWidth()*DCOS(8,ellipse.GetAngle()); // DCOS
    (*t)[1] = -ellipse.GetHeight()*DSIN(8,ellipse.GetAngle());  // DSINE
    (*t)[2] = 0;
    (*t)[3] = ellipse.GetX0();

    (*t)[4] = ellipse.GetWidth()*DSIN(8,ellipse.GetAngle()); // DSINE
    (*t)[5] = ellipse.GetHeight()*DCOS(8,ellipse.GetAngle());  // DCOS
    (*t)[6] = 0;
    (*t)[7] = ellipse.GetY0();

    (*t)[8] = 0;
    (*t)[9] = 0;
    (*t)[10] = 1;
    (*t)[11] = 1;

    (*t)[12] = 0;
    (*t)[13] = 0;
    (*t)[14] = 0;
    (*t)[15] = 1;

    return true;
  }

}
