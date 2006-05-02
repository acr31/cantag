#noinst_PROGRAMS= NewTest NewTest3 DrawTag PositioningTest NewTest4 LSEllipseFit
noinst_PROGRAMS=FollowContour DrawTag NewTest5 AllCircles DrawTag2 CircleLocate
#LSEllipseFit_SOURCES=LSEllipseFit.cc
FollowContour_SOURCES=FollowContour.cc
AllCircles_SOURCES=AllCircles.cc
CircleLocate_SOURCES=CircleLocate.cc
#test_SOURCES=test.cc
#PositioningTest_SOURCES=PositioningTest.cc
#TextOnly DrawTag
#GnuplotOutput
#TextOnly_SOURCES=TextOnly.cc 
#TextFinder_SOURCES=TextFinder.cc
DrawTag_SOURCES=DrawTag.cc
DrawTag2_SOURCES=DrawTag2.cc
#NewTest3_SOURCES=NewTest3.cc
#NewTest4_SOURCES=NewTest4.cc
NewTest5_SOURCES=NewTest5.cc
#NewTest_SOURCES=NewTest.cc
#GnuplotOutput_SOURCES=GnuplotOutput.cc

if HAVE_LINUX_VIDEODEV_H
if HAVE_X11_XLIB_H
if HAVE_X11_XUTIL_H
if HAVE_X11_EXTENSIONS_XSHM_H
#noinst_PROGRAMS+=XOutput
#XOutput_SOURCES=XOutput.cc
endif
endif
endif
endif

if HAVE_GL_GL_H
if HAVE_GL_GLU_H
if HAVE_GL_OSMESA_H
#noinst_PROGRAMS+=GLOutput
#GLOutput_SOURCES=GLOutput.cc
endif
endif
endif


AM_CPPFLAGS=-I$(srcdir)/../include -I$(top_builddir)/include 
AM_LDFLAGS=-L../src -lcantag

if HAVE_DC1394_CONTROL_H
if HAVELIB_DC1394
AM_LDFLAGS += -ldc1394_control
endif
endif
