/*
  Copyright (C) 2004 Andrew C. Rice

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
 * $Header$
 */
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

#include <cantag/CompilationOptions.h>

#ifdef TEXT_DEBUG
#include <iostream>
#define PROGRESS(x) std::cout.precision(32); std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl
#else
#define PROGRESS(x)
#endif

#ifdef HAVE_GSL_MULTIMIN_H
#ifdef HAVELIB_GSLCBLAS
#ifdef HAVELIB_GSL
#define HAVE_GSL
#endif
#endif
#endif

#ifdef HAVE_X11_XLIB_H
#ifdef HAVE_X11_XUTIL_H
#ifdef HAVE_X11_EXTENSIONS_XSHM_H
#define HAVE_XWINDOWS
#endif
#endif
#endif

#ifdef HAVE_AVCODEC_H
#define HAVE_AVCODEC
#endif

#endif//CONFIG_GUARD
