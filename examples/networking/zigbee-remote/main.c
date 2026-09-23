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
#include "board.h"

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

#define APPLICATION_VERSION 1

#define INPUT_CL_COUNT 1

#define OUTPUT_CL_COUNT 2

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

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

static zb_zcl_basic_srv_attr_t basic_attrs = {
    .app_version = APPLICATION_VERSION,
    .power_source = ZB_ZCL_BASIC_POWER_SRC_BATTERY,
    .phys_environment = ZB_ZCL_BASIC_PHYS_ENV_OFFICE,
    .generic_device_type = ZB_ZCL_BASIC_GEN_DEV_TYPE_REMOTE_CONTROLLER,
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

/* This will declare a simple_desc_remote  */
ZB_DECLARE_SIMPLE_DESC(INPUT_CL_COUNT, OUTPUT_CL_COUNT);
ZB_SET_SIMPLE_DESC(remote, APPLICATION_ENDPOINT, ZB_HA_PROFILE_ID,  \
        ZB_HA_ON_OFF_L_SW_DEV_ID, APPLICATION_VERSION, \
        INPUT_CL_COUNT, OUTPUT_CL_COUNT, \
        /* input cl: */ ZB_BASIC_CLUSTER_ID, ZB_ZLL_CLUSTER_ID, \
        /* output cl: */ ZB_ON_OFF_CLUSTER_ID);

static zb_af_ep_desc_t remote_ep = {
    APPLICATION_ENDPOINT,
    ZB_HA_PROFILE_ID,
    (INPUT_CL_COUNT + OUTPUT_CL_COUNT),
    (zb_af_simple_desc_1_1_t *)&simple_desc_remote,
    {
        ZB_ZCL_CLUSTER_DESC(&basic_attrs, ZB_BASIC_CLUSTER_ID, ZB_ZCL_SERVER_ROLE),
        ZB_ZCL_CLUSTER_DESC(NULL, ZB_ZLL_CLUSTER_ID, ZB_ZCL_CLIENT_ROLE),
        ZB_ZCL_CLUSTER_DESC(NULL, ZB_ON_OFF_CLUSTER_ID, ZB_ZCL_CLIENT_ROLE),
    }  
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT Zigbee ZLL example application");

    zb_zcl_init_ep(&remote_ep);

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
