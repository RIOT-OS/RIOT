/* Copyright (c) 2002,2007 Marek Michalkiewicz
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#ifndef ERRNO_H
#define ERRNO_H

/**
 *  @addtogroup cpu_atmega_common
 *
 *  @{
 *  @file
 *
 *  @code #include <errno.h>@endcode
 *
 *  Some functions in the library set the global variable @c errno when an
 *  error occurs. The file, @c <errno.h>, provides symbolic names for various
 *  error codes.
 *
 *  @warning The @c errno global variable is not safe to use in a threaded or
 *  multi-task system. A race condition can occur if a task is interrupted
 *  between the call which sets @c error and when the task examines @c
 *  errno. If another task changes @c errno during this time, the result will
 *  be incorrect for the interrupted task.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief number of last error
 */
extern int errno;

#ifdef __cplusplus
}
#endif

/**
 * @name    Error numbers
 * @{
 */
#define E2BIG           (1)     /**< Argument list too long. */
#define EACCES          (2)     /**< Permission denied. */
#define EADDRINUSE      (3)     /**< Address in use. */
#define EADDRNOTAVAIL   (4)     /**< Address not available. */
#define EAFNOSUPPORT    (5)     /**< Address family not supported. */
#define EAGAIN          (6)     /**< Resource unavailable, try again (may be the same value as [EWOULDBLOCK]). */
#define EALREADY        (7)     /**< Connection already in progress. */
#define EBADF           (8)     /**< Bad file descriptor. */
#define EBADMSG         (9)     /**< Bad message. */
#define EBUSY           (10)    /**< Device or resource busy. */
#define ECANCELED       (11)    /**< Operation canceled. */
#define ECHILD          (12)    /**< No child processes. */
#define ECONNABORTED    (13)    /**< Connection aborted. */
#define ECONNREFUSED    (14)    /**< Connection refused. */
#define ECONNRESET      (15)    /**< Connection reset. */
#define EDEADLK         (16)    /**< Resource deadlock would occur. */
#define EDESTADDRREQ    (17)    /**< Destination address required. */
#define EMFILE          (18)    /**< File descriptor value too large. */
#define EDQUOT          (19)    /**< Reserved. */
#define EEXIST          (20)    /**< File exists. */
#define EFAULT          (21)    /**< Bad address. */
#define EFBIG           (22)    /**< File too large. */
#define EHOSTUNREACH    (23)    /**< Host is unreachable. */
#define EIDRM           (24)    /**< Identifier removed. */
#define EILSEQ          (25)    /**< Illegal byte sequence. */
#define EINPROGRESS     (26)    /**< Operation in progress. */
#define EINTR           (27)    /**< Interrupted function. */
#define EINVAL          (28)    /**< Invalid argument. */
#define EIO             (29)    /**< I/O error. */
#define EISCONN         (30)    /**< Socket is connected. */
#define EISDIR          (31)    /**< Is a directory. */
#define ELOOP           (32)    /**< Too many levels of symbolic links. */
#define EDOM            (33)    /**< Mathematics argument out of domain of function. */
#define ERANGE          (34)    /**< Result too large. */
#define EMSGSIZE        (35)    /**< Message too large. */
#define EMULTIHOP       (36)    /**< Reserved. */
#define ENAMETOOLONG    (37)    /**< Filename too long. */
#define ENETDOWN        (38)    /**< Network is down. */
#define ENETRESET       (39)    /**< Connection aborted by network. */
#define ENETUNREACH     (40)    /**< Network unreachable. */
#define ENFILE          (41)    /**< Too many files open in system. */
#define ENOBUFS         (42)    /**< No buffer space available. */
#define ENODATA         (43)    /**< [OB XSR] [Option Start] No message is available on the STREAM head read queue. [Option End] */
#define ENODEV          (44)    /**< No such device. */
#define ENOENT          (45)    /**< No such file or directory. */
#define ENOEXEC         (46)    /**< Executable file format error. */
#define ENOLCK          (47)    /**< No locks available. */
#define ENOLINK         (48)    /**< Reserved. */
#define ENOMEM          (49)    /**< Not enough space. */
#define ENOMSG          (50)    /**< No message of the desired type. */
#define ENOPROTOOPT     (51)    /**< Protocol not available. */
#define ENOSPC          (52)    /**< No space left on device. */
#define ENOSR           (53)    /**< [OB XSR] [Option Start] No STREAM resources. [Option End] */
#define ENOSTR          (54)    /**< [OB XSR] [Option Start] Not a STREAM. [Option End] */
#define ENOSYS          (55)    /**< Function not supported. */
#define ENOTCONN        (56)    /**< The socket is not connected. */
#define ENOTDIR         (57)    /**< Not a directory or a symbolic link to a directory. */
#define ENOTEMPTY       (58)    /**< Directory not empty. */
#define ENOTRECOVERABLE (59)    /**< State not recoverable. */
#define ENOTSOCK        (60)    /**< Not a socket. */
#define ENOTSUP         (61)    /**< Not supported (may be the same value as [EOPNOTSUPP]). */
#define ENOTTY          (62)    /**< Inappropriate I/O control operation. */
#define ENXIO           (63)    /**< No such device or address. */
#define EOPNOTSUPP      (64)    /**< Operation not supported on socket (may be the same value as [ENOTSUP]). */
#define EOVERFLOW       (65)    /**< Value too large to be stored in data type. */
#define EOWNERDEAD      (66)    /**< Previous owner died. */
#define EPERM           (67)    /**< Operation not permitted. */
#define EPIPE           (68)    /**< Broken pipe. */
#define EPROTO          (69)    /**< Protocol error. */
#define EPROTONOSUPPORT (70)    /**< Protocol not supported. */
#define EPROTOTYPE      (71)    /**< Protocol wrong type for socket. */
#define EMLINK          (72)    /**< Too many links. */
#define EROFS           (73)    /**< Read-only file system. */
#define ESPIPE          (74)    /**< Invalid seek. */
#define ESRCH           (75)    /**< No such process. */
#define ESTALE          (76)    /**< Reserved. */
#define ETIME           (77)    /**< [OB XSR] [Option Start] Stream ioctl() timeout. [Option End] */
#define ETIMEDOUT       (78)    /**< Connection timed out. */
#define ETXTBSY         (79)    /**< Text file busy. */
#define EWOULDBLOCK     (80)    /**< Operation would block (may be the same value as [EAGAIN]). */
#define EXDEV           (81)    /**< Cross-device link. */
/** @} */

/** @} */

#endif /* ERRNO_H */
