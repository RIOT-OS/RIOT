/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#ifndef ZEP_PARSER_H
#define ZEP_PARSER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Parse l2 source address of a ZEP frame
 *
 * @param[in]  buffer   ZEP frame
 * @param[in]  len      size of buffer
 * @param[out] out      destination for l2 address
 * @param[out] out_len  l2 address length
 *
 * @return true if l2 address was found
 */
bool zep_parse_mac(const void *buffer, size_t len, void *out, uint8_t *out_len);

/**
 * @brief   Set link quality information in ZEP frame
 *
 * @param[out] buffer   ZEP frame to modify
 * @param[in]  lqi      link quality to write to ZEP header
 */
void zep_set_lqi(void *buffer, uint8_t lqi);

#ifdef __cplusplus
}
#endif

#endif /* ZEP_PARSER_H */
