/**
 * $Header$
 */
#ifndef CONFIG_GUARD
#define CONFIG_GUARD

#include <CompilationOptions.h>

#ifdef TEXT_DEBUG
#include <iostream>
#define PROGRESS(x) std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl
#else
#define PROGRESS(x)
#endif

#endif//CONFIG_GUARD
