/*
 * Copyright (C) 2014 Hochschule für Angewandte Wissenschaften Hamburg (HAW)
 * Copyright (C) 2014 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_base64 base64 encoder decoder
 * @ingroup     sys
 * @brief       base64 encoder and decoder
 * @{
 *
 * @brief       encoding and decoding functions for base64
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 */

#ifndef _BASE64_ENCODER_DECODER_H
#define _BASE64_ENCODER_DECODER_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BASE64_SUCCESS                (0)  /**< return value for success                       */
#define BASE64_ERROR_BUFFER_OUT       (-1) /**< error value for invalid output buffer pointer  */
#define BASE64_ERROR_BUFFER_OUT_SIZE  (-2) /**< error value for invalid output buffer size     */
#define BASE64_ERROR_DATA_IN          (-3) /**< error value for invalid input buffer           */
#define BASE64_ERROR_DATA_IN_SIZE     (-4) /**< error value for invalid output buffer size     */

/**
 * @brief           Encodes a given datum to base64 and save the result to the given destination.
 * @param[in]       data_in           pointer to the datum to encode
 * @param[in]       data_in_size      the size of `data_in`
 * @param[out]      base64_out        pointer to store the encoded base64 string
 * @param[in,out]   base64_out_size   pointer to the variable containing the size of `base64_out.`
                                      This value is overwritten with the estimated size used for
                                      the encoded base64 string on BASE64_ERROR_BUFFER_OUT_SIZE.
                                      This value is overwritten with the actual used size for the
                                      encoded base64 string on BASE64_SUCCESS.

 * @returns BASE64_SUCCESS on success,
            BASE64_ERROR_BUFFER_OUT_SIZE on insufficient size for encoding to `base64_out`,
            BASE64_ERROR_BUFFER_OUT if `base64_out` equals NULL
                                    but the `base64_out_size` is sufficient,
            BASE64_ERROR_DATA_IN if `data_in` equals NULL,
            BASE64_ERROR_DATA_IN_SIZE if `data_in_size` is less then 1.
 */
int base64_encode(unsigned char *data_in, size_t data_in_size, \
                  unsigned char *base64_out, size_t *base64_out_size);

/**
 * @brief           Decodes a given base64 string and save the result to the given destination.
 * @param[out]      base64_in        pointer to store the encoded base64 string
 * @param[in]       base64_in_size   pointer to the variable containing the size of `base64_out.`
 * @param[in]       data_out         pointer to the datum to encode
 * @param[in, out]  data_out_size    the size of `data_out`.
                                     This value is overwritten with the estimated size used for the
                                     decoded string on BASE64_ERROR_BUFFER_OUT_SIZE.
                                     This value is overwritten with the actual used size for the
                                     decoded string on BASE64_SUCCESS.

 * @returns BASE64_SUCCESS on success,
            BASE64_ERROR_BUFFER_OUT_SIZE on insufficient size for decoding to `data_out`,
            BASE64_ERROR_BUFFER_OUT if `data_out` equals NULL
                                    but the size for `data_out_size` is sufficient,
            BASE64_ERROR_DATA_IN if `base64_in` equals NULL,
            BASE64_ERROR_DATA_IN_SIZE if `base64_in_size` is less then 4.
 */
int base64_decode(unsigned char *base64_in, size_t base64_in_size, \
                  unsigned char *data_out, size_t *data_out_size);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _BASE64_ENCODER_DECODER_H */
