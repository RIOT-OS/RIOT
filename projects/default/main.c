/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <string.h>

#include <posix_io.h>
#include <shell.h>
#include <shell_commands.h>
#include <board_uart0.h>

int shell_readc() {
    char c = 0;
    posix_read(uart0_handler_pid, &c, 1);
    return c;
}

void shell_putchar(int c) {
    putchar(c);
}

int main(void) {
    board_uart0_init();
    posix_open(uart0_handler_pid, 0);
    
    puts("Welcome to ukleos!");

    shell_t shell;
    shell_init(&shell, _shell_command_list, shell_readc, shell_putchar);

    shell_run(&shell);

    return 0;
}


