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

    dest.m_transform = new float[16];

    dest.m_transform[0] = ellipse.GetWidth()*DCOS(8,ellipse.GetAngle()); // DCOS
    dest.m_transform[1] = -ellipse.GetHeight()*DSIN(8,ellipse.GetAngle());  // DSINE
    dest.m_transform[2] = 0;
    dest.m_transform[3] = ellipse.GetX0();

    dest.m_transform[4] = ellipse.GetWidth()*DSIN(8,ellipse.GetAngle()); // DSINE
    dest.m_transform[5] = ellipse.GetHeight()*DCOS(8,ellipse.GetAngle());  // DCOS
    dest.m_transform[6] = 0;
    dest.m_transform[7] = ellipse.GetY0();

    dest.m_transform[8] = 0;
    dest.m_transform[9] = 0;
    dest.m_transform[10] = 1;
    dest.m_transform[11] = 1;

    dest.m_transform[12] = 0;
    dest.m_transform[13] = 0;
    dest.m_transform[14] = 0;
    dest.m_transform[15] = 1;

    dest.m_transformDone = true;
    return true;
  }

}
