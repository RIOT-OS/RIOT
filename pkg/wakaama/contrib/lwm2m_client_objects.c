/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     lwm2m_client
 *
 * @file
 * @brief       Helper functions to interact with the basic objects provided by
 *              Wakaama from a LwM2M client.
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "kernel_defines.h"

#include "lwm2m_client.h"
#include "lwm2m_client_config.h"
#include "lwm2m_client_objects.h"

/* These functions are defined by the objects (object_security.c and
 * object_server.c are implemented by the Wakaama package. device.c can be
 * found in 'contrib/objects') */
lwm2m_object_t *get_security_object(int server_id, const char *server_uri,
                                    char *bs_psk_id, char *psk,
                                    uint16_t psk_len, bool is_bootstrap);
lwm2m_object_t *get_server_object(int server_id, const char *binding,
                                  int lifetime, bool storing);
lwm2m_object_t *lwm2m_get_object_device(void);

lwm2m_object_t *lwm2m_client_get_security_object(
                        lwm2m_client_data_t *client_data)
{
    (void)client_data;
    lwm2m_object_t *ret;
    char *server_uri = CONFIG_LWM2M_SERVER_URI;
    int server_id = CONFIG_LWM2M_SERVER_ID;
    uint16_t psk_len = -1;
    char *psk_buffer = NULL;
    char *psk_id = NULL;

    ret = get_security_object(server_id, server_uri, psk_id, psk_buffer,
                              psk_len, IS_ACTIVE(CONFIG_LWM2M_BOOTSTRAP));

    return ret;
}

lwm2m_object_t *lwm2m_client_get_server_object(
                        lwm2m_client_data_t *client_data)
{
    (void)client_data;
    lwm2m_object_t *ret;
    int server_id = CONFIG_LWM2M_SERVER_ID;
    int lifetime = CONFIG_LWM2M_DEVICE_TTL;

    ret = get_server_object(server_id, CONFIG_LWM2M_DEVICE_BINDINGS, lifetime,
                            false);
    return ret;
}

lwm2m_object_t *lwm2m_client_get_device_object(
                        lwm2m_client_data_t *client_data)
{
    (void)client_data;
    return lwm2m_get_object_device();
}
