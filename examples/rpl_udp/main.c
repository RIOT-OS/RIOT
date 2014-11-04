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
    id = net_if_get_hardware_address(0);

    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);

    shell_run(&shell);
    return 0;
}
/*
> ps                                                                                             
        pid | name                 | state    Q | pri | stack ( used) | location                 
          1 | idle                 | pending  Q |  15 |   512 (  160) | 0x2000097c               
          2 | main                 | running  Q |   7 |  2176 ( 1504) | 0x200000fc               
          3 | uart0                | bl rx    _ |   6 |   512 (  296) | 0x20000cd0               
          4 | Transceiver          | bl rx    _ |   4 |  1024 (  476) | 0x20005958               
          5 | radio                | bl rx    _ |   5 |  1024 (  300) | 0x20005140               
          6 | ip_process           | bl rx    _ |   6 |  1024 (  308) | 0x20004158               
          7 | lowpan_context_rem   | bl mutex _ |   8 |  1024 (  688) | 0x20004cc8               
          8 | lowpan_transfer      | sleeping _ |   6 |  1024 (  196) | 0x20004560               
          9 | udp_packet_handler   | bl rx    _ |   7 |  1024 (  272) | 0x20005f34               
         10 | trickle_timer_over   | sleeping _ |   6 |   512 (  180) | 0x20000f30               
         11 | trickle_interval_over | sleeping _ |   6 |   256 (  188) | 0x20001130              
         12 | dao_delay_over       | sleeping _ |   6 |   512 (  180) | 0x20001230               
         13 | rt_timer_over        | bl mutex _ |   6 |  1024 (  712) | 0x20001430               
         14 | rpl_process          | bl rx    _ |   6 |   512 (  252) | 0x20003300               
         15 | monitor              | bl rx    _ |   5 |   512 (  248) | 0x20002524               
            | SUM                  |            |     | 12672 ( 5960)        
/*
    List of changed stacksizes:
    TRANSCEIVER_STACK_SIZE : KERNEL_CONF_STACKSIZE_MAIN    -> 512
    RADIO_STACK_SIZE       : KERNEL_CONF_STACKSIZE_MAIN    -> 512+256
    UART0_STACKSIZE        : KERNEL_CONF_STACKSIZE_DEFAULT -> 512
    IP_PROCESS_STACKSIZE   : KERNEL_CONF_STACKSIZE_DEFAULT -> 1024
    CON_STACKSIZE          : KERNEL_CONF_STACKSIZE_DEFAULT -> 1024
    LOWPAN_TRANSFER_BUF_STACKSIZE : KERNEL_CONF_STACKSIZE_DEFAULT
    udp_server_stack_buffer: KERNEL_CONF_STACKSIZE_MAIN    -> 512
    MONITOR_STACK_SIZE     : KERNEL_CONF_STACKSIZE_MAIN    -> 512

    ETX_BEACON_STACKSIZE   : KERNEL_CONF_STACKSIZE_MAIN    -> 512
    ETX_RADIO_STACKSIZE    : KERNEL_CONF_STACKSIZE_MAIN    -> 128
    ETX_CLOCK_STACKSIZE    : KERNEL_CONF_STACKSIZE_MAIN    -> 128

    TRICKLE_TIMER_STACKSIZE: KERNEL_CONF_STACKSIZE_DEFAULT -> 1024
    TRICKLE_INTERVAL_STACKSIZE: KERNEL_CONF_STACKSIZE_MAIN -> 256
    DAO_DELAY_STACKSIZE    : KERNEL_CONF_STACKSIZE_MAIN    -> 512
    RT_STACKSIZE           : KERNEL_CONF_STACKSIZE_DEFAULT -> 1024+512

    RPL_PROCESS_STACKSIZE  : KERNEL_CONF_STACKSIZE_DEFAULT -> 1024+512
    LOWPAN_TRANSFER_BUF_STACKSIZE : KERNEL_CONF_STACKSIZE_DEFAULT -> 1024
    UDP_STACK_SIZE         : KERNEL_CONF_STACKSIZE_MAIN    -> 1024
    
    Other buffers
    ETX_MAX_CANDIDATE_NEIGHBORS : 40 -> 5
    TRANSCEIVER_BUFFER_SIZE     : 64 -> 10

*/
