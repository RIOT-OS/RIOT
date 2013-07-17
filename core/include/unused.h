/**
 * @file
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#ifndef UNUSED_H_
#define UNUSED_H_

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif

#endif /* UNUSED_H_ */
