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

#include <map>

#include <cantag/algorithms/FitQuadTangleConvexHull.hh>

namespace Cantag {

  bool FitQuadTangleConvexHull::operator()(const ContourEntity& contour, ShapeEntity<QuadTangle>& shape) const {
    
    const std::vector<float>& points = contour.GetPoints();

    // Take a convex hull of the polyline ( O(n) )
    int h[points.size()/2];
    int n = ConvexHull(points,points.size()/2,h);
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
      if (fabs(ct) <0.98) angles.insert( std::pair<float,int>(fabs(ct),h[i]) );
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

    shape.m_shapeDetails = new QuadTangle(points[indexes[0]*2],points[indexes[0]*2+1],
					  points[indexes[1]*2],points[indexes[1]*2+1],
					  points[indexes[2]*2],points[indexes[2]*2+1],
					  points[indexes[3]*2],points[indexes[3]*2+1]);
    shape.m_shapeFitted=true;
    return true;
  }
  
  float FitQuadTangleConvexHull::isLeft(const std::vector<float> &V,int l0, int l1, int p) const {
    return (V[l1*2] - V[l0*2])*(V[p*2+1] - V[l0*2+1]) - (V[p*2] - V[l0*2])*(V[l1*2+1] - V[l0*2+1]);
  }

  /** 
   * The hull code is adapted from softSurfer who requires the
   * following copyright be displayed: Copyright 2001, softSurfer
   * (www.softsurfer.com) This code may be freely used and modified
   * for any purpose providing that this copyright notice is included
   * with it.
   */
  int FitQuadTangleConvexHull::ConvexHull(const std::vector<float> &V, int n, int* H) const {
    // initialize a deque D[] from bottom to top so that the
    // 1st three vertices of V[] are a counterclockwise triangle
    int D[2*n+1];
    for (int i=0;i<2*n+1; i++) D[i]=0;
    int bot = n-2, top = bot+3;   // initial bottom and top deque indices
    D[bot] = D[top] = 2;       // 3rd vertex is at both bot and top
    if (isLeft(V, 0, 1, 2) > 0) {
      D[bot+1] = 0;
      D[bot+2] = 1;          // ccw vertices are: 2,0,1,2
    }
    else {
      D[bot+1] = 1;
      D[bot+2] = 0;          // ccw vertices are: 2,1,0,2
    }
    
    // compute the hull on the deque D[]
    for (int i=3; i < n; i++) {   // process the rest of vertices
      // test if next vertex is inside the deque hull
      if ((isLeft(V, D[bot], D[bot+1], i) >= 0) &&
	  (isLeft(V, D[top-1],D[top], i) >= 0) )
	continue;         // skip an interior vertex
      
      // incrementally add an exterior vertex to the deque hull
      // get the rightmost tangent at the deque bot
      while (isLeft(V,D[bot], D[bot+1], i) <= 0)
	++bot;                // remove bot of deque
      D[--bot] = i;          // insert V[i] at bot of deque
      
      // get the leftmost tangent at the deque top
      while (isLeft(V,D[top-1],D[top], i) <= 0)
	--top;                // pop top of deque
      D[++top] = i;          // push V[i] onto top of deque
      if (top-bot < 2) return -1;
    }
    
    // transcribe deque D[] to the output hull array H[]
    int h;        // hull vertex counter
    for (h=0; h <= (top-bot); h++) {
      H[h] = D[bot + h];
    } 
    return h-1;
  }

}
