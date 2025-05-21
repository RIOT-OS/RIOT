/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_avr8_common
 * @{
 *
 * @file
 * @brief       Implementation of flash_utils
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 */


#include <stdio.h>
#include <stdarg.h>
#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

/* this is documented in sys/include/flash_utils.h - let's not confuse
 * Doxygen */
#ifndef DOXYGEN

#define FLASH_ATTR __flash
#define PRIsflash "S"
#define TO_FLASH(x) __extension__({static FLASH_ATTR const char __c[] = (x); &__c[0];})

static inline int flash_strcmp(const char *ram, FLASH_ATTR const char *flash)
{
    return strcmp_P(ram, (const char *)flash);
}

static inline int flash_strncmp(const char *ram, FLASH_ATTR const char *flash, size_t n)
{
    return strncmp_P(ram, (const char *)flash, n);
}

static inline size_t flash_strlen(FLASH_ATTR const char *flash)
{
    return strlen_P((const char *)flash);
}

static inline char * flash_strcpy(char *ram, FLASH_ATTR const char *flash)
{
    return strcpy_P(ram, (const char *)flash);
}

static inline char * flash_strncpy(char *ram, FLASH_ATTR const char *flash, size_t n)
{
    return strncpy_P(ram, (const char *)flash, n);
}


static inline int flash_vprintf(FLASH_ATTR const char *flash, va_list args)
{
    /* vprintf_P() is not provided by avr-libc. But vfprintf_P() with
     * stdout as stream can be used to implement it */
    return vfprintf_P(stdout, (const char *)flash, args);
}

static inline int flash_vfprintf(FILE *stream, FLASH_ATTR const char *flash,
                                 va_list args)
{
    return vfprintf_P(stream, (const char *)flash, args);
}

static inline int flash_vsnprintf(char *buf, size_t buf_len,
                                  FLASH_ATTR const char *flash, va_list args)
{
    return vsnprintf_P(buf, buf_len, (const char *)flash, args);
}

static inline void flash_puts(FLASH_ATTR const char *flash)
{
    puts_P((const char *)flash);
}

static inline void * flash_memcpy(void *dest, FLASH_ATTR const void *src,
                                  size_t n)
{
    return memcpy_P(dest, (const void *)src, n);
}

/* aliases need to be provided by the linker, as passing through va-args is
 * not possible */
int flash_printf(FLASH_ATTR const char *flash, ...);
int flash_fprintf(FILE *stream, FLASH_ATTR const char *flash, ...);
int flash_snprintf(char *buf, size_t buf_len, FLASH_ATTR const char *flash, ...);

#endif /* Doxygen */

#ifdef __cplusplus
}
#endif

/** @} */
