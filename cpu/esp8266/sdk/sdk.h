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
 * @brief       ESP8266 SDK container
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef SDK_H
#define SDK_H

#include "c_types.h"
#include "ets_sys.h"

#include "sdk/ets.h"
#include "sdk/main.h"
#include "sdk/phy.h"
#include "sdk/pp.h"
#include "sdk/rom.h"
#include "sdk/user.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_ESP_SDK
#include "espressif/osapi.h"
#include "espressif/user_interface.h"
#endif /* MODULE_ESP_SDK */

#ifdef __cplusplus
}
#endif

#endif /* SDK_H */
