/*
 * Copyright (C) 2015 José Ignacio Alamos <jialaos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_hdlc_hdr    HDLC header
 * @ingroup     net_hdlc
 * @brief       HDLC header architecture
 *
 * @{
 *
 * @file
 * @brief       Definitions for HDLC header
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 */
#ifndef HDLC_HDR_H
#define HDLC_HDR_H

#include <stdint.h>
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data type to represent an HDLC header.
 */
typedef struct __attribute__((packed)) {
    uint8_t address;            /**< Address field oh HDLC header */
    uint8_t control;            /**< Control field of HDLC header */
    network_uint16_t protocol;  /**< Protocol field of HDLC header */
} hdlc_hdr_t;

#ifdef __cplusplus
}
#endif

#endif /* HDLC_HDR_H */
/** @} */
