/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/25 14:54:36  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.3  2004/01/21 11:55:07  acr31
 * added keywords for substitution
 *
 */
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

#define TEXT_DEBUG
#define IMAGE_DEBUG
#define FILENAME "Config.hh"
#ifdef TEXT_DEBUG
#include <iostream>
#define PROGRESS(x) std::cout << FILENAME << ": " << x << std::endl
#else
#define PROGRESS(x)
#endif

#ifdef IMAGE_DEBUG
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#endif//CONFIG_GUARD
