/**
 * $Header$
 *
 * A convenience header file for including all the other tripover headers
 */

#ifndef TRIPOVER_GUARD
#define TRIPOVER_GUARD

#include <Config.hh>

// basic tag code
#include <RingTag.hh>
#include <QuadTangle.hh>
//#include <TemplateTag.hh>

// coders
//#include <CRCCoder.hh>
//#include <GF4Coder.hh>
#include <SCCCoder.hh>
#include <ParityCoder.hh>
#include <SCCCoder.hh>
#include <SymbolChunkCoder.hh>
#if defined(HAVE_GMPXX) && defined(HAVELIB_GMP) && defined(HAVELIB_GMPXX)
# include <TripOriginalCoder.hh>
#endif

// image sources
#include <FileImageSource.hh>
#ifdef HAVE_LINUX_VIDEODEV_H
# include <V4LImageSource.hh>
#endif

// output mechanisms
#include <TextOutputMechanism.hh>
#if defined(HAVE_X11_XLIB_H) && defined(HAVE_X11_XUTIL_H) && defined(HAVE_X11_EXTENSIONS_XSHM_H)
# include <XOutputMechanism.hh>
# include <XOutputStagesMechanism.hh>
#endif

#if defined(HAVE_GL_GL_H) && defined(HAVELIB_GL) && defined(HAVELIB_GLU) && defined(HAVELIB_GLUT)
# include <GLOutputMechanism.hh>
#endif

// some predefined tags
#include <Tags.hh>

#endif//TRIPOVER_GUARD
