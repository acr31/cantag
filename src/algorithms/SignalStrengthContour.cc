/*
  Copyright (C) 2005 Andrew C. Rice

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

#include <cantag/algorithms/SignalStrengthContour.hh>

namespace Cantag {
  
  /**
   * Go round the points in the contour and work out the minimum
   * distance from the centre to the edge
   */
  bool SignalStrengthContour::operator()(const ContourEntity& ce, SignalStrengthEntity& se) const {

    float min_distance = FLT_MAX;
    
    const float centralX = ce.GetCentralX();
    const float centralY = ce.GetCentralY();

    for(std::vector<float>::const_iterator i = ce.GetPoints().begin();i!=ce.GetPoints().end();++i) {
      const float x = *(i++);
      const float y = *i;
      
      float distance = (centralX - x)*(centralX - x) + (centralY - y)*(centralY - y);

      if (distance < min_distance) min_distance = distance;      
    }
    
    se.SetSignalStrength(sqrt(min_distance), ce.GetWidth() < ce.GetHeight() ? ce.GetWidth() : ce.GetHeight());

    return true;
  }
}
