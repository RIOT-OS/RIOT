/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_posix
 * @{
 *
 * @file    strings.h
 * @brief   string operations
 *
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/strings.h.html">
 *              The Open Group Base Specifications Issue 7, <strings.h>
 *          </a>
 *
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef STRINGS_H
#define STRINGS_H

#include <sys/types.h>
#include <stddef.h>

/**
 * @brief   Returns the position of the first (least significant) bit set in
 *          integer *i*, or 0 if no bits are set in i.
 *
 * @param[in] i    an integer number.
 *
 * @return  Position (1 for least significant bit) of the first bit set,
 *          0 if no bits are set.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/ffs.html">
 *          The Open Group Base Specification Issue 7, ffs
 *      </a>
 */
#define ffs(i) __builtin_ffs(i)

/**
 * @brief   Compares two strings *s1* and *s2*, ignoring the case of the
 *          characters for not more than n bytes from the strings.
 *
 * @param[in] s1    a string.
 * @param[in] s2    another string.
 * @param[in] n     number of bytes to be compared
 *
 * @return  A value greater 0 if, ignoring the case of the character, *s1* is
 *          greater than *s2* up to n bytes, less than 0 if smaller, and 0 if
 *          equal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcasecmp.html">
 *          The Open Group Base Specification Issue 7, strcasecmp
 *      </a>
 */
int strncasecmp(const char *s1, const char *s2, size_t n);

/**
 * @brief   Compares two strings *s1* and *s2*, ignoring the case of the
 *          characters for not more than n bytes from the strings, but using
 *          the current locale settings to determine the case.
 *
 * @note    Since RIOT does not support locales (yet), this is just an
 *          alias for strncasecmp()
 *
 * @param[in] s1    a string.
 * @param[in] s2    another string.
 * @param[in] n     number of bytes to be compared
 *
 * @return  A value greater 0 if, ignoring the case of the character, *s1* is
 *          greater than *s2* up to n bytes, less than 0 if smaller, and 0 if
 *          equal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcasecmp.html">
 *          The Open Group Base Specification Issue 7, strcasecmp
 *      </a>
 */
#define strncasecmp_l(s1, s2, n, l) (strncasecmp(s1, s2, n))

/**
 * @brief   Compares two strings *s1* and *s2*, ignoring the case of the characters.
 *
 * @param[in] s1    a string.
 * @param[in] s2    another string.
 *
 * @return  A value greater 0 if, ignoring the case of the character, *s1* is
 *          greater than *s2*, less than 0 if smaller, and 0 if equal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcasecmp.html">
 *          The Open Group Base Specification Issue 7, strcasecmp
 *      </a>
 */
#define strcasecmp(s1, s2) strncasecmp(s1, s2, -1)

/**
 * @brief   Compares two strings *s1* and *s2*, ignoring the case of the
 *          characters, but using the current locale settings to determine the
 *          case.
 *
 * @note    Since RIOT does not support locales (yet), this is just an
 *          alias for strcasecmp()
 *
 * @param[in] s1    a string.
 * @param[in] s2    another string.
 *
 * @return  A value greater 0 if, ignoring the case of the character, *s1* is
 *          greater than *s2*, less than 0 if smaller, and 0 if equal
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcasecmp.html">
 *          The Open Group Base Specification Issue 7, strcasecmp
 *      </a>
 */
#define strcasecmp_l(s1, s2, l) strncasecmp(s1, s2, -1)

#endif /* STRINGS_H */
