/**
 * $Header$
 *
 * $Log$
 * Revision 1.4  2004/03/07 17:23:21  acr31
 * *** empty log message ***
 *
 * Revision 1.3  2004/03/07 11:10:49  acr31
 * *** empty log message ***
 *
 * Revision 1.2  2004/02/03 16:24:55  acr31
 * various function signature changes and use of __FILE__ and __LINE__ in debug macros
 *
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

#ifdef TEXT_DEBUG
#include <iostream>
#define PROGRESS(x) std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl
#else
#define PROGRESS(x)
#endif

#ifdef IMAGE_DEBUG
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#endif//CONFIG_GUARD
