dnl Process this file with autoconf to produce a configure script.
AC_INIT(tripover,1.0)
AC_CONFIG_SRCDIR(src/Makefile.am)
AM_INIT_AUTOMAKE
AC_LANG([C++])

dnl Checks for programs.
AM_PROG_LIBTOOL
AC_PROG_CXX

dnl Checks for header files.
AC_CHECK_HEADER(opencv/cv.h,
	[],
	[AC_MSG_ERROR([opencv/cv.h not found in include path])])

AC_CHECK_HEADER(opencv/highgui.h,
        [],
        [AC_MSG_ERROR([opencv/highgui.h not found in include path])])

AC_CHECK_HEADER(GL/gl.h,
	[],
	[])

AC_CHECK_HEADER(GL/osmesa.h,
	[],
	[])

AC_CHECK_HEADER(boost/random.hpp,
	[AC_DEFINE(HAVE_BOOST_RANDOM)
         AM_CONDITIONAL(BUILD_RANDOM_INCLUDE,true)],
	[AM_CONDITIONAL(BUILD_RANDOM_INCLUDE,false)
         AC_MSG_NOTICE([boost/random.hh not found in include path])])
AC_CHECK_HEADER(boost/test/unit_test.hpp,
	[AM_CONDITIONAL(BUILD_TEST_FRAMEWORK_INCLUDE,true)],
	[AM_CONDITIONAL(BUILD_TEST_FRAMEWORK_INCLUDE,false)
         AC_MSG_NOTICE([boost/test/unit_test.hpp not found in include path])])
AC_CHECK_HEADER(gmpxx.h,
	[],
	[])
AC_CHECK_HEADER(asm/types.h,[],[])
AC_CHECK_HEADER(linux/videodev.h,
	[AM_CONDITIONAL(BUILD_VIDEO4LINUX,true)],
	[AM_CONDITIONAL(BUILD_VIDEO4LINUX,false)])



dnl Checks for libraries.
AC_CHECK_LIB(opencv,
	[cvGetErrStatus],
	[],
	[
AC_CHECK_LIB(cxcore,
        [cvGetErrStatus],
	[],
        AC_MSG_ERROR([libcxcore.so not found]))
AC_CHECK_LIB(cv,
	[cvAdd],
	[],
	AC_MSG_ERROR([libopencv.so or libcv.so not found]))])

AC_CHECK_LIB(highgui,
        [cvGetWindowName],
        [],
        [AC_MSG_ERROR([libhighgui.so not found])])

AC_CHECK_LIB(GL,
	[glFinish],
	[
LIBS="-lGL $LIBS"
AC_DEFINE(HAVELIB_GL)
AM_CONDITIONAL(BUILD_GL_LIB,true)
],
	[
AM_CONDITIONAL(BUILD_GL_LIB,false)
AC_MSG_NOTICE([libGL.so not found])
])

AC_CHECK_LIB(OSMesa,
	[OSMesaGetCurrentContext],
	[
LIBS="-lOSMesa $LIBS"
AC_DEFINE(HAVELIB_OSMESA)
AM_CONDITIONAL(BUILD_OSMESA_LIB,true)
],
	[
AM_CONDITIONAL(BUILD_OSMESA_LIB,false)
AC_MSG_NOTICE([libOSMesa.so not found])
])

AC_CHECK_LIB(GLU,
	[gluNewTess],
	[
LIBS="-lGLU $LIBS"
AC_DEFINE(HAVELIB_GLU)
AM_CONDITIONAL(BUILD_GLU_LIB,true)
],
	[
AM_CONDITIONAL(BUILD_GLU_LIB,false)
AC_MSG_NOTICE([libGLU.so not found])
])

AC_CHECK_LIB(glut,
	[glXChooseVisual],
	[
LIBS="-lglut $LIBS"
AC_DEFINE(HAVELIB_GLUT)
AM_CONDITIONAL(BUILD_GLUT_LIB,true)
],
	[
AM_CONDITIONAL(BUILD_GLUT_LIB,false)
AC_MSG_NOTICE([libglut.so not found])
])

AC_CHECK_LIB(gmp,
	[__gmpz_init],
	[],
	[])

AC_CHECK_LIB(gmpxx,
	[__gmpz_set],
	[],
	[])

LIBSSAVEACR31="$LIBS"
LIBS="-lboost_unit_test_framework-gcc-1_32 $LIBS"
AC_LINK_IFELSE(
AC_LANG_SOURCE([[
#ifdef __cplusplus
 extern "C"
#endif
int main();
int init_unit_test_suite(int, char**) {
    main();
   ;
   return 0;
}
]]),
[
AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_LIBBOOST_UNIT_TEST_FRAMEWORK))
AM_CONDITIONAL(BUILD_TEST_FRAMEWORK_LIB,true)
AC_MSG_NOTICE([found boost])],
[
AM_CONDITIONAL(BUILD_TEST_FRAMEWORK_LIB,false)
AC_MSG_NOTICE([boost unit test framework not found - disabling unit tests])])
LIBS="$LIBSSAVEACR31"

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

dnl Checks for library functions.

AC_OUTPUT([
Makefile
src/Makefile
tests/Makefile
unit/Makefile
])
