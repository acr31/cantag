/**
 * $Header$
 */

#ifndef DISTORTION_CORRECTION_GUARD
#define DISTORTION_CORRECTION_GUARD

#include <total/Entity.hh>
#include <total/Camera.hh>

namespace Total {
  class DistortionCorrection {
  public:
    typedef ContourEntity SourceType;
    typedef ContourEntity DestinationType;

  private:
    const Camera& m_camera;
  public:
    DistortionCorrection(const Camera& camera) : m_camera(camera) {};
    bool operator()(const ContourEntity& source, ContourEntity& dest) const;
  };
}

#endif//DISTORTION_CORRECTION_GUARD
