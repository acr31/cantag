/**
 * $Header$
 *
 * Definitions for various tag types
 */ 

#ifndef TAGS_GUARD
#define TAGS_GUARD

#include <tripover/Config.hh>
#include <tripover/RingTag.hh>
#include <tripover/MatrixTag.hh>
#include <tripover/TripOriginalCoder.hh>
//#include <tripover/GF4Coder.hh>
#include <tripover/ParityCoder.hh>
#include <tripover/SCCCoder.hh>
#include <tripover/SymbolChunkCoder.hh>
#include <tripover/EllipseTransform.hh>
#include <tripover/Ellipse.hh>

/**
 * Something very like an original TRIP tag.  17 sectors and 2 rings
 * using the TripOriginal coding scheme with 2 sectors of checksum.
 */
#if defined(HAVE_GMPXX_H) && defined(HAVELIB_GMP) && defined(HAVELIB_GMPXX)
class TripTag : public RingTag<2,17,Ellipse>, private virtual TripOriginalCoder<34,2,2>, private virtual FullEllipseTransform {
public:
  TripTag();
};

/**
 * Another tag using the same coding structure as a TRIP tag but with
 * the data sectors inside the target
 */
class OuterRing : public RingTag<2,17,SimpleEllipse>, private virtual TripOriginalCoder<34,2,2>, private virtual FullEllipseTransform {
public:
  OuterRing();
};

/**
 * A tag much like Rekimoto's Matrix Tag but using a Trip coding scheme
 */
class MatrixSquare : public MatrixTag<6>,  private virtual TripOriginalCoder<36,3,2>, private virtual ProjectiveQuadTangleTransform {};

/**
 * Another tag that uses the same coding structure as TRIP but with
 * the data sectors written on top of the bullseye ring
 */
class SplitTarget : public RingTag<2,17,Ellipse>, private virtual TripOriginalCoder<34,2,2>, private virtual FullEllipseTransform {
public:
  SplitTarget();
};
#endif
#endif//TAGS_GUARD
