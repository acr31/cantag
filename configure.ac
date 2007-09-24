dnl Process this file with autoconf to produce a configure script.
AC_INIT(cantag,1.0)
AC_CONFIG_HEADERS(include/cantag/CompilationOptions.h)
AC_CONFIG_SRCDIR(src/Makefile.am)
AM_INIT_AUTOMAKE
AC_LANG([C++])

dnl Checks for programs.
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL
AC_PROG_CXX

AC_CHECK_PROG(magicconfig,[Magick++-config --version],yes,no)

if test $CXX = "g++"; then
   CXXFLAGS="$CXXFLAGS -finline-limit=1000 -Wall -Wno-non-virtual-dtor"
fi

if test $magicconfig = "yes"; then
   CPPFLAGS="$CPPFLAGS `Magick-config --cppflags`"
#   CXXFLAGS="$CXXFLAGS `Magick-config --cflags`"
   LDFLAGS="$LDFLAGS `Magick-config --ldflags`"
fi

AC_ARG_ENABLE([image-debug],
	AC_HELP_STRING([--enable-image-debug],[enable drawing and saving to disk of debug images (disabled by default)]),
	AC_DEFINE([IMAGE_DEBUG],[],[enable drawing and saving to disk of debug images]),
	[])
	
AC_ARG_ENABLE([text-debug],
	AC_HELP_STRING([--enable-text-debug],[enable printing of debug information (disabled by default)]),
	AC_DEFINE([TEXT_DEBUG],[],[enable printing of debug information]),
	[])
	
AC_DEFUN([CANTAG_CHECK_HEADER],
         [AC_CHECK_HEADER([$1],
                         [AC_DEFINE([$2],[],[Define this to 1 if you have the <$1> header file])
                          $3=yes],
			 [$3=no],[$4])
          AM_CONDITIONAL([$2], [test X@S|@$3 = Xyes])])

AC_DEFUN([CANTAG_CHECK_LIB],
         [AC_CHECK_LIB([$1],[$2],
	               [LIBS="-l$1 $LIBS"
                        AC_DEFINE([$3],[],[Define this to 1 if you have the lib$1.so library file])
                        $4=yes],
                       [$4=no],[$5])
          AM_CONDITIONAL([$3], [test X@S|@$4 = Xyes])])

dnl check for covariant returns

AC_COMPILE_IFELSE(
AC_LANG_SOURCE([[
struct A {
    virtual A* Get() = 0;
    virtual ~A() {};
};

struct B {
    virtual B* Get() = 0;
    virtual ~B() {};
};

struct C : public A, public B {
    virtual C* Get() = 0;
    virtual ~C() {};
};
]]),
[
AC_DEFINE(HAVECXX_COVARIANT_RETURNS,[],[Define this to 1 if your compiler properly supports covariant return types])
havecxx_covariant_returns="yes"],
[havecxx_covariant_returns="no"])
AM_CONDITIONAL(HAVECXX_COVARIANT_RETURNS,test "havecxx_covariant_returns"="yes")
if test "$havecxx_covariant_returns" = "no"; then
   AC_MSG_ERROR([Your compiler does not support covariant return types (combined with multiple inheritance) for some of the features contained within Cantag - please upgrade to gcc 3.4.4 or newer])
fi


dnl Checks for header files.
CANTAG_CHECK_HEADER(sys/socket.h,HAVE_SYS_SOCKET_H,have_sys_socket_h)
CANTAG_CHECK_HEADER(netinet/in.h,HAVE_NETINET_IN_H,have_netinet_in_h)
CANTAG_CHECK_HEADER(Magick++.h,HAVE_MAGICKXX,have_magickxx_h)
CANTAG_CHECK_HEADER(GL/gl.h,HAVE_GL_GL_H,have_gl_gl_h)
CANTAG_CHECK_HEADER(GL/glu.h,HAVE_GL_GLU_H,have_gl_glu_h)
CANTAG_CHECK_HEADER(GL/glut.h,HAVE_GL_GLUT_H,have_gl_glut_h)
CANTAG_CHECK_HEADER(GL/freeglut.h,HAVE_GL_FREEGLUT_H,have_gl_freeglut_h)
CANTAG_CHECK_HEADER(GL/osmesa.h,HAVE_GL_OSMESA_H,have_gl_osmesa_h)
CANTAG_CHECK_HEADER(boost/random.hpp,HAVE_BOOST_RANDOM_H,have_boost_random_h)
CANTAG_CHECK_HEADER(boost/test/unit_test.hpp,HAVE_BOOST_TEST_UNITTEST_H,have_boost_test_unittest_h)
CANTAG_CHECK_HEADER(gmpxx.h,HAVE_GMPXX_H,have_gmpxx_h)
CANTAG_CHECK_HEADER(asm/types.h,HAVE_ASM_TYPES_H,have_asm_types_h)
CANTAG_CHECK_HEADER(linux/videodev.h,HAVE_LINUX_VIDEODEV_H,have_linux_videodev_h)
CANTAG_CHECK_HEADER(gsl/gsl_multimin.h,HAVE_GSL_MULTIMIN_H,have_gsl_multimin_h)
CANTAG_CHECK_HEADER(libdc1394/dc1394_control.h,HAVE_DC1394_CONTROL_H_V1,have_dc1394_control_h_v1)
CANTAG_CHECK_HEADER(dc1394/control.h,HAVE_DC1394_CONTROL_H_V2,have_dc1394_control_h_v2)
CANTAG_CHECK_HEADER(X11/Xlib.h,HAVE_X11_XLIB_H,have_x11_xlib_h)
CANTAG_CHECK_HEADER(X11/Xutil.h,HAVE_X11_XUTIL_H,have_x11_xutil_h)
CANTAG_CHECK_HEADER(ffmpeg/avcodec.h,HAVE_AVCODEC_H,have_avcodec_h)
CANTAG_CHECK_HEADER(ffmpeg/avformat.h,HAVE_AVFORMAT_H,have_avformat_h)
CANTAG_CHECK_HEADER(uEye.h,HAVE_UEYE_H,have_ueye_h)
CANTAG_CHECK_HEADER(X11/extensions/XShm.h,HAVE_X11_EXTENSIONS_XSHM_H,have_x11_extensions_xshm_h,
[
#ifdef HAVE_X11_XLIB_H
#include <X11/Xlib.h>
#endif
]
)

dnl Checks for libraries.
dnl new version of opencv needs check for cvGetErrStatus in cxcore and cvAdd in cv
CANTAG_CHECK_LIB(Magick,MagickError,HAVELIB_MAGICK,havelib_magick)
CANTAG_CHECK_LIB(Magick++,MagickPlusPlusDestroyMagick,HAVELIB_MAGICKXX,havelib_magickxx)
CANTAG_CHECK_LIB(GL,glFinish,HAVELIB_GL,havelib_gl)
CANTAG_CHECK_LIB(OSMesa,OSMesaGetCurrentContext,HAVELIB_OSMESA,havelib_osmesa)
CANTAG_CHECK_LIB(GLU,gluNewTess,HAVELIB_GLU,havelib_glu)
CANTAG_CHECK_LIB(glu32,gluNewTess,HAVELIB_GLU32,havelib_glu32)
CANTAG_CHECK_LIB(glut,glutSolidTeapot,HAVELIB_GLUT,havelib_glut)
CANTAG_CHECK_LIB(gmp,__gmpz_init,HAVELIB_GMP,havelib_gmp)
CANTAG_CHECK_LIB(gmpxx,__gmpz_set,HAVELIB_GMPXX,havelib_gmpxx)
CANTAG_CHECK_LIB(gslcblas,cblas_caxpy,HAVELIB_GSLCBLAS,havelib_gslcblas)
CANTAG_CHECK_LIB(gsl,gsl_multimin_diff,HAVELIB_GSL,havelib_gsl)
CANTAG_CHECK_LIB(raw1394,raw1394_iso_stop,HAVELIB_RAW1394,havelib_raw1394)
CANTAG_CHECK_LIB(dc1394_control,dc1394_dma_setup_capture,HAVELIB_DC1394_V1,havelib_dc1394_v1)
CANTAG_CHECK_LIB(dc1394, dc1394_find_cameras,HAVELIB_DC1394_V2,havelib_dc1394_v2)
CANTAG_CHECK_LIB(avcodec, img_convertno,HAVELIB_AVCODEC,havelib_avcodec)
CANTAG_CHECK_LIB(avformat,av_register_allno ,HAVELIB_AVFORMAT,havelib_avformat)
CANTAG_CHECK_LIB(z,uncompress ,HAVELIB_Z,havelib_z)
CANTAG_CHECK_LIB(ueye_api,is_InitCamera ,HAVELIB_UEYE_API,havelib_ueye_api)

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
AC_DEFINE(HAVELIB_BOOST_UNITTEST,[],[Define this to 1 if you have the libboost_unit_testframework library file])
havelib_boost_unittest="yes"],
[havelib_boost_unittest="no"])
LIBS="$LIBSSAVEACR31"
AM_CONDITIONAL(HAVELIB_BOOST_UNITTEST,test "$havelib_boost_unittest" = "yes")

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

dnl Checks for library functions.

AC_OUTPUT([
Makefile
src/Makefile
samples/Makefile
command/Makefile
include/Makefile
include/cantag/Makefile
include/cantag/algorithms/Makefile
include/cantag/coders/Makefile
include/cantag/entities/Makefile
])

if test "$havecxx_covariant_returns" = "yes"; then
   echo "+ Enabled ComposedEntity classes"
else
   echo "! Disabled ComposedEntity classes"
fi

if test "$havelib_dc1394_v2$have_dc1394_control_h_v2" = "yesyes"; then
   echo "+ Enabled IEEE1394 API version 2"
else
   echo "| Disabled IEEE1943 API version 2"
fi


if test "$havelib_dc1394_v1$have_dc1394_control_h_v1" = "yesyes"; then
   echo "+ Enabled IEEE1934 API version 1"
else
   echo "! Disabled IEEE1394 API version 1"
fi

if test "$have_gl_gl_h$havelib_gl$havelib_glu$havelib_glut" = "yesyesyesyes"; then
   echo "+ Enabled use of OpenGL"
else
   echo "! Disabled use of OpenGL"
fi

if test "$have_gl_osmesa_h$havelib_osmesa" = "yesyes"; then
   echo "+ Enabled MESA Off-screen rendering"
else
   echo "! Disabled use of MESA Off-screen rendering"
fi

if test "$have_boost_random_h" = "yes"; then
   echo "+ Enabled use of Boost C++ libraries \"Random\" module"
else
   echo "! Disabled use of Boost C++ \"Random\""
fi

if test "$have_boost_test_unittest_h$havelib_boost_unittest" = "yesyes"; then
   echo "+ Enabled use of Boost C++ libraries \"UnitTest\" module"
else
   echo "! Disabled use of Boost++ libraries \"UnitTest\""
fi

if test "$have_gmpxx_h$havelib_gmp$havelib_gmpxx" = "yesyesyes"; then
   echo "+ Enabled use of GNU MP Bignum Library (with C++ support)"
else
   echo "! Disabled use of GNU MP Bignum library (with C++ support)"
fi

if test "$have_linux_videodev_h" = "yes"; then
   echo "+ Enabled use of Video4Linux"
else
   echo "! Disabled use of Video4Linux"
fi

if test "$have_x11_xlib_h$have_x11_xutil_h$have_x11_extensions_xshm_h" = "yesyesyes"; then
   echo "+ Enabled Xlib and MIT XSHM Extension"
else
   echo "! Disabled use of Xlib and MIT XSHM Extension"
fi

if test "$have_magickxx_h$havelib_magick$havelib_magickxx" = "yesyesyes"; then
   echo "+ Enabled ImageMagick support"
else
   echo "! Disabled ImageMagick support"
fi

if test "$have_avformat_h$have_avcodec_h$havelib_z$havelib_avcodec$havelib_avformat" = "yesyesyesyesyes"; then
   echo "+ Enabled Movie support (libavcodec)"
else
   echo "! Disabled Movie support (libavcodec)"
fi

if test "$have_gsl_multimin_h$havelib_gsl$havelib_gslcblas" = "yesyesyes"; then
   echo "+ Enabled GNU Scientific Library support"
else
   echo "! Disabled GNU Scientific Library support"
fi

if test "$have_sys_socket_h$have_netinet_in_h" = "yesyes"; then
   echo "+ Enabled BSD Socket support"
else
   echo "! Disabled BSD Socket support"
fi

if test "$have_ueye_h$havelib_ueye_api" = "yesyes"; then
   echo "+ Enabled uEye Camera support"
else
   echo "! Disabled uEye Camera support"
fi
