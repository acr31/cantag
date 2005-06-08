/**
 * $Header$
 */

#ifndef BIND_GUARD
#define BIND_GUARD

#include <total/Function.hh>

namespace Total {
  template<class S, class T>
  class Bound {
  };

  template<class Algorithm, class SourceType1, class ReturnType>
  class Bound<Algorithm, Function1<SourceType1,ReturnType> > : 
    public Function0<ReturnType> {
  private:
    const Algorithm& m_algorithm;
    const SourceType1& m_source;
  public:
    
    Bound(const Algorithm& a, const SourceType1& source) : m_algorithm(a), m_source(source) {}
    bool operator()(ReturnType& dest) const {
      return m_algorithm(m_source,dest);
    }    
  };

  template<class Algorithm, class SourceType1, class SourceType2, class ReturnType>
  class Bound<Algorithm, Function2<SourceType1, SourceType2, ReturnType> > : 
    public Function1<SourceType2, ReturnType> {
  private:
    const Algorithm& m_algorithm;
    const SourceType1& m_source;
  public:
    Bound(const Algorithm& a, const SourceType1& source) : m_algorithm(a),m_source(source) {}
    bool operator()(const SourceType2& source2, ReturnType& dest) const {
      return m_algorithm(m_source,source2,dest);
    }
  };

  template<class Algorithm, class SourceType1, class SourceType2, class SourceType3, class ReturnType>
  class Bound<Algorithm, Function3<SourceType1, SourceType2, SourceType3, ReturnType> > : public Function2<SourceType2, SourceType3, ReturnType> {
  private:
    const Algorithm& m_algorithm;
    const SourceType1& m_source;
  public:
    Bound(const Algorithm& a, const SourceType1& source) : m_algorithm(a),m_source(source) {}
    bool operator()(const SourceType2& source2, const SourceType3& source3,ReturnType& dest) const {
      return m_algorithm(m_source,source2, source3,dest);
    }
  };
  
  template<class Algorithm> Bound<Algorithm, typename Algorithm::FunctionType> Bind(const Algorithm& a, const typename Algorithm::SourceType1& source) {
    return Bound<Algorithm,typename Algorithm::FunctionType>(a,source);
  }
}

#endif//BIND_GUARD
