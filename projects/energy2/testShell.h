/*
 * testShell.h
 *
 *  Created on: 05.06.2012
 *      Author: julsch
 */

#ifndef TESTSHELL_H_
#define TESTSHELL_H_

#include <posix_io.h>
#include <shell.h>
#include <board_uart0.h>

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <thread.h>
#include <ps.h>


void startShell(void);
void print_teststart(char* str);
void print_testend(char* str);
int shell_readc();
void shell_putchar(int c);

#endif /* TESTSHELL_H_ */
