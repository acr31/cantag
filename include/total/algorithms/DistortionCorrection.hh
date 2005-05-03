/**
 * $Header$
 */

#ifndef DISTORTION_CORRECTION_GUARD
#define DISTORTION_CORRECTION_GUARD

#include <total/Entity.hh>
#include <total/Camera.hh>

namespace Total {
  class DistortionCorrection {
  private:
    const Camera& m_camera;
  public:
    DistortionCorrection(const Camera& camera) : m_camera(camera) {};
    void operator()(const ContourEntity& source, ContourEntity& dest) const;
  };
}

#endif//DISTORTION_CORRECTION_GUARD
