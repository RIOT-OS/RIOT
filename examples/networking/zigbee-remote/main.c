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
#include <ws281x.h>

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

#define APPLICATION_ENDPOINT 11

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

int cmd_dev_info(int argc, char *argv[]);
SHELL_COMMAND(dev_info, "shows devices joined using touchlink", cmd_dev_info);

SHELL_COMMAND(reset, "reset the device and perform reboot", zb_reset);

int zcl_cmd(int argc, char **argv)
{
    if (argc != 3) {
        puts("usage: zcl_[toggle|on|off] <short addr> <endpoint>");
    }
    if (strlen(argv[1]) != 4) {
        puts("addr must be 4 digits in hex");
    }
    uint16_t addr = strtoul(argv[1], NULL, 16);
    uint8_t ep = strtoul(argv[2], NULL, 0);
    zb_buf_t *buf = zb_get_out_buf();
    if (!buf) {
        puts("no buffer left");
        return 0;
    }
    if (!strcmp("zcl_toggle", argv[0])) {
        printf("sending toggle to %04x at endpoint %d...\n", addr, ep);
        zb_zcl_on_off_send_toggle(ZB_REF_FROM_BUF(buf), ZB_HA_PROFILE_ID, addr, ep, APPLICATION_ENDPOINT);
    }
    else if (!strcmp("zcl_on", argv[0])) {
        printf("sending on to %04x at endpoint %d...\n", addr, ep);
        zb_zcl_on_off_send_on(ZB_REF_FROM_BUF(buf), ZB_HA_PROFILE_ID, addr, ep, APPLICATION_ENDPOINT);
    }
    else if (!strcmp("zcl_off", argv[0])) {
        printf("sending off to %04x at endpoint %d...\n", addr, ep);
        zb_zcl_on_off_send_off(ZB_REF_FROM_BUF(buf), ZB_HA_PROFILE_ID, addr, ep, APPLICATION_ENDPOINT);
    }
    else {
        puts("wrong command");
    }
    return 0;
}
SHELL_COMMAND(zcl_toggle, "sending zcl toggle command", zcl_cmd);
SHELL_COMMAND(zcl_on, "sending zcl on command", zcl_cmd);
SHELL_COMMAND(zcl_off, "sending zcl off command", zcl_cmd);

int tl_scan(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    zll_start_tl_scan();
    return 0;
}
SHELL_COMMAND(tl_scan, "manual touchlink scan", tl_scan);

static zb_zcl_basic_srv_attr_t basic_attrs;

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT Zigbee ZLL example application");

    /* set device descriptor */
    /* zb_af_simple_desc_1_1_t is already declared by default,
     * uncomment and change numbers as needed */
    //ZB_DECLARE_SIMPLE_DESC(1/* in clusters. on-off */, 1/* out clusters. on-off */);
    static zb_af_simple_desc_1_1_t desc;
    zb_set_simple_descriptor((zb_af_simple_desc_1_1_t *)&desc,
                             APPLICATION_ENDPOINT, /* endpoint */
                             ZB_HA_PROFILE_ID,
                             ZB_HA_ON_OFF_L_SW_DEV_ID,
                             1, /* app version */
                             1, /* input_cl count*/
                             1  /* output_cl count*/
                            );
    /* input clusters are typically servers -> call *_srv_setup*/
    zb_set_input_cluster_id((zb_af_simple_desc_1_1_t *)&desc, 0, ZB_BASIC_CLUSTER_ID);

    zb_set_output_cluster_id((zb_af_simple_desc_1_1_t *)&desc, 0, ZB_ON_OFF_CLUSTER_ID);

    zb_add_simple_descriptor((zb_af_simple_desc_1_1_t *)&desc);

    /* set basic cluster attributes */
    zb_zcl_basic_srv_set_defaults(&basic_attrs);

    basic_attrs.app_version = 1;
    basic_attrs.power_source = ZB_ZCL_BASIC_POWER_SRC_MAINS;
    basic_attrs.phys_environment = ZB_ZCL_BASIC_PHYS_ENV_OFFICE;
    basic_attrs.generic_device_type = ZB_ZCL_BASIC_GEN_DEV_TYPE_LED_BULB;

    char manufacturer[] = "RIOT OS";
    basic_attrs.manufacturer_name_len = sizeof(manufacturer);
    ZB_MEMCPY(basic_attrs.manufacturer_name, manufacturer, sizeof(manufacturer));

    char model_id[] = RIOT_BOARD;
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

    zb_zcl_basic_srv_setup(APPLICATION_ENDPOINT, &basic_attrs);

    zb_zcl_zll_initiator_setup();

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
