/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/
#include <ringbuffer.h>
#include <mutex.h>
#include <stdbool.h>
#include <kernel.h>

/**
 * @file
 * @ingroup simple_shell
 * @brief contains implementation of synchronous read functions,
 * @brief hardcoded to uart0
 * @author Kaspar Schleiser
 * 
 */

extern void (*uart0_callback)(int);

#define UART0_BUFSIZE 255
static ringbuffer uart0_rb;
static char uart0_buffer[UART0_BUFSIZE];
static char uart0_waiting = 0;

static mutex_t uart0_mutex;

void uart0_process_byte(int c) {
    rb_add_element(&uart0_rb, c);

    if (uart0_waiting) {
        uart0_waiting = 0;
        mutex_unlock(&uart0_mutex, false);
    }
}

void uart0_init() {
    dINT();
    mutex_init(&uart0_mutex);
    mutex_lock(&uart0_mutex);
    ringbuffer_init(&uart0_rb, uart0_buffer, UART0_BUFSIZE);
    uart0_callback = uart0_process_byte;
    eINT();
}

/** @brief read a char from uart0. Block if none available. */
int uart0_readc() {
    dINT();
    
    if (uart0_rb.avail == 0) {
        uart0_waiting++;
        mutex_lock(&uart0_mutex);
    } 

    int c = rb_get_element(&uart0_rb);    
    
    eINT();

    return c;
}

/** @brief read a char from uart0. return -1 if none available */
int uart0_readc_nb() {
    dINT();
    
    if (uart0_rb.avail == 0) {
        eINT();
        return -1;
    } 

    int c = rb_get_element(&uart0_rb);    
    
    eINT();

    return c;
}

