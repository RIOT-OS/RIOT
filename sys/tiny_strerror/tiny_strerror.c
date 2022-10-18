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

#include "kernel_defines.h"
#include "tiny_strerror.h"

static const char *lookup[] = {
    [0]                 = "OK",
    [E2BIG]             = "-E2BIG",
    [EACCES]            = "-EACCES",
    [EADDRINUSE]        = "-EADDRINUSE",
    [EADDRNOTAVAIL]     = "-EADDRNOTAVAIL",
    [EAFNOSUPPORT]      = "-EAFNOSUPPORT",
    [EAGAIN]            = "-EAGAIN",
    [EALREADY]          = "-EALREADY",
    [EBADF]             = "-EBADF",
    [EBADMSG]           = "-EBADMSG",
    [EBUSY]             = "-EBUSY",
    [ECANCELED]         = "-ECANCELED",
    [ECHILD]            = "-ECHILD",
    [ECONNABORTED]      = "-ECONNABORTED",
    [ECONNREFUSED]      = "-ECONNREFUSED",
    [ECONNRESET]        = "-ECONNRESET",
    [EDEADLK]           = "-EDEADLK",
    [EDESTADDRREQ]      = "-EDESTADDRREQ",
    [EDOM]              = "-EDOM",
    [EEXIST]            = "-EEXIST",
    [EFAULT]            = "-EFAULT",
    [EFBIG]             = "-EFBIG",
    [EHOSTUNREACH]      = "-EHOSTUNREACH",
    [EILSEQ]            = "-EILSEQ",
    [EINPROGRESS]       = "-EINPROGRESS",
    [EINTR]             = "-EINTR",
    [EINVAL]            = "-EINVAL",
    [EIO]               = "-EIO",
    [EISCONN]           = "-EISCONN",
    [EISDIR]            = "-EISDIR",
    [ELOOP]             = "-ELOOP",
    [EMFILE]            = "-EMFILE",
    [EMLINK]            = "-EMLINK",
    [EMSGSIZE]          = "-EMSGSIZE",
    [ENAMETOOLONG]      = "-ENAMETOOLONG",
    [ENETDOWN]          = "-ENETDOWN",
    [ENETRESET]         = "-ENETRESET",
    [ENETUNREACH]       = "-ENETUNREACH",
    [ENFILE]            = "-ENFILE",
    [ENOBUFS]           = "-ENOBUFS",
    [ENODEV]            = "-ENODEV",
    [ENOENT]            = "-ENOENT",
    [ENOEXEC]           = "-ENOEXEC",
    [ENOLCK]            = "-ENOLCK",
    [ENOMEM]            = "-ENOMEM",
    [ENOMSG]            = "-ENOMSG",
    [ENOPROTOOPT]       = "-ENOPROTOOPT",
    [ENOSPC]            = "-ENOSPC",
    [ENOSYS]            = "-ENOSYS",
    [ENOTCONN]          = "-ENOTCONN",
    [ENOTDIR]           = "-ENOTDIR",
    [ENOTEMPTY]         = "-ENOTEMPTY",
    [ENOTSOCK]          = "-ENOTSOCK",
    [ENOTSUP]           = "-ENOTSUP",
    [ENOTTY]            = "-ENOTTY",
    [ENXIO]             = "-ENXIO",
    [EPERM]             = "-EPERM",
    [EPIPE]             = "-EPIPE",
    [EPROTONOSUPPORT]   = "-EPROTONOSUPPORT",
    [EPROTOTYPE]        = "-EPROTOTYPE",
    [ERANGE]            = "-ERANGE",
    [EROFS]             = "-EROFS",
    [ESPIPE]            = "-ESPIPE",
    [ESRCH]             = "-ESRCH",
    [ETIMEDOUT]         = "-ETIMEDOUT",
    [EXDEV]             = "-EXDEV",
    /* EAGAIN and EWOULDBLOCK have the exact same meaning and consequently may
     * have the same numeric value */
#if EAGAIN != EWOULDBLOCK
    [EWOULDBLOCK]       = "-EWOULDBLOCK",
#endif
    /* ENOTSUP and EOPNOTSUPP do not have the exact same meaning. Still, they
     * have the same numeric value on Linux, breaking POSIX standard */
#if ENOTSUP != EOPNOTSUPP
    [EOPNOTSUPP]        = "-EOPNOTSUPP",
#endif
};

const char *tiny_strerror(int errnum)
{
    /* dark magic: All error strings start with a "-". For positive error codes
     * an offset of 1 is added to the address of the string, jumping one char
     * behind the "-". This way the strings do not have to be allocated twice
     * (once with and once without minus char).
     */
    const char *retval = "-unknown";
    unsigned offset = 1;

    if (IS_USED(MODULE_TINY_STRERROR_MINIMAL)) {
        static char buf[4];
        snprintf(buf, sizeof(buf), "%d", errnum);
        return buf;
    }

    if (errnum <= 0) {
        offset = 0;
        errnum = -errnum;
    }

    if (((unsigned)errnum < ARRAY_SIZE(lookup))
            && (lookup[(unsigned)errnum] != NULL)) {
        retval = lookup[(unsigned)errnum];
    }

    return retval + offset;
}

#if IS_USED(MODULE_TINY_STRERROR_AS_STRERROR)
__attribute__((alias("tiny_strerror"))) const char *__wrap_strerror(int errnum);
#endif

/** @} */
