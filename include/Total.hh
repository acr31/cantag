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
#include <TripOriginalCoder.hh>

// image sources
#include <FileImageSource.hh>
#include <V4LImageSource.hh>

// output mechanisms
#include <TextOutputMechanism.hh>
#include <XOutputMechanism.hh>
#include <XOutputStagesMechanism.hh>

#if defined(HAVELIB_GL) && defined(HAVELIB_GLU) && defined(HAVELIB_GLUT)
#include <GLOutputMechanism.hh>
#endif

// some predefined tags
#include <Tags.hh>

#endif//TRIPOVER_GUARD
