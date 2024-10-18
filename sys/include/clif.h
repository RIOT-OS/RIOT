/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_clif CoRE Link Format
 * @ingroup     sys_serialization
 * @brief       Simple encoding and decoding of CoRE Link Format (RFC 6690)
 *              strings
 * @see RFC 6690: https://tools.ietf.org/html/rfc6690
 *
 * clif provides a high-level API for CoRE Link Format encoding and decoding of
 * links, through the @ref clif_encode_link and @ref clif_decode_link
 * respectively.
 *
 * The high-level API is built on top of low-level functions provided by clif,
 * such as @ref clif_add_target, @ref clif_add_attr, and @ref clif_get_attr.
 * Also, some convenience functions like @ref clif_get_attr_type,
 * @ref clif_attr_type_to_str and @ref clif_init_attr are provided, to
 * facilitate the work with links.
 *
 * ### Decoding
 * @ref clif_decode_link takes a buffer which contains an encoded link and
 * returns the information of it in a @ref clif_t structure and each attribute
 * in a @ref clif_attr_t structure of a given array.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * // A buffer 'input_buf' of length 'input_len' contains the links to decode
 * clif_attr_t out_attrs[ATTRS_NUM];
 * clif_t out_links[LINKS_NUM];
 *
 * const char *pos = input_buf;
 * unsigned links_numof = 0;
 * unsigned attrs_numof = 0;
 *
 * while (pos < &input_buf[input_len]) {
 *      ssize_t res = clif_decode_link(&out_links[links_numof],
 *                                     &out_attrs[attrs_numof],
 *                                     ATTRS_NUM - attrs_numof, pos,
 *                                     &input_buf[input_len]- pos);
 *      if (res < 0) {
 *          break;
 *      }
 *      pos += res;
 *      attrs_numof += out_links[links_numof].attrs_len;
 *      links_numof++;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ### Encoding
 * @ref clif_encode_link encodes a given link into a buffer, it can be called
 * with a NULL pointer, in that case it will only calculate the amount of bytes
 * needed to encode the link. After every call to this function a separator
 * needs to be added to the buffer.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * // A 'links' array of clif_t, of length 'links_len' contains the links to encode
 * char out[OUT_SIZE];
 * ssize_t res;
 * size_t pos = 0;
 *
 * for (unsigned i = 0; i < links_len; i++) {
 *      if (i) {
 *          res = clif_add_link_separator(&out[pos], OUT_SIZE - pos);
 *          if (res <= 0) {
 *              break;
 *          }
 *          pos += res;
 *      }
 *
 *      res = clif_encode_link(&links[i], &out[pos], OUT_SIZE - pos);
 *      if (res <= 0) {
 *          break;
 *      }
 *      pos += res;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note 'attribute', in this module, extends to the term 'link-param' on the
 *       ABNF in [section 2 of RFC 6690](https://tools.ietf.org/html/rfc6690#section-2).
 *
 * @{
 *
 * @file
 * @brief       CoRE Link Format encoding and decoding library public
 *              definitions
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef CLIF_H
#define CLIF_H

#include <sys/types.h>

#include "clif_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return types for the @ref sys_clif API
 */
enum {
    CLIF_OK          = 0,    /**< success */
    CLIF_NO_SPACE    = -1,   /**< not enough space in the buffer */
    CLIF_NOT_FOUND   = -2    /**< could not find a component in a buffer */
};

/**
 * @brief Types of link format attributes
 */
typedef enum {
    CLIF_ATTR_ANCHOR        = 0,  /**< anchor */
    CLIF_ATTR_REL           = 1,  /**< rel */
    CLIF_ATTR_LANG          = 2,  /**< hreflang */
    CLIF_ATTR_MEDIA         = 3,  /**< media */
    CLIF_ATTR_TITLE         = 4,  /**< title */
    CLIF_ATTR_TITLE_EXT     = 5,  /**< title* */
    CLIF_ATTR_TYPE          = 6,  /**< type */
    CLIF_ATTR_RT            = 7,  /**< rt */
    CLIF_ATTR_IF            = 8,  /**< if */
    CLIF_ATTR_SZ            = 9,  /**< sz */
    CLIF_ATTR_CT            = 10, /**< ct */
    CLIF_ATTR_OBS           = 11, /**< obs */
    CLIF_ATTR_EXT           = 12  /**< extensions */
} clif_attr_type_t;

/**
 * @brief Link format attribute descriptor
 */
typedef struct {
    char *value;                   /**< string with the value */
    unsigned value_len;            /**< length of the value */
    const char *key;               /**< attribute name */
    unsigned key_len;              /**< length of the attribute name */
} clif_attr_t;

/**
 * @brief Link format descriptor
 */
typedef struct {
    char *target;                /**< target string */
    unsigned target_len;         /**< length of target string */
    clif_attr_t *attrs;          /**< array of attributes */
    unsigned attrs_len;          /**< size of array of attributes */
} clif_t;

/**
 * @brief Encodes a given link in link format into a given buffer
 *
 * @pre `link != NULL`
 *
 * @param[in] link      link to encode.Must not be NULL.
 * @param[out] buf      buffer to output the encoded link. Can be NULL
 * @param[in] maxlen    size of @p buf
 *
 * @note If @p buf is NULL this will return the amount of bytes that would be
 *       needed
 *
 * @return amount of bytes used from @p buf in success
 * @return CLIF_NO_SPACE if there is not enough space in the buffer
 */
ssize_t clif_encode_link(const clif_t *link, char *buf, size_t maxlen);

/**
 * @brief   Decodes a string of link format. It decodes the first occurrence of
 *          a link.
 *
 * @pre `(link != NULL) && (buf != NULL)`
 *
 * @param[out] link         link to populate. Must not be NULL.
 * @param[out] attrs        array of attrs to populate
 * @param[in]  attrs_len    length of @p attrs
 * @param[in]  buf          string to decode. Must not be NULL.
 * @param[in]  maxlen       size of @p buf
 *
 * @return number of bytes parsed from @p buf in success
 * @return CLIF_NOT_FOUND if the string is malformed
 */
ssize_t clif_decode_link(clif_t *link, clif_attr_t *attrs, unsigned attrs_len,
                         const char *buf, size_t maxlen);

/**
 * @brief   Adds a given @p target to a given buffer @p buf using link format
 *
 * @pre `target != NULL`
 *
 * @param[in]  target       buffer containing the path to the resource.
 *                          Must not be NULL.
 * @param[in]  target_len   size of @p target
 * @param[out] buf          buffer to output the formatted path. Can be NULL
 * @param[in]  maxlen       size of @p buf
 *
 * @note If @p buf is NULL this will return the amount of bytes that would be
 *       needed
 *
 * @return in success the amount of bytes used in the buffer
 * @return CLIF_NO_SPACE if there is not enough space in the buffer
 */
ssize_t clif_add_target_from_buffer(const char *target, size_t target_len,
                                    char *buf, size_t maxlen);

/**
 * @brief   Adds a given @p target to a given buffer @p buf using link format
 *
 * @pre `target != NULL`
 *
 * @param[in]  target  zero terminated string containing the path to the resource.
 *                     Must not be NULL.
 * @param[out] buf     buffer to output the formatted path. Can be NULL
 * @param[in]  maxlen  size of @p buf
 *
 * @note If @p buf is NULL this will return the amount of bytes that would be
 *       needed
 *
 * @return in success the amount of bytes used in the buffer
 * @return CLIF_NO_SPACE if there is not enough space in the buffer
 */
ssize_t clif_add_target(const char *target, char *buf, size_t maxlen);

/**
 * @brief   Adds a given @p attr to a given buffer @p buf using link format
 *
 * @pre `(attr != NULL) && (attr->key != NULL)`
 *
 * @param[in]  attr      pointer to the attribute to add. Must not be NULL, and
 *                       must contain a key.
 * @param[out] buf       buffer to add the attribute to. Can be NULL
 * @param[in]  maxlen    size of @p buf
 *
 * @note
 *       - If @p buf is NULL this will return the amount of bytes that would be
 *         needed.
 *       - The length of the key must be set in `attr->key_len`.
 *
 * @return amount of bytes used from the buffer if successful
 * @return CLIF_NO_SPACE if there is not enough space in the buffer
 */
ssize_t clif_add_attr(clif_attr_t *attr, char *buf, size_t maxlen);

/**
 * @brief   Adds the link separator character to a given @p buf, using link
 *          format
 *
 * @param[out] buf       buffer to add the separator to. Can be NULL
 * @param[in]  maxlen    size of @p buf
 *
 * @note If @p buf is NULL this will return the amount of bytes that would be
 *       needed
 *
 * @return amount of bytes used from buffer if successful
 * @return CLIF_NO_SPACE if there is not enough space in the buffer
 */
ssize_t clif_add_link_separator(char *buf, size_t maxlen);

/**
 * @brief   Looks for a the target URI of a given link.
 *
 * @pre `input != NULL`
 *
 * @param[in]  input        string where to look for the target. It should only
 *                          be ONE link. Must not be NULL.
 * @param[in]  input_len    length of @p input.
 * @param[out] output       if a target is found this will point to the
 *                          beginning of it
 *
 * @return length of the target if found
 * @return CLIF_NOT_FOUND if no valid target is found
 */
ssize_t clif_get_target(const char *input, size_t input_len, char **output);
/**
 * @brief   Looks for the first attribute in a given link.
 *
 * @pre `(input != NULL) && (attr != NULL)`
 *
 * @note In order to consider that the string contains a valid attribute,
 *       @p input should start with the attribute separator character ';'.
 *
 * @param[in]  input        string where to look for the attribute. It should
 *                          only be ONE link. Must not be NULL.
 * @param[in]  input_len    length of @p input
 * @param[out] attr         pointer to store the found attribute information.
 *                          Must not be NULL.
 *
 * @return length of the attribute in the buffer if found
 * @return CLIF_NOT_FOUND if no valid attribute is found
 */
ssize_t clif_get_attr(const char *input, size_t input_len, clif_attr_t *attr);

/**
 * @brief   Returns the attribute type of a given string.
 *
 * @pre `(input != NULL) && (input_len > 0)`
 *
 * @param[in] input      string containing the attribute name. Must not be NULL.
 * @param[in] input_len  length of @p input. Must be greater than 0.
 *
 * @return  type of the attribute
 */
clif_attr_type_t clif_get_attr_type(const char *input, size_t input_len);

/**
 * @brief   Returns a constant string of a given attribute type
 *
 * @param[in]  type      type of the attribute
 * @param[out] str       pointer to store the string pointer
 *
 * @return length of the string
 * @return CLIF_NOT_FOUND if the type is an extension or unknown
 */
ssize_t clif_attr_type_to_str(clif_attr_type_t type, const char **str);

/**
 * @brief   Initializes the key of a given attribute according to a given type.
 *
 * @param[out] attr        attribute to initialize
 * @param[in]  type        type of the attribute
 *
 * @return 0 if successful
 * @return <0 otherwise
 */
int clif_init_attr(clif_attr_t *attr, clif_attr_type_t type);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CLIF_H */
