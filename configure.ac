dnl Process this file with autoconf to produce a configure script.
AC_INIT(tripover,1.0)
AC_CONFIG_SRCDIR(src/Ellipse2D.cc)
AM_INIT_AUTOMAKE
AC_LANG([C++])

dnl Checks for programs.
AM_PROG_LIBTOOL
AC_PROG_CXX

dnl Checks for libraries.
AC_CHECK_LIB(opencv,
	[cvGetErrStatus],
	[],
	[AC_MSG_ERROR([libopencv.so not found])])

AC_CHECK_LIB(highgui,
	[cvGetWindowName],
	[],
	[AC_MSG_ERROR([libhighgui.so not found])])

AC_CHECK_LIB(GL,
	[glFinish],
	[],
	[AC_MSG_ERROR([libGL.so not found])])

AC_CHECK_LIB(OSMesa,
	[OSMesaGetCurrentContext],
	[],
	[AC_MSG_ERROR([libOSMesa.so not found])])

dnl Checks for header files.
AC_CHECK_HEADER(opencv/cv.h,
	[],
	[AC_MSG_ERROR([opencv/cv.h not found in include path])])
AC_CHECK_HEADER(opencv/highgui.h,
	[],
	[AC_MSG_ERROR([opencv/highgui.h not found in include path])])
AC_CHECK_HEADER(GL/gl.h,
	[],
	[AC_MSG_ERROR([GL/gl.h not found in include path])])
AC_CHECK_HEADER(GL/osmesa.h,
	[],
	[AC_MSG_ERROR([GL/osmesa.h not found in include path])])



dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

dnl Checks for library functions.

AC_OUTPUT([
Makefile
src/Makefile
tests/Makefile
gltest/Makefile
])
