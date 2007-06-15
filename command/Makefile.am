bin_PROGRAMS  = cGrabImage cThresholdGlobal cThresholdAdaptive cSaveImage cFollowContour
cGrabImage_SOURCES = cGrabImage.cc
cThresholdGlobal_SOURCES = cThresholdGlobal.cc
cThresholdAdaptive_SOURCES = cThresholdAdaptive.cc
cSaveImage_SOURCES = cSaveImage.cc
cFollowContour_SOURCES = cFollowContour.cc

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
