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

SHELL_COMMAND(reset, "reset the device and perform reboot", reset);

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
    printf("sending toggle to %04x at endpoint %d...\n", addr, ep);
    zb_zcl_send_on_off_toggle(addr, ep, 1, true);
    return 0;
}
SHELL_COMMAND(zcl_toggle, "sending zcl command on", zcl_toggle);

int tl_scan(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    zll_start_tl_scan();
    return 0;
}
SHELL_COMMAND(tl_scan, "manual touchlink scan", tl_scan);

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
