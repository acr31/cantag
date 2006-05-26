/*
  Copyright (C) 2006 Andrew C. Rice

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

#ifndef CHECK_ELLIPSE_SAFAEERAD2_GUARD
#define CHECK_ELLIPSE_SAFAEERAD2_GUARD


#include <cantag/Config.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/Function.hh>
#include <cantag/Aggregation.hh>
#include <cantag/EllipseRestrictions.hh>
#include <cantag/SpeedMath.hh>
namespace Cantag {

  template<class AggregationFunction = AggregateMean<float> >
  class CANTAG_EXPORT CheckEllipseSafaeeRad2 : public Function<TL1(ContourEntity),TL1(ShapeEntity<Ellipse>) > {
  public:
      typedef AggregationFunction Aggregator;
  private:
    const EllipseRestrictions& m_restrict;
  public:
    CheckEllipseSafaeeRad2(const EllipseRestrictions& restrict) : m_restrict(restrict) {};
    bool operator()(const ContourEntity& contour_entity, ShapeEntity<Ellipse>& ellipse_entity) const;
    static float eval(const Ellipse& e, float x, float y);
  };

    template<class AggregationFunction>
    float CheckEllipseSafaeeRad2<AggregationFunction>::eval(const Ellipse& e, float xi, float yi) {
      float a = e.GetWidth();
      float b = e.GetHeight();
      float theta  = e.GetAngle();
      float x0 = e.GetX0();
      float y0 = e.GetY0();
      float k = (y0-yi)/(x0-xi);

      float sint = sin(theta);
      float cost = cos(theta);


      float rtsub1 = -sint+k*cost;
      float rtsub2 = cost+k*sint;
      float rt = sqrt(a*a*rtsub1*rtsub1 + b*b*rtsub2*rtsub2);

      // compute ix
      float ix;
      if (xi > x0) {
	ix = x0 + a*b/rt;
      }
      else {
	ix = x0 - a*b/rt;
      }

      // compute iy
      float iy;
      if ((yi >= y0 && k > 0) ||
	  (yi < y0 && k < 0)) {
	iy = y0 + a*b*k/rt;
      }
      else {
	iy = y0 - a*b*k/rt;
      }

      float m = sqrt((x0-ix)*(x0-ix)+(y0-iy)*(y0-iy));
      float q = (e.GetA()*xi*xi+
		 e.GetB()*xi*yi+
		 e.GetC()*yi*yi+
		 e.GetD()*xi+
		 e.GetE()*yi+
		 e.GetF());

      float dist = abs(m*q);
      return dist;
    }

  /**
   * draw a ray between each point and the centre of the ellipse C //
   * intersecting the ellipse at Ij.  The lengths of the bisected //
   * portions of the ray mj and nj are determined
   */
  template<class AggregationFunction>
  bool CheckEllipseSafaeeRad2<AggregationFunction>::operator()(const ContourEntity& c, ShapeEntity<Ellipse>& e_ent) const {

    if (!e_ent.IsValid()) return false;

    const Ellipse& e = *(e_ent.GetShape());
    AggregationFunction f;
    
    for (std::vector<float>::const_iterator i = c.GetPoints().begin();
	 i != c.GetPoints().end();
	 ) {  
      float xi = *i;
      ++i;
      float yi = *i;
      ++i;

      f(eval(e,xi,yi));
    }
    float fit = f();
    e_ent.GetShape()->SetFitError(fit);
    return fit < m_restrict.GetMaxFitError();
  }
}



#endif//CHECK_ELLIPSE_SAFAEERAD2_GUARD
