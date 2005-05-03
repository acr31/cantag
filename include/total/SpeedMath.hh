#ifndef SPEEDMATH_GUARD
#define SPEEDMATH_GUARD

#include <cmath>
#include <cassert>

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
static double DSIN(const int accuracy, const double x) { 
  assert(x<=M_PI);
  assert(x>=-M_PI);
  return taylor<8,0,double>::sin(x,1);
}

static double DCOS(const int accuracy, const double x) {
  assert(x<=M_PI);
  assert(x>=-M_PI);
  double y=x+M_PI/2;
  if (y > M_PI) {
    y = 2*M_PI - y;
  }
  return taylor<8,0,double>::sin(y,1);
}

#endif //SPEEDMATH_GUARD
