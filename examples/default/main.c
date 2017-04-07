/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
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
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "shell.h"
#include "shell_commands.h"

#if FEATURE_PERIPH_RTC
#include "periph/rtc.h"
#endif

#ifdef MODULE_NETIF
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#endif

#include "cpu.h"

static int info(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("RADIO status\n");
    printf("MODE : 0x%08x\n", (int)NRF_RADIO->MODE);
    printf("POWER: 0x%08x\n", (int)NRF_RADIO->POWER);
    printf("STATE: 0x%08x\n", (int)NRF_RADIO->STATE);

    printf("FREQ:  0x%08x\n", (int)NRF_RADIO->FREQUENCY);
    printf("PCNF0: 0x%08x\n", (int)NRF_RADIO->PCNF0);
    printf("PCNF1: 0x%08x\n", (int)NRF_RADIO->PCNF1);
    printf("SFD:   0x%08x\n", (int)NRF_RADIO->SFD);
    printf("TXPOW: 0x%08x\n", (int)NRF_RADIO->TXPOWER);

    printf("MHRMATCHCONF: 0x%08x\n", (int)NRF_RADIO->MHRMATCHCONF);
    printf("MHRMATCHMAS:  0x%08x\n", (int)NRF_RADIO->MHRMATCHMAS);

    return 0;
}


static const shell_command_t shell_commands[] = {
    { "info", "starts a test", info },
    { NULL, NULL, NULL }
};

int main(void)
{
#ifdef FEATURE_PERIPH_RTC
    rtc_init();
#endif

#ifdef MODULE_NETIF
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                          gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);
#endif

    (void) puts("Welcome to RIOT!");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
