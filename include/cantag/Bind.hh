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

#ifndef BIND_GUARD
#define BIND_GUARD

#include <cantag/Function.hh>

namespace Cantag {
  template<class S, class T>
  class Bound {
  };

  template<class Algorithm, class ST1, class ReturnType>
  class Bound<Algorithm, Function1<ST1,ReturnType> > : 
    public Function0<ReturnType> {
  private:
    const Algorithm& m_algorithm;
    const ST1& m_source;
  public:
    
    Bound(const Algorithm& a, const ST1& source) : m_algorithm(a), m_source(source) {}
    bool operator()(ReturnType& dest) const {
      return m_algorithm(m_source,dest);
    }    
  };

  template<class Algorithm, class ST1, class ST2, class ReturnType>
  class Bound<Algorithm, Function2<ST1, ST2, ReturnType> > : 
    public Function1<ST2, ReturnType> {
  private:
    const Algorithm& m_algorithm;
    const ST1& m_source;
  public:
    Bound(const Algorithm& a, const ST1& source) : m_algorithm(a),m_source(source) {}
    bool operator()(const ST2& source2, ReturnType& dest) const {
      return m_algorithm(m_source,source2,dest);
    }
  };

  template<class Algorithm, class ST1, class ST2, class ST3, class ReturnType>
  class Bound<Algorithm, Function3<ST1, ST2, ST3, ReturnType> > : public Function2<ST2, ST3, ReturnType> {
  private:
    const Algorithm& m_algorithm;
    const ST1& m_source;
  public:
    Bound(const Algorithm& a, const ST1& source) : m_algorithm(a),m_source(source) {}
    bool operator()(const ST2& source2, const ST3& source3,ReturnType& dest) const {
      return m_algorithm(m_source,source2, source3,dest);
    }
  };
  
  template<class Algorithm> Bound<Algorithm, typename Algorithm::FunctionType> Bind(const Algorithm& a, const typename Algorithm::SourceType1& source) {
    return Bound<Algorithm,typename Algorithm::FunctionType>(a,source);
  }
}

#endif//BIND_GUARD
