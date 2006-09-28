/**
  Copyright (C) 2006 Andrew C. Rice, Robert K. Harle

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

#ifndef AUTO_THRESHOLD_GLOBAL_GUARD
#define AUTO_THRESHOLD_GLOBAL_GUARD

#include <cantag/MonochromeImage.hh>

namespace Cantag {

  /**
   * Inter-frame evolution of the thresholding value used in the
   * ThresholdGlobal algorithm based on a search for balancing the
   * number of black and white pixels in the image.  
   */
  class AutoThresholdGlobal {
  private:
    int m_previous_variation;
    int m_threshold_step;
    int m_current_threshold;
  public:
    
    AutoThresholdGlobal() : m_previous_variation(0), m_threshold_step(1), m_current_threshold(128) {};

    /**
     * Update the tracking threshold based on the new image.  Return
     * true if no major search was needed.
     */
    bool UpdateThreshold(const MonochromeImage& mi);
    inline int GetThreshold() const { return m_current_threshold; }
  };

}

#endif//AUTO_THRESHOLD_GLOBAL_GUARD
