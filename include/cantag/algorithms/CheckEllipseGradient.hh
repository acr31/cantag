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

#ifndef CHECK_ELLIPSE_GRADIENT_GUARD
#define CHECK_ELLIPSE_GRADIENT_GUARD


#include <cantag/Config.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/Function.hh>
#include <cantag/Aggregation.hh>
#include <cantag/EllipseRestrictions.hh>

namespace Cantag {

  template<class AggregationFunction = AggregateMean<float> >
  class CANTAG_EXPORT CheckEllipseGradient : public Function<TL1(ContourEntity),TL1(ShapeEntity<Ellipse>) > {
  public:
      typedef AggregationFunction Aggregator;
  private:
    const EllipseRestrictions& m_restrict;
  public:
    CheckEllipseGradient(const EllipseRestrictions& restrict) : m_restrict(restrict) {};
    bool operator()(const ContourEntity& contour_entity, ShapeEntity<Ellipse>& ellipse_entity) const;
    static float eval(const Ellipse& e, float x, float y);
  };

    template<class AggregationFunction>
    float CheckEllipseGradient<AggregationFunction>::eval(const Ellipse& e, float x, float y) {
	float dist = abs(e.GetA()*x*x+
			 e.GetB()*x*y+
			 e.GetC()*y*y+
			 e.GetD()*x+
			 e.GetE()*y+
			 e.GetF());
	
	float dx = (e.GetA()*2*x+
		    e.GetB()*y+
		    e.GetD());
	float dy = (e.GetB()*x+
		    e.GetC()*2*y+
		    e.GetE());
	
	float norm = sqrt(dx*dx + dy*dy);
	
	if (norm != 0.f) {
	    dist /= norm;
	    return dist;
	}
	else {
	    return(0.f);
	}	
    }
    

  /**
   * calculate the algebraic distance inversely weighted by the gradient
   */
  template<class AggregationFunction>
  bool CheckEllipseGradient<AggregationFunction>::operator()(const ContourEntity& c, ShapeEntity<Ellipse>& e_ent) const {
    if (!e_ent.IsValid()) return false;
    const Ellipse& e = *(e_ent.GetShape());
    AggregationFunction f;

    for (std::vector<float>::const_iterator i = c.GetPoints().begin();
	 i != c.GetPoints().end();
	 ++i) {  
      float x = *i;
      ++i;
      float y = *i;
      f(eval(e,x,y));
    }
    return f() < m_restrict.GetMaxFitError();
  }
}



#endif//CHECK_ELLIPSE_GRADIENT_GUARD
