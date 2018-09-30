/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_gsm
 * @brief
 *
 * @{
 *
 * @file
 * @brief
 *
 * @author  Max van Kessel
 */
#ifndef GSM_INTERNAL_H
#define GSM_INTERNAL_H

#include <stdint.h>

#include "gsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief locks the gsm with a mutal exclusion
 *
 * @param dev   gsm device to lock
 */
void gsm_lock(gsm_t *dev);

/**
 * @brief unlocks the gsm
 *
 * @param dev   gsm device to unlock
 */
void gsm_unlock(gsm_t *dev);

/**
 * @brief Calculates signal value to rssi value in dBm
 *
 * @param signal    signal to convert
 *
 * @return  RSSI value in dBm
 */
int gsm_signal_to_rssi(unsigned signal);

#ifdef __cplusplus
}
#endif

#endif /* GSM_INTERNAL_H */
