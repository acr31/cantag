/**
 * $Header$
 *
 * A convenience header file for including all the other total headers
 */

#ifndef TOTAL_GUARD
#define TOTAL_GUARD

#include <total/Config.hh>

///////////////////////
// entities
#include <total/entities/Entity.hh>
#include <total/EntityTree.hh>

#include <total/entities/ContourEntity.hh>
#include <total/entities/ConvexHullEntity.hh>

///////////////////////
// composed entities
#if defined(HAVECXX_COVARIANT_RETURNS)
#include <total/ComposeEntity.hh>
#include <total/ComposeEntityTree.hh>
#endif

#include <total/Apply.hh>
#include <total/Bind.hh>

///////////////////////
// fundamental tag types
#include <total/TagCircle.hh>
#include <total/TagSquare.hh>

///////////////////////
// algorithms
#include <total/algorithms/FitEllipseLS.hh>
#include <total/algorithms/FitEllipseSimple.hh>
#include <total/algorithms/FitQuadTangleCorner.hh>
#include <total/algorithms/FitQuadTangleConvexHull.hh>
#include <total/algorithms/FitQuadTanglePolygon.hh>
#include <total/algorithms/DistortionCorrection.hh>
#include <total/algorithms/TransformEllipseLinear.hh>
#include <total/algorithms/TransformEllipseFull.hh>
#include <total/algorithms/TransformQuadTangleProjective.hh>
#include <total/algorithms/TransformQuadTangleReduced.hh>
#if defined(HAVE_GSL_MULTIMIN_H) and defined(HAVELIB_GSLCBLAS) and defined(HAVELIB_GSL)
#include <total/algorithms/TransformQuadTangleSpaceSearch.hh>
#endif
#include <total/algorithms/SampleTagCircle.hh>
#include <total/algorithms/SampleTagSquare.hh>
#include <total/algorithms/ContourFollowerTree.hh>
#include <total/algorithms/Decode.hh>
#include <total/algorithms/ThresholdAdaptive.hh>
#include <total/algorithms/ThresholdGlobal.hh>
#include <total/algorithms/ConvexHull.hh>

///////////////////////
// coders
#include <total/coders/RawCoder.hh>
#include <total/coders/SCCCoder.hh>
#include <total/coders/ParityCoder.hh>
#include <total/coders/SCCCoder.hh>
#include <total/coders/SymbolChunkCoder.hh>
#if defined(HAVE_GMPXX_H) && defined(HAVELIB_GMP) && defined(HAVELIB_GMPXX)
# include <total/coders/TripOriginalCoder.hh>
#endif

//////////////////////
// image sources
#include <total/FileImageSource.hh>
#ifdef HAVE_LINUX_VIDEODEV_H
# include <total/V4LImageSource.hh>
#endif
#if defined(HAVE_GL_GL_H) && defined(HAVE_GL_GLU_H) && defined(HAVE_GL_OSMESA_H) && defined(HAVELIB_GL) && defined(HAVELIB_OSMESA) && defined(HAVELIB_GLU) && false
# include <total/GLImageSource.hh>
#endif
#if defined(HAVELIB_DC1394) && defined(HAVE_DC1394_CONTROL_H)
#include <total/IEEE1394ImageSource.hh>
#endif

//////////////////////
// output mechanisms
// #include <total/TextOutputMechanism.hh>
// #include <total/ImageOutputMechanism.hh>
#if defined(HAVE_X11_XLIB_H) && defined(HAVE_X11_XUTIL_H) && defined(HAVE_X11_EXTENSIONS_XSHM_H)
# include <total/XOutputMechanism.hh>
//# include <total/XOutputStagesMechanism.hh>
#endif

#if defined(HAVE_GL_GL_H) && defined(HAVELIB_GL) && defined(HAVELIB_GLU) && defined(HAVELIB_GLUT)
//# include <total/GLOutputMechanism.hh>
#endif



#endif//TOTAL_GUARD
