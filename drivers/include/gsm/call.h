/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_gsm
 * @brief       A generic implementation of the GSM Call API
 *
 * @{
 *
 * @file
 * @brief   GSM-independent Call driver
 *
 * @author  Max van Kessel
 */

#ifndef GSM_CALL_H
#define GSM_CALL_H

#include <stdint.h>
#include <stdbool.h>

#include "gsm.h"

/**
 * @brief   GSM default PPP number
 */
#ifndef GSM_PPP_NUMBER
#define GSM_PPP_NUMBER         "*99#"
#endif

/**
 * @brief Start dialing
 *
 * @param[in]   dev             device to operate on
 * @param[in]   number          the number to call
 * @param[in]   is_voice_call   set to true for a voice call, false for data call
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_call_dial_up(gsm_t *dev, const char * number, bool is_voice_call);

/**
 * @brief Stop calling
 *
 * @param[in] dev   device to operate on
 */
void gsm_call_dial_down(gsm_t *dev);
/**
 * @brief Switch from data mode to command mode
 *
 * @param[in]   dev             device to operate on
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_call_switch_to_command_mode(gsm_t *dev);

/**
 * @brief Switch from command mode to data mode
 *
 * @param[in]   dev             device to operate on
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_call_switch_to_data_mode(gsm_t *dev);

#endif /* GSM_CALL_H */
