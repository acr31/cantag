/**
 * $Header$
 *
 * Definitions for various tag types
 */ 

#ifndef TAGS_GUARD
#define TAGS_GUARD

#include <tripover/RingTag.hh>
#include <tripover/MatrixTag.hh>
#include <tripover/TripOriginalCoder.hh>
//#include <tripover/GF4Coder.hh>
#include <tripover/ParityCoder.hh>
#include <tripover/SCCCoder.hh>
#include <tripover/SymbolChunkCoder.hh>

/**
 * Something very like an original TRIP tag.  17 sectors and 2 rings
 * using the TripOriginal coding scheme with 2 sectors of checksum.
 */
#if defined(HAVE_GMPXX_H) && defined(HAVELIB_GMP) && defined(HAVELIB_GMPXX)
class TripTag : public RingTag<2,17>, public virtual TripOriginalCoder<34,2,2> {
public:
  TripTag();
};

/**
 * Another tag using the same coding structure as a TRIP tag but with
 * the data sectors inside the target
 */
class OuterRing : public RingTag<2,17>, public virtual TripOriginalCoder<34,2,2> {
public:
  OuterRing();
};

/**
 * A tag much like Rekimoto's Matrix Tag but using a Trip coding scheme
 */
class MatrixSquare : public MatrixTag<6>,  private virtual TripOriginalCoder<36,3,2> {
public:
  MatrixSquare();
};


/**
 * Another tag that uses the same coding structure as TRIP but with
 * the data sectors written on top of the bullseye ring
 */
class SplitTarget : public RingTag<2,17>, public virtual TripOriginalCoder<34,2,2> {
public:
  SplitTarget();
};
#endif
#endif//TAGS_GUARD
