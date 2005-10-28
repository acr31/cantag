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

#include <cstring>

#include <cmath>
#include <cantag/algorithms/ConvexHull.hh>
#include <map>

namespace Cantag {


  bool ConvexHull::operator()(const ContourEntity& source, ConvexHullEntity& dest) const {
    const std::vector<float>& V = source.GetPoints();
    std::vector<int>& H = dest.GetIndices();
    


    // Find the point that has the greatest
    // x value. If there is a tie, take the
    // one with the highest y value
    int index=0;
    float ix=V[0],iy=V[1];
    for (int i=2; i<V.size();i+=2) {
      if (V[i]>ix) {
	ix=V[i];
	iy=V[i+1];
	index=i;
      }
      else if (V[i]==ix && V[i+1]>iy) {
	ix=V[i];
	iy=V[i+1];
	index=i;
      }
    }

    // Sort the points by angle (0->2pi)
    // with this point as origin
    std::map<float,int> imap;
    for (int i=0; i<V.size();i+=2) {
      if (i!=index) {
	float x = V[i]-ix;
	float y = V[i+1]-iy;
	float ang=atan2(y,x);
	if (ang<0.0) ang+=2*M_PI;
	std::map<float,int>::const_iterator ci = imap.find(ang);
	if (ci==imap.end()) imap[ang]=i/2;
	else {
	  int idx = ci->second;
	  float x1=V[i]-ix;
	  float y1=V[i+1]-iy;
	  float x2=V[2*idx]-ix;
	  float y2=V[2*idx+1]-iy;
	  if ((x1*x1+y1*y1) > (x2*x2+y2*y2)) imap[ang]=i/2;
	}
      }
    }

    // Start the hull
    H.push_back(index/2);
    std::map<float,int>::const_iterator ci = imap.begin();
    H.push_back(ci->second);
    ++ci;

    // Walk through the map
    // When we have to turn left add that vertex
    // When we have to turn right, remove the last vertex
    for (;ci!=imap.end();++ci) {
      float px = V[ci->second*2];
      float py = V[ci->second*2+1];
      float px_last = V[(H[H.size()-1])*2];
      float py_last = V[(H[H.size()-1])*2+1];
      float px_last2 = V[(H[H.size()-2])*2];
      float py_last2 = V[(H[H.size()-2])*2+1];

      float s_x = (px_last2-px_last);
      float s_y = (py_last2-py_last);

      float t_x = (px-px_last);
      float t_y = (py-py_last);

      if ((s_x*t_y - s_y*t_x)<0) {
	// Add this vertex
	H.push_back(ci->second);
      }
      else {
	float z=1;

	while (z>0 && H.size()>1) {
	  std::vector<int>::iterator dit = H.end();
	  dit--;
	  H.erase(dit);

	  float px_last = V[(H[H.size()-1])*2];
	  float py_last = V[(H[H.size()-1])*2+1];
	  float px_last2 = V[(H[H.size()-2])*2];
	  float py_last2 = V[(H[H.size()-2])*2+1];

	  s_x = (px_last2-px_last);
	  s_y = (py_last2-py_last);
	  t_x = (px-px_last);
	  t_y = (py-py_last);
	  z=(s_x*t_y - s_y*t_x);
	}
	// Add the new vertex
	H.push_back(ci->second);
      }
    }

    float px_last = V[(H[H.size()-1])*2];
    float py_last = V[(H[H.size()-1])*2+1];
    float px_last2 = V[(H[H.size()-2])*2];
    float py_last2 = V[(H[H.size()-2])*2+1];

    float s_x = (px_last2-px_last);
    float s_y = (py_last2-py_last);

    float t_x = (ix-px_last);
    float t_y = (iy-py_last);

    if ((s_x*t_y - s_y*t_x)>0) {
      // Remove the last vertex
      std::vector<int>::iterator vi = H.end();
      vi--;
      H.erase(vi);
    }

    return true;

  };
}
