/**
 * $Header$
 */
#ifndef LOCATED_OBJECT_GUARD
#define LOCATED_OBJECT_GUARD

#include <tripover/Config.hh>
#include <tripover/Image.hh>
#include <tripover/CyclicBitSet.hh>
#include <tripover/findtransform.hh>
#include <tripover/Camera.hh>

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
  std::vector<CyclicBitSet<PAYLOAD_SIZE>*> tag_codes;

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
  count += socket.Send((int)tag_codes.size());
  for(typename std::vector<CyclicBitSet<PAYLOAD_SIZE>*>::const_iterator i = tag_codes.begin();
      i != tag_codes.end();
      ++i) {
    count += (*i)->Save(socket);
  }
  return count;
}

template<int PAYLOAD_SIZE> LocatedObject<PAYLOAD_SIZE>::LocatedObject(Socket& socket) : tag_codes() {
    socket.Recv(transform,16);
    socket.Recv(location,3);
    angle = socket.RecvFloat();
    int codecount = socket.RecvInt();
    for(int i=0;i<codecount;++i) {
      tag_codes.push_back(new CyclicBitSet<PAYLOAD_SIZE>(socket));
    }
}

template<int PAYLOAD_SIZE> LocatedObject<PAYLOAD_SIZE>::LocatedObject() : tag_codes() {}

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
  for(typename std::vector<CyclicBitSet<PAYLOAD_SIZE>*>::const_iterator i = tag_codes.begin();
      i != tag_codes.end();
      ++i) {
    delete *i;    
  }
}

#endif//LOCATED_OBJECT_GUARD
