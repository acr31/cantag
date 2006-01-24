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

#define WINDOWSIZE 10
#define CURVTHRESH -0.6

namespace Cantag {

  static float curvature(const std::vector<float>& points, int ref, int i1, int i2) {
    float ax = points[2*i1]-points[2*ref];
    float ay = points[2*i1+1]-points[2*ref+1];
    float bx = points[2*i2]-points[2*ref];
    float by = points[2*i2+1]-points[2*ref+1];
    return (ax*bx+ay*by)/(sqrt(ax*ax+ay*ay)*sqrt((bx*bx+by*by)));
  }

  bool FitQuadTangleCorner::operator()(const ContourEntity& contour, ShapeEntity<QuadTangle>& shape) const {
    const std::vector<float>& points = contour.GetPoints();
    const int numindexes =  points.size()/2;
    int corner_index=-1;
    int corners[4];
    bool peak=false;
    float last_curv=curvature(points,numindexes-1, numindexes-1-WINDOWSIZE,WINDOWSIZE-1);
    
    for(unsigned int ii=0; ii<points.size()/2;ii++) {
      int p1 = ii-WINDOWSIZE;
      int p2 = (ii+WINDOWSIZE)%numindexes;
      while (p1<0) p1+=numindexes;
      float c = curvature(points,ii, p1,p2);
      
      if (c>CURVTHRESH && last_curv<=CURVTHRESH) {peak=true;corner_index++;}
      if (c<=CURVTHRESH && last_curv>CURVTHRESH) {peak=false;}
      if (peak && c>=last_curv) corners[corner_index]=ii;
      last_curv=c;
      if (corner_index>3) return false;
    }
    int i=0;
    while (peak && corner_index==3) {
      int p1 = i-WINDOWSIZE;
      int p2 = (i+WINDOWSIZE)%numindexes;
      while (p1<0) p1+=numindexes;
      float c = curvature(points,i, p1,p2);
      if (c<=CURVTHRESH && last_curv>CURVTHRESH) {peak=false;}
      if (peak && c>=last_curv) corners[corner_index]=i;
      last_curv=c;
      i++;
    }
    
    if (corner_index == 3) {
      shape.SetShape(new QuadTangle(points[2*corners[0]],points[2*corners[0]+1],
				    points[2*corners[1]],points[2*corners[1]+1],
				    points[2*corners[2]],points[2*corners[2]+1],
				    points[2*corners[3]],points[2*corners[3]+1],
				    corners[0],corners[1],corners[2],corners[3]));
      return true;
    }
    return false;   
  }

}
