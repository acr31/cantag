/**
 * $Header$
 */

#include <tripover/Tags.hh>

#if defined(HAVE_GMPXX_H) && defined(HAVELIB_GMP) && defined(HAVELIB_GMPXX)
TripTag::TripTag() : TripOriginalCoder<34,2,2>(), RingTag<2,17,Ellipse>(0.2,0.4,0.6,1.0)  {}

OuterRing::OuterRing() : TripOriginalCoder<34,2,2>(), RingTag<2,17,SimpleEllipse>(0.8,1.0,0.2,0.6)  {}

SplitTarget::SplitTarget() : TripOriginalCoder<34,2,2>(), RingTag<2,17,Ellipse>(0.2,1.0,0.4,0.8) {}
#endif
