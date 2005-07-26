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
