/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-FileCopyrightText: 2018 Federico Pellegrin <fede@evolware.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       riotboot bootloader test
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Federico Pellegrin <fede@evolware.org>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "riotboot/slot.h"
#include "shell.h"

static int cmd_print_slot_nr(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Current slot=%d\n", riotboot_slot_current());
    return 0;
}

static int cmd_print_slot_hdr(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int current_slot = riotboot_slot_current();
    riotboot_slot_print_hdr(current_slot);
    return 0;
}

static int cmd_print_slot_addr(int argc, char **argv)
{
    (void)argc;

    int reqslot=atoi(argv[1]);
    printf("Slot %d address=0x%08" PRIx32 "\n",
           reqslot, riotboot_slot_get_image_startaddr(reqslot));
    return 0;
}

static int cmd_dumpaddrs(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    riotboot_slot_dump_addrs();
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "curslotnr", "Print current slot number", cmd_print_slot_nr },
    { "curslothdr", "Print current slot header", cmd_print_slot_hdr },
    { "getslotaddr", "Print address of requested slot", cmd_print_slot_addr },
    { "dumpaddrs", "Prints all slot data in header", cmd_dumpaddrs },
    { NULL, NULL, NULL }
};

int main(void)
{
    int current_slot;

    puts("Hello riotboot!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s CPU.\n", RIOT_CPU);

    /* print some information about the running image */
    current_slot = riotboot_slot_current();
    if (current_slot != -1) {
        printf("riotboot_test: running from slot %d\n", current_slot);
        riotboot_slot_print_hdr(current_slot);
    }
    else {
        printf("[FAILED] You're not running riotboot\n");
    }

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
