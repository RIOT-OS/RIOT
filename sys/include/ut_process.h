/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup sys_ut_process URI template processor
 * @ingroup  sys
 * @brief   URI template processor
 *
 * A parser and processor for URI templates up to level 3 expression types
 * according to [RFC 6570](https://tools.ietf.org/html/rfc6570).
 *
 * @see [RFC 6570](https://tools.ietf.org/html/rfc6570)
 *
 * @{
 *
 * @file
 * @brief   URI template processor definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef UT_PROCESS_H
#define UT_PROCESS_H

#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Name-value-pair of a variable for URI template expansion
 */
typedef struct {
    const char *name;       /**< name of the variable */
    const char *value;      /**< value for the variable */
} ut_process_var_t;

/**
 * @brief   Expands a URI template by a given value set
 *
 * @pre `(vars_len == 0) || (vars != NULL)`
 * @pre `(uri != NULL) && (uri_len > 0)`
 *
 * @param[in] ut            A URI template.
 * @param[in] ut_len        Length of @p ut.
 * @param[in] vars          A set of variable-value pairs.
 * @param[in] vars_len      The length of @p vars.
 * @param[out] uri          The resulting URI.
 * @param[in, out] uri_len  The maximum length for @p uri on in, the actual
 *                          length of @p uri on out.
 *
 * @return  The length of @p uri on success
 * @return  -EINVAL, when @p ut is not parseable.
 * @return  -ENOBUFS, when @p uri_len is too small to fit the resulting URI.
 *          Potentially broken data will be written to @p uri.
 */
int ut_process_expand(const char *ut, size_t ut_len,
                      const ut_process_var_t *vars, size_t vars_len,
                      char *uri, size_t *uri_len);

/**
 * @brief   Expands a URI template by a given value set
 *
 * @pre `(vars_len == 0) || (vars != NULL)`
 * @pre `(uri != NULL) && (uri_len > 0)`
 *
 * @param[in] ut            A `\0`-terminated URI template.
 * @param[in] vars          A set of variable-value pairs.
 * @param[in] vars_len      The length of @p vars.
 * @param[out] uri          The resulting URI.
 * @param[in, out] uri_len  The maximum length for @p uri on in, the actual
 *                          length of @p uri on out.
 *
 * @return  The length of @p uri on success
 * @return  -EINVAL, when @p ut is not parseable.
 * @return  -ENOENT, when any ut_process_var_t::name in @p vars contains
 *          an invalid character.
 * @return  -ENOBUFS, when @p uri_len is too small to fit the resulting URI.
 *          A truncated version of the resulting URI will then be stored in
 *          @p uri.
 */
static inline int ut_process_str_expand(const char *ut,
                                        const ut_process_var_t *vars,
                                        size_t vars_len,
                                        char *uri, size_t *uri_len)
{
    return ut_process_expand(ut, strlen(ut), vars, vars_len, uri, uri_len);
}

#ifdef __cplusplus
}
#endif

#endif /* UT_PROCESS_H */
/** @} */
