/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef FLASH_UTILS_ARCH_H
#define FLASH_UTILS_ARCH_H

#include <stdio.h>
#include <string.h>
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
#define flash_strcmp strcmp_P
#define flash_strncmp strncmp_P
#define flash_strlen strlen_P
#define flash_strcpy strcpy_P
#define flash_strncpy strncpy_P
#define flash_printf printf_P
/* avrlibc seemingly forgot to provide vprintf_P(), but vfprintf_P() is there */
#define flash_vprintf(fmt, arglist) vfprintf_P(stdout, fmt, arglist)
#define flash_fprintf fprintf_P
#define flash_vfprintf vfprintf_P
#define flash_snprintf snprintf_P
#define flash_vsnprintf vsnprintf_P
#define flash_puts puts_P
#define flash_memcpy memcpy_P

#endif /* Doxygen */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FLASH_UTILS_ARCH_H */
