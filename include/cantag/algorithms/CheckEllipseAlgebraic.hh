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

#ifndef CHECK_ELLIPSE_ALGEBRAIC_GUARD
#define CHECK_ELLIPSE_ALGEBRAIC_GUARD


#include <cantag/Config.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/Function.hh>
#include <cantag/Aggregation.hh>
#include <cantag/EllipseRestrictions.hh>

namespace Cantag {

  template<class AggregationFunction = AggregateMean<float> >
  class CANTAG_EXPORT CheckEllipseAlgebraic : public Function<TL1(ContourEntity),TL1(ShapeEntity<Ellipse>) > {
  private:
    const EllipseRestrictions& m_restrict;
  public:
    CheckEllipseAlgebraic(const EllipseRestrictions& restrict) : m_restrict(restrict) {};
    bool operator()(const ContourEntity& contour_entity, ShapeEntity<Ellipse>& ellipse_entity) const;
  };

  template<class AggregationFunction>
  bool CheckEllipseAlgebraic<AggregationFunction>::operator()(const ContourEntity& c, ShapeEntity<Ellipse>& e_ent) const {
    // calculate the algebraic distance
    if (!e_ent.IsValid()) return false;

    AggregationFunction f;

    const Ellipse& e = *(e_ent.GetShape());

    for (std::vector<float>::const_iterator i = c.GetPoints().begin();
	 i != c.GetPoints().end();
	 ++i) {
      float x = *i;
      ++i;
      float y = *i;
      float dist = abs(e.GetA()*x*x+
		       e.GetB()*x*y+
		       e.GetC()*y*y+
		       e.GetD()*x+
		       e.GetE()*y+
		       e.GetF());
      f(dist);
    }
    return f() < m_restrict.GetMaxFitError();
  }
}



#endif//CHECK_ELLIPSE_ALGEBRAIC_GUARD
