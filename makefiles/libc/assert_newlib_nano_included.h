/* Warn if newlib_nano is requested but not actually used */
#include <newlib.h>
#ifndef _NANO_FORMATTED_IO
#warning newlib-nano requested but not included by the build system
#endif
