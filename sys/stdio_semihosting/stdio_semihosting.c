/*
 * Copyright (C) 2018 Phil Wise
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "stdio_uart.h"
#include "xtimer.h"


static uint32_t send_command(int command, void *message);


// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0471g/Bgbjjgij.html

#define SYS_OPEN 0x01
#define SYS_WRITE 0x05
#define SYS_READ 0x06
#define SYS_READC 0x07
#define SYS_ISTTY 0x09


struct read_message {
    uint32_t fd;
    void* data;
    uint32_t length;
};

struct write_message {
    uint32_t fd;
    const void* data;
    uint32_t length;
};

struct open_message {
    const char* filename;
    uint32_t mode;
    uint32_t filename_length;
};

/* -1 means invalid */
int stdout_fd;
int stdin_fd;

static uint32_t send_command(int command, void *message)
{
    uint32_t result;
   __asm__ __volatile__("mov r0, %[cmd];"
       "mov r1, %[msg];"
       "bkpt #0xAB;"
       "mov %[res], r0"
         : [res] "=r" (result)
         : [cmd] "r" (command),
           [msg] "r" (message)
         : "r0", "r1", "memory");
    return result;
}

void stdio_init(void)
{
    struct open_message m;
    uint32_t stdin_is_tty;
    m.filename = ":tt";
    m.filename_length = 3;
    m.mode = 0; // Read
    stdin_fd = send_command(SYS_OPEN, &m);

    m.mode = 4; // Write
    stdout_fd = send_command(SYS_OPEN, &m);

    stdin_is_tty = send_command(SYS_ISTTY, &stdin_fd);
    if (stdin_is_tty == 0) {
        stdin_fd = -1;
        printf("stdin is not a TTY, disabling it\n");
    }
}

ssize_t stdio_read(void* buffer, size_t count)
{
    int bytes_read;
    struct read_message m;
    uint32_t bytes_not_filled;

    if (stdin_fd == -1) {
        printf("Can't read from closed stdin!\n");
        while(1) {
            // We don't have a stdin. No data is coming...
            xtimer_sleep(10);
        }
    }

    m.fd = stdin_fd;
    m.data = buffer;
    m.length = count;
    bytes_not_filled = send_command(SYS_READ, &m);

    bytes_read = count - bytes_not_filled;
    if (bytes_read == 0) {
        stdin_fd = -1;  // Mark as closed
    }
    return bytes_read;
}



ssize_t stdio_write(const void* buffer, size_t len)
{
    struct write_message m;
    m.fd = stdout_fd;
    m.data = buffer;
    m.length = len;

    send_command(SYS_WRITE, &m);
    return len;
}
