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

#ifndef FITQUADTANGLEPOLYGON_GUARD
#define FITQUADTANGLEPOLYGON_GUARD

#include <cantag/QuadTangle.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/Function.hh>

namespace Cantag {
  class FitQuadTanglePolygon : public Function<TL1(ContourEntity),TL1(ShapeEntity<QuadTangle>) > {
  public:
    typedef ContourEntity SourceType;
    typedef ShapeEntity<QuadTangle> DestinationType;


    bool operator()(const ContourEntity& contour, ShapeEntity<QuadTangle>& shape) const;

  private:
    /**
     * Compute the cosine of the angle between the vectors pq and qr
     */
    float DPAngle(const std::pair<float,float>& p, const std::pair<float,float>& q, const std::pair<float,float>& r) const ;

    /**
     * Join two lists of vertexes together.  List 1 is of the form
     * [....,y,z] and List 2 of the form [a,b,...].  If the curvature of
     * yz to za is less than the threshold (or in our case the cosine of
     * the curvature is greater than the threshold) then delete point 'z'.
     * If the curvature of za to ab is less than the threshold then
     * delete point 'a'.
     *
     * fulllist is the list of all the working vertexes
     * start is an iterator pointing to the start of the list 1 window (inclusive)
     * mid is an iterator pointing at the end of the list 1 window (exclusive)
     * mid is also an iterator pointing at the start of the list 2 window (inclusive) 
     * this must be one place in the list past that pointed to by reverse_mid
     * end is an iterator pointing at the end of the list 2 window (exclusive)
     */
    void DPJoin(std::list<std::pair<float,float> >& fulllist,
		std::list<std::pair<float,float> >::iterator start,
		std::list<std::pair<float,float> >::iterator mid,
		std::list<std::pair<float,float> >::iterator end) const;

    /**
     * Split the list by finding the point on the contour that is
     * maximally far from the edge running from the first point to the
     * last point.  
     *
     * Returns an iterator pointing to the first value of the second list in the split
     *
     * fulllist is the working list of vertexes
     * start points to the first element in the list that our current window covers (inclusive)
     * end points to the end of our window (exclusive)
     */
    std::list<std::pair<float,float> >::iterator DPSplit(std::list<std::pair<float,float> >& fulllist,
							 std::list<std::pair<float,float> >::iterator start,
							 std::list<std::pair<float,float> >::iterator end) const;
    
    /**
     * Split the list, recurse on the two halves, and then join them
     * back together.  If split does not split the list - i.e. all the
     * points between the first and last are close enough to the line
     * then delete all the intervening points
     *
     * fulllist is the list of all vertexes
     * start is an iterator pointing at the start of our window (inclusive)
     * end is an iterator pointing at the end of our window (exlusive)
     */
    void DPRecurse(std::list<std::pair<float,float> >& fulllist,
		   std::list<std::pair<float,float> >::iterator start,
		   std::list<std::pair<float,float> >::iterator end) const;
      

  };
}

#endif//FITQUADTANGLEPOLYGON_GUARD
