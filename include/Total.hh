/**
 * $Header$
 *
 * A convenience header file for including all the other tripover headers
 */

#ifndef TRIPOVER_GUARD
#define TRIPOVER_GUARD

#include <tripover/Config.hh>

// basic tag code
#include <tripover/RingTag.hh>
#include <tripover/QuadTangle.hh>
//#include <tripover/TemplateTag.hh>

// coders
//#include <tripover/CRCCoder.hh>
//#include <tripover/GF4Coder.hh>
#include <tripover/SCCCoder.hh>
#include <tripover/ParityCoder.hh>
#include <tripover/SCCCoder.hh>
#include <tripover/SymbolChunkCoder.hh>
#if defined(HAVE_GMPXX) && defined(HAVELIB_GMP) && defined(HAVELIB_GMPXX)
# include <tripover/TripOriginalCoder.hh>
#endif

// image sources
#include <tripover/FileImageSource.hh>
#ifdef HAVE_LINUX_VIDEODEV_H
# include <tripover/V4LImageSource.hh>
#endif

// output mechanisms
#include <tripover/TextOutputMechanism.hh>
#if defined(HAVE_X11_XLIB_H) && defined(HAVE_X11_XUTIL_H) && defined(HAVE_X11_EXTENSIONS_XSHM_H)
# include <tripover/XOutputMechanism.hh>
# include <tripover/XOutputStagesMechanism.hh>
#endif

#if defined(HAVE_GL_GL_H) && defined(HAVELIB_GL) && defined(HAVELIB_GLU) && defined(HAVELIB_GLUT)
# include <tripover/GLOutputMechanism.hh>
#endif

// some predefined tags
#include <tripover/Tags.hh>

#endif//TRIPOVER_GUARD
