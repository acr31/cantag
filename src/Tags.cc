/**
 * $Header$
 */

#include <tripover/Tags.hh>

#if defined(HAVE_GMPXX_H) && defined(HAVELIB_GMP) && defined(HAVELIB_GMPXX)
TripTag::TripTag() : RingTag<2,17,Ellipse>(0.2,0.4,0.6,1.0), TripOriginalCoder<34,2,2>() {}

OuterRing::OuterRing() : RingTag<2,17,SimpleEllipse>(0.8,1.0,0.2,0.6), TripOriginalCoder<34,2,2>() {}

SplitTarget::SplitTarget() : RingTag<2,17,Ellipse>(0.2,1.0,0.4,0.8), TripOriginalCoder<34,2,2>() {}
#endif
