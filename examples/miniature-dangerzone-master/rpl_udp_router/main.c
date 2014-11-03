#include <stdio.h>

#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"
#include "destiny.h"

#include "demo.h"

const shell_command_t shell_commands[] = {
    {"init", "Initialize network", init},
    {"set", "Set ID", set_id},
    {"table", "", table},
    {"dodag", "", dodag},
    {"loop", "", loop},
    {"server", "Starts a UDP server", udp_server},
    {"send", "Send a UDP datagram", udp_send},
    {"ip", "Print all assigned IP addresses", ip},
    {"ign", "ignore node", ignore},
    {NULL, NULL, NULL}
};

int main(void)
{
    puts("RPL router v"APP_VERSION);

    /* start shell */
    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

    shell_run(&shell);
    return 0;
}
