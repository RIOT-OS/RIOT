/*
 * SPDX-FileCopyrightText: 2015 Ell-i open source co-operative
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_encx24j600
 * @{
 *
 * @file
 * @brief       Internal definitions for the ENCX24J600 Ethernet device
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief structure resembling format as sent by device
 */
typedef struct __attribute__((packed)) encx24j600_frame_hdr {
    uint16_t rx_next_ptr;       /**< ptr to next packet within devices memory */

    /* Receive Status Vector */
    uint16_t frame_len;         /**< length of ethernet frame including 4 bytes
                                     checksum */
    uint32_t flags;             /**< random flag field just mentioned for the
                                     sake of documentation completeness */
} encx24j600_frame_hdr_t;

#ifdef __cplusplus
}
#endif
/** @} */
