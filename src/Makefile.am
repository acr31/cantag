lib_LTLIBRARIES = libtripover.la
libtripover_la_SOURCES = Ellipse.cc QuadTangle.cc findtransform.cc Camera.cc gaussianelimination.cc Image.cc FileImageSource.cc  Template.cc  Tags.cc polysolve.cc ContourTree.cc Socket.cc XOutputMechanism.cc

if BUILD_VIDEO4LINUX
libtripover_la_SOURCES += V4LImageSource.cc
endif

AM_CPPFLAGS=-I../include
