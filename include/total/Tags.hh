/**
 * $Header$
 *
 * Definitions for various tag types
 */ 

#ifndef TAGS_GUARD
#define TAGS_GUARD

#include <total/Config.hh>
#include <total/RingTag.hh>
#include <total/MatrixTag.hh>
#include <total/TripOriginalCoder.hh>
#include <total/ParityCoder.hh>
#include <total/SCCCoder.hh>
#include <total/SymbolChunkCoder.hh>
#include <total/EllipseTransform.hh>
#include <total/Ellipse.hh>

namespace Total {

#if defined(HAVE_GMPXX_H) && defined(HAVELIB_GMP) && defined(HAVELIB_GMPXX)
  /**
   * Something very like an original TRIP tag.  17 sectors and 2 rings
   * using the TripOriginal coding scheme with 2 sectors of checksum.
   */
  class TripTag : public RingTag<2,17,Ellipse>, protected virtual TripOriginalCoder<34,2,2>, private virtual FullEllipseTransform {
  public:
    TripTag() : RingTag<2,17,Ellipse>(0.2,0.4,0.6,1.0) {}
  };

  /**
   * Another tag using the same coding structure as a TRIP tag but with
   * the data sectors inside the target
   */
  class OuterRing : public RingTag<2,17,Ellipse>, protected virtual TripOriginalCoder<34,2,2>, private virtual FullEllipseTransform {
  public:
    OuterRing() : RingTag<2,17,Ellipse>(0.8,1.0,0.2,0.6)  {}
  };

  /**
   * A tag much like Rekimoto's Matrix Tag but using a Trip coding scheme
   */
  class MatrixSquare : public MatrixTag<6,RegressConvexHullQuadTangle>,  protected virtual TripOriginalCoder<36,3,2>, private virtual ProjectiveQuadTangleTransform {};

  /**
   * Another tag that uses the same coding structure as TRIP but with
   * the data sectors written on top of the bullseye ring
   */
  class SplitTarget : public RingTag<2,17,Ellipse>, protected virtual TripOriginalCoder<34,2,2>, private virtual FullEllipseTransform {
  public:
    SplitTarget() : RingTag<2,17,Ellipse>(0.2,1.0,0.4,0.8) {}
  };
#endif

  class SimpleSquare : public MatrixTag<10,CornerQuadTangle>, private virtual SymbolChunkCoder<100,25>, private virtual ProjectiveQuadTangleTransform {};
  class SimpleCircle : public RingTag<4,25,Ellipse>, private virtual SymbolChunkCoder<100,4>, private virtual FullEllipseTransform {
  public:
    SimpleCircle() : RingTag<4,25,Ellipse>(0.2,0.4,0.6,1.0) {};
  };

}

#endif//TAGS_GUARD
