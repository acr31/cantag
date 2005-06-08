/**
 * $Header$
 */

#include <total/Config.hh>

class Transform {
private:
  float m_transform[16];

public:
  
  Transform();
  Transform(float* transform);

  float& operator[](int index) {
    return m_transform[index];
  }

  float operator[](int index) const {
    return m_transform[index];
  }

  void Apply(float x, float y, float* resx, float* resy) const;
  void Apply(float x, float y, float z,float* resx, float* resy) const;
  void Apply(float* points, int num_points) const;
};
