/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     cpu_esp8266
 * @brief       Default configurations required by the SDK
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default CPU frequency in MHz.
 * Possible values are 80 and 160.
 */
#ifndef ESP8266_CPU_FREQUENCY
#define ESP8266_CPU_FREQUENCY   80
#endif

#ifdef __cplusplus
}
#endif

#endif /* USER_CONFIG_H */
/** @} */
