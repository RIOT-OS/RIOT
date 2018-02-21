/*******************************************************************************
*
* Copyright (c) 2014 Bosch Software Innovations GmbH, Germany.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at
*    http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    Bosch Software Innovations GmbH - Please refer to git log
*    Christian Manal - Ported to RIOT OS
*
*******************************************************************************/
/*
 * lwm2mclient.h
 *
 *  General functions of lwm2m test client.
 *
 *  Created on: 22.01.2015
 *  Author: Achim Kraus
 *  Copyright (c) 2015 Bosch Software Innovations GmbH, Germany. All rights reserved.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Header for main loop for Wakaama example client.
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 */

#ifndef LWM2MCLIENT_H
#define LWM2MCLIENT_H

#include "liblwm2m.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * object_device.c
 */

/**
 * @brief  Instantiate an lwm2m device object.
 *
 * @return Pointer to object on success.
 * @param  NULL on error.
 */
lwm2m_object_t *lwm2m_get_object_device(void);

/**
 * @brief  Release resouces of an lwm2m device object.
 *
 * @param[in] objectP  Pointer to the object.
 */
void lwm2m_free_object_device(lwm2m_object_t *objectP);

/**
 * @brief  Indicates when a reboot has been requested via an lwm2m device object.
 *
 * @return true   When reboot has been requested.
 * @return false  Otherwise
 */
bool lwm2m_device_reboot_requested(void);

/*
 * object_server.c
 */

/**
 * @brief  Instantiate an lwm2m server object.
 *
 * @return Pointer to object on success.
 * @param  NULL on error.
 */
lwm2m_object_t *get_server_object(int serverId, const char *binding, int lifetime, bool storing);

/**
 * @brief  Release resouces of an lwm2m server object.
 *
 * @param[in] objectP  Pointer to the object.
 */
void clean_server_object(lwm2m_object_t *object);

/**
 * @brief  Prints a string representation of an lwm2m server object to stdout.
 *
 * @param[in] objectP  Pointer to the object.
 */
void display_server_object(lwm2m_object_t *objectP);

/**
 * @brief  Creates a copy of an lwm2m server object.
 *
 * @param[out] objectDest  Destination object to write to.
 * @param[in]  objectSrc   Source object to copy from.
 */
void copy_server_object(lwm2m_object_t *objectDest, lwm2m_object_t *objectSrc);

/*
 * object_access_control.c
 */

/**
 * @brief  Instantiate an lwm2m access control object.
 *
 * @return Pointer to object on success.
 * @param  NULL on error.
 */
lwm2m_object_t *acc_ctrl_create_object(void);

/**
 * @brief  Release resouces of an lwm2m access control object.
 *
 * @param[in] objectP  Pointer to the object.
 */
void acl_ctrl_free_object(lwm2m_object_t *objectP);

/**
 * @brief  Add an instance of an lwm2m access control object to a base object.
 *
 * @param[out] accCtrlObjP  The access control base object to add an instance to.
 * @param[in]  instId       The instance ID of the new instance.
 * @param[in]  acObjectId   The object ID of the target object.
 * @param[in]  acObjInstId  The instance ID of the target object.
 * @param[in]  acOwner      The owner
 *
 * @return     true         When the instance was successfully created.
 * @return     false        Otherwise.
 */
bool  acc_ctrl_obj_add_inst(lwm2m_object_t *accCtrlObjP, uint16_t instId,
                            uint16_t acObjectId, uint16_t acObjInstId, uint16_t acOwner);

/**
 * @brief  Add an access control list entry to an lwm2m access control object instance.
 *
 * @param[out] accCtrlObjP  The access control base object to modify.
 * @param[in]  instId       The instance ID of the object to modify.
 * @param[in]  aclResId     The resource ID to control access to.
 * @param[in]  acValue      The permissions to set.
 */
bool  acc_ctrl_oi_add_ac_val(lwm2m_object_t *accCtrlObjP, uint16_t instId,
                             uint16_t aclResId, uint16_t acValue);
/*
 * object_security.c
 */

/**
 * @brief  Instantiate an lwm2m security object.
 *
 * @return Pointer to object on success.
 * @param  NULL on error.
 */
lwm2m_object_t *get_security_object(int serverId, const char *serverUri, char *bsPskId, char *psk, uint16_t pskLen, bool isBootstrap);

/**
 * @brief  Release resouces of an lwm2m security object.
 *
 * @param[in] objectP  Pointer to the object.
 */
void clean_security_object(lwm2m_object_t *objectP);

/**
 * @brief  Get the URI of the corresponding server from an lwm2m security object.
 *
 * @param[in] objectP       The security base object to read from.
 * @param[in] secObjInstID  The instance ID of the object to read from.
 *
 * @return    URI string    On success.
 * @retuen    NULL          On error.
 */
char *get_server_uri(lwm2m_object_t *objectP, uint16_t secObjInstID);

/**
 * @brief  Prints a string representation of an lwm2m security object to stdout.
 *
 * @param[in] objectP  Pointer to the object.
 */
void display_security_object(lwm2m_object_t *objectP);

/**
 * @brief  Creates a copy of an lwm2m security object.
 *
 * @param[out] objectDest  Destination object to write to.
 * @param[in]  objectSrc   Source object to copy from.
 */
void copy_security_object(lwm2m_object_t *objectDest, lwm2m_object_t *objectSrc);

/*
 * Run the wakaama process.
 *
 * @param[in] arg Ignored.
 */
void *lwm2m_run_server(void *arg);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LWM2MCLIENT_H */
