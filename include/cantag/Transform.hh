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
#include <iostream>

namespace Cantag {
  
  class LocationElement;
  class PoseElement;
  class SizeElement;

  /**
   * A class to represent a general affine transformation
   */
  class Transform {
  private:
    float m_transform[16];
    float m_confidence;
    
  public:
    
    Transform();
    Transform(float confidence);
    Transform(float* transform, float confidence);
    Transform(const LocationElement& loc,const PoseElement& pose, const SizeElement& size);
    Transform(float x, float y, float z, float theta, float phi, float psi, float size);
    
    inline float& operator[](int index) { return m_transform[index]; }
    inline float operator[](int index) const { return m_transform[index]; }
    
    inline float GetConfidence() { return m_confidence;} 
    inline void SetConfidence(float confidence) { m_confidence = confidence; }
    inline void AccrueConfidence(float confidence) { m_confidence *= confidence; }

    /**
     * Invert the transform, *assuming* this is a valid affine transform
     */
    void Invert();

    /**
     * Convert the rotation matrix part of the transform
     * into a more useful angular representation. .
     *
     * The convention is that theta and phi are spherical polar
     * coordinates. Phi is the angle from the x axis in the x-y plane.
     * Theta is the angle from the z axis. The phi and theta specify
     * a unit vector about which we rotate by psi degrees using the
     * right hand rule
     *
     * For clarity, the unit axis vector n relates as follows:
     * nx = sin(theta)cos(phi)
     * ny = sin(theta)sin(phi)
     * nz = cos(theta)
     *
     */
    void GetAngleRepresentation(float *theta, float *phi, float *psi) const ;


    /**
     * Set the Transform contents based on the information supplied as a position, 
     * pose and scale
     */
    void SetupFromAngles(float x, float y, float z, float theta, float phi, float psi, float size);
  
 
    void Apply(float x, float y, float* resx, float* resy) const;
    void Apply(float x, float y, float z,float* resx, float* resy) const;
    void Apply(float* points, int num_points) const;
    void Apply(std::vector<float>& points) const;

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
     * Calculate the origin for this tag.
     */
    void GetLocation(const Camera& cam, float location[3], float tag_size) const;

    /**
     * Calculate the normal vector for the tag in this transform
     */
    void GetNormalVector(const Camera& cam, float normal[3]) const;

    /**
     * Check the 3x3 rotation matrix and correct if
     * the determinant is not 1. This is used
     * to check for floating point errors
     * which can cause the |det| to be >1
     */
    bool NormaliseRotation();

    void Print() const {
      for (int i=0; i<16; i++) {
	if (i%4==0) std::cout << std::endl;
	std::cout << m_transform[i] << " ";
      }
      std::cout << std::endl;   
    }

    /**
     * Mutiply transforms together;
     */
    friend Transform operator*(const Transform &a, const Transform &b);
  };
}

#else
namespace Cantag {
  class Transform;
}
#endif
