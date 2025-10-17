/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core uwb module configurations
 *              taken from decawave-uwb-core/hw/drivers/uwb/syscfg.yml
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Interrupt task priority for instance 0
 */
#ifndef MYNEWT_VAL_UWB_DEV_TASK_PRIO
#define MYNEWT_VAL_UWB_DEV_TASK_PRIO    (THREAD_PRIORITY_MAIN - 5)
#endif

/**
 * @brief Size of interrupt task stack
 */
#ifndef MYNEWT_VAL_UWB_DEV_TASK_STACK_SZ
#define MYNEWT_VAL_UWB_DEV_TASK_STACK_SZ (1024)
#endif

/**
 * @brief Size of the rx buffer in the uwb_dev
 */
#ifndef MYNEWT_VAL_UWB_RX_BUFFER_SIZE
#define MYNEWT_VAL_UWB_RX_BUFFER_SIZE (1024)
#endif

/**
 * @brief Enable init messages showing each package has been initialised
 */
#ifndef MYNEWT_VAL_UWB_PKG_INIT_LOG
#define MYNEWT_VAL_UWB_PKG_INIT_LOG (0)
#endif

/**
 * @brief Maximum size of rxdiag structure
 */
#ifndef MYNEWT_VAL_UWB_DEV_RXDIAG_MAXLEN
#define MYNEWT_VAL_UWB_DEV_RXDIAG_MAXLEN (20)
#endif

/**
 * @brief UWB_0 Device Enable. BSP uses this to enable the specific uwb device
 *
 * @note in uwb-core you need to tell exactly how many uwb-device there
 *       are, ideally thus would be done different
 */
#ifndef MYNEWT_VAL_UWB_DEVICE_0
#define MYNEWT_VAL_UWB_DEVICE_0 (1)
#endif

/**
 * @brief Max number of UWB_DEVICES allowed in system
 *
 * @note uwb-core uses arrays to keep track of devices, currently se use
 *       linked list, this is temporary...
 */
#ifndef MYNEWT_VAL_UWB_DEVICE_MAX
#define MYNEWT_VAL_UWB_DEVICE_MAX (3)
#endif

/**
 * @brief If ipatov and sts timestamps differ by more than this value
 *       they are considered invalid
 */
#ifndef MYNEWT_VAL_UWB_STS_TS_MATCH_THRESHOLD
#define MYNEWT_VAL_UWB_STS_TS_MATCH_THRESHOLD (30)
#endif

/**
 * @brief Default Anchor X Coordinate
 */
#ifndef MYNEWT_VAL_LOCAL_COORDINATE_X
#define MYNEWT_VAL_LOCAL_COORDINATE_X (((float)0.0f))
#endif

/**
 * @brief Default Anchor Y Coordinate
 */
#ifndef MYNEWT_VAL_LOCAL_COORDINATE_Y
#define MYNEWT_VAL_LOCAL_COORDINATE_Y (((float)0.0f))
#endif

/**
 * @brief Default Anchor Z Coordinate
 */
#ifndef MYNEWT_VAL_LOCAL_COORDINATE_Z
#define MYNEWT_VAL_LOCAL_COORDINATE_Z (((float)0.0f))
#endif

/**
 * @brief Range Measurement Variance
 */
#ifndef MYNEWT_VAL_RANGE_VARIANCE
#define MYNEWT_VAL_RANGE_VARIANCE (((float)5.4444e-04))
#endif

/**
 * @brief Azimuth Measurement Variance
 */
#ifndef MYNEWT_VAL_AZIMUTH_VARIANCE
#define MYNEWT_VAL_AZIMUTH_VARIANCE (((float)2.91e-2))
#endif

/**
 * @brief OS Latency Guardband (usec)
 */
#ifndef MYNEWT_VAL_OS_LATENCY
#define MYNEWT_VAL_OS_LATENCY (((uint32_t)800))
#endif

/**
 * @brief Default Pan ID
 */
#ifndef MYNEWT_VAL_PANID
#define MYNEWT_VAL_PANID (((const uint16_t)0xdeca))
#endif

#ifdef __cplusplus
}
#endif
