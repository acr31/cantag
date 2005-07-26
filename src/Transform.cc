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

#include <total/Transform.hh>

#undef TRANSFORM_DEBUG

namespace Total {

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

  /**
   * Rotate the tag by angle radians.
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

    for(int row=0;row<4;row++) {
      for(int col=0;col<4;col++) {
	for(int k=0;k<4;k++) {
	  m_transform[row*4+col] += store[row*4+k] * rotation[k*4+col];
	}
      }
    }
  }
}
