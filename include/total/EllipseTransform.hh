/**
 * $Header$
 */

#ifndef ELLIPSE_TRANSFORM_GUARD
#define ELLIPSE_TRANSFORM_GUARD

#include <total/Config.hh>
#include <total/shapefit/Ellipse.hh>

namespace Total {
  class EllipseTransform {
  public:
    virtual bool TransformEllipse(const Ellipse& ellipse, float transform1[16], float transform2[16]) const = 0;
  };

  class FullEllipseTransform : public virtual EllipseTransform {
  public:
    bool TransformEllipse(const Ellipse& ellipse, float transform1[16], float transform2[16]) const;
  };

  class LinearEllipseTransform : public virtual EllipseTransform  {
  public:
    /**
     * We then build the transformation matrix that transforms the unit circle onto the ellipse
     *
     * This is a scale factor   ( width 0      0 0 )
     *                          ( 0     height 0 0 ) 
     *                          ( 0     0      1 0 )
     *                          ( 0     0      0 1 )
     *
     * premultiplied by a rotation around the z axis  ( cos(theta)  -sin(theta) 0 0 )
     *                                                ( sin(theta)  cos(theta)  0 0 )
     *                                                ( 0           0           1 0 )
     *                                                ( 0           0           0 1 )
     *
     *
     * premultiplied by a translation in x and y ( 1 0 0 x0 )
     *                                           ( 0 1 0 y0 )
     *                                           ( 0 0 1 1  )
     *                                           ( 0 0 0 1  ) 
     *
     * (translate by 1 in z so that the perspective transform doesn't do
     * anything when we apply it later)
     *
     */
    bool TransformEllipse(const Ellipse& ellipse, float transform1[16], float transform2[16]) const;
  };
}
#endif//ELLIPSE_TRANSFORM_GUARD
