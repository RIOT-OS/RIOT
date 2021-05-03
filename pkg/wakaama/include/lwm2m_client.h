/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_wakaama
 * @defgroup    lwm2m_client LwM2M Client using Wakaama
 * @brief Wakaama adaption to RIOT for implementing a LwM2M client
 * @{
 * @file
 * @brief Definitions and public API for a LwM2M client using Wakaama
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef LWM2M_CLIENT_H
#define LWM2M_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#include "periph/pm.h"
#include "net/sock/udp.h"

#include "lwm2m_client_config.h"
#include "liblwm2m.h"

/**
 * @brief Connection to server descriptor
 */
typedef struct lwm2m_client_connection {
    struct lwm2m_client_connection *next; /**< pointer to the next connection */
    sock_udp_ep_t remote; /**< remote endpoint */
    time_t last_send; /**< last sent packet to the server */
} lwm2m_client_connection_t;

/**
 * @brief LwM2M client descriptor
 */
typedef struct {
    kernel_pid_t pid;              /**< PID of the client thread */
    sock_udp_t sock;               /**< UDP server sock */
    sock_udp_ep_t local_ep;        /**< Local endpoint */
    lwm2m_context_t *lwm2m_ctx;    /**< LwM2M context */
    lwm2m_object_t *obj_security;  /**< LwM2M security object */
    lwm2m_client_connection_t *conn_list; /**< LwM2M connections list */
} lwm2m_client_data_t;

/**
 * @brief Size of the buffer for the UDP packet reception
 */
#define LWM2M_CLIENT_RCV_BUFFER_SIZE  (200)

/**
 * @brief Time in seconds to wait until reboot after a server
 *        request
 */
#define LWM2M_CLIENT_REBOOT_TIME  (5)

/**
 * @brief Time in seconds to wait until LwM2M is refreshed.
 *
 * @note This time is used as the timeout for receiving UDP packets and will be
 *       the maximum time to wait between calls to wakaama core.
 */
#define LWM2M_CLIENT_MIN_REFRESH_TIME   (1)

/**
 * @brief Starts a LwM2M client
 *
 * @param[in, out] client_data Pointer to a LwM2M client data descriptor
 * @param[in] obj_list List of LwM2M objects to be registered
 * @param[in] obj_numof Number of objects in @p obj_list
 *
 * @return Context of the LwM2M client
 */
lwm2m_context_t *lwm2m_client_run(lwm2m_client_data_t *client_data,
                                   lwm2m_object_t *obj_list[],
                                   uint16_t obj_numof);

/**
 * @brief Initializes a LwM2M client
 *
 * @note This functions initializes the memory allocation and is needed before
 *       calling any object creation (i.e. any call to lwm2m_malloc).
 *
 * @param[in] client_data Pointer to a LwM2M client data descriptor
 */
void lwm2m_client_init(lwm2m_client_data_t *client_data);

/**
 * @brief Returns the LwM2M context of a LwM2M client
 *
 * @param[in] client_data pointer to the LwM2M client descriptor
 *
 * @return Pointer to the LwM2M context
 */
static inline lwm2m_context_t *lwm2m_client_get_ctx(
                                    lwm2m_client_data_t *client_data)
{
    return client_data->lwm2m_ctx;
}

#ifdef __cplusplus
}
#endif

#endif /* LWM2M_CLIENT_H */
/** @} */
