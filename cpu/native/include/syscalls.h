/*
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

#include <sys/time.h>
#include <stdint.h>
#include <ifaddrs.h>
#include <stdio.h>

/**
 * @addtogroup cpu_native
 * @{
 */

#ifndef DOXYGEN
#  if NATIVE_SYSCALLS_DEFINITION
#    define __SPECIFIER
#  else
#    define __SPECIFIER extern
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - System call wrappers */
/** @cond */
/**
 * @name System call wrappers
 *
 * Internally, these function pointers are assigned their implementation in the standard library.
 * We wrap system calls and syscall-invoking library calls to ensure **no context switches occur during a system call**.
 * @{
 */
__SPECIFIER ssize_t (*real_read)(int fd, void *buf, size_t count);
__SPECIFIER ssize_t (*real_write)(int fd, const void *buf, size_t count);
__SPECIFIER size_t (*real_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
__SPECIFIER ssize_t (*real_recv)(int sockfd, void *buf, size_t len, int flags);
__SPECIFIER void (*real_clearerr)(FILE *stream);
__SPECIFIER __attribute__((noreturn)) void (*real_exit)(int status);
__SPECIFIER void (*real_free)(void *ptr);
__SPECIFIER void* (*real_malloc)(size_t size);
__SPECIFIER void* (*real_calloc)(size_t nmemb, size_t size);
__SPECIFIER void* (*real_realloc)(void *ptr, size_t size);
__SPECIFIER void (*real_freeaddrinfo)(struct addrinfo *res);
__SPECIFIER void (*real_freeifaddrs)(struct ifaddrs *ifa);
__SPECIFIER void (*real_srandom)(unsigned int seed);
__SPECIFIER int (*real_accept)(int socket, ...);
__SPECIFIER int (*real_bind)(int socket, ...);
__SPECIFIER int (*real_connect)(int socket, ...);
__SPECIFIER int (*real_printf)(const char *format, ...);
__SPECIFIER int (*real_getaddrinfo)(const char *node, ...);
__SPECIFIER int (*real_getifaddrs)(struct ifaddrs **ifap);
__SPECIFIER int (*real_gettimeofday)(struct timeval *t, ...);
__SPECIFIER int (*real_getpid)(void);
__SPECIFIER int (*real_chdir)(const char *path);
__SPECIFIER int (*real_close)(int);
__SPECIFIER int (*real_fcntl)(int, int, ...);
__SPECIFIER int (*real_creat)(const char *path, ...);
__SPECIFIER int (*real_dup2)(int, int);
__SPECIFIER int (*real_execve)(const char *, char *const[], char *const[]);
__SPECIFIER int (*real_fork)(void);
__SPECIFIER int (*real_feof)(FILE *stream);
__SPECIFIER int (*real_ferror)(FILE *stream);
__SPECIFIER int (*real_listen)(int socket, int backlog);
__SPECIFIER int (*real_ioctl)(int fildes, unsigned long request, ...);
__SPECIFIER int (*real_open)(const char *path, int oflag, ...);
__SPECIFIER int (*real_pause)(void);
__SPECIFIER int (*real_pipe)(int[2]);
__SPECIFIER int (*real_select)(int nfds, ...);
__SPECIFIER int (*real_poll)(struct pollfd *fds, ...);
__SPECIFIER int (*real_setsid)(void);
__SPECIFIER int (*real_setsockopt)(int socket, ...);
__SPECIFIER int (*real_socket)(int domain, int type, int protocol);
__SPECIFIER int (*real_unlink)(const char *);
__SPECIFIER long int (*real_random)(void);
__SPECIFIER const char* (*real_gai_strerror)(int errcode);
__SPECIFIER FILE* (*real_fopen)(const char *path, const char *mode);
__SPECIFIER int (*real_fclose)(FILE *stream);
__SPECIFIER int (*real_fseek)(FILE *stream, long offset, int whence);
__SPECIFIER long (*real_ftell)(FILE *stream);
__SPECIFIER int (*real_fputc)(int c, FILE *stream);
__SPECIFIER int (*real_fgetc)(FILE *stream);
__SPECIFIER mode_t (*real_umask)(mode_t cmask);
__SPECIFIER ssize_t (*real_writev)(int fildes, const struct iovec *iov, int iovcnt);
__SPECIFIER ssize_t (*real_send)(int sockfd, const void *buf, size_t len, int flags);
__SPECIFIER off_t (*real_lseek)(int fd, off_t offset, int whence);
__SPECIFIER off_t (*real_fstat)(int fd, struct stat *statbuf);
__SPECIFIER int (*real_fsync)(int fd);
__SPECIFIER int (*real_mkdir)(const char *pathname, mode_t mode);
__SPECIFIER int (*real_rmdir)(const char *pathname);
__SPECIFIER DIR *(*real_opendir)(const char *name);
__SPECIFIER struct dirent *(*real_readdir)(DIR *dirp);
__SPECIFIER int (*real_closedir)(DIR *dirp);
__SPECIFIER int (*real_rename)(const char *, const char *);
__SPECIFIER int (*real_statvfs)(const char *restrict path, struct statvfs *restrict buf);
/** @}*/
/** @endcond */

#ifdef __cplusplus
}
#endif

#if !defined(DOXYGEN)
# undef __SPECIFIER
#endif

/** @} */
