/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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


#ifndef DOXYGEN

#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t phy_get_mactime(void);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
