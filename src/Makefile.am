lib_LTLIBRARIES = libcantag.la

libcantag_la_SOURCES = Ellipse.cc QuadTangle.cc Camera.cc gaussianelimination.cc Image.cc FileImageSource.cc polysolve.cc Socket.cc algorithms/FitEllipseSimple.cc algorithms/FitEllipseLS.cc algorithms/DistortionCorrection.cc algorithms/TransformEllipseLinear.cc algorithms/TransformEllipseFull.cc Transform.cc algorithms/FitQuadTangleCorner.cc algorithms/FitQuadTangleConvexHull.cc algorithms/FitQuadTanglePolygon.cc algorithms/TransformQuadTangleProjective.cc algorithms/TransformQuadTangleReduced.cc algorithms/ContourFollowerTree.cc MonochromeImage.cc algorithms/ThresholdGlobal.cc algorithms/ThresholdAdaptive.cc algorithms/ConvexHull.cc entities/ContourEntity.cc algorithms/RemoveNonConcentricEllipse.cc algorithms/DrawEntity.cc algorithms/FitQuadTangleRegression.cc algorithms/DistortionCorrectionIterative.cc EstimateTransform.cc 

# disabled (acr31 branch)
#libcantag_la_SOURCES += algorithms/ValidateFitEllipse.cc algorithms/ValidateFitQuadTangle.cc algorithms/ValidateTransformQuadTangle.cc algorithms/ValidateDistortionCorrection.cc ImageOutputMechanism.cc

#disabled   Template.cc
#obsolete findtransform.cc 

if HAVE_GSL_MULTIMIN_H
if HAVELIB_GSLCBLAS
if HAVELIB_GSL
libcantag_la_SOURCES += algorithms/TransformQuadTangleSpaceSearch.cc algorithms/TransformQuadTangleCyberCode.cc
endif
endif
endif


if HAVE_LINUX_VIDEODEV_H
libcantag_la_SOURCES += V4LImageSource.cc
endif

if HAVE_GL_GL_H
if HAVELIB_GL
# disabled
# libcantag_la_SOURCES += GLOutputMechanism.cc
if HAVE_GL_OSMESA_H
if HAVELIB_OSMESA
if HAVE_GL_GLU_H
if HAVELIB_GLU
libcantag_la_SOURCES += GLImageSource.cc
endif
endif
endif
endif
endif
endif


if HAVELIB_AVCODEC
if HAVELIB_AVFORMAT
if HAVELIB_Z
if HAVE_AVCODEC_H
if HAVE_AVFORMAT_H
libcantag_la_SOURCES += VideoImageSource.cc
endif
endif
endif
endif
endif


if HAVE_DC1394_CONTROL_H
if HAVELIB_DC1394
libcantag_la_SOURCES += IEEE1394ImageSource.cc
endif
endif


if HAVE_X11_XLIB_H
if HAVE_X11_XUTIL_H
if HAVE_X11_EXTENSIONS_XSHM_H
#libcantag_la_SOURCES += XOutputStagesMechanism.cc
endif
endif
endif

AM_CPPFLAGS=-I$(srcdir)/../include -I$(top_builddir)/include 
