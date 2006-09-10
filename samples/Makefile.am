bin_PROGRAMS  = DrawTagSVG DrawTagImage SimpleRecogniseCircle SimpleRecogniseSquare GLOverlayCircle GLOverlaySquare 
DrawTagSVG_SOURCES  = DrawTagSVG.cc
DrawTagImage_SOURCES = DrawTagImage.cc
SimpleRecogniseCircle_SOURCES = SimpleRecogniseCircle.cc
SimpleRecogniseSquare_SOURCES = SimpleRecogniseSquare.cc
GLOverlayCircle_SOURCES = GLOverlayCircle.cc
GLOverlaySquare_SOURCES = GLOverlaySquare.cc

AM_CPPFLAGS=-I$(srcdir)/../include -I$(top_builddir)/include 
AM_LDFLAGS=-L../src -lcantag

if HAVE_DC1394_CONTROL_H
if HAVELIB_DC1394
AM_LDFLAGS += -ldc1394_control
endif
endif
