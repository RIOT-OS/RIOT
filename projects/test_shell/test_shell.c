/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <posix_io.h>
#include <shell.h>
#include <board_uart0.h>


void print_teststart(char* str) {
    printf("[TEST_START]\n");
}

void print_testend(char* str) {
    printf("[TEST_END]\n");
}

int shell_readc() {
    char c = 0;
    posix_read(uart0_handler_pid, &c, 1);
    return c;
}

void shell_putchar(int c) {
    putchar(c);
}

const shell_command_t shell_commands[] = {
    {"start_test", "", print_teststart},
    {"end_test", "", print_testend},
    {NULL, NULL, NULL}
};

int main(void) {
    //printf("Moin. build on %s %s SVN-Revision: %s\n", kernel_builddate, kernel_buildtime, kernel_svnrevision);
    printf("test_shell.\n");

    board_uart0_init();

    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, shell_readc, shell_putchar);

    shell_run(&shell);

    return 0;
}


