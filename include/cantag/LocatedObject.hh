/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */
#ifndef LOCATED_OBJECT_GUARD
#define LOCATED_OBJECT_GUARD

#include <cantag/Config.hh>
#include <cantag/Image.hh>
#include <cantag/CyclicBitSet.hh>
#include <cantag/findtransform.hh>
#include <cantag/Camera.hh>

namespace Cantag {
  /**
   * Represents a tag that has been located.  Contains its 3D location,
   * pose, transformation matrix and its coded value.
   *
   * \todo we want some method of checking for expected tags in the image
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

    void LoadTransform(const float transform[16],float tag_size, const Camera& camera);

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

  template<int PAYLOAD_SIZE> void LocatedObject<PAYLOAD_SIZE>::LoadTransform(const float t[16],float tag_size,  const Camera& camera) {
    for(int i=0;i<16;i++) {
      transform[i] = t[i];
    }
    angle = 0;
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
}
#endif//LOCATED_OBJECT_GUARD
