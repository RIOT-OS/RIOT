#ifndef INET_PTON_H_
#define INET_PTON_H_

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
