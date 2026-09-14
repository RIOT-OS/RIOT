/*
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Configuration for OpenThread example
 *
 * @author      Moritz Voigt
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Set log level OT_LOG_LEVEL_CRIT is the default */
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_CRIT
/* Enables the option to change log level via CLI (only OPENTHREAD_CONFIG_LOG_LEVEL or lower) */
#define OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE 0

/* Allows for easy troubleshooting with the CLI */
#define OPENTHREAD_CONFIG_MESH_DIAG_ENABLE 1

/* Set the amount of available message buffers */
/* #define OPENTHREAD_CONFIG_NUM_MESSAGE_BUFFERS (20U)*/

/* Enables the ot ping command in the CLI */
#define OPENTHREAD_CONFIG_PING_SENDER_ENABLE 1

/* Configuration for Thread Management Framework */
#define OPENTHREAD_CONFIG_TMF_ANYCAST_LOCATOR_ENABLE 1
#define OPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE 1
#define OPENTHREAD_CONFIG_TMF_NETDIAG_CLIENT_ENABLE 1

/* Enables ot uptime command in the CLI */
#define OPENTHREAD_CONFIG_UPTIME_ENABLE 1

#ifdef __cplusplus
}
#endif
