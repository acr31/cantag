
if BUILD_TEST_FRAMEWORK_LIB
if BUILD_TEST_FRAMEWORK_INCLUDE
if BUILD_RANDOM_INCLUDE
noinst_PROGRAMS = TestCoder TestBigInt TestCamera TestGaussian TestEigen TestContourTree
TestGaussian_SOURCES = TestGaussian.cc
TestCoder_SOURCES = TestCoder.cc
TestBigInt_SOURCES = TestBigInt.cc
TestCamera_SOURCES = TestCamera.cc
TestEigen_SOURCES = TestEigen.cc
TestContourTree_SOURCES = TestContourTree.cc

if BUILD_VIDEO4LINUX2
noinst_PROGRAMS += TestV4LImageSource
TestV4LImageSource_SOURCES = TestV4LImageSource.cc
endif

endif
endif
endif



AM_CPPFLAGS=-I../include
AM_LDFLAGS=-L../src -ltripover -lboost_unit_test_framework-gcc-1_32
