noinst_PROGRAMS= NewTest2 NewTest
#TextOnly DrawTag
#GnuplotOutput
#TextOnly_SOURCES=TextOnly.cc 
#DrawTag_SOURCES=DrawTag.cc
NewTest2_SOURCES=NewTest2.cc
NewTest_SOURCES=NewTest.cc
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


AM_CPPFLAGS=-I../include
AM_LDFLAGS=-L../src -ltotal

if HAVE_DC1394_CONTROL_H
if HAVELIB_DC1394
AM_LDFLAGS += -ldc1394_control
endif
endif
