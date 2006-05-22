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

#ifndef CHECK_ELLIPSE_STRICKER_GUARD
#define CHECK_ELLIPSE_STRICKER_GUARD


#include <cantag/Config.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/Function.hh>
#include <cantag/Aggregation.hh>
#include <cantag/EllipseRestrictions.hh>

namespace Cantag {

  template<class AggregationFunction = AggregateMean<float> >
  class CANTAG_EXPORT CheckEllipseStricker : public Function<TL1(ContourEntity),TL1(ShapeEntity<Ellipse>) > {
  public:
      typedef AggregationFunction Aggregator;
  private:
    const EllipseRestrictions& m_restrict;
  public:
    CheckEllipseStricker(const EllipseRestrictions& restrict) : m_restrict(restrict) {};
    bool operator()(const ContourEntity& contour_entity, ShapeEntity<Ellipse>& ellipse_entity) const;
    static float eval(const Ellipse& e, float x, float y);
  };

    template<class AggregationFunction>
    float CheckEllipseStricker<AggregationFunction>::eval(const Ellipse& e, float x, float y) {
	float a = e.GetWidth();
	float b = e.GetHeight();
	float x0 = e.GetX0();
	float y0 = e.GetY0();
	float theta = e.GetAngle();
	
	// we need a^2-b^2 to be >0 so check that
	if (a < b) {
	    float swap = a;
	    a=b;
	    b=swap;
	    
	    theta=FLT_PI/2.f-theta;
	}
	
	float c = sqrt(a*a-b*b);
	
	float f1x = x0 + c*cos(theta);
	float f1y = y0 + c*sin(theta);
	
	float f2x = x0 - c*cos(theta);
	float f2y = x0 - c*sin(theta);
	
	float modf1_f2 = sqrt( (f1x-f2x)*(f1x-f2x) + (f1y-f2y)*(f1y-f2y) );
	
	float temp = sqrt( (x-f1x)*(x-f1x) + (y-f1y)*(y-f1y) );
	float temp2 = sqrt( (x-f2x)*(x-f2x) + (y-f2y)*(y-f2y) );
	
	float aest = 0.5f * ( temp + temp2 );
	float best = sqrt(aest*aest - a*a + b*b);
	
	float dest = 0.5f *(aest - a + best - b);
	
	float ctilde = sqrt( (a+dest)*(a+dest) - (b+dest)*(b+dest) );
	
	float f1xtilde = (f1x + f2x)/2+ctilde*(f1x-f2x)/modf1_f2;
	float f1ytilde = (f1y + f2y)/2+ctilde*(f1y-f2y)/modf1_f2;
	
	float f2xtilde = (f1x + f2x)/2-ctilde*(f1x-f2x)/modf1_f2;
	float f2ytilde = (f1y + f2y)/2-ctilde*(f1y-f2y)/modf1_f2;
	
	float temp4 = sqrt( (x-f1xtilde)*(x-f1xtilde) + (y-f1ytilde)*(y-f1ytilde) );
	float temp5 = sqrt( (x-f2xtilde)*(x-f2xtilde) + (y-f2ytilde)*(y-f2ytilde) );
	float atilde = 0.5f * ( temp4 + temp5 );
	
	float dist = abs(atilde - a);	

	if (is_nan(dist)) return 0;

	return dist;	
    }

  template<class AggregationFunction>
  bool CheckEllipseStricker<AggregationFunction>::operator()(const ContourEntity& c_ent, ShapeEntity<Ellipse>& e_ent) const {
    // calculate the stricker distance
    if (!e_ent.IsValid()) return false;

    AggregationFunction f;

    const Ellipse& e = *(e_ent.GetShape());


    for (std::vector<float>::const_iterator i = c_ent.GetPoints().begin();
	 i != c_ent.GetPoints().end();
	 ++i) {
      float x = *i;
      ++i;
      float y = *i;

      f(eval(e,x,y));
    }
    return f() < m_restrict.GetMaxFitError();
  }
}



#endif//CHECK_ELLIPSE_STRICKER_GUARD
