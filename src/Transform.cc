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
#include <cantag/TagDictionary.hh>
#undef TRANSFORM_DEBUG

namespace Cantag {

  Transform::Transform() : m_confidence(0.0) {}
  
  Transform::Transform(float confidence) : m_confidence(confidence) {};

  Transform::Transform(float* transform, float confidence) : m_confidence(confidence) {
    for(int i=0;i<16;++i) {
      m_transform[i] = transform[i];
    }
  }

  Transform::Transform(const LocationElement& loc,const PoseElement& pose, const SizeElement& size_e) : m_confidence(1.f) {
    float x = loc.x;
    float y = loc.y;
    float z = loc.z;
    float theta = pose.theta;
    float phi = pose.phi;
    float psi = pose.psi;
    float size = size_e.tag_size;
    SetupFromAngles(x,y,z,theta,phi,psi,size);
  }


  Transform::Transform(float x, float y, float z, float theta, float phi, float psi, float size) : m_confidence(1.f) {
    SetupFromAngles(x, y, z,theta, phi, psi, size);
  }

 

  void Transform::SetupFromAngles(float x, float y, float z, float theta, float phi, float psi, float size) {
    // See http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/index.htm
    // for explanation of pose computation
    m_transform[3] = size*x;
    m_transform[7] = size*y;
    m_transform[11] = size*z;
    m_transform[15] = 1.0;

    // Best not to use DCOS etc here
    // We need the precision!
    float nx = sin(theta)*cos(phi);
    float ny = sin(theta)*sin(phi);
    float nz = cos(theta);
    
    float c = cos(psi);
    float s = sin(psi);
    float t = 1-cos(psi);

    m_transform[0] = t*nx*nx+c;
    m_transform[1] = t*nx*ny-nz*s;
    m_transform[2] = t*nx*nz+ny*s;

    m_transform[4] = t*nx*ny+nz*s;
    m_transform[5] = t*ny*ny+c;
    m_transform[6] = t*ny*nz-nx*s;

    m_transform[8] = t*nx*nz-ny*s;
    m_transform[9] = t*ny*nz+nx*s;
    m_transform[10] = t*nz*nz+c;

    m_transform[12] = 0.0;
    m_transform[13] = 0.0;
    m_transform[14] = 0.0;
  }


  void Transform::Apply(float* points, int numpoints) const {
    for(int i=0;i<numpoints*2;i+=2) {
      Apply(points[i],points[i+1],points+i,points+i+1);
    }
  }

  void Transform::Apply(std::vector<float>& points) const {
    for(int i=0;i<points.size();i+=2) {
      float resultx;
      float resulty;
      Apply(points[i],points[i+1],&resultx,&resulty);
      points[i] = resultx;
      points[i+1] = resulty;
    }
  }

  void Transform::Invert() {
    float a[16]={0.0}, b[16]={0.0};
    // The inversion separates the rotation from the translation
    // and inverts them separately, the multiplies them together

    // Rotation 3x3 is transported
    a[15]=1.0;
    a[0]=m_transform[0];
    a[4]=m_transform[1];
    a[8]=m_transform[2];
    
    a[1]=m_transform[4];
    a[5]=m_transform[5];
    a[9]=m_transform[6];
    
    a[2]=m_transform[8];
    a[6]=m_transform[9];
    a[10]=m_transform[10];

    // Translation is an easy inversion!
    b[0]=1.0;
    b[5]=1.0;
    b[10]=1.0;
    b[15]=1.0;
    b[3] = -m_transform[3];
    b[7] = -m_transform[7];
    b[11] = -m_transform[11];

    // Multiply a * b
    m_transform[0]  = a[0]*b[0] + a[1]*b[4] + a[2]*b[8] + a[3]*b[12];
    m_transform[1]  = a[0]*b[1] + a[1]*b[5] + a[2]*b[9] + a[3]*b[13];
    m_transform[2]  = a[0]*b[2] + a[1]*b[6] + a[2]*b[10] + a[3]*b[14];
    m_transform[3]  = a[0]*b[3] + a[1]*b[7] + a[2]*b[11] + a[3]*b[15];
    
    m_transform[4]  = a[4]*b[0] + a[5]*b[4] + a[6]*b[8] + a[7]*b[12];
    m_transform[5]  = a[4]*b[1] + a[5]*b[5] + a[6]*b[9] + a[7]*b[13];
    m_transform[6]  = a[4]*b[2] + a[5]*b[6] + a[6]*b[10] + a[7]*b[14];
    m_transform[7]  = a[4]*b[3] + a[5]*b[7] + a[6]*b[11] + a[7]*b[15];
    
    m_transform[8]  = a[8]*b[0] + a[9]*b[4] + a[10]*b[8] + a[11]*b[12];
    m_transform[9]  = a[8]*b[1] + a[9]*b[5] + a[10]*b[9] + a[11]*b[13];
    m_transform[10] = a[8]*b[2] + a[9]*b[6] + a[10]*b[10] + a[11]*b[14];
    m_transform[11] = a[8]*b[3] + a[9]*b[7] + a[10]*b[11] + a[11]*b[15];
    
    m_transform[12] = a[12]*b[0] + a[13]*b[4] + a[14]*b[8] + a[15]*b[12];
    m_transform[13] = a[12]*b[1] + a[13]*b[5] + a[14]*b[9] + a[15]*b[13];
    m_transform[14] = a[12]*b[2] + a[13]*b[6] + a[14]*b[10] + a[15]*b[14];
    m_transform[15] = a[12]*b[3] + a[13]*b[7] + a[14]*b[11] + a[15]*b[15];
    
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
      float projH = m_transform[12]*x + m_transform[3]*y + m_transform[14]*z + m_transform[15];
      
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

  /**
   * Calculate the origin for this tag.
   */
  void Transform::GetLocation(const Camera& camera, float location[3], float tag_size) const {
    location[0] = tag_size*m_transform[3]/m_transform[15];
    location[1] = tag_size*m_transform[7]/m_transform[15];
    location[2] = tag_size*m_transform[11]/m_transform[15];
    camera.CameraToWorld(location[0],location[1],location[2],location,location+1,location+2);
#ifdef APPLY_TRANSFORM_DEBUG
    PROGRESS("Found location ("<<location[0]<<","<<location[1]<<","<<location[2]<<")");
#endif
  }


  void Transform::GetAngleRepresentation(float *theta, float *phi, float *psi) const {
    // See http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToAngle/index.htm
    // for explanation.  This code derives the angle/axis values then converts the axis to a spherical 
    // polars representation (theta,phi), where phi is in the xy plane.

    static const float epsilon = 0.001;
    
    // Is this matrix symmetric?
    if ( fabs(m_transform[9]-m_transform[6]) < epsilon &&
	 fabs(m_transform[8]-m_transform[2]) < epsilon &&
	 fabs(m_transform[4]-m_transform[1]) < epsilon ) {
      
      if ( fabs(m_transform[9]+m_transform[6]) < epsilon &&
	   fabs(m_transform[8]+m_transform[2]) < epsilon &&
	   fabs(m_transform[4]+m_transform[1]) < epsilon ) {
	*theta=0.0;
	*phi=0.0;
	*psi=0.0;
	return;
      }
      else {
	*psi=M_PI;;

	float x = sqrt((m_transform[0]+1.0)/2.0);
	float y = sqrt((m_transform[5]+1.0)/2.0);
	float z = sqrt((m_transform[10]+1.0)/2.0);

	bool xZero = fabs(x)<epsilon;
	bool yZero = fabs(y)<epsilon;
	bool zZero = fabs(z)<epsilon;
	bool xyPositive = (m_transform[1] > 0);
	bool xzPositive = (m_transform[2] > 0);
	bool yzPositive = (m_transform[6] > 0);
	if (xZero && !yZero && !zZero) y = -y;
	else if (yZero && !zZero) z = -z;
	else if (zZero) x = -x;
	else if (xyPositive && xzPositive && yzPositive) return;
	else if (yzPositive) x=-x;
	else if (xzPositive) y=-y;
	else if (xyPositive) z=-z;

	*theta = acos(z);
	*phi = atan2(y,z);
      }
    }
    else {
      *psi = acos( (m_transform[0]+m_transform[5]+m_transform[10]-1.0)/2.0 );
      float denom = sqrt( (m_transform[6]-m_transform[9])*(m_transform[6]-m_transform[9]) +
			  (m_transform[2]-m_transform[8])*(m_transform[2]-m_transform[8]) +
			  (m_transform[4]-m_transform[1])*(m_transform[4]-m_transform[1]) );
      float nx = (m_transform[9] - m_transform[6])/denom;
      float ny = (m_transform[2] - m_transform[8])/denom;
      float nz = (m_transform[4] - m_transform[1])/denom;
      
      *theta = acos(nz);
      *phi = atan2(ny,nz);
    }   
  }


  Transform operator*(const Transform &a, const Transform &b) {
    Transform r;
    r[0]  = a[0]*b[0] + a[1]*b[4] + a[2]*b[8] + a[3]*b[12];
    r[1]  = a[0]*b[1] + a[1]*b[5] + a[2]*b[9] + a[3]*b[13];
    r[2]  = a[0]*b[2] + a[1]*b[6] + a[2]*b[10] + a[3]*b[14];
    r[3]  = a[0]*b[3] + a[1]*b[7] + a[2]*b[11] + a[3]*b[15];
    
    r[4]  = a[4]*b[0] + a[5]*b[4] + a[6]*b[8] + a[7]*b[12];
    r[5]  = a[4]*b[1] + a[5]*b[5] + a[6]*b[9] + a[7]*b[13];
    r[6]  = a[4]*b[2] + a[5]*b[6] + a[6]*b[10] + a[7]*b[14];
    r[7]  = a[4]*b[3] + a[5]*b[7] + a[6]*b[11] + a[7]*b[15];
    
    r[8]  = a[8]*b[0] + a[9]*b[4] + a[10]*b[8] + a[11]*b[12];
    r[9]  = a[8]*b[1] + a[9]*b[5] + a[10]*b[9] + a[11]*b[13];
    r[10] = a[8]*b[2] + a[9]*b[6] + a[10]*b[10] + a[11]*b[14];
    r[11] = a[8]*b[3] + a[9]*b[7] + a[10]*b[11] + a[11]*b[15];
    
    r[12] = a[12]*b[0] + a[13]*b[4] + a[14]*b[8] + a[15]*b[12];
    r[13] = a[12]*b[1] + a[13]*b[5] + a[14]*b[9] + a[15]*b[13];
    r[14] = a[12]*b[2] + a[13]*b[6] + a[14]*b[10] + a[15]*b[14];
    r[15] = a[12]*b[3] + a[13]*b[7] + a[14]*b[11] + a[15]*b[15];

    return r;
  }


  bool Transform::NormaliseRotation() {
    // Compute the determinant of the 3x3 rottation part of the trans
    float det = m_transform[0]*(m_transform[5]*m_transform[10]-m_transform[9]*m_transform[6]);
    det -= m_transform[1]*(m_transform[4]*m_transform[10]-m_transform[6]*m_transform[8]);
    det += m_transform[2]*(m_transform[4]*m_transform[9]-m_transform[5]*m_transform[8]);
    
    if (det==1.0) return false;

    for (int i=0; i<11; i++) {
      if ((i!=3) && (i!=7)) m_transform[i]/= det;
    }
    return true;
  }



}
