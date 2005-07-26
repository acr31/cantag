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

#include <cstring>

#include <cmath>
#include <cantag/algorithms/ConvexHull.hh>

namespace Cantag {

  static inline float isLeft(const std::vector<float> &V,int l0, int l1, int p) {
    return (V[l1*2] - V[l0*2])*(V[p*2+1] - V[l0*2+1]) - (V[p*2] - V[l0*2])*(V[l1*2+1] - V[l0*2+1]);
  }


  /** 
   * The hull code is adapted from softSurfer who requires the
   * following copyright be displayed: Copyright 2001, softSurfer
   * (www.softsurfer.com) This code may be freely used and modified
   * for any purpose providing that this copyright notice is included
   * with it.
   */
  bool ConvexHull::operator()(const ContourEntity& source, ConvexHullEntity& dest) const {
    const std::vector<float>& V = source.GetPoints();
    const int n = V.size()/2;
    std::vector<int>& H = dest.GetIndices();
    H.resize(V.size()/2+2);
    // initialize a deque D[] from bottom to top so that the
    // 1st three vertices of V[] are a counterclockwise triangle
    int D[2*n+1];
    memset(D,0,2*n+1);
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
      while (isLeft(V,D[bot], D[bot+1], i) <= 0) ++bot; // remove bot of deque
      D[--bot] = i;          // insert V[i] at bot of deque
      
      // get the leftmost tangent at the deque top
      while (isLeft(V,D[top-1],D[top], i) <= 0) --top; // pop top of deque
      D[++top] = i;          // push V[i] onto top of deque
      if (top-bot < 2) return false;
    }
    
    // transcribe deque D[] to the output hull array H[]
    int h;        // hull vertex counter
    for (h=0; h <= (top-bot); h++) {
      H[h] = D[bot + h];
    } 

    // now work out if any of the points not on the convex hull are
    // too far from the hull itself we do this by taking each pair of
    // indices from the index list and measuring the shortest distance
    // of each point between the indices to the line
    int index = -1;
    for(std::vector<int>::const_iterator i = H.begin();i!=H.end();++i) {
      if (index != -1) {
	for(int count=index+1;count<*i;++count) {
	  float dist = (V[2*index]-V[2*count])*(V[2*index]-V[2*count]) +
	    (V[2*index+1]-V[2*count+1])*(V[2*index+1]-V[2*count+1]);
	  if (dist > m_restriction.GetMaxDeviation()) {
	    return false;
	  }
	}
      }
      index = *i;
    }

    return true;
    
  };
}
