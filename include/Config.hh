/**
 * $Header$
 */
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

#define TEXT_DEBUG
//#define IMAGE_DEBUG

#ifdef TEXT_DEBUG
#include <iostream>
#define PROGRESS(x) std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl
#else
#define PROGRESS(x)
#endif

#endif//CONFIG_GUARD
