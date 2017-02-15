/**
 * OpenGL
 */
#if 0
# define HAVELIB_GL
# define HAVELIB_GLU
# define HAVELIB_GLUT
# define HAVE_GL_FREEGLUT_H
# define HAVE_GL_GLUT_H
# define HAVE_GL_GLU_H
# define HAVE_GL_GL_H
#endif

/** GLX
 *
 */
#if 0
# define HAVE_GL_GLX_H
#endif


/**
 * Support for libavcodec
 */
#if 0
# define HABLIB_AVCODEC
# define HAVELIB_AVFORMAT
/* #undef HAVE_AVCODEC_H */
/* #undef HAVE_AVFORMAT_H */
#endif

/**
 * Boost
 */
#if 0
# define HAVE_BOOST_TEST_UNITTEST_H
# define HAVELIB_BOOST_UNITTEST
# define HAVE_BOOST_RANDOM_H
#endif

/**
 * Libdc1394 - Linux IEEE Firewire Libraries
 */
#if 0
# define HAVELIB_DC1394
# define HAVELIB_RAW1394
# define HAVE_DC1394_CONTROL_H
#endif

/**
 * GNU Bignum library
 */
#if 0
# define HAVELIB_GMP
# define HAVELIB_GMPXX
# define HAVE_GMPXX_H
#endif

/**
 * GNU Scientific Library
 */
#if 0
# define HAVELIB_GSL
# define HAVELIB_GSLCBLAS
# define HAVE_GSL_MULTIMIN_H
# define GSL_DLL   /* we use the GSL as a dll so this must be defined before we include their header files */
#endif

/**
 * ImageMagick
 */
#if 0
# define HAVELIB_MAGICK
# define HAVELIB_MAGICKXX
# define HAVE_MAGICKXX
#endif

/**
 * Video4Linux
 */
#if 0
# define HAVE_LINUX_VIDEODEV_H
#endif


/**
 * BSD Sockets
 */
#if 0
# define HAVE_NETINET_IN_H
# define HAVE_SYS_SOCKET_H
# define HAVE_UNISTD_H
#endif

/**
 * XWindows Stuff
 */
#if 0
# define HAVE_X11_EXTENSIONS_XSHM_H
# define HAVE_X11_XLIB_H
# define HAVE_X11_XUTIL_H
#endif

#define HAVECXX_COVARIANT_RETURNS
/* enable drawing and saving to disk of debug images */
/* #undef IMAGE_DEBUG */

/* enable printing of debug information */
/* #undef TEXT_DEBUG */
