/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       This file is a modification of original sys/types.h
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is just a wrapper around sys/types.h to fix missing types
 * fsblkcnt_t and fsfilcnt_t needed in statvfs.h and to avoid type conflicts
 * with pthread types from pthread module.
 */

/* unified sys/types.h:
   start with sef's sysvi386 version.
   merge go32 version -- a few ifdefs.
   h8300hms, h8300xray, and sysvnecv70 disagree on the following types:

   typedef int gid_t;
   typedef int uid_t;
   typedef int dev_t;
   typedef int ino_t;
   typedef int mode_t;
   typedef int caddr_t;

   however, these aren't "reasonable" values, the sysvi386 ones make far
   more sense, and should work sufficiently well (in particular, h8300
   doesn't have a stat, and the necv70 doesn't matter.) -- eichin
 */

#ifndef SYS_TYPES_H
#define SYS_TYPES_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _FSBLKCNT_T_DECLARED
#include <stdint.h>
typedef uint32_t fsblkcnt_t;
typedef uint32_t fsfilcnt_t;
#define _FSBLKCNT_T_DECLARED
#endif

#ifndef _SYS_TYPES_H

#include <_ansi.h>

#ifndef __INTTYPES_DEFINED__
#define __INTTYPES_DEFINED__

#include <machine/_types.h>

#if defined(__rtems__) || defined(__XMK__)
/*
 *  The following section is RTEMS specific and is needed to more
 *  closely match the types defined in the BSD sys/types.h.
 *  This is needed to let the RTEMS/BSD TCP/IP stack compile.
 */

/* deprecated */
#if ___int8_t_defined
typedef __uint8_t    u_int8_t;
#endif
#if ___int16_t_defined
typedef __uint16_t    u_int16_t;
#endif
#if ___int32_t_defined
typedef __uint32_t    u_int32_t;
#endif

#if ___int64_t_defined
typedef __uint64_t    u_int64_t;

/* deprecated */
typedef    __uint64_t    u_quad_t;
typedef    __int64_t    quad_t;
typedef    quad_t *    qaddr_t;
#endif

#endif

#endif /* ! __INTTYPES_DEFINED */

#ifndef __need_inttypes

#define _SYS_TYPES_H
#include <sys/_types.h>

#ifdef __i386__
#if defined (GO32) || defined (__MSDOS__)
#define __MS_types__
#endif
#endif

# include <stddef.h>
# include <machine/types.h>

/* To ensure the stat struct's layout doesn't change when sizeof(int), etc.
   changes, we assume sizeof short and long never change and have all types
   used to define struct stat use them and not int where possible.
   Where not possible, _ST_INTxx are used.  It would be preferable to not have
   such assumptions, but until the extra fluff is necessary, it's avoided.
   No 64 bit targets use stat yet.  What to do about them is postponed
   until necessary.  */
#ifdef __GNUC__
#define _ST_INT32 __attribute__ ((__mode__ (__SI__)))
#else
#define _ST_INT32
#endif

# ifndef    _POSIX_SOURCE

#  define    physadr        physadr_t
#  define    quad        quad_t

#ifndef _BSDTYPES_DEFINED
/* also defined in mingw/gmon.h and in w32api/winsock[2].h */
#ifndef __u_char_defined
typedef    unsigned char    u_char;
#define __u_char_defined
#endif
#ifndef __u_short_defined
typedef    unsigned short    u_short;
#define __u_short_defined
#endif
#ifndef __u_int_defined
typedef    unsigned int    u_int;
#define __u_int_defined
#endif
#ifndef __u_long_defined
typedef    unsigned long    u_long;
#define __u_long_defined
#endif
#define _BSDTYPES_DEFINED
#endif

typedef    unsigned short    ushort;        /* System V compatibility */
typedef    unsigned int    uint;        /* System V compatibility */
typedef    unsigned long    ulong;        /* System V compatibility */
# endif    /*!_POSIX_SOURCE */

#ifndef __clock_t_defined
typedef _CLOCK_T_ clock_t;
#define __clock_t_defined
#endif

#ifndef __time_t_defined
typedef _TIME_T_ time_t;
#define __time_t_defined
#endif

#ifndef __timespec_defined
#define __timespec_defined
/* Time Value Specification Structures, P1003.1b-1993, p. 261 */

struct timespec {
  time_t  tv_sec;   /* Seconds */
  long    tv_nsec;  /* Nanoseconds */
};
#endif

struct itimerspec {
  struct timespec  it_interval;  /* Timer period */
  struct timespec  it_value;     /* Timer expiration */
};

#ifndef __daddr_t_defined
typedef    long    daddr_t;
#define __daddr_t_defined
#endif
#ifndef __caddr_t_defined
typedef    char *    caddr_t;
#define __caddr_t_defined
#endif

#ifndef __CYGWIN__
#if defined(__MS_types__) || defined(__rtems__) || \
    defined(__sparc__) || defined(__SPU__)
typedef    unsigned long    ino_t;
#else
typedef    unsigned short    ino_t;
#endif
#endif /*__CYGWIN__*/

#ifdef __MS_types__
typedef unsigned long vm_offset_t;
typedef unsigned long vm_size_t;

#define __BIT_TYPES_DEFINED__

typedef signed char int8_t;
typedef unsigned char u_int8_t;
typedef short int16_t;
typedef unsigned short u_int16_t;
typedef int int32_t;
typedef unsigned int u_int32_t;
typedef long long int64_t;
typedef unsigned long long u_int64_t;
typedef int32_t register_t;
#endif /* __MS_types__ */

/*
 * All these should be machine specific - right now they are all broken.
 * However, for all of Cygnus' embedded targets, we want them to all be
 * the same.  Otherwise things like sizeof (struct stat) might depend on
 * how the file was compiled (e.g. -mint16 vs -mint32, etc.).
 */

#ifndef __CYGWIN__    /* which defines these types in it's own types.h. */
typedef _off_t    off_t;
typedef __dev_t dev_t;
typedef __uid_t uid_t;
typedef __gid_t gid_t;
#endif

#if defined(__XMK__)
typedef signed char pid_t;
#else
typedef int pid_t;
#endif

#if defined(__rtems__)
typedef _mode_t mode_t;
#endif

#ifndef __CYGWIN__
typedef    long key_t;
#endif
typedef _ssize_t ssize_t;

#if !defined(__CYGWIN__) && !defined(__rtems__)
#ifdef __MS_types__
typedef    char *    addr_t;
typedef int mode_t;
#else
#if defined (__sparc__) && !defined (__sparc_v9__)
#ifdef __svr4__
typedef unsigned long mode_t;
#else
typedef unsigned short mode_t;
#endif
#else
typedef unsigned int mode_t _ST_INT32;
#endif
#endif /* ! __MS_types__ */
#endif /*__CYGWIN__*/

typedef unsigned short nlink_t;

/* We don't define fd_set and friends if we are compiling POSIX
   source, or if we have included (or may include as indicated
   by __USE_W32_SOCKETS) the W32api winsock[2].h header which
   defines Windows versions of them.   Note that a program which
   includes the W32api winsock[2].h header must know what it is doing;
   it must not call the cygwin32 select function.
*/
# if !(defined (_POSIX_SOURCE) || defined (_WINSOCK_H) || defined (_WINSOCKAPI_) || defined (__USE_W32_SOCKETS))
#  define _SYS_TYPES_FD_SET
#  define    NBBY    8        /* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#  ifndef    FD_SETSIZE
#    define    FD_SETSIZE    64
#  endif

typedef    long    fd_mask;
#  define    NFDBITS    (sizeof (fd_mask) * NBBY)    /* bits per mask */
#  ifndef    howmany
#    define    howmany(x,y)    (((x)+((y)-1))/(y))
#  endif

/* We use a macro for fd_set so that including Sockets.h afterwards
   can work.  */
typedef    struct _types_fd_set {
    fd_mask    fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} _types_fd_set;

#define fd_set _types_fd_set

#  define    FD_SET(n, p)    ((p)->fds_bits[(n)/NFDBITS] |= (1L << ((n) % NFDBITS)))
#  define    FD_CLR(n, p)    ((p)->fds_bits[(n)/NFDBITS] &= ~(1L << ((n) % NFDBITS)))
#  define    FD_ISSET(n, p)    ((p)->fds_bits[(n)/NFDBITS] & (1L << ((n) % NFDBITS)))
#  define    FD_ZERO(p)    (__extension__ (void)({ \
     size_t __i; \
     char *__tmp = (char *)p; \
     for (__i = 0; __i < sizeof (*(p)); ++__i) \
       *__tmp++ = 0; \
}))

# endif    /* !(defined (_POSIX_SOURCE) || defined (_WINSOCK_H) || defined (_WINSOCKAPI_) || defined (__USE_W32_SOCKETS)) */

#undef __MS_types__
#undef _ST_INT32

#ifndef __clockid_t_defined
typedef _CLOCKID_T_ clockid_t;
#define __clockid_t_defined
#endif

#ifndef __timer_t_defined
typedef _TIMER_T_ timer_t;
#define __timer_t_defined
#endif

typedef unsigned long useconds_t;
typedef long suseconds_t;

#endif  /* !__need_inttypes */

#undef __need_inttypes

#endif /* _SYS_TYPES_H */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
#endif /* SYS_TYPES_H */
