/**
 * $Header$
 */

#include <total/algorithms/DistortionCorrection.hh>

namespace Total {

  bool DistortionCorrection::operator()(ContourEntity& destination) const {
    m_camera.ImageToNPCF(destination.GetPoints());
    return true;
  }
}
