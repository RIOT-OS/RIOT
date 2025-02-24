/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    fido2_ctap_utils FIDO2 CTAP utils
 * @ingroup     fido2_ctap
 * @brief       FIDO2 CTAP utility helper
 *
 * @{
 *
 * @file
 * @brief       Definition for CTAP utility functions
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 */

#ifndef FIDO2_CTAP_CTAP_UTILS_H
#define FIDO2_CTAP_CTAP_UTILS_H

#include <stdint.h>
#include "fido2/ctap/ctap.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LED animation to indicate that user action is required
 */
void fido2_ctap_utils_led_animation(void);

/**
 * @brief Initialize button to be used for user presence test
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_utils_init_gpio_pin(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank);

/**
 * @brief Test user presence
 *
 * Successful if user clicks button in less than @ref CTAP_UP_TIMEOUT
 *
 * @return @ref ctap_status_code_t
 */
ctap_status_code_t fido2_ctap_utils_user_presence_test(void);

/**
 * @brief Compare fido2 credentials based on id to find more recent one
 *
 * The more recent credential has a higher id. Therefore we sort in
 * descending order.
 *
 * @param[in] k1    first resident key
 * @param[in] k2    second resident key
 *
 * @return <0 if k1 > k2
 * @return >0 if k1 < k2
 */
static inline int fido2_ctap_utils_cred_cmp(const void *k1, const void *k2)
{
    ctap_resident_key_t *_k1 = (ctap_resident_key_t *)k1;
    ctap_resident_key_t *_k2 = (ctap_resident_key_t *)k2;

    /* multiply by -1 because we want descending order. */
    return (_k1->id - _k2->id) * -1;
}

/**
 * @brief Check equality of resident keys based on rp_id_hash and user_id
 *
 * @param[in] k1      first resident key
 * @param[in] k2      second resident key
 *
 * @return true if equal false otherwise
 */
static inline bool fido2_ctap_utils_ks_equal(const ctap_resident_key_t *k1,
                                             const ctap_resident_key_t *k2)
{
    return memcmp(k1->rp_id_hash, k2->rp_id_hash, sizeof(k1->rp_id_hash)) == 0 &&
           memcmp(k1->user_id, k2->user_id, sizeof(k1->user_id)) == 0;
}

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_CTAP_UTILS_H */
/** @} */
