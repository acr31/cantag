/**
 * $Header$
 */
#ifndef LOCATED_OBJECT_GUARD
#define LOCATED_OBJECT_GUARD

#include <Config.hh>
#include <Image.hh>
#include <CyclicBitSet.hh>
#include <findtransform.hh>
#include <Camera.hh>

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
   * world co-ordinates
   */
  float normal[3];


  /**
   * A 3x1 matrix containing this located object's position in world
   * co-ordinates.
   */
  float location[3];

  /**
   * The angle (radians) of this object relative to the camera co-ordinate x axis.
   */
  float angle;

  /**
   * The code stored on the tag.
   */
  CyclicBitSet<PAYLOAD_SIZE>* tag_code;

  void LoadTransform(float transform[16],float tag_size, float agle, const Camera& camera);

  LocatedObject();
  ~LocatedObject();

  int Save(Socket& socket) const;
  LocatedObject(Socket& socket);
};

template<int PAYLOAD_SIZE> int LocatedObject<PAYLOAD_SIZE>::Save(Socket& socket) const {
  int count = socket.Send(transform,16);
  count += socket.Send(normal,3);
  count += socket.Send(angle);
  count += tag_code->Save(socket);
  return count;
}

template<int PAYLOAD_SIZE> LocatedObject<PAYLOAD_SIZE>::LocatedObject(Socket& socket) {
    socket.Recv(transform,16);
    socket.Recv(location,3);
    socket.Recv(angle);
    tag_code = new CyclicBitSet<PAYLOAD_SIZE>(socket);
}

template<int PAYLOAD_SIZE> LocatedObject<PAYLOAD_SIZE>::LocatedObject() : tag_code(NULL) {}

template<int PAYLOAD_SIZE> void LocatedObject<PAYLOAD_SIZE>::LoadTransform(float t[16],float tag_size, float agle, const Camera& camera) {
  for(int i=0;i<16;i++) {
    transform[i] = t[i];
  }
  angle = agle;
  GetNormalVector(transform,camera,normal);
  GetLocation(transform,location,tag_size);
  camera.CameraToWorld(location,1);
  
}

template<int PAYLOAD_SIZE> LocatedObject<PAYLOAD_SIZE>::~LocatedObject() {
  if (tag_code) {
    delete tag_code;
  }
}

#endif//LOCATED_OBJECT_GUARD
