/**
 * $Header$
 */
#ifndef LOCATED_OBJECT_GUARD
#define LOCATED_OBJECT_GUARD

#include <Config.hh>
#include <Image.hh>
#include <CyclicBitSet.hh>
#include <findtransform.hh>
#include <boost/shared_ptr.hpp>

/**
 * Represents a tag that has been located.  Contains its 3D location,
 * pose, transformation matrix and its coded value.
 *
 * \todo we want some method of checking for expected tags in the image
 *
 * \todo how to output the location data found
 *
 * \todo how to integrate with image overlay
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
   * A 3x1 matrix containing this located object's normal vector in
   * camera co-ordinates
   */
  float normal[3];


  /**
   * A 3x1 matrix containing this located object's position in camera
   * co-ordinates.
   */
  float location[3];

  /**
   * The angle of this object relative to the camera co-ordinate x axis.
   */
  float angle;

  /**
   * The code stored on the tag.
   */
  boost::shared_ptr< CyclicBitSet<PAYLOAD_SIZE> > tag_code;

  void Refresh(Image& image) {};

  void LoadTransform(float transform[16],float tag_size);
};


template<int PAYLOAD_SIZE> void LocatedObject<PAYLOAD_SIZE>::LoadTransform(float t[16],float tag_size) {
  for(int i=0;i<16;i++) {
    transform[i] = t[i];
  }
  
  GetNormalVector(transform,normal);
  GetLocation(transform,location,tag_size);
  
}

#endif//LOCATED_OBJECT_GUARD
