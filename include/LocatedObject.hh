/**
 * $Header$
 */
#ifndef LOCATED_OBJECT_GUARD
#define LOCATED_OBJECT_GUARD

#include <Config.hh>
#include <Image.hh>
#include <CyclicBitSet.hh>

#include <boost/shared_ptr.hpp>

/**
 * Represents a tag that has been located.  Contains its 3D location,
 * pose, transformation matrix and its coded value.
 */
template<int PAYLOAD_SIZE>
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

  /**
   * The code stored on the tag.
   */
  boost::shared_ptr< CyclicBitSet<PAYLOAD_SIZE> > tag_code;

  void Refresh(Image& image) {};
};

#endif//LOCATED_OBJECT_GUARD
