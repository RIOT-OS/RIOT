/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266_sdk
 * @{
 *
 * @file
 * @brief       ESP8266 user defined SDK function prototypes
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef USER_H
#define USER_H

#ifndef DOXYGEN

#include <stdint.h>
#include <stdarg.h>

#include "c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MODULE_ESP_SDK

extern uint32_t user_rf_cal_sector_set(void);
extern void     uart_tx_flush (uint32_t);
extern void     system_set_pll (uint8_t);

#endif /* MODULE_ESP_SDK */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* USER_H */
