#ifndef CAMERA_GUARD
#define CAMERA_GAURD

#include <Drawing.hh>

class Camera {
private:
  float m_extrinsic[9];
  float m_intrinsic[9];
  
  // radial distortion co-efficients
  float m_r2;
  float m_r4;
  float m_r6;

  // tangential distortion co-efficients
  float m_d1;
  float m_d2;

public:
  /**
   * Create a camera with default options - unit size, zero skew,
   * principle point at 0.5,0.5 and no distortion.
   */
  Camera();
  ~Camera();

  /** 
   * Set the extrinsic parameters for this camera - this is a 3d
   * transformation matrix from camera co-ordinates to world
   * co-ordinates
   */ 
  void SetExtrinsic(float extrinsic[9]);
  void SetIntrinsic(float intrinsic[9]);
  void SetIntrinsic(float xscale, float yscale, float principlex, float principley, float skew);
  void SetRadial(float r2,float r4,float r6);
  void SetTangential(float d1, float d2);

  void MapPoints(float* points, int num_points);
  void UnMapPoints(float* points, int num_points);  

  void UnDistortImage(Image* image);
 
};

#endif//CAMERA_GAURD
