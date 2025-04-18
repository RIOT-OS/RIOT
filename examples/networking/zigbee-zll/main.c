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

#define ENABLE_DEBUG (0)
#include "debug.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* these commands are provided by the zboss stack */
int zb_input_packet(int argc, char **argv);
SHELL_COMMAND(zigp, "input a zigbee packet like 030806ffffffff07", zb_input_packet);

int zb_inject_packet(int argc, char **argv);
SHELL_COMMAND(inj, "inject a raw 802.15.4 packet like 030885ffffffff07 ",zb_inject_packet);

int cmd_zconfig(int argc, char *argv[]);
SHELL_COMMAND(zconfig, "ZBOSS configuration", cmd_zconfig);


int zcl_toggle(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    zcl_onoff_toggle();
    return 0;
}
SHELL_COMMAND(zcl_toggle, "sending zcl command on", zcl_toggle);

int tl_scan(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    zdo_zll_touchlink_scan();
    return 0;
}
SHELL_COMMAND(tl_scan, "manual touchlink scan", tl_scan);

int set_channel(int argc, char **argv)
{
    if(argc != 2){
        puts("set_channel: invalid args");
        return 1;
    }
    if (strlen(argv[1]) > 2) {
        puts("set_channel: invalid channel number");
        return 1;
    }
     ZB_TRANSCEIVER_SET_CHANNEL(strtol(argv[1], NULL, 10));
     return 0;
}
SHELL_COMMAND(set_ch, "set channel to tune into", set_channel);

/* handle an indication from the zboss stack that we've received some data */
void zb_data_indication(zb_uint8_t param)
{
    /* get the packet buffer corresponding to param */
    zb_buf_t *buf = ZB_BUF_FROM_REF(param);

    /* get the indication parameters attached to the packet buffer */
    zb_apsde_data_indication_t *ind = ZB_GET_BUF_PARAM(buf,
                                                    zb_apsde_data_indication_t);
    DEBUG("got indication 0x%04x\n", ind->clusterid);

    /* set pointers to the beginnings of the aps and zcl payloads */
    zb_uint8_t *aps = ZB_BUF_BEGIN(buf);
    zb_uint8_t *zcl = aps + zb_aps_full_hdr_size(aps);
    uint8_t zcl_command;
    switch (ind->clusterid) {
        case 0x0019: /* zdo start */
            /* don't free buf */
            break;
        case 0x0003: /* identify */
            puts("got identify request");
            zb_free_buf(buf);
            break;
        case 0x0021:
            puts("got binding req in here");
            break;
        case 0x0022:
            puts("got unbind req");
            break;
        case 0x0004: /* groups */
            zb_zcl_handle_group_request(param);
            break;
        case 0x0006: /* on/off */
            zcl_command = zcl[2];
            switch (zcl_command) {
                case 0x0:
                    LED0_OFF;
                    puts("OFF");
                    break;
                case 0x1:
                    LED0_ON;
                    puts("ON");
                    break;
                case 0x2:
                    LED0_TOGGLE;
                    puts("TOGGLE");
                    break;
                default:
                    printf("unhandled on/off command: 0x%x\n", zcl_command);
            }
            zb_free_buf(buf);
            break;

        case 0x0008: /* level control */
            zcl_command = zcl[2];
            switch (zcl_command) {
                default:
                    printf("unhandled level control command: 0x%x\n", zcl_command);
            }
            zb_free_buf(buf);
            break;

        case 0x0005: /* color change */
            zcl_command = zcl[4];
            switch (zcl_command) {
                default:
                    printf("unhandled color command: 0x%x\n", zcl_command);
            }
            zb_free_buf(buf);
            break;

        default:
            printf("unhandled cluster: 0x%x\n", ind->clusterid);
            zb_free_buf(buf);
    }
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT Zigbee ZLL example application");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    //zdo_bind();
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
