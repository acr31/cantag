/*
  Copyright (C) 2004,2005 Alastair R. Beresford

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
#include <cassert>
#include <limits>
#include <sys/param.h>

#define FLT_INFINITY std::numeric_limits<float>::infinity()
#define DBL_INFINITY std::numeric_limits<double>::infinity()

#ifndef INT_MAX
#define INT_MAX std::numeric_limits<int>::max()
#endif

#ifndef INT_MIN
#define INT_MIN std::numeric_limits<int>::min()
#endif

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

  //#define DSIN(N,x) taylor<N,0,double>::sin(x,1)
  //#define FSIN(N,x) taylor<N,0,float>::sin(x,1)

  //#define DCOS(N,x) taylor<N,0,double>::sin(x-M_PI/2,1)
  //#define FCOS(N,x) taylor<N,0,float>::sin(x-M_PI/2,1)

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
    return (int)round(d);
    //#endif
  }

  class Minima {
  private:
    float m_minimum;
  public:
    Minima() : m_minimum(FLT_INFINITY) {}
    void UpdateMinima(float newval) { if (newval < m_minimum) m_minimum = newval; }
    float GetMinima() const { return m_minimum; }
  };

  class Maxima {
  private:
    float m_maximum;
  public:
    Maxima() : m_maximum(-FLT_INFINITY) {}
    void UpdateMaxima(float newval) { if (newval > m_maximum) m_maximum = newval; }
    float GetMaxima() const { return m_maximum; }
  };


}

#endif //SPEEDMATH_GUARD
