/**
 * $Header$
 */

#include <total/Transform.hh>


#undef TRANSFORM_DEBUG

Transform::Transform() {};

Transform::Transform(float* transform) {
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
