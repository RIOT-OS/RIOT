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
 * @brief       ESP8266 SDK libpp function prototypes
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef PP_H
#define PP_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include "c_types.h"

#ifndef MODULE_ESP_SDK
/*
 * The following functions are mappings or dummies for source code
 * compatibility of SDK and NON-SDK version
 */

#define system_soft_wdt_feed()

#endif /* MODULE_ESP_SDK */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* PP_H */
