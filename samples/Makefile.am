bin_PROGRAMS = DrawTagSVG
bin_PROGRAMS += DrawTagImage
bin_PROGRAMS += SimpleRecogniseCircle 
bin_PROGRAMS += SimpleRecogniseSquare 
bin_PROGRAMS += GLOverlayCircle 
bin_PROGRAMS += GLOverlaySquare
bin_PROGRAMS += SimpleRecogniseAnalogue
bin_PROGRAMS += XOverlaySquare
bin_PROGRAMS += DrawTagAnalogueImage

DrawTagSVG_SOURCES  = DrawTagSVG.cc
DrawTagImage_SOURCES = DrawTagImage.cc
DrawTagAnalogueImage_SOURCES = DrawTagAnalogueImage.cc
SimpleRecogniseCircle_SOURCES = SimpleRecogniseCircle.cc
SimpleRecogniseSquare_SOURCES = SimpleRecogniseSquare.cc
SimpleRecogniseAnalogue_SOURCES = SimpleRecogniseAnalogue.cc
GLOverlayCircle_SOURCES = GLOverlayCircle.cc
GLOverlaySquare_SOURCES = GLOverlaySquare.cc
XOverlaySquare_SOURCES = XOverlaySquare.cc

AM_CPPFLAGS=-I$(srcdir)/../include -I$(top_builddir)/include 
AM_LDFLAGS=-L../src -lcantag

if HAVE_DC1394_CONTROL_H_V1
if HAVELIB_DC1394_V1
AM_LDFLAGS += -ldc1394_control
endif
endif

if HAVE_DC1394_CONTROL_H_V2
if HAVELIB_DC1394_V2
AM_LDFLAGS += -ldc1394
endif
endif
