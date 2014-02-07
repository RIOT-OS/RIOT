#ifndef INET_NTOP_H_
#define INET_NTOP_H_
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

#if !defined(EAFNOSUPPORT) && defined(WSAEAFNOSUPPORT)
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#endif

/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */

const char *inet_ntop(int af, const void *src, char *dst, size_t size);

#endif /* INET_NTOP_H_ */
