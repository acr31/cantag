#ifndef CAMERA_GUARD
#define CAMERA_GAURD

class Camera {
private:
  float m_extrinsic[9];
  float m_intrinsic[9];
  
  // radial distortion co-efficients
  float m_r1;
  float m_r2;
  float m_r3;
  float m_r4;
  float m_r5;
  float m_r6;

  // tangential distortion co-efficients
  float m_d1;
  float m_d2;

public:
  Camera();
  ~Camera();
  void SetExtrinsic(float extrinsic[9]);
  void SetIntrinsic(float intrinsic[9]);
  void SetIntrinsic(float xscale, float yscale, float principlex, float principley, float skew);
  void SetRadial(float r1,float r2,float r3,float r4,float r5,float r6);
  void SetTangential(float d1, float d2);

  void DistortPoints(float* points, int num_points);
  void UnDistortPoints(float* points, int num_points);  
};

#endif//CAMERA_GAURD
