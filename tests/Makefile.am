noinst_PROGRAMS= BatchTest
BatchTest_SOURCES=BatchTest.cc

AM_CPPFLAGS=-I../include
AM_LDFLAGS=-L../src -ltotal

if HAVE_DC1394_CONTROL_H
if HAVELIB_DC1394
AM_LDFLAGS += -ldc1394_control
endif
endif
