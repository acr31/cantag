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

/**
 * $Header$
 */


#include <cantag/algorithms/SimulateMinDistance.hh>

namespace Cantag {

  namespace Simulate { 
    double ComputeDistance(double xc, double yc, double x0, double y0, double x1, double y1) {
      double top = fabs( (x1-x0)*(y0-yc) - (x0-xc)*(y1-y0) );
      double bottom = sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) );
      
      return top/bottom;
    }
    
    double ComputeAll(double xc, double yc, double* corners, int corner_count) {
      double currentMin = INFINITY;
      for(int i=0;i<corner_count;++i) {
	double min = ComputeDistance(xc,yc,
				     corners[2*i],corners[2*i+1],
				     corners[(2*i+2) % (corner_count*2)], corners[(2*i+3) % (corner_count*2)]);
	if (min < currentMin) currentMin = min;
      }
      return currentMin;
    }
  }
}
