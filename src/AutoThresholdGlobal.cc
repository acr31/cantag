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


#include <cantag/AutoThresholdGlobal.hh>

namespace Cantag {
  
  bool AutoThresholdGlobal::UpdateThreshold(const MonochromeImage& mi) {
    
    const int var = mi.GetVariation();
    const int image_size_2 = mi.GetSize() >> 1;
  
    const int add_step = 2;

    // we are happy with the threshold when the variation in the image
    // is within some percentage of 50% of the number of pixels in the
    // image.
    // this value is 100/desired percentage
    const int steady_proportion = 10;
    
    bool result = true;
    if ((((var-image_size_2)*steady_proportion)/image_size_2) == 0) { 
      m_current_threshold += m_previous_variation < var ? -1 : 1;
      if (m_current_threshold < 1) m_current_threshold = 1;
      if (m_current_threshold > 254) m_current_threshold = 254;
      return false;
    }
    else {
      m_current_threshold += ((var > image_size_2) ? -m_threshold_step : m_threshold_step);
      if (m_current_threshold < 1) m_current_threshold = 1;
      if (m_current_threshold > 254) m_current_threshold = 254;

      if ((m_previous_variation > image_size_2) ^
	  (var > image_size_2)) {
	m_threshold_step /= 2;
	if (m_threshold_step == 0) m_threshold_step = 1;
      }
      else {
	m_threshold_step += add_step;
      }

      m_previous_variation = var;	    
      result =false;
    }
    return result;
  }
}
