/*
 * Copyright (C) 2016 CREATE-NET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     Examples
 * @{
 *
 * @file        config.c
 * @brief       This is a configuration file for IoTivity environment
 *
 * @author      Mattia Antonini <mattia.antonini1@studenti.unipr.it>
 *                              <m.antonini@create-net.org>
 *
 * @}
 */

#ifndef CONFIG_H
#define CONFIG_H

#define OC_DEBUG 0

#ifdef __cplusplus
extern "C" {
#endif

/* Time resolution */
#include <stdint.h>
typedef uint64_t oc_clock_time_t;
/* 1 clock tick = 1ms */
#define OC_CLOCK_CONF_TICKS_PER_SECOND (1000)

#define OC_BYTES_POOL_SIZE (900)
#define OC_INTS_POOL_SIZE (4)
#define OC_DOUBLES_POOL_SIZE (4)

/* Server-side parameters */
/* Maximum number of server resources */
#define MAX_APP_RESOURCES (1)

/* Common paramters */
/* Maximum number of concurrent requests */
#define MAX_NUM_CONCURRENT_REQUESTS (2)

/* Estimated number of nodes in payload tree structure */
#define EST_NUM_REP_OBJECTS (60)

/* Maximum size of request/response PDUs */
#define MAX_PAYLOAD_SIZE (512)

/* Number of devices on the OCF platform */
#define MAX_NUM_DEVICES (1)

/* Platform payload size */
#define MAX_PLATFORM_PAYLOAD_SIZE (256)

/* Device payload size */
#define MAX_DEVICE_PAYLOAD_SIZE (256)

/* Security layer */
/* Maximum number of authorized clients */
#define MAX_NUM_SUBJECTS (1)

/* Maximum number of concurrent DTLS sessions */
#define MAX_DTLS_PEERS (1)

/* Max inactivity timeout before tearing down DTLS connection */
#define DTLS_INACTIVITY_TIMEOUT (10)

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
