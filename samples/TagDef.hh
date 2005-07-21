/** 
 $Header$
 */

#ifndef TAGDEF_GUARD
#define TAGDEF_GUARD

#include <Total.hh>

struct TestTag : public Total::TagCircle<2,17>,Total::TripOriginalCoder<34,2,2> {
  TestTag() : Total::TagCircle<2,17>(0.8,1.0,0.2,0.6) {}
};

typedef TestTag TagType;

#endif//TAGDEF_GUARD
