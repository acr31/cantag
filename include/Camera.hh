#ifndef CAMERA_GUARD
#define CAMERA_GUARD

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
   * (homogenous) transformation matrix from camera co-ordinates to world
   * co-ordinates
   */ 
  void SetExtrinsic(float extrinsic[16]);

  /**
   * Set the intrinsic parameters for this camera. This is a matrix
   * containing x and y scale factors, principle point and skew
   */
  void SetIntrinsic(float intrinsic[9]);

  /**
   * Set the intrinsic parameters for this camera to the given values.
   */
  void SetIntrinsic(float xscale, float yscale, float principlex, float principley, float skew);

  /**
   * Set the radial distortion parameters.  This method accepts
   * co-efficients for the radius squared, to the four and to the six.
   */
  void SetRadial(float r2,float r4,float r6);

  /**
   * Set the tangential distortion co-efficients for this camera.
   */
  void SetTangential(float d1, float d2);

  /**
   * Convert a set of image co-ordinates to normalised principle
   * co-ordinate frame (NPCF) points.  This involves removing the
   * distortion predicted by the parameters set and removing the
   * extrinsic parameters of the camera.
   */
  void ImageToNPCF(float* points, int num_points);

  /**
   * Convert a set of normalised principle co-ordinate frame (NPCF)
   * points to image points.  This involves applying the correct
   * distortion predicted by the camera parameters and applying the
   * extrinsic parameters of the camera.
   */
  void NPCFToImage(float* points, int num_points);  

  /**
   * Remove radial and tangential distortion from the whole image.
   */
  void UnDistortImage(Image* image);
 
};

#endif//CAMERA_GAURD
