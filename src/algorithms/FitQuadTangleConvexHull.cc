/*
  Copyright (C) 2004 Robert K. Harle

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

  Email: rkh23@cam.ac.uk
*/

/**
 * $Header$
 */

#include <map>

#include <cantag/algorithms/FitQuadTangleConvexHull.hh>

namespace Cantag {

  bool FitQuadTangleConvexHull::operator()(const ContourEntity& contour, const ConvexHullEntity& hull, ShapeEntity<QuadTangle>& shape) const {
    
    const std::vector<float>& points = contour.GetPoints();

    const std::vector<int>& h = hull.GetIndices();
    int n = h.size();
    if (n<4) return false;
    
    // Throw away vertices with large angles ~ 180
    // It doesn't matter too much if some get through
    std::multimap<float,int> angles;
    for (int i=0; i<n; i++) {
      int lastidx = i-1;
      if (lastidx==-1) lastidx=n-1;
      float lx = points[h[lastidx]*2] - points[h[i]*2];
      float ly = points[h[lastidx]*2+1] - points[h[i]*2+1];
      float nx = points[h[(i+1)%n]*2] - points[h[i]*2];
      float ny = points[h[(i+1)%n]*2+1] - points[h[i]*2+1];
      float ct = (lx*nx+ly*ny)/(sqrt(lx*lx+ly*ly)*sqrt(nx*nx+ny*ny));
      if (abs(ct) <0.98f) angles.insert( std::pair<float,int>(abs(ct),h[i]) );
    }

    // If there aren't 4 vertices left, this can't be a quadtangle
    if (angles.size()<4) { return false; }

    std::multimap<float,int>::const_iterator ci = angles.begin();
    std::vector<int> indexes;
    
    // Copy the remaining vertex indices
    // to a vector and sort them numerically
    ci = angles.begin();
    for (unsigned int i=0; i<angles.size(); i++) {
      indexes.push_back (ci->second);
      ++ci;
    }
    sort(indexes.begin(), indexes.end());
    

    if (indexes.size()>4) {
      // Have too many vertices left :-(
      
      // Find the longest side in the current poly
      float longest = 0.0;
      
      for (unsigned int i=0; i<indexes.size(); i++) {
	float xd = points[indexes[i]*2] - points[indexes[(i+1)%indexes.size()]*2];
	float yd = points[indexes[i]*2+1] - points[indexes[(i+1)%indexes.size()]*2+1];
	if ((xd*xd+yd*yd) > longest) longest = xd*xd+yd*yd;
      }
      
      // Iterate over again, storing the index and
      // its ensuing side length relative to the longest
      std::multimap<float,int> dist;    
      for (unsigned int i=0; i<indexes.size(); i++) {
	float xd = points[indexes[i]*2] - points[indexes[(i+1)%indexes.size()]*2];
	float yd = points[indexes[i]*2+1] - points[indexes[(i+1)%indexes.size()]*2+1];
	dist.insert( std::pair<float,int>(-(xd*xd+yd*yd)/longest,indexes[i]));
      }
      
      // The vertices associated with the 4 longest sides 
      // will do
      std::multimap<float,int>::const_iterator di = dist.begin();
      indexes.clear();
      for (int i=0; i<4; i++) {
	indexes.push_back(di->second);
	di++;
      }
    }
    sort(indexes.begin(), indexes.end());

    shape.SetShape(new QuadTangle(points[indexes[0]*2],points[indexes[0]*2+1],
				  points[indexes[1]*2],points[indexes[1]*2+1],
				  points[indexes[2]*2],points[indexes[2]*2+1],
				  points[indexes[3]*2],points[indexes[3]*2+1],
				  indexes[0],indexes[1],indexes[2],indexes[3]));
    return true;
  }
}
