/*
 * Copyright (C) 2019 Thomas Stilwell <stilwellt@openlabs.co>
 *  Copyright (C) 2025 TU Dresden
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

int zcl_toggle(int argc, char **argv)
{
    if (argc != 3) {
        puts("usage: toggle <short addr> <endpoint>");
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
    printf("sending toggle to %04x at endpoint %d...\n", addr, ep);
    zb_zcl_on_off_send_toggle(ZB_REF_FROM_BUF(buf), ZB_HA_PROFILE_ID, addr, ep, 1);
    return 0;
}
SHELL_COMMAND(zcl_toggle, "sending zcl command on", zcl_toggle);

int tl_scan(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    zll_start_tl_scan();
    return 0;
}
SHELL_COMMAND(tl_scan, "manual touchlink scan", tl_scan);

void set_led(zb_uint8_t param)
{
    if (param) {
        LED0_ON;
    }
    else {
        LED0_OFF;
    }
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
                              ZB_HA_COLOR_LIGHT_DEV_ID,
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
    basic_attrs.power_source = ZB_ZCL_BASIC_POWER_SRC_THREE_P, // 3-Phase power ;)
    basic_attrs.phys_environment = ZB_ZCL_BASIC_PHYS_ENV_POOL, // Its too hot and needs water cooling
    basic_attrs.generic_device_type = ZB_ZCL_BASIC_GEN_DEV_TYPE_CFL; // love the flicker

    char manufacturer[] = "SKYNET";
    basic_attrs.manufacturer_name_len = sizeof(manufacturer);
    ZB_MEMCPY(basic_attrs.manufacturer_name, manufacturer, sizeof(manufacturer));

    char model_id[] = "Terminator";
    basic_attrs.model_id_len = sizeof(model_id);
    ZB_MEMCPY(basic_attrs.model_id, model_id, sizeof(model_id));

    char date[] = "19700101";
    basic_attrs.date_code_len = sizeof(date);
    ZB_MEMCPY(basic_attrs.date_code, date, sizeof(date));

    char location[] = "Backrooms";
    basic_attrs.location_desc_len = sizeof(location);
    ZB_MEMCPY(basic_attrs.location_desc, location, sizeof(location));

    /* stolen from phillips hue white & color */
    basic_attrs.product_code_id = ZB_ZCL_BASIC_PRODUCT_CODE_EAN;
    char ean[] = "8719514291171";
    basic_attrs.product_code_len = sizeof(ean);
    ZB_MEMCPY(basic_attrs.product_code, ean, sizeof(ean));

    basic_attrs.software_build_id_len = sizeof(RIOT_VERSION);
    ZB_MEMCPY(basic_attrs.software_build_id, RIOT_VERSION, sizeof(RIOT_VERSION));

    zb_zcl_on_off_srv_setup(11, &on_off_attrs);
    zb_zcl_groups_srv_setup(11, &group_attrs);
    zb_zcl_basic_srv_setup(11, &basic_attrs);

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    //zdo_bind();
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
