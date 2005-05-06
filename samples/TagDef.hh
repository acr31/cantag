/**
 * $Header$
 */

#ifndef TAG_DEF_GUARD
#define TAG_DEF_GUARD

#include <Total.hh>

namespace Total {

  class CustomCircle : public RingTag<2,17,Ellipse>, protected virtual RawCoder<34,2>, private virtual FullEllipseTransform {
  public:
    CustomCircle() : RingTag<2,17,Ellipse>(0.2,1.0,0.4,0.8) {};
  };
  
  class PSquare : public MatrixTag<5,ConvexHullQuadTangle>, protected virtual  SymbolChunkCoder<24,6>, private virtual ProjectiveQuadTangleTransform {};


};

typedef Total::OuterRing TagType;

#endif//TAG_DEF_GUARD
