#include <stdio.h>
#include <string.h>

#include "xtimer.h"
#include "shell.h"
#include "shell_commands.h"

#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"

#define MAIN_QUEUE_SIZE     (8)

int sc_nrf24ctl(int argc, char *argv[]);

static const shell_command_t shell_commands[] = {
    { "nrf24ctl", "Configure an NRF24L01P device", sc_nrf24ctl },
    { NULL, NULL, NULL }
};

static gnrc_netreg_entry_t dump;

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gnrc_netreg_entry_init_pid(&dump, GNRC_NETREG_DEMUX_CTX_ALL,
                               gnrc_pktdump_pid);
#ifdef MODULE_SIXLOWPAN
    if (gnrc_netreg_register(GNRC_NETTYPE_SIXLOWPAN, &dump)) {
        puts("Failed to register packet dumping");
    }
#endif
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
