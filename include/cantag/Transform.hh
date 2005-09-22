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

#ifndef TRANSFORM_GUARD
#define TRANSFORM_GUARD

#include <cantag/Config.hh>
#include <cantag/Camera.hh>

namespace Cantag {
  
  /**
   * A class to represent a transformation from camera co-ordinates to
   * object co-ordinates
   */
  class Transform {
  private:
    float m_transform[16];
    float m_confidence;
    
  public:
    
    Transform();
    Transform(float confidence);
    Transform(float* transform, float confidence);
    
    inline float& operator[](int index) { return m_transform[index]; }
    inline float operator[](int index) const { return m_transform[index]; }
    
    inline float GetConfidence() { return m_confidence;} 
    inline void SetConfidence(float confidence) { m_confidence = confidence; }
    inline void AccrueConfidence(float confidence) { m_confidence *= confidence; }

    /**
     * Invert the transform, assuming this is a valid affine transform
     */
    void Invert();
    
    void Apply(float x, float y, float* resx, float* resy) const;
    void Apply(float x, float y, float z,float* resx, float* resy) const;
    void Apply(float* points, int num_points) const;

    /**
     * Transform 3d object co-ordinates to 3d camera co-ordinates
     */
    void Apply3D(float* points, int num_points) const;
    
    /**
     * Rotate the tag's object co-ordinates by a specific angle. Takes
     * the sine and cos of this angle as arguments.
     */
    void Rotate(float cos,float sin);

    /**
     * Calculate the origin for this tag.
     */
    void GetLocation(float location[3], float tag_size) const;

    /**
     * Calculate the normal vector for the tag in this transform
     */
    void GetNormalVector(const Camera& cam, float normal[3]) const;
 
  };
}


#endif
