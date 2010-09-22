/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <shell.h>

void print_teststart(char* str) {
    printf("[TEST_START]\n");
}

void print_testend(char* str) {
    printf("[TEST_END]\n");
}

extern int uart0_init();
extern int uart0_readc();

int main(void) {
    //printf("Moin. build on %s %s SVN-Revision: %s\n", kernel_builddate, kernel_buildtime, kernel_svnrevision);
    printf("test_shell.\n");

    uart0_init();

    shell_t shell;
    shell_init(&shell, uart0_readc);
    
    shell_register_cmd(&shell, "start_test", print_teststart);
    shell_register_cmd(&shell, "end_test", print_testend);

    shell_run(&shell);

    return 0;
}


