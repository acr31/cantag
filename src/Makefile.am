lib_LTLIBRARIES = libtripover.la
libtripover_la_SOURCES = Ellipse.cc QuadTangle.cc findtransform.cc Camera.cc gaussianelimination.cc Image.cc FileImageSource.cc  Template.cc  Tags.cc polysolve.cc ContourTree.cc Socket.cc EllipseTransform.cc QuadTangleTransform.cc
#GF4Coder.cc GF4Poly.cc

if HAVE_LINUX_VIDEODEV_H
libtripover_la_SOURCES += V4LImageSource.cc
endif

if HAVE_GL_GL_H
if HAVELIB_GL
libtripover_la_SOURCES += GLOutputMechanism.cc
endif
endif

if HAVE_X11_XLIB_H
if HAVE_X11_XUTIL_H
if HAVE_X11_EXTENSIONS_XSHM_H
libtripover_la_SOURCES += XOutputMechanism.cc XOutputStagesMechanism.cc
endif
endif
endif

AM_CPPFLAGS=-I../include
