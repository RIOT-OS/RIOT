/*
 * Copyright (C) 2013, 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @file
 * @brief UDP RPL example application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "net_if.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"
#include "udp.h"

#include "rpl_udp.h"

const shell_command_t shell_commands[] = {
    {"init", "Initialize network", rpl_udp_init},
    {"set", "Set ID", rpl_udp_set_id},
    {"dodag", "Shows the dodag", rpl_udp_dodag},
    {"server", "Starts a UDP server", udp_server},
    {"send", "Send a UDP datagram", udp_send},
    {"ign", "Ignore a node", rpl_udp_ignore},
    {NULL, NULL, NULL}
};
shell_t shell;
int main(void)
{
    puts("RPL router v"APP_VERSION);

    /* start shell */
    posix_open(uart0_handler_pid, 0);
    net_if_set_src_address_mode(0, NET_IF_TRANS_ADDR_M_SHORT);

    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);

    shell_run(&shell);
    return 0;
}
/*
> ps
        pid | name                 | state    Q | pri | stack ( used) location
          1 | idle                 | pending  Q |  15 |   512 (  160) 0x2000096c
          2 | main                 | running  Q |   7 |  2176 ( 1656) 0x200000ec
          3 | uart0                | bl rx    _ |   6 |   512 (  296) 0x20000cbc
          4 | Transceiver          | bl rx    _ |   4 |  1024 (  488) 0x20005670
          5 | radio                | bl rx    _ |   5 |   512 (  300) 0x20005058
          6 | ip_process           | bl rx    _ |   6 |   512 (  308) 0x20004070
          7 | lowpan_context_rem   | bl mutex _ |   8 |   512 (  344) 0x20004de0
          8 | lowpan_transfer      | sleeping _ |   6 |  2048 (  196) 0x20004278
          9 | udp_packet_handler   | bl rx    _ |   7 |  2176 (  272) 0x20005c4c
         10 | �r                   | sleeping _ | 65533 | 268435456 (268435456) 0x10000000
         11 | �r                   | sleeping _ | 65533 | 268435456 (268435456) 0x10000000
        268 | �r                   | sleeping _ | 65533 | 268435456 (268435456) 0x10000000
         13 | rt_timer_over        | bl mutex _ |   6 |   256 (  256) 0x2000109c
         14 | rpl_process          | bl rx    _ |   6 |  2048 (  248) 0x20002c14
         15 | monitor              | bl rx    _ |   5 |   512 (  252) 0x20001e38
         16 | init_udp_server      | bl rx    _ |   7 |   512 (  508) 0x20002060
            | SUM                  |            |     | 805319680 (805311652)

/*
    List of changed stacksizes:
    TRANSCEIVER_STACK_SIZE : KERNEL_CONF_STACKSIZE_MAIN    -> 512
    RADIO_STACK_SIZE       : KERNEL_CONF_STACKSIZE_MAIN    -> 1024
    UART0_STACKSIZE        : KERNEL_CONF_STACKSIZE_DEFAULT -> 512
    IP_PROCESS_STACKSIZE   : KERNEL_CONF_STACKSIZE_DEFAULT -> 1024
    CON_STACKSIZE          : KERNEL_CONF_STACKSIZE_DEFAULT -> 512 + 256
    LOWPAN_TRANSFER_BUF_STACKSIZE : KERNEL_CONF_STACKSIZE_DEFAULT
    udp_server_stack_buffer: KERNEL_CONF_STACKSIZE_MAIN    -> 512
    MONITOR_STACK_SIZE     : KERNEL_CONF_STACKSIZE_MAIN    -> 512

    ETX_BEACON_STACKSIZE   : KERNEL_CONF_STACKSIZE_MAIN    -> 512
    ETX_RADIO_STACKSIZE    : KERNEL_CONF_STACKSIZE_MAIN    -> 128
    ETX_CLOCK_STACKSIZE    : KERNEL_CONF_STACKSIZE_MAIN    -> 128

    TRICKLE_TIMER_STACKSIZE: KERNEL_CONF_STACKSIZE_DEFAULT -> 256
    TRICKLE_INTERVAL_STACKSIZE: KERNEL_CONF_STACKSIZE_MAIN -> 256
    DAO_DELAY_STACKSIZE    : KERNEL_CONF_STACKSIZE_MAIN    -> 256
    RT_STACKSIZE           : KERNEL_CONF_STACKSIZE_DEFAULT -> 512

    RPL_PROCESS_STACKSIZE  : KERNEL_CONF_STACKSIZE_DEFAULT -> 512
    LOWPAN_TRANSFER_BUF_STACKSIZE : KERNEL_CONF_STACKSIZE_DEFAULT -> 1024
    UDP_STACK_SIZE         : KERNEL_CONF_STACKSIZE_MAIN    -> 1024
    
    Other buffers
    ETX_MAX_CANDIDATE_NEIGHBORS : 40 -> 5
    TRANSCEIVER_BUFFER_SIZE     : 64 -> 10

*/