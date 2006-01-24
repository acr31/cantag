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
#include <algorithm>
#include <cantag/algorithms/FitQuadTangleRegression.hh>

namespace Cantag {

  bool FitQuadTangleRegression::operator()(const ContourEntity& contour, ShapeEntity<QuadTangle>& shape) const {

    const std::vector<float>& points = contour.GetPoints();

    // Calculate the indexes of the current points
    std::vector<int> indexes(4);
    indexes[0] = shape.GetShape()->GetIndex0();
    indexes[1] = shape.GetShape()->GetIndex1();
    indexes[2] = shape.GetShape()->GetIndex2();
    indexes[3] = shape.GetShape()->GetIndex3();
	std::sort(indexes.begin(), indexes.end());
    // Now we have estimates of the corner indexes within points So do
    // some regression. We ignore the 4 points next to an estimated
    // corner since these are less reliable indicators of the side

    // Each line has equation y=mx+c
    // OR x=c

    float m[4]={0.0};
    float c[4]={0.0};
    bool  yeq[4]={1};
    for (int j=0; j<4; j++) {
      //      std::cerr << std::endl;
      float xsum=0.0;
      float ysum = 0.0;
      float xxsum=0.0;
      float xysum=0.0;
      int count=0;
      
      int start = indexes[j];
      int end = indexes[(j+1)%4]%(points.size()/2);
      
      if (end < start) end+=points.size()/2;

      // If there are only a few points along the side
      // we can't really regress anything!
      if (end-start < 5) return false;
      
      // Ignore the first and last 4 points
      float lastx=points[((start+4)*2)%points.size()];
      bool vertical=true;
      for (int i=start+4; i<end-4; i++) {
	int ii = i%(points.size()/2);
	
	float x = points[ii*2];
	float y = points[ii*2+1];
	//	std::cerr << x << "  " << y << std::endl;
	xsum+=x;
	ysum+=y;
	xxsum+=x*x;
	xysum+=x*y;
	count++;
	if (fabs(x-lastx) > 1e-5)  vertical=false;
	lastx = x;
      }

      if (vertical) {
	c[j] = lastx;
	yeq[j]=0;
      }
      else {
	float numer = (xysum - xsum*ysum/(float)count);
	float denom = (xxsum - xsum*xsum/(float)count);
	m[j] = numer/denom;
	c[j] = ysum/(float)count - m[j]*xsum/(float)count;
	yeq[j]=1;
      }
    }
    
    // Now have four lines and we need the intersections
    
    float xres[4];
    float yres[4];
    
    
    
    for (int j=0; j<4; j++) {
      //    std::cout << "Y " << yeq[j] << " " << (j-1)%4 << std::endl;
      int lastj = (j-1);
      if (lastj==-1) lastj=3;
      if (yeq[j] && yeq[lastj]) {
	xres[j] = (c[lastj]-c[j])/(m[j]-m[lastj]);
	yres[j] = m[j]*xres[j]+c[j];
      }
      else if (yeq[j] && !yeq[lastj]) {
	xres[j] = c[lastj];
	yres[j] = m[j]*xres[j] + c[j];
      }
      else if (!yeq[j] && yeq[lastj]) {
	xres[j] = c[j];
	yres[j] = m[lastj]*xres[j] + c[lastj];
      }
      else {
	// so they don't intersect
	return false;
      }
    }

//     std::cout << "L " << xres[0] << " " << yres[0] << std::endl;
//     std::cout << "L " << xres[1] << " " << yres[1] << std::endl;
//     std::cout << "L " << xres[2] << " " << yres[2] << std::endl;
//     std::cout << "L " << xres[3] << " " << yres[3] << std::endl;
//     std::cout << "L " << xres[0] << " " << yres[0] << std::endl;
//     std::cout << std::endl;

    shape.GetShape()->Set(xres[0],yres[0],
			  xres[1],yres[1],
			  xres[2],yres[2],
			  xres[3],yres[3],
			  -1,-1,-1,-1);

    return true;

  }

}
