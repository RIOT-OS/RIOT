/*
 * Copyright (C) 2015 Ell-i open source co-operative
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_encx24j600
 * @{
 *
 * @file
 * @brief       Internal definitions for the ENCX24J600 Ethernet device
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef ENCX24J600_INTERNAL_H
#define ENCX24J600_INTERNAL_H

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
#endif /* ENCX24J600_INTERNAL_H */
/** @} */
