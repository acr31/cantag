if HAVE_BOOST_TEST_UNITTEST_H
if HAVE_BOOST_RANDOM_H
if HAVELIB_BOOST_UNITTEST
noinst_PROGRAMS = TestCoder TestBigInt TestCamera TestGaussian TestEigen TestContourTree TestSocket TestEllipse
TestGaussian_SOURCES = TestGaussian.cc
TestCoder_SOURCES = TestCoder.cc
TestBigInt_SOURCES = TestBigInt.cc
TestCamera_SOURCES = TestCamera.cc
TestEigen_SOURCES = TestEigen.cc
TestContourTree_SOURCES = TestContourTree.cc
TestSocket_SOURCES = TestSocket.cc
TestEllipse_SOURCES = TestEllipse.cc

if HAVE_LINUX_VIDEODEV_H
noinst_PROGRAMS += TestV4LImageSource
TestV4LImageSource_SOURCES = TestV4LImageSource.cc
endif

endif
endif
endif



AM_CPPFLAGS=-I../include
AM_LDFLAGS=-L../src -ltotal -lboost_unit_test_framework-gcc-1_31
