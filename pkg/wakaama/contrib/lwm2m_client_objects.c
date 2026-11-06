/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "objects/device.h"
#include "lwm2m_client_config.h"
#include "lwm2m_client_objects.h"

/* These functions are defined by the objects (object_server.c is implemented by
 * the Wakaama package. security.c and device.c can be found in
 * 'contrib/objects') */
lwm2m_object_t *get_server_object(int server_id, const char *binding,
                                  int lifetime, bool storing);

lwm2m_object_t *lwm2m_client_get_server_object(lwm2m_client_data_t *client_data,
                                               int server_id)
{
    (void)client_data;
    lwm2m_object_t *ret;
    int lifetime = CONFIG_LWM2M_DEVICE_TTL;

    ret = get_server_object(server_id, CONFIG_LWM2M_DEVICE_BINDINGS, lifetime,
                            false);
    return ret;
}
