/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 *
 * A convenience header file for including all the other cantag headers
 */

#ifndef CANTAG_GUARD
#define CANTAG_GUARD

#include <cantag/Config.hh>

///////////////////////
// entities
#include <cantag/entities/Entity.hh>
#include <cantag/EntityTree.hh>

#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ConvexHullEntity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/DecodeEntity.hh>

///////////////////////
// composed entities
#if defined(HAVECXX_COVARIANT_RETURNS)
#include <cantag/ComposeEntity.hh>
#include <cantag/ComposeEntityTree.hh>
#endif

#include <cantag/SpeedMath.hh>
#include <cantag/Aggregation.hh>
#include <cantag/Apply.hh>
#include <cantag/Bind.hh>
#include <cantag/TagDictionary.hh>
#include <cantag/Correspondence.hh>

///////////////////////
// fundamental tag types
#include <cantag/TagCircle.hh>
#include <cantag/TagCircleInner.hh>
#include <cantag/TagCircleOuter.hh>
#include <cantag/TagCircleSplit.hh>
#include <cantag/TagSquare.hh>

///////////////////////
// algorithms
#include <cantag/algorithms/FitEllipseLS.hh>
#include <cantag/algorithms/FitEllipseSimple.hh>
#include <cantag/algorithms/FitQuadTangleCorner.hh>
#include <cantag/algorithms/FitQuadTangleConvexHull.hh>
#include <cantag/algorithms/FitQuadTanglePolygon.hh>
#include <cantag/algorithms/FitQuadTangleRegression.hh>
#include <cantag/algorithms/DistortionCorrectionNone.hh>
#include <cantag/algorithms/DistortionCorrectionSimple.hh>
#include <cantag/algorithms/TransformEllipseLinear.hh>
#include <cantag/algorithms/TransformEllipseFull.hh>
#include <cantag/algorithms/TransformEllipseRotate.hh>
#include <cantag/algorithms/TransformQuadTangleProjective.hh>
#include <cantag/algorithms/TransformQuadTangleReduced.hh>
#ifdef HAVE_GSL_MULTIMIN_H
#ifdef HAVELIB_GSLCBLAS
#ifdef HAVELIB_GSL
# include <cantag/algorithms/TransformQuadTangleSpaceSearch.hh>
# include <cantag/algorithms/TransformQuadTangleCyberCode.hh>
# include <cantag/algorithms/DistortionCorrectionIterative.hh>
#endif
#endif
#endif
#include <cantag/algorithms/TransformRotateToPayload.hh>
#include <cantag/algorithms/SampleTagCircle.hh>
#include <cantag/algorithms/SampleTagSquare.hh>
#include <cantag/algorithms/ContourFollowerClearImageBorder.hh>
#include <cantag/algorithms/ContourFollowerTree.hh>
#include <cantag/algorithms/Encode.hh>
#include <cantag/algorithms/Decode.hh>
#include <cantag/algorithms/ThresholdAdaptive.hh>
#include <cantag/algorithms/ThresholdGlobal.hh>
#include <cantag/algorithms/ConvexHull.hh>
#include <cantag/algorithms/DrawTagCircle.hh>
#include <cantag/algorithms/DrawTagCircleSVG.hh>
#include <cantag/algorithms/DrawTagSquare.hh>
#include <cantag/algorithms/DrawTagSquareSVG.hh>
#include <cantag/algorithms/TransformSelectEllipse.hh>
#include <cantag/algorithms/TransformSelectEllipseErrorOfFit.hh>
#include <cantag/algorithms/RemoveNonConcentricEllipse.hh>
#include <cantag/algorithms/DrawEntity.hh>
#include <cantag/algorithms/PrintEntity.hh>
#include <cantag/algorithms/AccumulateCorrespondences.hh>
#include <cantag/algorithms/AccumulateCornerCorrespondences.hh>
#include <cantag/algorithms/SimulateMinDistance.hh>
#include <cantag/algorithms/SimulateMaxSampleError.hh>
#include <cantag/algorithms/SimulateContour.hh>
#include <cantag/algorithms/SignalStrengthContour.hh>
#include <cantag/algorithms/CheckEllipseAlgebraic.hh>
#include <cantag/algorithms/CheckEllipseGradient.hh>
#include <cantag/algorithms/CheckEllipseNakagawa.hh>
#include <cantag/algorithms/CheckEllipseSafaeeRad.hh>
#include <cantag/algorithms/CheckEllipseSafaeeRad2.hh>
#include <cantag/algorithms/CheckEllipseStricker.hh>


///////////////////////
// coders
#include <cantag/coders/RawCoder.hh>
#include <cantag/coders/SCCCoder.hh>
#include <cantag/coders/ParityCoder.hh>
#include <cantag/coders/SCCCoder.hh>
#include <cantag/coders/SymbolChunkCoder.hh>
#include <cantag/coders/CRCSymbolChunkCoder.hh>
#ifdef HAVE_GMPXX_H
#ifdef HAVELIB_GMP
#ifdef HAVELIB_GMPXX
# include <cantag/coders/TripOriginalCoder.hh>
#endif
#endif
#endif

//////////////////////
// image sources
#include <cantag/FileImageSource.hh>
#ifdef HAVE_LINUX_VIDEODEV_H
# include <cantag/V4LImageSource.hh>
#endif
#ifdef HAVE_GL_GL_H
#ifdef HAVE_GL_GLU_H
#ifdef HAVE_GL_OSMESA_H
#ifdef HAVELIB_GL
#ifdef HAVELIB_OSMESA
#ifdef HAVELIB_GLU
# include <cantag/GLImageSource.hh>
#endif
#endif
#endif
#endif
#endif
#endif
#ifdef HAVELIB_DC1394
#ifdef HAVE_DC1394_CONTROL_H
# include <cantag/IEEE1394ImageSource.hh>
#endif
#endif
#ifdef WIN32
# include <cantag/VFWImageSource.hh>
# include <cantag/DSVLImageSource.hh>
#endif
#ifdef HAVE_UEYE_H
# include <cantag/UEyeImageSource.hh>
#endif
//////////////////////
// output mechanisms
#include <cantag/Keypress.hh>
#ifdef HAVE_X11_XLIB_H
#ifdef HAVE_X11_XUTIL_H
#ifdef HAVE_X11_EXTENSIONS_XSHM_H
# include <cantag/XOutputMechanism.hh>
# include <cantag/XDisplay.hh>
#endif
#endif
#endif


#ifdef HAVE_GL_GL_H
#ifdef HAVELIB_GL
#ifdef HAVELIB_GLU
#ifdef HAVELIB_GLUT
#ifdef HAVE_GL_GLX_H
# include <cantag/GLRenderWindow.hh>
#endif
#ifdef HAVE_GL_FREEGLUT_H
# include <cantag/GlutRenderWindow.hh>
#endif
# include <cantag/GLOutputMechanism.hh>
#endif
#endif
#endif
#endif

#ifdef HAVELIB_AVCODEC
#ifdef HAVELIB_AVFORMAT
#ifdef HAVELIB_Z
#ifdef HAVE_AVCODEC_H
#ifdef HAVE_AVFORMAT_H
# include <cantag/VideoImageSource.hh>
# include <cantag/VideoOutputMechanism.hh>
#endif
#endif
#endif
#endif
#endif

#ifdef HAVE_GSL_MULTIMIN_H
#ifdef HAVELIB_GSLCBLAS
#ifdef HAVELIB_GSL
# include <cantag/EstimateTransform.hh>
#endif
#endif
#endif

#endif//CANTAG_GUARD
