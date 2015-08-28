/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_lwm2m_cli
 * @{
 *
 * @file
 * @brief       Temporary object access function header
 *
 * @todo        REMOVE THIS FILE ONCE DEPENDECIES ARE CLEARED
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LWM2M_CLI_OBJECTS_H
#define LWM2M_CLI_OBJECTS_H

#include "net/ipv6/addr.h"
#include "liblwm2m.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * object_device.c
 */
extern lwm2m_object_t * get_object_device(void);
uint8_t device_change(lwm2m_tlv_t * dataArray, lwm2m_object_t * objectP);
extern void display_device_object(lwm2m_object_t * objectP);

/*
 * object_server.c
 */
extern lwm2m_object_t * get_server_object(int serverId, const char* binding, int lifetime, bool storing);
extern void display_server_object(lwm2m_object_t * objectP);
extern void copy_server_object(lwm2m_object_t * objectDest, lwm2m_object_t * objectSrc);

/*
 * object_security.c
 */
lwm2m_object_t * get_security_object(int serverId, bool isBootstrap);
extern char * get_server_uri(lwm2m_object_t * objectP, uint16_t secObjInstID);
extern void display_security_object(lwm2m_object_t * objectP);
extern void copy_security_object(lwm2m_object_t * objectDest, lwm2m_object_t * objectSrc);

#ifdef __cplusplus
}
#endif

#endif /* LWM2M_CLI_OBJECTS_H */
/** @} */
