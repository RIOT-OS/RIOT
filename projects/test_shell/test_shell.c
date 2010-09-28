/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <uart0.h>
#include <posix_io.h>
#include <shell.h>

void print_teststart(char* str) {
    printf("[TEST_START]\n");
}

void print_testend(char* str) {
    printf("[TEST_END]\n");
}

//extern int uart0_init();
//extern int uart0_handler_pid;

int shell_readc() {
    char c = 0;
    posix_read(uart0_handler_pid, &c, 1);
    return c;
}

int main(void) {
    //printf("Moin. build on %s %s SVN-Revision: %s\n", kernel_builddate, kernel_buildtime, kernel_svnrevision);
    printf("test_shell.\n");

    uart0_init();

    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_readc);
    shell_auto_init(&shell);
    
    shell_register_cmd(&shell, "start_test", print_teststart);
    shell_register_cmd(&shell, "end_test", print_testend);
    
    shell_run(&shell);

    return 0;
}


