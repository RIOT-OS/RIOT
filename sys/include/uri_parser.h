/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef URI_PARSER_H
#define URI_PARSER_H

/**
 * @defgroup    sys_uri_parser A minimal, non-destructive URI parser
 * @ingroup     sys
 * @brief       A minimal, non-destructive parser for URI references
 *
 * This module acts on URI references, and thus process both relative
 * references and URIs.
 *
 * Note that fragment identifiers are not handled by the implementation.
 *
 * @see https://tools.ietf.org/html/rfc3986
 *
 * @{
 *
 * @brief       Handler functions for uri_parser
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 *
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief container that holds all results
 */
typedef struct {
    const char *scheme;             /**< scheme */
    const char *userinfo;           /**< userinfo */

    /**
     * @brief host part
     *
     * @note for IPv6 addresses, @ref host also includes the brackets
     * '[' and ']' as well as the zoneid (with leading '%'), if
     * present.
     */
    const char *host;

    /**
     * @brief Pointer to the start of the address, if @ref host is an
     * IPv6 address and NULL otherwise
     *
     * @note @ref ipv6addr does not include the brackets '[' and ']'
     * and the zoneid part.
     */
    const char *ipv6addr;

    /**
     * @brief zoneid if @ref host is IPv6 address, NULL otherwise
     *
     * @see https://tools.ietf.org/html/rfc6874
     */
    const char *zoneid;

    const char *port_str;           /**< port as str */
    const char *path;               /**< path */
    const char *query;              /**< query */
    uint16_t port;                  /**< port as uint16_t */
    uint16_t scheme_len;            /**< length of @ref scheme */
    uint16_t userinfo_len;          /**< length of @ref userinfo */
    uint16_t host_len;              /**< length of @ref host */
    uint16_t ipv6addr_len;          /**< length of @ref ipv6addr */
    uint16_t zoneid_len;            /**< length of @ref zoneid */
    uint16_t port_str_len;          /**< length of @ref port_str */
    uint16_t path_len;              /**< length of @ref path */
    uint16_t query_len;             /**< length of @ref query */
} uri_parser_result_t;

/**
 * @brief   Container to represent a query parameter
 */
typedef struct {
    const char *name;               /**< name of the query parameter */
    const char *value;              /**< value of the query parameter */
    uint16_t name_len;              /**< length of @ref name */
    uint16_t value_len;             /**< length of @ref value */
} uri_parser_query_param_t;

/**
 * @brief Checks whether @p uri is in absolute form
 *
 * @param[in]   uri       URI reference to check. Must not be `NULL`
 * @param[in]   uri_len   Length of @p uri
 *
 * @pre `uri != NULL`
 *
 * @return      true      if @p uri is a URI
 * @return      false     if @p uri is a relative reference
 */
bool uri_parser_is_absolute(const char *uri, size_t uri_len);

/**
 * @brief Checks whether @p uri is in absolute form
 *
 * @param[in]   uri       Zero-terminated URI reference to check. Must not be `Null`
 *
 * @pre `uri != NULL`
 *
 * @return      true      if @p uri is a URI
 * @return      false     if @p uri is a relative reference
 */
bool uri_parser_is_absolute_string(const char *uri);

/**
 * @brief Parse a URI reference
 *
 * @param[out]  result    pointer to a container that will hold the result
 * @param[in]   uri       URI to parse. Must not be `NULL`
 * @param[in]   uri_len   Length of @p uri
 *
 * @pre `uri != NULL`
 *
 * @return      0         on success
 * @return      -1        on parsing error
 */
int uri_parser_process(uri_parser_result_t *result, const char *uri,
                       size_t uri_len);

/**
 * @brief Parse a URI reference
 *
 * @param[out]  result    pointer to a container that will hold the result
 * @param[in]   uri       Zero-terminated URI to parse. Must not be `NULL`
 *
 * @pre `uri != NULL`
 *
 * @return      0         on success
 * @return      -1        on parsing error
 */
int uri_parser_process_string(uri_parser_result_t *result, const char *uri);

/**
 * @brief   Provides a list of URI query parameters from a given URI parser
 *          result.
 *
 * @note    The function **DOES NOT** check for duplicate query parameters.
 *
 * @pre `uri_parsed != NULL`
 * @pre `params != NULL` and all its elements are set to zero.
 *
 * @param[in] uri_parsed   A parsed URI result. Must not be NULL.
 * @param[out] params      An array of @ref uri_parser_query_param_t.
 *                         Must not be NULL and all zero-valued on call. Will be
 *                         filled with the name-value-pairs in
 *                         uri_parser_result_t::query of @p uri_parsed. If the
 *                         number of query parameters in @p uri_parsed exceeds
 *                         @p params_len, the list will be truncated and the
 *                         function returns -2.
 * @param[in] params_len   The length of @p params
 *
 * @return  number of filled entries in @p params on success. Might be 0 if
 *          uri_parser_result_t::query is NULL.
 * @return  -1 on parsing error.
 * @return  -2 when the number of query parameters exceeds @p params_len.
 *          In that case, the array is filled with the first @p params_len
 *          name-value-pairs in uri_parser_result_t::query of @p uri_parsed.
 */
int uri_parser_split_query(const uri_parser_result_t *uri_parsed,
                           uri_parser_query_param_t *params,
                           size_t params_len);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* URI_PARSER_H */
