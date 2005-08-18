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

#include <cantag/Transform.hh>

#undef TRANSFORM_DEBUG

namespace Cantag {

  Transform::Transform(float confidence) : m_confidence(confidence) {};

  Transform::Transform(float* transform, float confidence) : m_confidence(confidence) {
    for(int i=0;i<16;++i) {
      m_transform[i] = transform[i];
    }
  }

  void Transform::Apply(float* points, int numpoints) const {
    for(int i=0;i<numpoints*2;i+=2) {
      Apply(points[i],points[i+1],points+i,points+i+1);
    }
  }

  void Transform::Apply(float x, float y, float z, float* projX, float* projY) const {
    *projX = m_transform[0]*x + m_transform[1]*y + m_transform[2]*z + m_transform[3];
    *projY = m_transform[4]*x + m_transform[5]*y + m_transform[6]*z + m_transform[7];
    float projZ = m_transform[8]*x + m_transform[9]*y + m_transform[10]*z + m_transform[11];
    float projH = m_transform[12]*x + m_transform[13]*y + m_transform[14]*z + m_transform[15];
  
#ifdef TRANSFORM_DEBUG
    PROGRESS("Transformed ("<<x<<","<<y<<","<<z<<") on to ("<<*projX<<","<<*projY<<","<<projZ<<","<<projH<<")");
#endif
  
    *projX /= projH;
    *projY /= projH;
    projZ /= projH;
  
    *projX /= projZ;
    *projY /= projZ;
  
#ifdef TRANSFORM_DEBUG
    PROGRESS("Projected ("<<x<<","<<y<<","<<z<<") on to ("<<*projX<<","<<*projY<<")");
#endif

  }

  void Transform::Apply(float x, float y, float* projX, float* projY) const {
    *projX = m_transform[0]*x + m_transform[1]*y + m_transform[3];
    *projY = m_transform[4]*x + m_transform[5]*y + m_transform[7];
    float projZ = m_transform[8]*x + m_transform[9]*y + m_transform[11];
    float projH = m_transform[12]*x + m_transform[13]*y + m_transform[15];
  
#ifdef TRANSFORM_DEBUG
    PROGRESS("Transformed ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<","<<projZ<<","<<projH<<")");
#endif
  
    *projX /= projH;
    *projY /= projH;
    projZ /= projH;
  
    *projX /= projZ;
    *projY /= projZ;
  
#ifdef TRANSFORM_DEBUG
    PROGRESS("Projected ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<")");
#endif

  }

  void Transform::Apply3D(float* points ,int numpoints) const {
    for(int i=0;i<numpoints*3;i+=3) {
      float x = points[i];
      float y = points[i+1];
      float z = points[i+2];
      points[i] = m_transform[0]*x + m_transform[1]*y + m_transform[2]*z + m_transform[3];
      points[i+1] = m_transform[4]*x + m_transform[5]*y + m_transform[6]*z + m_transform[7];
      points[i+2] = m_transform[8]*x + m_transform[9]*y + m_transform[10]*z + m_transform[11];
      float projH = m_transform[12]*x + m_transform[13]*y + m_transform[14]*z + m_transform[15];
      
      points[i] /= projH;
      points[i+1] /= projH;
      points[i+2] /= projH;
    }
  }

  /**
   * Destructive rotation of the transform so the tag is rotated by
   * angle radians.
   */
  void Transform::Rotate(float cos,float sin) {
    float rotation[16] = { cos, -sin,0,0,
			   sin, cos,0,0,
			   0,0,1,0,
			   0,0,0,1};

    float store[16];
    for(int i=0;i<16;++i) {
      store[i] = m_transform[i];
      m_transform[i] = 0;
    }

    for(int row=0;row<4;++row) {
      for(int col=0;col<4;++col) {
	for(int k=0;k<4;++k) {
	  m_transform[row*4+col] += store[row*4+k] * rotation[k*4+col];
	}
      }
    }
  }

  /**
   * Calculate the normal vector for the tag in this transform
   */
  void Transform::GetNormalVector(const Camera& cam, float normal[3]) const {
    // project (0,0,0) and (0,0,1).  Take the difference between them and normalize it
    
    // (0,0,0)
    float proj0x = m_transform[3];
    float proj0y = m_transform[7];
    float proj0z = m_transform[11];
    float proj0h = m_transform[15];
    cam.CameraToWorld(proj0x/proj0h,proj0y/proj0h,proj0z/proj0h,&proj0x,&proj0y,&proj0z);
    
    // (0,0,1)
    float proj1x = m_transform[2] + m_transform[3];
    float proj1y = m_transform[6] + m_transform[7];
    float proj1z = m_transform[10] + m_transform[11];
    float proj1h = m_transform[14] + m_transform[15];
    cam.CameraToWorld(proj1x/proj1h,proj1y/proj1h,proj1z/proj1h,&proj1x,&proj1y,&proj1z);

    // (1,0,0)
    float proj2x = m_transform[0] + m_transform[3];
    float proj2y = m_transform[4] + m_transform[7];
    float proj2z = m_transform[8] + m_transform[11];
    float proj2h = m_transform[12] + m_transform[15];
    cam.CameraToWorld(proj2x/proj2h,proj2y/proj2h,proj2z/proj2h,&proj2x,&proj2y,&proj2z);

    // (0,1,0)
    float proj3x = m_transform[1] + m_transform[3];
    float proj3y = m_transform[5] + m_transform[7];
    float proj3z = m_transform[9] + m_transform[11];
    float proj3h = m_transform[13] + m_transform[15];
    cam.CameraToWorld(proj3x/proj3h,proj3y/proj3h,proj3z/proj3h,&proj3x,&proj3y,&proj3z);


    float v1x = proj2x - proj0x;
    float v1y = proj2y - proj0y;
    float v1z = proj2z - proj0z;

    float v2x = proj3x - proj0x;
    float v2y = proj3y - proj0y;
    float v2z = proj3z - proj0z;


    /*  
	normal[0] = proj1x/proj1h - proj0x/proj0h;
	normal[1] = proj1y/proj1h - proj0y/proj0h;
	normal[2] = proj1z/proj1h - proj0z/proj0h;
    */
  
  
    // normal vector is the cross product
    normal[0] = v1y * v2z - v1z * v2y;
    normal[1] = v1z * v2x - v1x * v2z;
    normal[2] = v1x * v2y - v1y * v2x;

    float modulus = -sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
  
    normal[0]/=modulus;
    normal[1]/=modulus;
    normal[2]/=modulus;

#ifdef APPLY_TRANSFORM_DEBUG
    PROGRESS("Found normal vector ("<<normal[0]<<","<<normal[1]<<","<<normal[2]<<")");
#endif

  }
  
  /**
   * Calculate the origin for this tag.
   *
   * \todo call cameratoworld as in GetNormal
   */
  void Transform::GetLocation(float location[3], float tag_size) const {
    location[0] = tag_size*m_transform[3]/m_transform[15];
    location[1] = tag_size*m_transform[7]/m_transform[15];
    location[2] = tag_size*m_transform[11]/m_transform[15];
#ifdef APPLY_TRANSFORM_DEBUG
    PROGRESS("Found location ("<<location[0]<<","<<location[1]<<","<<location[2]<<")");
#endif
  }
}
