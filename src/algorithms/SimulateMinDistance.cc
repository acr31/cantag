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

    // Copyright 2001, softSurfer (www.softsurfer.com)
    // This code may be freely used and modified for any purpose
    // providing that this copyright notice is included with it.
    // SoftSurfer makes no warranty for this code, and cannot be held
    // liable for any real or imagined damage resulting from its use.
    // Users of this code must verify correctness for their application.
    double ComputeDistance(double xc, double yc, double x0, double y0, double x1, double y1) {

      double vx = x1 - x0;
      double vy = y1 - y0;
      
      double wx = xc - x0;
      double wy = yc - y0;

      double c1 = wx*vx+wy*vy;
      double c2 = vx*vx+vy*vy;

      double result;
      if (c1 <= 0.0) {
	result = sqrt( (xc-x0)*(xc-x0) + (yc-y0)*(yc-y0) );
      }
      else if (c2 <= c1) {
	result =  sqrt( (xc-x1)*(xc-x1) + (yc-y1)*(yc-y1) );
      }
      else {
	double b = c1/c2;
	double px = x0 + b*vx;
	double py = y0 + b*vy;
	
	result =  sqrt( (xc-px)*(xc-px) + (yc-py)*(yc-py) );
      }

      return result;
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
