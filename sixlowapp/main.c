/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       6LoWPAN example application - main function
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdio.h>

#include "kernel.h"
#include "thread.h"
#include "net_if.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"

#include "sixlowapp.h"

kernel_pid_t sixlowapp_udp_server_pid = KERNEL_PID_UNDEF;

char addr_str[IPV6_MAX_ADDR_STR_LEN];
char monitor_stack_buffer[KERNEL_CONF_STACKSIZE_MAIN];
char udp_server_stack_buffer[KERNEL_CONF_STACKSIZE_MAIN];

const shell_command_t shell_commands[] = {
    {"ping", "Send an ICMPv6 echo request to another node", sixlowapp_send_ping},
    {"nc", "RIOT netcat - arbitrary UDP connections and listens", sixlowapp_netcat},
    {NULL, NULL, NULL}
};

int main(void)
{
    puts("RIOT 6LoWPAN example v"APP_VERSION);

    sixlowpan_lowpan_init_interface(IF_ID);

    /* start thread for monitor mode */
    kernel_pid_t monitor_pid = thread_create(monitor_stack_buffer,
                                             sizeof(monitor_stack_buffer),
                                             PRIORITY_MAIN - 2,
                                             CREATE_STACKTEST,
                                             sixlowapp_monitor, NULL,
                                             "monitor");

    ipv6_register_packet_handler(monitor_pid);

    /* Start the UDP server thread */
    sixlowapp_udp_server_pid = thread_create(udp_server_stack_buffer,
                                             sizeof(udp_server_stack_buffer),
                                             PRIORITY_MAIN, CREATE_STACKTEST,
                                             sixlowapp_udp_server_loop, NULL,
                                             "UDP receiver");

    /* Open the UART0 for the shell */
    posix_open(uart0_handler_pid, 0);
    /* initialize the shell */
    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    /* start the shell loop */
    shell_run(&shell);

    return 0;
}
