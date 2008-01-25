bin_PROGRAMS = DrawTagSVG
bin_PROGRAMS += DrawTagImage
bin_PROGRAMS += SimpleRecogniseCircle 
bin_PROGRAMS += SimpleRecogniseSquare 


DrawTagSVG_SOURCES  = DrawTagSVG.cc
DrawTagImage_SOURCES = DrawTagImage.cc
SimpleRecogniseCircle_SOURCES = SimpleRecogniseCircle.cc
SimpleRecogniseSquare_SOURCES = SimpleRecogniseSquare.cc


if HAVE_GL_GL_H
if HAVELIB_GL
if HAVELIB_GLU
if HAVELIB_GLUT
if HAVE_GL_FREEGLUT_H
bin_PROGRAMS += GLOverlayCircle 
bin_PROGRAMS += GLOverlaySquare
GLOverlayCircle_SOURCES = GLOverlayCircle.cc
GLOverlaySquare_SOURCES = GLOverlaySquare.cc
endif
endif
endif
endif
endif

if HAVE_X11_XLIB_H
if HAVE_X11_XUTIL_H
if HAVE_X11_EXTENSIONS_XSHM_H
bin_PROGRAMS += XOverlaySquare
XOverlaySquare_SOURCES = XOverlaySquare.cc
endif
endif
endif


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
