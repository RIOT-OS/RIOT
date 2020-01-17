/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        ccm.h
 * @brief       Common header definitions for  CBC-MAC (CCM) and CCM* (CCMS)
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#ifndef CRYPTO_MODES_CCM_COMMON_H
#define CRYPTO_MODES_CCM_COMMON_H

#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CCM error codes
 * @{
 */
#define CCM_ERR_INVALID_NONCE_LENGTH        (-2)
#define CCM_ERR_INVALID_CBC_MAC             (-3)
#define CCM_ERR_INVALID_DATA_LENGTH         (-3)
#define CCM_ERR_INVALID_LENGTH_ENCODING     (-4)
#define CCM_ERR_INVALID_MAC_LENGTH          (-5)
/** @} */

/**
 * @brief Block size required for the cipher. CCM is only defined for 128 bit ciphers.
 */
#define CCM_BLOCK_SIZE                      16

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_MODES_CCM_H */
/** @} */
