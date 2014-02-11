#ifndef INET_PTON_H_
#define INET_PTON_H_
#include <stdlib.h>

#ifndef AF_INET
#define AF_INET             2           ///< internetwork address family: UDP, TCP, etc.
#endif
#ifndef AF_INET6
/**
 * IPv6 address family.
 *
 * @see AF_INET
 */
#define AF_INET6            28
#endif

#ifndef IN6ADDRSZ
#define IN6ADDRSZ   16
#endif

#ifndef INT16SZ
#define INT16SZ sizeof(int16_t)
#endif

#ifndef INADDRSZ
#define INADDRSZ    4
#endif

#if !defined(EAFNOSUPPORT) && defined(WSAEAFNOSUPPORT)
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#endif

/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */

int inet_pton(int af, const char *src, void *dst);

#endif /* INET_PTON_H_ */
