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
#include "objects/light_control.h"
#include "objects/common.h"

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
    obj_list[0] = lwm2m_client_get_security_object(&client_data);
    obj_list[1] = lwm2m_client_get_server_object(&client_data);
    obj_list[2] = lwm2m_client_get_device_object(&client_data);
    obj_list[3] = lwm2m_object_light_control_init(&client_data);

    lwm2m_obj_light_control_args_t args = {
        .cb = _light_cb,
        .cb_arg = NULL,
        .color = LED_COLOR,
        .color_len = sizeof(LED_COLOR) - 1,
        .app_type = LED_APP_TYPE,
        .app_type_len = sizeof(LED_APP_TYPE) - 1
    };

    int res = lwm2m_object_light_control_instance_create(&args, 0);

    if (res < 0) {
        puts("Error instantiating light control");
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

int lwm2m_cli_cmd(int argc, char **argv)
{
    if (argc == 1) {
        goto help_error;
    }

    if (!strcmp(argv[1], "start")) {
        /* run the LwM2M client */
        if (!connected && lwm2m_client_run(&client_data, obj_list, OBJ_COUNT)) {
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
