/**
 * $Header$
 */

#include <Tags.hh>

TripTag::TripTag() : RingTag<2,17>(0.2,0.4,0.6,1.0), TripOriginalCoder<34,2,2>() {}

OuterRing::OuterRing() : RingTag<2,17>(0.8,1.0,0.2,0.6), TripOriginalCoder<34,2,2>() {}

MatrixSquare::MatrixSquare() : MatrixTag<6>(), TripOriginalCoder<36,3,2>() {};

SplitTarget::SplitTarget() : RingTag<2,17>(0.2,1.0,0.4,0.8), TripOriginalCoder<34,2,2>() {}