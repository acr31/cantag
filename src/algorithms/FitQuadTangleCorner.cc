/*
  Copyright (C) 2004 Andrew C. Rice

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

#include <cantag/algorithms/FitQuadTangleCorner.hh>

#define LOGMAXWINDOW 5
#define CURVTHRESH -0.8
#define TOTAL_MASK(x) ((x) & ((1<<LOGMAXWINDOW)-1))
namespace Cantag {

  static float curvature(const float* xwindow, const float* ywindow, int datapointer, int k) {
    float ax = xwindow[TOTAL_MASK(datapointer+k)] - xwindow[datapointer];
    float ay = ywindow[TOTAL_MASK(datapointer+k)] - ywindow[datapointer];

    float bx = xwindow[TOTAL_MASK(datapointer-k)] - xwindow[datapointer];
    float by = ywindow[TOTAL_MASK(datapointer-k)] - ywindow[datapointer];

    float moda = sqrt(ax*ax+ay*ay);
    float modb = sqrt(bx*bx+by*by);

    float result = (ax*bx+ay*by)/moda/modb;

    return result;
  }

  bool FitQuadTangleCorner::operator()(const ContourEntity& contour, ShapeEntity<QuadTangle>& shape) const {
    const std::vector<float>& points = contour.GetPoints();

    if (points.size() > (2<<LOGMAXWINDOW)) {
      float xcorners[4];
      float ycorners[4];
      int cornerindices[4];
      float curvecorners[4];
      int corner_counter = 0;
      bool corner_set = false;
      float xwindow[1<<LOGMAXWINDOW];
      float ywindow[1<<LOGMAXWINDOW];
      int indexcounter = 0;
      int loadpointer = 0;
      std::vector<float>::const_iterator i = points.begin();
      for(;loadpointer < (1<<LOGMAXWINDOW);++loadpointer) {
	xwindow[loadpointer] = *i;
	++i;
	ywindow[loadpointer] = *i;
	++i;
      }
      int datapointer = 1<<(LOGMAXWINDOW-1);
    
      float curve2 = curvature(xwindow,ywindow,datapointer,10);
      if (curve2 > CURVTHRESH) {
	xcorners[0] = xwindow[datapointer];
	ycorners[0] = ywindow[datapointer];
	cornerindices[0] = datapointer;
	curvecorners[0] = curve2;
	++corner_counter;
      }
    
      float previous = curve2;
      float currentmax = -10;
      int count = points.size()/2;
      for(int c=1;c<count;++c) {
	datapointer = TOTAL_MASK(datapointer+1);
	loadpointer = TOTAL_MASK(loadpointer+1);
	xwindow[loadpointer] = *i;
	++i;
	ywindow[loadpointer] = *i;
	++i;
	++indexcounter;
	if (i == points.end()) { i = points.begin(); }
      
	float curve = curvature(xwindow,ywindow,datapointer,10);
	curve = -fabs(curve);
	if (curve < CURVTHRESH) { 
	  if (previous > CURVTHRESH && corner_set) {
	    ++corner_counter;
	    corner_set = false;
	    currentmax = -10;
	    if (corner_counter > 4) { return false; }
	  }
	}
	else {
	  if (curve > currentmax && corner_counter < 4) { 
	    currentmax = curve;
	    xcorners[corner_counter] = xwindow[datapointer];
	    ycorners[corner_counter] = ywindow[datapointer];
	    corner_set = true;
	    cornerindices[corner_counter] = datapointer+indexcounter;
	  }
	}
	previous = curve;
      }
    
      if (corner_counter == 4) {
	shape.SetShape(new QuadTangle(xcorners[0],ycorners[0],
				      xcorners[1],ycorners[1],
				      xcorners[2],ycorners[2],
				      xcorners[3],ycorners[3],
				      cornerindices[0],cornerindices[1],cornerindices[2],cornerindices[3]));
	return true;
      }
    }
    return false;

  }

}
