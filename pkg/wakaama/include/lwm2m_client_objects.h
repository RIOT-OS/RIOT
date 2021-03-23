/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         lwm2m_client
 * @{
 * @brief           Public API and definitions for the helper functions to
 *                  interact with basic objects from a LwM2M client.
 *
 * @file
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef LWM2M_CLIENT_OBJECTS_H
#define LWM2M_CLIENT_OBJECTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lwm2m_client.h"

/**
 * @name    Access Control Bits
 * @brief   Bit definitions for the ACL property of the LwM2M Access Control
 *          Object.
 *
 * @see http://www.openmobilealliance.org/tech/profiles/LWM2M_Access_Control-v1_0_3.xml
 * @{
 */
#define LWM2M_ACC_CTRL_READ        (1 << 0) /**< Read access */
#define LWM2M_ACC_CTRL_WRITE       (1 << 1) /**< Write access */
#define LWM2M_ACC_CTRL_EXECUTE     (1 << 2) /**< Execution access */
#define LWM2M_ACC_CTRL_DELETE      (1 << 3) /**< Deletion access */
#define LWM2M_ACC_CTRL_CREATE      (1 << 4) /**< Creation access */
/** @} */

/**
 * @brief Creates a LwM2M server object with the default configuration from
 *        net/lwm2m.h
 *
 * @param[in, out] client_data Pointer to a LwM2M client data descriptor
 * @param[in]      server_id   Server ID (SID) to assign to the new instance
 *
 * @return Pointer to the created object
 * @return NULL otherwise
 */
lwm2m_object_t *lwm2m_client_get_server_object(
                        lwm2m_client_data_t *client_data, int server_id);

/**
 * @brief Creates a LwM2M access control object with the default configuration
 *
 * @param[in] client_data Pointer to a LwM2M client data descriptor
 *
 * @return Pointer to the created object
 * @return NULL otherwise
 */
lwm2m_object_t *lwm2m_client_get_acc_ctrl_object(
                        lwm2m_client_data_t *client_data);

#ifdef __cplusplus
}
#endif

#endif /* LWM2M_CLIENT_OBJECTS_H */
/** @} */
