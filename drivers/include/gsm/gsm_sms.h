/*
 * Copyright (C) 2018 Max van Kessel <maxvankessel@betronic.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_gsm_sms SMS
 * @ingroup     drivers_gsm
 * @brief       A generic implementation of the GSM SMS API
 *
 * @{
 *
 * @file
 * @brief   GSM-independent SMS message driver
 *
 * @author  Max van Kessel <maxvankessel@betronic.nl>
 */
#ifndef GSM_SMS_H
#define GSM_SMS_H

#include <stdint.h>

#include "gsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief   Default sms message character set
*/
#ifndef GSM_SMS_DEFAULT_CHARACTER_SET
#define GSM_SMS_DEFAULT_CHARACTER_SET       "GSM"
#endif

/**
* @brief   GSM SMS message format enumeration
*/
#ifndef HAVE_GSM_SMS_MESSAGE_FORMAT_T
typedef enum {
    GSM_SMS_MESSAGE_FORMAT_PDU = 0,
    GSM_SMS_MESSAGE_FORMAT_TEXT,
} gsm_sms_message_format_t;
#endif

typedef void (*gsm_sms_cb_t)(void *arg, char *sms, char *sender, char *date);

/**
 * @brief GSM SMS device structure
 */
typedef struct gsm_sms {
    gsm_t *base;            /**< Pointer to the base driver */
    gsm_sms_cb_t callback;  /**< */
    void *callback_args;    /**< */
} gsm_sms_t;

/**
 * @brief   Initialize gsm sms module
 *
 * @param[in]   sms     Sms device
 * @param[in]   dev     Device to operate on
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_sms_init(gsm_sms_t *sms, gsm_t *dev);

/**
 * @brief   Sets character format of sms message
 *
 * @param[in]   sms     Device to operate on
 * @param[in]   format  Character format of sms message
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_sms_set_format(gsm_sms_t * sms, gsm_sms_message_format_t format);

/**
 * @brief   Sets character set of sms message
 * @note    if set equals to null, default message set is loaded
 *
 * @param[in]   sms     Device to operate on
 * @param[in]   set     Character set of sms message
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_sms_set_characters(gsm_sms_t * sms, const char * set);

/**
 * @brief   Enable sms message reception
 *
 * @param[in]   sms     Device to operate on
 * @param[in]   cb      sms message callback
 * @param[in]   arg     sms message callback argument
 *
 * @return    0 for success
 * @return  < 0 for failure
 */
int gsm_sms_enable_reception(gsm_sms_t * sms, gsm_sms_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* GSM_SMS_H */
