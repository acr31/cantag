/**
 * $Header$
 */
#ifndef LOCATED_OBJECT_GUARD
#define LOCATED_OBJECT_GUARD

#include <Drawing.hh>
class LocatedObject {
public:
  /**
   * A 4x4 homogenous matrix transformation for mapping points from
   * object co-ordinates to camera co-ordinates
   */
  float transform[16];

  /**
   * Centre x co-ordinate (camera)
   */
  float xc;

  /**
   * Centre y co-ordinate (camera)
   */
  float yc;

  /**
   * Centre z co-ordinate (camera)
   */
  float zc;

  /**
   * x component of the object's normal vector
   */
  float xn;

  /**
   * y component of the object's normal vector
   */
  float yn;

  /**
   * z component of the object's normal vector
   */
  float zn;

  /**
   * The angle of this object relative to the camera co-ordinate x axis.
   */
  float angle;

  LocatedObject();
  ~LocatedObject();
  void Refresh(Image* image);

};

#endif//LOCATED_OBJECT_GUARD
