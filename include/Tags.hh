/**
 * $Header$
 *
 * Definitions for various tag types
 */ 

#ifndef TAGS_GUARD
#define TAGS_GUARD

#include <RingTag.hh>
#include <TripOriginalCoder.hh>
//#include <GF4Coder.hh>
#include <ParityCoder.hh>
#include <SCCCoder.hh>
#include <SymbolChunkCoder.hh>

/**
 * Something very like an original TRIP tag.  17 sectors and 2 rings
 * using the TripOriginal coding scheme with 2 sectors of checksum.
 */
class TripTag : public RingTag<2,17>, private virtual TripOriginalCoder<2,17,2> {
public:
  TripTag();
};

#endif//TAGS_GUARD
