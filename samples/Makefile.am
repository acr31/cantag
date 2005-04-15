noinst_PROGRAMS=TextOnly DrawTag
TextOnly_SOURCES=TextOnly.cc 
DrawTag_SOURCES=DrawTag.cc

if HAVE_LINUX_VIDEODEV_H
if HAVE_X11_XLIB_H
if HAVE_X11_XUTIL_H
if HAVE_X11_EXTENSIONS_XSHM_H
noinst_PROGRAMS+=XOutput
XOutput_SOURCES=XOutput.cc
endif
endif
endif
endif

AM_CPPFLAGS=-I../include
AM_LDFLAGS=-L../src -ltotal -lm -lgslcblas -lgsl
