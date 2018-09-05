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
 * @brief       ESP8266 SDK libphy function prototypes
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef PHY_H
#define PHY_H

#ifndef DOXYGEN

#include <stdint.h>
#include <stdarg.h>

#include "c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MODULE_ESP_SDK
/*
 * The following functions are mappings or dummies for source code
 * compatibility of SDK and NON-SDK version
 */

#include "esp/dport_regs.h"

extern void     phy_afterwake_set_rfoption(int op);
extern int      phy_check_data_table(void * gdctbl, int x, int flg);
extern int      register_chipv6_phy(uint8_t * esp_init_data);
extern void     sleep_reset_analog_rtcreg_8266(void);
extern uint32_t test_tout(bool);
extern void     write_data_to_rtc(uint8_t *);

#endif /* MODULE_ESP_SDK */

extern uint32_t phy_get_mactime(void);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* PHY_H */
