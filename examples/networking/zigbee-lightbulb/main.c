/*
 * SPDX-FileCopyrightText: 2019 Thomas Stilwell <stilwellt@openlabs.co>
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example ZLL application for demonstrating the ZBOSS Zigbee stack
 *
 * @author      Thomas Stilwell <stilwellt@openlabs.co>
 * @author      Lukas Luger     <lukas.luger@mailbox.tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "led.h"
#include "board.h"
#include "periph/gpio.h"
#include "ws281x.h"

#include "zb_common.h"
#include "zb_aps.h"
#include "zb_zcl.h"
#include "zb_zdo.h"
#include "zb_g_context.h"
#include "zb_zcl_groups.h"
#include "zb_zcl_on_off.h"
#include "zb_zcl_basic.h"
#include "zb_osif.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
ws281x_t led_dev;
color_rgb_t led_white = {.r = 255, .g = 255, .b = 255};
color_rgb_t led_coff = {.r = 0, .g = 0, .b = 0};

/* these commands are provided by the zboss stack */
int zb_input_packet(int argc, char **argv);
SHELL_COMMAND(zigp, "input a zigbee packet like 030806ffffffff07", zb_input_packet);

int zb_inject_packet(int argc, char **argv);
SHELL_COMMAND(inj, "inject a raw 802.15.4 packet like 030885ffffffff07 ", zb_inject_packet);

int cmd_zconfig(int argc, char *argv[]);
SHELL_COMMAND(zconfig, "ZBOSS configuration", cmd_zconfig);

SHELL_COMMAND(reset, "reset the device and perform reboot", zb_reset);

void set_led(zb_uint8_t param)
{

#ifdef WS281X_PARAM_NUMOF
    ws281x_prepare_transmission(&led_dev);
    if (param) {
        ws281x_set(&led_dev, 0, led_white);
    }
    else {
        ws281x_set(&led_dev, 0, led_coff);
    }
    ws281x_write(&led_dev);
    ws281x_end_transmission(&led_dev);
#elif defined LED0_ON
    if (param) {
        LED0_ON;
    }
    else {
        LED0_OFF;
    }
#endif
}

/* Cluster setup */
static zb_zcl_groups_srv_attr_t group_attrs = { .name_support = 1 };

static zb_zcl_on_off_srv_attr_t on_off_attrs = {
    .on_off = ZB_FALSE,
    .global_scene_ctrl = ZB_TRUE,
    .on_time = 0x0000,
    .off_wait_time = 0x0000,
    .set_state = set_led,
};
static zb_zcl_basic_srv_attr_t basic_attrs;

int main(void)
{
    /* setup ws281x led */
#ifdef WS281X_PARAM_NUMOF
    ws281x_params_t led_params;
    led_params.numof = WS281X_PARAM_NUMOF;
    led_params.pin = WS281X_PARAM_PIN;
    ws281x_init(&led_dev, &led_params);
#endif
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT Zigbee ZLL example application");

    /* set device descriptor */
    ZB_DECLARE_SIMPLE_DESC(3/* in clusters. Groups, basic, on-off */, 1/* out clusters. on-off */);
    static zb_af_simple_desc_3_1_t desc;
    zb_set_simple_descriptor((zb_af_simple_desc_1_1_t *)&desc,
                              11, /* endpoint */
                              ZB_HA_PROFILE_ID,
                              ZB_HA_ON_OFF_LIGHT_DEV_ID,
                              1, /* app version */
                              3, /* input_cl count*/
                              1 /* output_cl count*/
                            );
    zb_set_input_cluster_id((zb_af_simple_desc_1_1_t *)&desc, 0, ZB_BASIC_CLUSTER_ID);
    zb_set_input_cluster_id((zb_af_simple_desc_1_1_t *)&desc, 1, ZB_GROUPS_CLUSTER_ID);
    zb_set_input_cluster_id((zb_af_simple_desc_1_1_t *)&desc, 2, ZB_ON_OFF_CLUSTER_ID);

    zb_set_output_cluster_id((zb_af_simple_desc_1_1_t *)&desc, 0, ZB_ON_OFF_CLUSTER_ID);

    zb_add_simple_descriptor((zb_af_simple_desc_1_1_t *)&desc);

    /* set cluster attributes */
    zb_zcl_basic_srv_set_defaults(&basic_attrs);

    basic_attrs.app_version = 1;
    basic_attrs.power_source = ZB_ZCL_BASIC_POWER_SRC_MAINS;
    basic_attrs.phys_environment = ZB_ZCL_BASIC_PHYS_ENV_OFFICE;
    basic_attrs.generic_device_type = ZB_ZCL_BASIC_GEN_DEV_TYPE_LED_BULB;

    char manufacturer[] = "RIOT OS";
    basic_attrs.manufacturer_name_len = sizeof(manufacturer);
    ZB_MEMCPY(basic_attrs.manufacturer_name, manufacturer, sizeof(manufacturer));

    char model_id[] = "nRF52840DK";
    basic_attrs.model_id_len = sizeof(model_id);
    ZB_MEMCPY(basic_attrs.model_id, model_id, sizeof(model_id));

    char date[] = "19700101";
    basic_attrs.date_code_len = sizeof(date);
    ZB_MEMCPY(basic_attrs.date_code, date, sizeof(date));

    char location[] = "Lab";
    basic_attrs.location_desc_len = sizeof(location);
    ZB_MEMCPY(basic_attrs.location_desc, location, sizeof(location));

    basic_attrs.product_code_id = ZB_ZCL_BASIC_PRODUCT_CODE_EAN;
    char ean[] = "1234567891011";
    basic_attrs.product_code_len = sizeof(ean);
    ZB_MEMCPY(basic_attrs.product_code, ean, sizeof(ean));

    basic_attrs.software_build_id_len = sizeof(RIOT_VERSION);
    ZB_MEMCPY(basic_attrs.software_build_id, RIOT_VERSION, sizeof(RIOT_VERSION));

    zb_zcl_on_off_srv_setup(11, &on_off_attrs);
    zb_zcl_groups_srv_setup(11, &group_attrs);
    zb_zcl_basic_srv_setup(11, &basic_attrs);

    zb_zcl_zll_target_setup();

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
