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
#include "zb_secur_api.h"
#include "zb_osif.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define APPLICATION_ENDPOINT 11

#define INPUT_CL_COUNT 2

#define OUTPUT_CL_COUNT 0

#define MAIN_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

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
ZB_DECLARE_SIMPLE_DESC(2, 0);
static zb_af_simple_desc_2_0_t simple_desc_target = {
    APPLICATION_ENDPOINT,
    ZB_HA_PROFILE_ID,
    ZB_HA_ON_OFF_LIGHT_DEV_ID,
    1,/* app version */
    0,/* reserved */
    INPUT_CL_COUNT,
    OUTPUT_CL_COUNT,
    {
        ZB_BASIC_CLUSTER_ID,
        ZB_ZLL_CLUSTER_ID
    }
};

static zb_af_ep_desc_t target_ep = {
    APPLICATION_ENDPOINT,
    ZB_HA_PROFILE_ID,
    (INPUT_CL_COUNT + OUTPUT_CL_COUNT),
    (zb_af_simple_desc_1_1_t *)&simple_desc_target,
    {
        ZB_ZCL_CLUSTER_DESC(&basic_attrs, ZB_BASIC_CLUSTER_ID, ZB_ZCL_SERVER_ROLE),
        ZB_ZCL_CLUSTER_DESC(NULL, ZB_ZLL_CLUSTER_ID, ZB_ZCL_SERVER_ROLE)
    }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    zb_zcl_init_ep(&target_ep);

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
