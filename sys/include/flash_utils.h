/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_flash_utils Utility functions, macros, and types for
 *                              read-only memory
 * @ingroup     sys
 *
 * This modules adds utility functions, macros, and functions for read-only
 * memory. The goal is to hide the differences between modified architectures
 * that map flash into the data address space (e.g. ARM) and those which
 * doesn't (e.g. most AVR, Xtensa).
 *
 * # Usage
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * #include "flash_utils.h"
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * No module needs to be selected, this is a header-only implementation that
 * is always available.
 *
 * # Porting Code to Use `flash_utils`
 *
 * This is mainly targeting applications developers to ease developing apps
 * that work well on both legacy modified Harvard architectures (e.g. ATmega)
 * and modern modified Harvard architectures (e.g. ARM, ATtiny, ...) as well
 * as von-Neumann machines.
 *
 * The intention is to limit in-tree use to a very small number of modules that
 * yield the most "bang for the buck" and not leak the use of `flash_utils`
 * through the API. Specifically, reverting to not using `flash_utils` should
 * not be noticed by any user (unless looking at memory consumption).
 *
 * @{
 *
 * @file
 * @brief       Utility functions, macros, and types for read-only memory
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef FLASH_UTILS_H
#define FLASH_UTILS_H

#include <stdio.h>
#include <string.h>

#include "cpu_conf.h"
#include "kernel_defines.h"

#if IS_ACTIVE(HAS_FLASH_UTILS_ARCH)
#include "flash_utils_arch.h" /* IWYU pragma: export */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DOXYGEN)
/**
 * @brief   C type qualifier required to place a variable in flash
 */
#define FLASH_ATTR <IMPLEMTATION_DEFINED>
/**
 * @brief   Format specifier for printing `FLASH CONST char *`
 *
 * Usage:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *     FLASH_ATTR const char fmt[] = "I am printing \"%" PRIsflash "\" from flash\n";
 *     FLASH_ATTR const char msg[] = "message from flash";
 *     flash_printf(fmt, msg);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define PRIsflash <IMPLEMTATION_DEFINED>

/**
 * @brief   Macro to allocate a string literal on flash and return a
 *          `FLASH_ATTR const char *` pointer to it
 * Usage:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *     flash_puts(TO_FLASH("Hello world"));
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define TO_FLASH(str_literal) <IMPLEMTATION_DEFINED>

/**
 * @brief   Like `strcmp()`, but the second string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `strcmp()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
int flash_strcmp(const char *ram, FLASH_ATTR const char *flash);

/**
 * @brief   Like `strncmp()`, but the first string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `strncmp()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
int flash_strncmp(const char *ram, FLASH_ATTR const char *flash, size_t n);

/**
 * @brief   Like `strlen()`, but the string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `strlen()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
size_t flash_strlen(FLASH_ATTR const char *flash);

/**
 * @brief   Like `strcpy()`, but the source flash resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `strcpy()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
char * flash_strcpy(char *ram, FLASH_ATTR const char *flash);

/**
 * @brief   Like `strncpy()`, but the source flash resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `strncpy()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
char * flash_strncpy(char *ram, FLASH_ATTR const char *flash, size_t n);

/**
 * @brief   Like `printf()`, but the format string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `printf()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
int flash_printf(FLASH_ATTR const char *flash, ...);

/**
 * @brief   Like `vprintf()`, but the format string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `vprintf()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
int flash_vprintf(FLASH_ATTR const char *flash, va_list args);

/**
 * @brief   Like `fprintf()`, but the format string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `fprintf()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
int flash_fprintf(FILE *stream, FLASH_ATTR const char *flash, ...);

/**
 * @brief   Like `vfprintf()`, but the format string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `vfprintf()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
int flash_vfprintf(FILE *stream, FLASH_ATTR const char *flash, va_list args);

/**
 * @brief   Like `snprintf()`, but the format string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `snprintf()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
int flash_snprintf(char *buf, size_t buf_len, FLASH_ATTR const char *flash, ...);

/**
 * @brief   Like `vsnprintf()`, but the format string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `vsnprintf()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
int flash_vsnprintf(char *buf, size_t buf_len, FLASH_ATTR const char *flash,
                    va_list args);

/**
 * @brief   Like `puts()`, but the string resides in flash
 *
 * @details This will be a zero-overhead wrapper on top of `puts()` for
 *          von-Neumann architectures or Harvard architectures that also map
 *          their flash into the data address space.
 */
void flash_puts(FLASH_ATTR const char *flash);

/**
 * @brief   Like `memcpy()`, but @p src resides in flash
 *
 * @param[out]  dest    buffer to copy into
 * @param[in]   src     flash data to copy
 * @param[in]   n       number of bytes to copy
 *
 */
void * flash_memcpy(void *dest, FLASH_ATTR const void *src, size_t n);
#elif !IS_ACTIVE(HAS_FLASH_UTILS_ARCH)
#  define FLASH_ATTR
#  define PRIsflash "s"
#  define ASSERT_IS_STR_LITERAL_HELPER(x) x
#  define ASSERT_IS_STR_LITERAL(x) ASSERT_IS_STR_LITERAL_HELPER("" x)
#  define TO_FLASH(x) ASSERT_IS_STR_LITERAL(x)
#  define flash_strcmp strcmp
#  define flash_strncmp strncmp
#  define flash_strlen strlen
#  define flash_strcpy strcpy
#  define flash_strncpy strncpy
#  define flash_printf printf
#  define flash_vprintf vprintf
#  define flash_fprintf fprintf
#  define flash_vfprintf vfprintf
#  define flash_snprintf snprintf
#  define flash_vsnprintf vsnprintf
#  define flash_puts puts
#  define flash_memcpy memcpy
#endif

/**
 * @brief   A convenience wrapper for `flash_puts(TO_FLASH("str literal"))`
 *
 * Usage:
 * ```
 * FLASH_PUTS("Hello world!");
 * ```
 */
#define FLASH_PUTS(x) flash_puts(TO_FLASH(x))

/**
 * @brief   Like @ref flash_puts but without line break
 */
static inline void flash_print_str(FLASH_ATTR const char *flash)
{
    printf("%" PRIsflash, flash);
}

#ifdef __cplusplus
}
#endif

#endif /* FLASH_UTILS_H */
/** @} */
