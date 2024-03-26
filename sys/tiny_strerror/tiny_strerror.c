/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *               2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Tiny strerror() implementation
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "flash_utils.h"
#include "kernel_defines.h"
#include "tiny_strerror.h"

static FLASH_ATTR const char _ok[] = "OK";
static FLASH_ATTR const char _e2big[] = "-E2BIG";
static FLASH_ATTR const char _eacces[] = "-EACCES";
static FLASH_ATTR const char _eaddrinuse[] = "-EADDRINUSE";
static FLASH_ATTR const char _eaddrnotavail[] = "-EADDRNOTAVAIL";
static FLASH_ATTR const char _eafnosupport[] = "-EAFNOSUPPORT";
static FLASH_ATTR const char _eagain[] = "-EAGAIN";
static FLASH_ATTR const char _ealready[] = "-EALREADY";
static FLASH_ATTR const char _ebadf[] = "-EBADF";
static FLASH_ATTR const char _ebadmsg[] = "-EBADMSG";
static FLASH_ATTR const char _ebusy[] = "-EBUSY";
static FLASH_ATTR const char _ecanceled[] = "-ECANCELED";
static FLASH_ATTR const char _echild[] = "-ECHILD";
static FLASH_ATTR const char _econnaborted[] = "-ECONNABORTED";
static FLASH_ATTR const char _econnrefused[] = "-ECONNREFUSED";
static FLASH_ATTR const char _econnreset[] = "-ECONNRESET";
static FLASH_ATTR const char _edeadlk[] = "-EDEADLK";
static FLASH_ATTR const char _edestaddrreq[] = "-EDESTADDRREQ";
static FLASH_ATTR const char _edom[] = "-EDOM";
static FLASH_ATTR const char _edquot[] = "-EDQUOT";
static FLASH_ATTR const char _eexist[] = "-EEXIST";
static FLASH_ATTR const char _efault[] = "-EFAULT";
static FLASH_ATTR const char _efbig[] = "-EFBIG";
static FLASH_ATTR const char _ehostdown[] = "-EHOSTDOWN";
static FLASH_ATTR const char _ehostunreach[] = "-EHOSTUNREACH";
static FLASH_ATTR const char _eidrm[] = "-EIDRM";
static FLASH_ATTR const char _eilseq[] = "-EILSEQ";
static FLASH_ATTR const char _einprogress[] = "-EINPROGRESS";
static FLASH_ATTR const char _eintr[] = "-EINTR";
static FLASH_ATTR const char _einval[] = "-EINVAL";
static FLASH_ATTR const char _eio[] = "-EIO";
static FLASH_ATTR const char _eisconn[] = "-EISCONN";
static FLASH_ATTR const char _eisdir[] = "-EISDIR";
static FLASH_ATTR const char _eloop[] = "-ELOOP";
static FLASH_ATTR const char _emfile[] = "-EMFILE";
static FLASH_ATTR const char _emlink[] = "-EMLINK";
static FLASH_ATTR const char _emsgsize[] = "-EMSGSIZE";
static FLASH_ATTR const char _emultihop[] = "-EMULTIHOP";
static FLASH_ATTR const char _enametoolong[] = "-ENAMETOOLONG";
static FLASH_ATTR const char _enetdown[] = "-ENETDOWN";
static FLASH_ATTR const char _enetreset[] = "-ENETRESET";
static FLASH_ATTR const char _enetunreach[] = "-ENETUNREACH";
static FLASH_ATTR const char _enfile[] = "-ENFILE";
static FLASH_ATTR const char _enobufs[] = "-ENOBUFS";
static FLASH_ATTR const char _enodata[] = "-ENODATA";
static FLASH_ATTR const char _enodev[] = "-ENODEV";
static FLASH_ATTR const char _enoent[] = "-ENOENT";
static FLASH_ATTR const char _enoexec[] = "-ENOEXEC";
static FLASH_ATTR const char _enolck[] = "-ENOLCK";
static FLASH_ATTR const char _enolink[] = "-ENOLINK";
static FLASH_ATTR const char _enomem[] = "-ENOMEM";
static FLASH_ATTR const char _enomsg[] = "-ENOMSG";
static FLASH_ATTR const char _enoprotoopt[] = "-ENOPROTOOPT";
static FLASH_ATTR const char _enospc[] = "-ENOSPC";
static FLASH_ATTR const char _enosr[] = "-ENOSR";
static FLASH_ATTR const char _enostr[] = "-ENOSTR";
static FLASH_ATTR const char _enosys[] = "-ENOSYS";
static FLASH_ATTR const char _enotconn[] = "-ENOTCONN";
static FLASH_ATTR const char _enotdir[] = "-ENOTDIR";
static FLASH_ATTR const char _enotempty[] = "-ENOTEMPTY";
static FLASH_ATTR const char _enotrecoverable[] = "-ENOTRECOVERABLE";
static FLASH_ATTR const char _enotsock[] = "-ENOTSOCK";
static FLASH_ATTR const char _enotsup[] = "-ENOTSUP";
static FLASH_ATTR const char _enotty[] = "-ENOTTY";
static FLASH_ATTR const char _enxio[] = "-ENXIO";
static FLASH_ATTR const char _eoverflow[] = "-EOVERFLOW";
static FLASH_ATTR const char _eownerdead[] = "-EOWNERDEAD";
static FLASH_ATTR const char _eperm[] = "-EPERM";
static FLASH_ATTR const char _epfnosupport[] = "-EPFNOSUPPORT";
static FLASH_ATTR const char _epipe[] = "-EPIPE";
static FLASH_ATTR const char _eprotonosupport[] = "-EPROTONOSUPPORT";
static FLASH_ATTR const char _eprototype[] = "-EPROTOTYPE";
static FLASH_ATTR const char _eproto[] = "-EPROTO";
static FLASH_ATTR const char _erange[] = "-ERANGE";
static FLASH_ATTR const char _erofs[] = "-EROFS";
static FLASH_ATTR const char _espipe[] = "-ESPIPE";
static FLASH_ATTR const char _esrch[] = "-ESRCH";
static FLASH_ATTR const char _estale[] = "-ESTALE";
static FLASH_ATTR const char _etimedout[] = "-ETIMEDOUT";
static FLASH_ATTR const char _etime[] = "-ETIME";
static FLASH_ATTR const char _etoomanyrefs[] = "-ETOOMANYREFS";
static FLASH_ATTR const char _etxtbsy[] = "-ETXTBSY";
static FLASH_ATTR const char _exdev[] = "-EXDEV";
/* EAGAIN and EWOULDBLOCK have the exact same meaning and consequently may
 * have the same numeric value */
#if EAGAIN != EWOULDBLOCK
static FLASH_ATTR const char _ewouldblock[] = "-EWOULDBLOCK";
#endif
/* ENOTSUP and EOPNOTSUPP do not have the exact same meaning. Still, they
 * have the same numeric value on Linux, breaking POSIX standard */
#if ENOTSUP != EOPNOTSUPP
static FLASH_ATTR const char _eopnotsupp[] = "-EOPNOTSUPP";
#endif

static FLASH_ATTR const char * FLASH_ATTR const lookup[] = {
    [0]                 = _ok,
    [E2BIG]             = _e2big,
    [EACCES]            = _eacces,
    [EADDRINUSE]        = _eaddrinuse,
    [EADDRNOTAVAIL]     = _eaddrnotavail,
    [EAFNOSUPPORT]      = _eafnosupport,
    [EAGAIN]            = _eagain,
    [EALREADY]          = _ealready,
    [EBADF]             = _ebadf,
    [EBADMSG]           = _ebadmsg,
    [EBUSY]             = _ebusy,
    [ECANCELED]         = _ecanceled,
    [ECHILD]            = _echild,
    [ECONNABORTED]      = _econnaborted,
    [ECONNREFUSED]      = _econnrefused,
    [ECONNRESET]        = _econnreset,
    [EDEADLK]           = _edeadlk,
    [EDESTADDRREQ]      = _edestaddrreq,
    [EDOM]              = _edom,
    [EDQUOT]            = _edquot,
    [EEXIST]            = _eexist,
    [EFAULT]            = _efault,
    [EFBIG]             = _efbig,
    [EHOSTDOWN]         = _ehostdown,
    [EHOSTUNREACH]      = _ehostunreach,
    [EIDRM]             = _eidrm,
    [EILSEQ]            = _eilseq,
    [EINPROGRESS]       = _einprogress,
    [EINTR]             = _eintr,
    [EINVAL]            = _einval,
    [EIO]               = _eio,
    [EISCONN]           = _eisconn,
    [EISDIR]            = _eisdir,
    [ELOOP]             = _eloop,
    [EMFILE]            = _emfile,
    [EMLINK]            = _emlink,
    [EMSGSIZE]          = _emsgsize,
    [EMULTIHOP]         = _emultihop,
    [ENAMETOOLONG]      = _enametoolong,
    [ENETDOWN]          = _enetdown,
    [ENETRESET]         = _enetreset,
    [ENETUNREACH]       = _enetunreach,
    [ENFILE]            = _enfile,
    [ENOBUFS]           = _enobufs,
    [ENODATA]           = _enodata,
    [ENODEV]            = _enodev,
    [ENOENT]            = _enoent,
    [ENOEXEC]           = _enoexec,
    [ENOLCK]            = _enolck,
    [ENOLINK]           = _enolink,
    [ENOMEM]            = _enomem,
    [ENOMSG]            = _enomsg,
    [ENOPROTOOPT]       = _enoprotoopt,
    [ENOSPC]            = _enospc,
    [ENOSR]             = _enosr,
    [ENOSTR]            = _enostr,
    [ENOSYS]            = _enosys,
    [ENOTCONN]          = _enotconn,
    [ENOTDIR]           = _enotdir,
    [ENOTEMPTY]         = _enotempty,
    [ENOTRECOVERABLE]   = _enotrecoverable,
    [ENOTSOCK]          = _enotsock,
    [ENOTSUP]           = _enotsup,
    [ENOTTY]            = _enotty,
    [ENXIO]             = _enxio,
    [EOVERFLOW]         = _eoverflow,
    [EOWNERDEAD ]       = _eownerdead,
    [EPERM]             = _eperm,
    [EPFNOSUPPORT]      = _epfnosupport,
    [EPIPE]             = _epipe,
    [EPROTONOSUPPORT]   = _eprotonosupport,
    [EPROTOTYPE]        = _eprototype,
    [EPROTO]            = _eproto,
    [ERANGE]            = _erange,
    [EROFS]             = _erofs,
    [ESPIPE]            = _espipe,
    [ESRCH]             = _esrch,
    [ESTALE]            = _estale,
    [ETIMEDOUT]         = _etimedout,
    [ETIME]             = _etime,
    [ETOOMANYREFS]      = _etoomanyrefs,
    [ETXTBSY]           = _etxtbsy,
    [EXDEV]             = _exdev,
#if EAGAIN != EWOULDBLOCK
    [EWOULDBLOCK]       = _ewouldblock,
#endif
/* ENOTSUP and EOPNOTSUPP do not have the exact same meaning. Still, they
 * have the same numeric value on Linux, breaking POSIX standard */
#if ENOTSUP != EOPNOTSUPP
    [EOPNOTSUPP]        = _eopnotsupp,
#endif
};

const char *tiny_strerror(int errnum)
{
    if (IS_USED(MODULE_TINY_STRERROR_MINIMAL)) {
        static char buf[4];
        snprintf(buf, sizeof(buf), "%d", errnum);
        return buf;
    }

    /* dark magic: All error strings start with a "-". For positive error codes
     * an offset of 1 is added to the address of the string, jumping one char
     * behind the "-". This way the strings do not have to be allocated twice
     * (once with and once without minus char).
     */
    unsigned offset = 1;
    if (errnum <= 0) {
        offset = 0;
        errnum = -errnum;
    }

    if (((unsigned)errnum >= ARRAY_SIZE(lookup))
            || (lookup[(unsigned)errnum] == NULL)) {
        return "unknown";
    }

#if IS_ACTIVE(HAS_FLASH_UTILS_ARCH)
    static char buf[16];
    flash_strncpy(buf, lookup[(unsigned)errnum] + offset, sizeof(buf));
    return buf;
#else
    return lookup[(unsigned)errnum] + offset;
#endif
}

#if IS_USED(MODULE_TINY_STRERROR_AS_STRERROR)
__attribute__((alias("tiny_strerror"))) const char *__wrap_strerror(int errnum);
#endif

/** @} */
