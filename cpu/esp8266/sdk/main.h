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
 * @brief       ESP8266 SDK libmain function prototypes
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef MAIN_H
#define MAIN_H

#ifndef DOXYGEN

#include <stdint.h>
#include <stdarg.h>

#include "c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MODULE_ESP_SDK
#include "esp/dport_regs.h"

/*
 * The following functions are mappings or dummies for source code
 * compatibility of SDK and NON-SDK version
 */

#define system_get_time       phy_get_mactime
#define system_get_chip_id()  (((DPORT.OTP_MAC1 & 0xffff) << 8) + ((DPORT.OTP_MAC0 >> 24) & 0xff))
#define system_get_cpu_freq   ets_get_cpu_frequency

extern int      os_printf_plus (const char* format, ...);

extern void     system_deep_sleep (uint32_t time_in_us);
extern uint8_t  system_get_checksum(uint8_t *ptr, uint32_t len);
extern void     system_restart (void);
extern bool     system_update_cpu_freq(uint8 freq);

extern bool     system_rtc_mem_read(uint32_t src_addr, void *des_addr, uint32_t save_size);
extern bool     system_rtc_mem_write(uint32_t src_addr, void *des_addr, uint32_t save_size);

#endif /* MODULE_ESP_SDK */

extern void NmiTimSetFunc(void (*func)(void));

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* MAIN_H */
