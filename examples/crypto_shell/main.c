/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       UDP Shell application
 *
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"
#include "kernel.h"
#include "crypto.h"

const shell_command_t shell_commands[] = {
    {"encrypt", "encrypts a given string", shell_cipher_encrypt},
    {"decrypt", "decrypts a given hexadecimal cyphertext", shell_cipher_decrypt},
    { NULL, NULL, NULL }
};


int main(void)
{
    puts("Crypto Shell");

    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, 1024, uart0_readc, uart0_putc);

    shell_run(&shell);

    return 0;
}
