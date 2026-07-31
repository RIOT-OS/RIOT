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

#ifdef WS281X_PARAM_NUMOF
#include "ws281x_params.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#define APPLICATION_ENDPOINT 11

#define INPUT_CL_COUNT 4

#define OUTPUT_CL_COUNT 0

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
ws281x_t led_dev;
static const color_rgb_t led_white = {.r = 255, .g = 255, .b = 255};
static const color_rgb_t led_coff = {.r = 0, .g = 0, .b = 0};

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
    if (param) {
        ws281x_set(&led_dev, 0, led_white);
    }
    else {
        ws281x_set(&led_dev, 0, led_coff);
    }
    ws281x_write(&led_dev);
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

static zb_zcl_basic_srv_attr_t basic_attrs = {
    .app_version = 1,
    .power_source = ZB_ZCL_BASIC_POWER_SRC_MAINS,
    .phys_environment = ZB_ZCL_BASIC_PHYS_ENV_OFFICE,
    .generic_device_type = ZB_ZCL_BASIC_GEN_DEV_TYPE_LED_BULB,
    .manufacturer_name = "RIOT OS",
    .manufacturer_name_len = sizeof("RIOT OS"),
    .model_id = RIOT_BOARD,
    .model_id_len = sizeof(RIOT_BOARD),
    .date_code = "19700101",
    .date_code_len = sizeof("19700101"),
    .location_desc = "Lab",
    .location_desc_len = sizeof("Lab"),
    .product_code_id = ZB_ZCL_BASIC_PRODUCT_CODE_EAN,
    .product_code = "1234567891011",
    .product_code_len = sizeof("1234567891011"),
    .software_build_id = RIOT_VERSION,
    .software_build_id_len = sizeof(RIOT_VERSION),
};

/* This should equal (INPUT_CL_COUNT, OUTPUT_CL_COUNT) */
ZB_DECLARE_SIMPLE_DESC(4, 0);
static zb_af_simple_desc_4_0_t simple_desc_lightbulb = {
    APPLICATION_ENDPOINT,
    ZB_HA_PROFILE_ID,
    ZB_HA_ON_OFF_LIGHT_DEV_ID,
    1,/* app version */
    0,/* reserved */
    INPUT_CL_COUNT,
    OUTPUT_CL_COUNT,
    {
        // first input(srv), then output(cli)
        ZB_BASIC_CLUSTER_ID,
        ZB_ZLL_CLUSTER_ID,
        ZB_GROUPS_CLUSTER_ID,
        ZB_ON_OFF_CLUSTER_ID
    }
};

static zb_af_ep_desc_t lightbulb_ep = {
    APPLICATION_ENDPOINT,
    ZB_HA_PROFILE_ID,
    (INPUT_CL_COUNT + OUTPUT_CL_COUNT),
    (zb_af_simple_desc_1_1_t *)&simple_desc_lightbulb,
    {
        ZB_ZCL_CLUSTER_DESC(&basic_attrs, ZB_BASIC_CLUSTER_ID, ZB_ZCL_SERVER_ROLE),
        ZB_ZCL_CLUSTER_DESC(NULL, ZB_ZLL_CLUSTER_ID, ZB_ZCL_SERVER_ROLE),
        ZB_ZCL_CLUSTER_DESC(&group_attrs, ZB_GROUPS_CLUSTER_ID, ZB_ZCL_SERVER_ROLE),
        ZB_ZCL_CLUSTER_DESC(&on_off_attrs, ZB_ON_OFF_CLUSTER_ID, ZB_ZCL_SERVER_ROLE),
    }  
};

int main(void)
{
    /* setup ws281x led */
#ifdef WS281X_PARAM_NUMOF
    ws281x_init(&led_dev, &ws281x_params[0]);
#endif
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT Zigbee ZLL Lightbulb example application");

    zb_zcl_init_ep(&lightbulb_ep);

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
