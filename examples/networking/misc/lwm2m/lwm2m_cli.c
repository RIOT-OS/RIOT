/*
 * Copyright (C) 2024 HAW Hamburg
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
 * @brief       Wakaama LwM2M Client example CLI support
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "board.h"
#include "lwm2m_client.h"
#include "lwm2m_client_objects.h"
#include "lwm2m_platform.h"

#include "shell.h"

#include "objects/common.h"
#include "objects/device.h"
#include "objects/security.h"
#include "objects/light_control.h"

#include "credentials.h"

#define LED_COLOR "FFFFFF"
#define LED_APP_TYPE "LED 0"
# define OBJ_COUNT (4)

uint8_t connected = 0;
lwm2m_object_t *obj_list[OBJ_COUNT];
lwm2m_client_data_t client_data;

void _light_cb(lwm2m_object_t *object, uint16_t instance_id, bool status, uint8_t dimmer,
               const char* color, const char* app_type, void *arg)
{
    (void)object;
    (void)instance_id;
    (void)arg;

    printf("LED (%s) is now %s, with color %s and intensity %d%%\n", app_type, status? "ON":"OFF",
           color, dimmer);

#ifdef LED0_ON
    if (status) {
        LED0_ON;
    }
    else {
        LED0_OFF;
    }
#endif
}

void lwm2m_cli_init(void)
{
    /* this call is needed before creating any objects */
    lwm2m_client_init(&client_data);

    /* add objects that will be registered */
    obj_list[0] = lwm2m_object_security_init(&client_data);
    obj_list[1] = lwm2m_client_get_server_object(&client_data, CONFIG_LWM2M_SERVER_SHORT_ID);
    obj_list[2] = lwm2m_object_device_init(&client_data);
    obj_list[3] = lwm2m_object_light_control_init(&client_data);

    /* create light control object instance */
    lwm2m_obj_light_control_args_t light_args = {
        .cb = _light_cb,
        .cb_arg = NULL,
        .color = LED_COLOR,
        .color_len = sizeof(LED_COLOR) - 1,
        .app_type = LED_APP_TYPE,
        .app_type_len = sizeof(LED_APP_TYPE) - 1
    };

    int res = lwm2m_object_light_control_instance_create(&light_args, 0);
    if (res < 0) {
        puts("Error instantiating light control");
    }

    /* create security object instance */
    lwm2m_obj_security_args_t security_args = {
        .server_id = CONFIG_LWM2M_SERVER_SHORT_ID,
        .server_uri = CONFIG_LWM2M_SERVER_URI,
        .security_mode = LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY,
        .pub_key_or_id = rpk_pub,
        .pub_key_or_id_len = sizeof(rpk_pub),
        .secret_key = rpk_priv,
        .secret_key_len = sizeof(rpk_priv),
        .server_pub_key = server_rpk_pub,
        .server_pub_key_len = sizeof(server_rpk_pub),
        .is_bootstrap = false, /* set to true when using Bootstrap server */
        .client_hold_off_time = 5,
        .bootstrap_account_timeout = 0
    };

    if (IS_ACTIVE(CONFIG_LWM2M_CRED_PSK)) {
        security_args.security_mode = LWM2M_SECURITY_MODE_PRE_SHARED_KEY;
        security_args.pub_key_or_id = psk_id;
        security_args.pub_key_or_id_len = sizeof(psk_id) - 1;
        security_args.secret_key = psk_key;
        security_args.secret_key_len = sizeof(psk_key) - 1;
    }
    else if (IS_ACTIVE(CONFIG_LWM2M_CRED_RPK)) {
        security_args.security_mode = LWM2M_SECURITY_MODE_RAW_PUBLIC_KEY;
        security_args.pub_key_or_id = rpk_pub;
        security_args.pub_key_or_id_len = sizeof(rpk_pub);
        security_args.secret_key = rpk_priv;
        security_args.secret_key_len = sizeof(rpk_priv);
        security_args.server_pub_key = server_rpk_pub;
        security_args.server_pub_key_len = sizeof(server_rpk_pub);
    }
    else {
        puts("Error: you must select a credential type");
        return;
    }

    res = lwm2m_object_security_instance_create(&security_args, 1);
    if (res < 0) {
        puts("Could not instantiate the security object");
        return;
    }

    if (!obj_list[0] || !obj_list[1] || !obj_list[2]) {
        puts("Could not create mandatory objects");
    }
}

static int _parse_lwm2m_light_cmd(int argc, char **argv)
{
    if (argc < 4) {
        printf("usage: %s light <on|off> <dimmer> [color]\n", argv[0]);
        return 1;
    }

    if (!connected) {
        puts("LwM2M client not connected");
        return 1;
    }

    bool status = !strcmp(argv[2], "on");
    uint8_t dimmer = atoi(argv[3]);

    if (argc > 4) {
        char* color = argv[4];
        lwm2m_object_light_control_update_color(0, color, strlen(color), false);
    }

    lwm2m_object_light_control_update_status(0, status, false);

    /* call the callback now to actually update the light */
    lwm2m_object_light_control_update_dimmer(0, dimmer, true);

    return 0;
}

static int _cli_cmd(int argc, char **argv)
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

    if (IS_ACTIVE(DEVELHELP) && !strcmp(argv[1], "mem")) {
        lwm2m_tlsf_status();
        return 0;
    }

    if (!strcmp(argv[1], "light")) {
        return _parse_lwm2m_light_cmd(argc, argv);
    }

help_error:
    if (IS_ACTIVE(DEVELHELP)) {
        printf("usage: %s <start|mem|light>\n", argv[0]);
    }
    else {
        printf("usage: %s <start|light>\n", argv[0]);
    }

    return 1;
}

SHELL_COMMAND(lwm2m, "Start LwM2M client", _cli_cmd);
