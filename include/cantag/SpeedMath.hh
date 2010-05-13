/*
  Copyright (C) 2004,2005 Alastair R. Beresford and Andrew C. Rice

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

  Email: arb33@cam.ac.uk
*/

#ifndef SPEEDMATH_GUARD
#define SPEEDMATH_GUARD

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <limits>
#include <iostream>
#include <queue>
#include <vector>

#include <cantag/Config.hh>

#define FLT_INFINITY std::numeric_limits<float>::infinity()
#define DBL_INFINITY std::numeric_limits<double>::infinity()

#ifndef FLT_EPSILON
#define FLT_EPSILON std::numeric_limits<float>::epsilon()
#endif

#ifndef DBL_EPSILON
#define DBL_EPSILON std::numeric_limits<double>::epsilon()
#endif

#ifndef INT_MAX
#define INT_MAX std::numeric_limits<int>::max()
#endif

#ifndef INT_MIN
#define INT_MIN std::numeric_limits<int>::min()
#endif

#ifndef FLT_MAX
#define FLT_MAX std::numeric_limits<float>::max()
#endif

#ifndef FLT_MIN
#define FLT_MIN stdly::numeric_limits<float>::min()
#endif

#define FLT_PI 3.14159265358979323846F
#define DBL_PI 3.14159265358979323846

namespace Cantag {

  template<unsigned int N> inline int t_fact() {
    return N*t_fact<N-1>();
  }

  template<> inline int t_fact<1>() {
    return 1;
  }

  //static instantiation of a n term taylor series expansion approximation for x
  template<unsigned int N, unsigned int COUNT, typename T>
  struct taylor {
    static T sin(const T x, const T accum) {
      return ((COUNT%2)?-1:1)*accum*x/(t_fact<2*COUNT+1>()) + taylor<N-1,COUNT+1,T>::sin(x,x*x*accum);;
    }
  };

  //base case
  template<unsigned int COUNT, typename T>
  struct taylor<1,COUNT,T> {
    static T sin(const T x, const T accum) {
      return ((COUNT%2)?-1:1)*accum*x/(t_fact<2*COUNT+1>());
    }
  };

  //cannot instantiate with more than 8 terms since denominator will overflow
  //factorial(17) > unsigned int on 32bit architectures
  //this should catch someone incorrectly using this at compile time
  template<unsigned int COUNT, typename T>
  struct taylor<9,COUNT,T> {
    static T sin(const T x, const T accum) {
      char error = "taylor:fact(17) > sizeof(unsigned int); do not use with N>8";
      throw error;
      return error;
    }
  };

  /*
#define DSIN(N,x) taylor<N,0,double>::sin(x,1)
#define FSIN(N,x) taylor<N,0,float>::sin(x,1)

#define DCOS(N,x) taylor<N,0,double>::sin(x-DBL_PI/2.0,1)
#define FCOS(N,x) taylor<N,0,float>::sin(x-FLT_PI/2.f,1)
  */

  static inline double DCOS(const int accuracy, const double x) {
    (void) accuracy;
    return cos(x);
  }

  static inline double DSIN(const int accuracy, const double x) {
    (void) accuracy;
    return sin(x);
  }

  static inline float FCOS(const int accuracy, const float x) {
    (void) accuracy;
    return cos(x);
  }

  static inline float FSIN(const int accuracy, const float x) {
    (void) accuracy;
    return sin(x);
  }
  /*
  //temporary test values: use to make sure that values are within range
  static inline double DSIN(const int accuracy, const double x) { 
    assert(x<=M_PI);
    assert(x>=-M_PI);
    return taylor<8,0,double>::sin(x,1);
  }

  static inline double DCOS(const int accuracy, const double x) {
    assert(x<=M_PI);
    assert(x>=-M_PI);
    double y=x+M_PI/2;
    if (y > M_PI) {
      y = 2*M_PI - y;
    }
    return taylor<8,0,double>::sin(y,1);
  }
  */
  template <int N> struct IsPowerOfTwo { static const bool check = (N%2 == 0) ? IsPowerOfTwo<(N>>1)>::check : false; };
  template <> struct IsPowerOfTwo<2> { static const bool check = true; };
  template <> struct IsPowerOfTwo<0> { static const bool check = false; };

  template<int N, int VAL=2> struct IsPrime {
    static const bool check = (N%VAL == 0) ? false : IsPrime<N,VAL+1>::check;
  };

  template<int N> struct IsPrime<N,N> {
    static const bool check = true;
  };

  static inline int Round(double d)  {
    /*
#if BYTE_ORDER == LITTLE_ENDIAN 
    double t =(d+6755399441055744.0); 
    return *(int*)&t;
#else
    */
    return (int)floor(d+0.5);
    //#endif
  }

  class CANTAG_EXPORT Minima {
  private:
    float m_minimum;
  public:
    Minima() : m_minimum(FLT_MAX) {}
    void UpdateMinima(float newval) { if (newval < m_minimum) m_minimum = newval; }
    float GetMinima() const { return m_minimum; }
  };

  class CANTAG_EXPORT MinimaQueue {
  public:
    typedef std::priority_queue<float,std::vector<float>, std::greater<float> > Queue;
  private:
    Queue m_minima;
  public:
    MinimaQueue() : m_minima() {}
    void UpdateMinima(float newval) { m_minima.push(newval); }
    float GetMinima() const { return m_minima.top(); }
    Queue& GetQueue() { return m_minima; }
  };

  class CANTAG_EXPORT Maxima {
  private:
    float m_maximum;
  public:
    Maxima() : m_maximum(-FLT_INFINITY) {}
    void UpdateMaxima(float newval) { if (newval > m_maximum) m_maximum = newval; }
    float GetMaxima() const { return m_maximum; }
  };

  template<class C> inline C sqrt(C val) { }
  template<> inline float sqrt<float>(float val) { return ::sqrtf(val); }
  template<> inline double sqrt<double>(double val) { return ::sqrt(val); }

  template<class C> inline C abs(C val) {   }
  template<> inline float abs<float>(float val) { return ::fabsf(val); }
  template<> inline double abs<double>(double val) { return ::fabs(val); }
  template<> inline int abs<int>(int val) { return ::abs(val); }

  template<class C> inline C sin(C val) {   }
  template<> inline float sin<float>(float val) { return ::sinf(val); }
  template<> inline double sin<double>(double val) { return ::sin(val); }

  template<class C> inline C cos(C val) {   }
  template<> inline float cos<float>(float val) { return ::cosf(val); }
  template<> inline double cos<double>(double val) { return ::cos(val); }
  
  template<class C> inline C atan(C val) {  }
  template<> inline float atan<float>(float val) { return ::atanf(val); }
  template<> inline double atan<double>(double val) { return ::atan(val); }

  template<class C> inline C atan2(C val, C val2) {   }
  template<> inline float atan2<float>(float val, float val2) { return ::atan2f(val,val2); }
  template<> inline double atan2<double>(double val, double val2) { return ::atan2(val,val2); }

  template<class C> inline C acos(C val) {  }
  template<> inline float acos<float>(float val) { return ::acosf(val); }
  template<> inline double acos<double>(double val) { return ::acos(val); }

  template<class C> inline C tan(C val) {  }
  template<> inline float tan<float>(float val) { return ::tanf(val); }
  template<> inline double tan<double>(double val) { return ::tan(val); }
  
  /**
   * Taken from Boost (http://www.boost.org)
   *
   *  Boost interval/checking.hpp template implementation file
   *
   * Copyright 2002 Herv351 Br366nnimann, Guillaume Melquiond, Sylvain Pion
   *
   * Distributed under the Boost Software License, Version 1.0.
   * (See accompanying file LICENSE_1_0.txt or
   * copy at http://www.boost.org/LICENSE_1_0.txt)
   */
  template<class C> inline bool is_nan(C val) { 
    return std::numeric_limits<C>::has_quiet_NaN && (val != val);
  }
}

#endif //SPEEDMATH_GUARD
