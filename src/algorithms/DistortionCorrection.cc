/**
 * $Header$
 */

#include <total/algorithms/DistortionCorrection.hh>

namespace Total {

  /**
   * \todo fix me!
   */
  void DistortionCorrection::operator()(const ContourEntity& source, ContourEntity& destination) const {
    m_camera.ImageToNPCF(destination.points);
  }
}
