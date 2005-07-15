/**
 * $Header$
 */

#ifndef TAG_SPEC_GUARD
#define TAG_SPEC_GUARD

#include <total/Config.hh>
#include <total/SpeedMath.hh>
#include <total/coders/Coder.hh>

namespace Total {

  /**
   * Concrete superclass for tag specifications.
   */ 
  template<int PAYLOAD_SIZE>
  class TagSpec : public virtual Coder<PAYLOAD_SIZE> {
  private:
    /**
     * Number of degrees of rotational symmetry
     */
    int m_symmetry;

    /**
     * The number of data cells we go past when we rotate by one degree of symmetry
     */
    int m_cells_per_rotation;

  public:
    TagSpec(int symmetry, int cells_per_rotation) : m_symmetry(symmetry),m_cells_per_rotation(cells_per_rotation) {}

    inline void GetCellRotation(int cells, float& cos, float& sin) const {
      float angle = (float)(cells / m_cells_per_rotation) / (float)m_symmetry * 2*M_PI;
      if (angle > M_PI) angle = M_PI - angle;
      cos = DCOS(8,angle);
      sin = DSIN(8,angle);
    }
  };

};

#endif//TAG_SPEC_GUARD
