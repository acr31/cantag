lib_LTLIBRARIES = libtotal.la
libtotal_la_SOURCES = Ellipse.cc QuadTangle.cc Camera.cc gaussianelimination.cc Image.cc FileImageSource.cc polysolve.cc Socket.cc algorithms/FitEllipseSimple.cc algorithms/FitEllipseLS.cc algorithms/DistortionCorrection.cc algorithms/TransformEllipseLinear.cc algorithms/TransformEllipseFull.cc Transform.cc algorithms/FitQuadTangleCorner.cc algorithms/FitQuadTangleConvexHull.cc algorithms/FitQuadTanglePolygon.cc algorithms/TransformQuadTangleProjective.cc algorithms/TransformQuadTangleReduced.cc algorithms/ContourFollowerTree.cc MonochromeImage.cc algorithms/ThresholdGlobal.cc algorithms/ThresholdAdaptive.cc algorithms/ConvexHull.cc entities/ContourEntity.cc 

#disabled  ImageOutputMechanism.cc Template.cc
#obsolete findtransform.cc 

if HAVE_GSL_MULTIMIN_H
if HAVELIB_GSLCBLAS
if HAVELIB_GSL
libtotal_la_SOURCES += algorithms/TransformQuadTangleSpaceSearch.cc
endif
endif
endif

if HAVE_LINUX_VIDEODEV_H
libtotal_la_SOURCES += V4LImageSource.cc
endif

if HAVE_GL_GL_H
if HAVELIB_GL
# disabled
# libtotal_la_SOURCES += GLOutputMechanism.cc

if HAVE_GL_OSMESA_H
if HAVELIB_OSMESA
if HAVE_GL_GLU_H
if HAVELIB_GLU
libtotal_la_SOURCES += GLImageSource.cc
endif
endif
endif
endif

endif
endif


if HAVE_DC1394_CONTROL_H
if HAVELIB_DC1394
libtotal_la_SOURCES += IEEE1394ImageSource.cc
endif
endif


if HAVE_X11_XLIB_H
if HAVE_X11_XUTIL_H
if HAVE_X11_EXTENSIONS_XSHM_H
#libtotal_la_SOURCES += XOutputStagesMechanism.cc
endif
endif
endif

AM_CPPFLAGS=-I../include
