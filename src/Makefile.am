lib_LTLIBRARIES = libtotal.la
libtotal_la_SOURCES = Ellipse.cc QuadTangle.cc findtransform.cc Camera.cc gaussianelimination.cc Image.cc FileImageSource.cc  Template.cc polysolve.cc ContourTree.cc Socket.cc EllipseTransform.cc QuadTangleTransform.cc  ImageOutputMechanism.cc ContourFollower.cc algorithms/SimpleEllipseFit.cc algorithms/FullEllipseFit.cc algorithms/DistortionCorrection.cc

if HAVE_LINUX_VIDEODEV_H
libtotal_la_SOURCES += V4LImageSource.cc
endif

if HAVE_GL_GL_H
if HAVELIB_GL
libtotal_la_SOURCES += GLOutputMechanism.cc
endif
endif

if HAVE_X11_XLIB_H
if HAVE_X11_XUTIL_H
if HAVE_X11_EXTENSIONS_XSHM_H
libtotal_la_SOURCES += XOutputMechanism.cc XOutputStagesMechanism.cc
endif
endif
endif

AM_CPPFLAGS=-I../include
