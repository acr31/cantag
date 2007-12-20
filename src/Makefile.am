lib_LTLIBRARIES = libcantag.la

libcantag_la_SOURCES = testsym.cc
libcantag_la_SOURCES += Ellipse.cc 
libcantag_la_SOURCES += QuadTangle.cc 
libcantag_la_SOURCES += Camera.cc
libcantag_la_SOURCES += gaussianelimination.cc
libcantag_la_SOURCES += Image.cc
libcantag_la_SOURCES += FileImageSource.cc
libcantag_la_SOURCES += NetworkImageSource.cc
libcantag_la_SOURCES += polysolve.cc
libcantag_la_SOURCES += Transform.cc
libcantag_la_SOURCES += MonochromeImage.cc
libcantag_la_SOURCES += GF4Poly.cc
libcantag_la_SOURCES += TagAnalogue.cc
libcantag_la_SOURCES += entities/ContourEntity.cc
libcantag_la_SOURCES += algorithms/FitEllipseSimple.cc
libcantag_la_SOURCES += algorithms/FitEllipseLS.cc
libcantag_la_SOURCES += algorithms/DistortionCorrectionNone.cc
libcantag_la_SOURCES += algorithms/DistortionCorrectionSimple.cc
libcantag_la_SOURCES += algorithms/TransformEllipseLinear.cc
libcantag_la_SOURCES += algorithms/TransformEllipseFull.cc
libcantag_la_SOURCES += algorithms/FitQuadTangleCorner.cc
libcantag_la_SOURCES += algorithms/FitQuadTangleConvexHull.cc
libcantag_la_SOURCES += algorithms/FitQuadTanglePolygon.cc
libcantag_la_SOURCES += algorithms/TransformQuadTangleProjective.cc
libcantag_la_SOURCES += algorithms/TransformQuadTangleReduced.cc
libcantag_la_SOURCES += algorithms/ContourFollowerTree.cc
libcantag_la_SOURCES += algorithms/ContourFollowerClearImageBorder.cc
libcantag_la_SOURCES += algorithms/ThresholdGlobal.cc
libcantag_la_SOURCES += algorithms/ThresholdAdaptive.cc
libcantag_la_SOURCES += algorithms/ConvexHull.cc
libcantag_la_SOURCES += algorithms/RemoveNonConcentricEllipse.cc
libcantag_la_SOURCES += algorithms/DrawEntity.cc
libcantag_la_SOURCES += algorithms/PrintEntity.cc
libcantag_la_SOURCES += algorithms/FitQuadTangleRegression.cc
libcantag_la_SOURCES += algorithms/SimulateMinDistance.cc
libcantag_la_SOURCES += algorithms/SignalStrengthContour.cc
libcantag_la_SOURCES += algorithms/SampleTagAnalogue.cc
libcantag_la_SOURCES += algorithms/HoughTransform.cc
libcantag_la_SOURCES += algorithms/DrawTagAnalogue.cc
libcantag_la_SOURCES += AutoThresholdGlobal.cc
libcantag_la_SOURCES += Timer.cc

if HAVE_SYS_SOCKET_H
if HAVE_NETINET_IN_H
libcantag_la_SOURCES += Socket.cc
endif
endif

if HAVE_GMPXX_H
if HAVELIB_GMP
if HAVELIB_GMPXX
libcantag_la_SOURCES += BigInt.cc
endif
endif
endif

# disabled (acr31 branch)
#libcantag_la_SOURCES += algorithms/ValidateFitEllipse.cc algorithms/ValidateFitQuadTangle.cc algorithms/ValidateTransformQuadTangle.cc algorithms/ValidateDistortionCorrection.cc ImageOutputMechanism.cc

#disabled   Template.cc
#obsolete findtransform.cc 

if HAVE_GSL_MULTIMIN_H
if HAVELIB_GSLCBLAS
if HAVELIB_GSL
libcantag_la_SOURCES += algorithms/DistortionCorrectionIterative.cc
libcantag_la_SOURCES += algorithms/TransformQuadTangleSpaceSearch.cc 
libcantag_la_SOURCES += algorithms/TransformQuadTangleCyberCode.cc
libcantag_la_SOURCES += EstimateTransform.cc
endif
endif
endif

if HAVE_GL_GL_H
if HAVE_GL_GLUT_H
if HAVELIB_GL
libcantag_la_SOURCES += GLRenderWindow.cc
if HAVE_GL_FREEGLUT_H
if HAVELIB_GLUT
libcantag_la_SOURCES += GlutRenderWindow.cc
endif
endif
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

if HAVE_DC1394_CONTROL_H_V1
if HAVELIB_DC1394_V1
libcantag_la_SOURCES += IEEE1394ImageSource_V1.cc
endif
endif

if HAVE_DC1394_CONTROL_H_V2
if HAVELIB_DC1394_V2
libcantag_la_SOURCES += IEEE1394ImageSource_V2.cc
endif
endif

if HAVE_UEYE_H
if HAVELIB_UEYE_API
libcantag_la_SOURCES += UEyeImageSource.cc
endif
endif


AM_CPPFLAGS=-I$(srcdir)/../include -I$(top_builddir)/include 

