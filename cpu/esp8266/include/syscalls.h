/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Implementation of required system calls
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Necessary initializations of system call functions */
extern void syscalls_init (void);

/** System standard printf function */
extern int printf(const char* format, ...);

/** System standard puts function */
extern int puts(const char * str);

/** Determine free heap size */
extern unsigned int get_free_heap_size (void);

#ifdef __cplusplus
}
#endif

#endif /* SYSCALLS_H */
