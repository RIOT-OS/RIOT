/*
 * testShell.c
 *
 *  Created on: 05.06.2012
 *      Author: julsch
 */

#include "testShell.h"
#include "testSHT11.h"
#include "testLTC4150.h"

//global variables
char shell_stack[MINIMUM_STACK_SIZE+4096];
shell_t shell;
int shell_pid;

//#ifdef _TESTSHELL_H_
extern void printAverageCurrent(void);
extern void printCurrent(void);
//#endif

void print_teststart(char* str) {
    printf("[TEST_START]\n");
}

void print_testend(char* str) {
    printf("[TEST_END]\n");
}

void print_temp(char* str) {
	printMeasurement(TEMPERATURE);
}

void print_avg_current(void) {
	printAverageCurrent();
}

void print_current(void) {
	printCurrent();
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
    {"start", "", print_teststart},
    {"end", "", print_testend},
    {"temp","",print_temp},
    {"curLTC","",print_current},
    {"avgLTC","",print_avg_current},
    {"upLTC","",startCurrentMeasurement},
    {"cur_LEDON","",printCurrentWithLED},
    {"cur_LEDOFF","",printCurrentWithoutLED},
    {"ps","",thread_print_all},
    {NULL, NULL, NULL}
};

void shell_runner(void) {

	posix_open(uart0_handler_pid, 0);
    printf("INIT SHELL.\n");
	shell_init(&shell,shell_commands, shell_readc, shell_putchar);
	printf("RUN SHELL.\n");
	shell_run(&shell);
}

void startShell(void) {
    //printf("Moin. build on %s %s SVN-Revision: %s\n", kernel_builddate, kernel_buildtime, kernel_svnrevision);
    printf("My Shell.\n");

    board_uart0_init();

    //posix_open(uart0_handler_pid, 0);
    //printf("INIT SHELL.\n");
    //shell_init(&shell,shell_commands, shell_readc, shell_putchar);
    //printf("RUN SHELL.\n");
    //shell_run(&shell);

    // Does not work, maybe the wrong thread is active when entering a command
    shell_pid = thread_create(shell_stack,sizeof(shell_stack),PRIORITY_MAIN-1,CREATE_STACKTEST,shell_runner,"s1");
}



