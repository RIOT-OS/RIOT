/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
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

#ifndef URI_PARSER_H
#define URI_PARSER_H

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
    char *scheme;                 /**< scheme */
    char *userinfo;               /**< userinfo */
    char *host;                   /**< host */
    char *port;                   /**< port */
    char *path;                   /**< path */
    char *query;                  /**< query */
    uint16_t scheme_len;          /**< length of @p scheme */
    uint16_t userinfo_len;        /**< length of @p userinfo */
    uint16_t host_len;            /**< length of @p host */
    uint16_t port_len;            /**< length of @p port */
    uint16_t path_len;            /**< length of @p path */
    uint16_t query_len;           /**< length of @p query */
} uri_parser_result_t;

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

#ifdef __cplusplus
}
#endif

#endif /* URI_PARSER_H */
/** @} */
