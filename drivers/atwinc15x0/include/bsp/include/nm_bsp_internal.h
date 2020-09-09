/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#if !DOXYGEN

/**
 * @ingroup     drivers_atwinc15x0
 * @{
 *
 * @file
 * @brief       Internal compile config for the ATWINC15x0 WiFi netdev driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BSP_INCLUDE_NM_BSP_INTERNAL_H
#define BSP_INCLUDE_NM_BSP_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_ATMEGA_COMMON
#define ARDUINO
#endif

#define CONF_WINC_USE_SPI   (1)
#define CONF_WINC_PRINTF    printf

#ifndef CONF_WINC_DEBUG
#define CONF_WINC_DEBUG     (0)
#endif

#ifndef M2M_LOG_LEVEL
#define M2M_LOG_LEVEL       M2M_LOG_ERROR
#endif

#define NM_EDGE_INTERRUPT   (1)

#ifdef __cplusplus
}
#endif

#endif /* BSP_INCLUDE_NM_BSP_INTERNAL_H */
#endif /* DOXYGEN */
