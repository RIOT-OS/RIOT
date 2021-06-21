/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Wakaama LwM2M Client CLI support
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "kernel_defines.h"
#include "lwm2m_client.h"
#include "lwm2m_client_objects.h"
#include "lwm2m_platform.h"
#include "objects/security.h"
#include "objects/device.h"

#include "net/credman.h"

#define OBJ_COUNT (3)

uint8_t connected = 0;
lwm2m_object_t *obj_list[OBJ_COUNT];
lwm2m_client_data_t client_data;

#ifndef CONFIG_PSK_ID
#define CONFIG_PSK_ID "Client_Identity"
#endif

#ifndef CONFIG_PSK_KEY
#define CONFIG_PSK_KEY "ThisIsRIOT!"
#endif

static const uint8_t psk_id[] = CONFIG_PSK_ID;
static const uint8_t psk_key[] = CONFIG_PSK_KEY;

static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = CREDMAN_TAG_EMPTY,
    .params = {
        .psk = {
            .key = { .s = psk_key, .len = sizeof(psk_key) - 1, },
            .id = { .s = psk_id, .len = sizeof(psk_id) - 1, },
        }
    },
};

void lwm2m_cli_init(void)
{
    /* this call is needed before creating any objects */
    lwm2m_client_init(&client_data);

    /* add objects that will be registered */
    lwm2m_obj_security_args_t args = {
        .server_id = CONFIG_LWM2M_SERVER_SHORT_ID,
        .server_uri = CONFIG_LWM2M_SERVER_URI,
        .security_mode = LWM2M_SECURITY_MODE_PRE_SHARED_KEY,
        .cred = &credential,
        .is_bootstrap = false, /* set to true when using Bootstrap server */
        .client_hold_off_time = 5,
        .bootstrap_account_timeout = 0
    };

    obj_list[0] = lwm2m_object_security_get();
    int res = lwm2m_object_security_instance_create(obj_list[0], 1, &args);

    if (res < 0) {
        puts("Could not instantiate the security object");
        return;
    }

    obj_list[1] = lwm2m_client_get_server_object(&client_data, CONFIG_LWM2M_SERVER_SHORT_ID);

    /* device object has a single instance. All the information for now is defined at
     * compile-time */
    obj_list[2] = lwm2m_object_device_get();

    if (!obj_list[0] || !obj_list[1] || !obj_list[2]) {
        puts("Could not create mandatory objects");
    }
}

int lwm2m_cli_cmd(int argc, char **argv)
{
    if (argc == 1) {
        goto help_error;
    }

    if (!strcmp(argv[1], "start")) {
        /* run the LwM2M client */
        if (!connected && lwm2m_client_run(&client_data, obj_list, ARRAY_SIZE(obj_list))) {
            connected = 1;
        }
        return 0;
    }

    if (IS_ACTIVE(DEVELHELP) && !strcmp(argv[1],"mem")) {
        lwm2m_tlsf_status();
        return 0;
    }

help_error:
    if (IS_ACTIVE(DEVELHELP)) {
        printf("usage: %s <start|mem>\n", argv[0]);
    }
    else {
        printf("usage: %s <start>\n", argv[0]);
    }

    return 1;
}
