noinst_PROGRAMS=  MinDistances BatchTest
BatchTest_SOURCES=BatchTest.cc
MinDistances_SOURCES=MinDistances.cc


AM_CPPFLAGS=-I$(srcdir)/../include -I$(top_builddir)/include 
AM_LDFLAGS=-L../src -lcantag

if HAVE_DC1394_CONTROL_H
if HAVELIB_DC1394
AM_LDFLAGS += -ldc1394_control
endif
endif
