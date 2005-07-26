/** 
 $Header$
 */

#ifndef TAGDEF_GUARD
#define TAGDEF_GUARD

#include <Cantag.hh>

struct TestTag : public Cantag::TagCircle<2,17>,Cantag::TripOriginalCoder<34,2,2> {
  TestTag() : Cantag::TagCircle<2,17>(0.8,1.0,0.2,0.6) {}
};

typedef TestTag TagType;

#endif//TAGDEF_GUARD
