#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <board.h>
#include <time.h>

#include "mrf24j40.h"
#include "mrf24j40_registers.h"
#include "mrf24j40_params.h"
#include "mrf24j40_internal.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "msg.h"

static const shell_command_t shell_commands[] = {
    { "prgs", "print registers", cmd_print_regs },
    { "it", "init transceiver", cmd_its },
    { "send", "send 32 bytes data", cmd_send },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Welcome to RIOT!");

    puts("Starting shell...");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}