/*
  Copyright (C) 2006 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/


/**
 * Manual compilation options file for windows builds
 */

/**
 * OpenGL
 */
#if 1
# define HAVELIB_GL 
# define HAVELIB_GLU 
# define HAVELIB_GLUT 
# define HAVE_GL_FREEGLUT_H 
# define HAVE_GL_GLUT_H 
# define HAVE_GL_GLU_H 
# define HAVE_GL_GL_H 
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
#if 1
# define HAVELIB_GMP 
# define HAVELIB_GMPXX 
# define HAVE_GMPXX_H 
#endif

/**
 * GNU Scientific Library
 */
#if 1
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

/* enable drawing and saving to disk of debug images */
/* #undef IMAGE_DEBUG */

/* enable printing of debug information */
/* #undef TEXT_DEBUG */
