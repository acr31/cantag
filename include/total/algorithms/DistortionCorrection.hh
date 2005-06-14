/**
 * $Header$
 */

#ifndef DISTORTION_CORRECTION_GUARD
#define DISTORTION_CORRECTION_GUARD

#include <total/ContourEntity.hh>
#include <total/Camera.hh>
#include <total/Function.hh>

namespace Total {
  class DistortionCorrection : public Function0<ContourEntity> {
  private:
    const Camera& m_camera;
  public:
    DistortionCorrection(const Camera& camera) : m_camera(camera) {};
    bool operator()(ContourEntity& dest) const;
  };
}

#endif//DISTORTION_CORRECTION_GUARD
