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

#include "zb_common.h"
#include "zb_aps.h"
#include "zb_zcl.h"
#include "zb_zdo.h"
#include "zb_g_context.h"
#include "zb_zcl_groups.h"
#include "zb_zcl_on_off.h"
#include "zb_zcl_basic.h"
#include "zb_secur_api.h"
#include "zb_osif.h"

#include "od.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define APPLICATION_ENDPOINT 11

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static uint8_t key[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
};

static zb_zcl_basic_srv_attr_t basic_attrs;

int tl_scan(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    zll_start_tl_scan();
    return 0;
}
SHELL_COMMAND(tl_scan, "manual touchlink scan", tl_scan);
// extern int cmd_zconfig(int argc, char *argv[]);
// SHELL_COMMAND(zconfig, "", cmd_zconfig);
int main(void)
{
    /* set our key */
    zb_secur_setup_preconfigured_key(key, 0);

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

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
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
