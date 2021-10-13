/*

    Author: Kaspar Schleiser <kaspar@schleiser.de>

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "serial.h"
#include "pthread.h"
#include "download.h"

int tty_fd;
int stopped = 0;
char *port_name = "/dev/ttyUSB1";
pthread_t serial_reader;

void *serial_reader_func(void *arg)
{
    unsigned char buf[255];

    while (1) {
        int n = read_serial_port(buf, sizeof(buf));

        if (n > 0) {
            write(tty_fd, buf, n);
        }
    }
}

int init()
{
    int result = open_serial_port(port_name);
    pthread_create(&serial_reader, NULL, serial_reader_func, NULL);
    hard_reset_to_user_code();
    return result;
}

struct termios old_term_setting;

void close_tty()
{
    tcsetattr(tty_fd, TCSANOW, &old_term_setting);
}

void sig_handler(int signal)
{
    if (signal == SIGUSR1) {
        if (stopped) {
            stopped = 0;
            printf("\nSignal received, opening port.\r\n");

            if (init() < 0) {
                printf("Cannot open port.\r\n");
                close_tty();
                exit(1);
            }
        }
    }
    else if (signal == SIGUSR2) {
        if (!stopped) {
            stopped = 1;
            printf("\nSignal received, closing port. \r\n");
            pthread_cancel(serial_reader);
            close_serial_port();
        }
    }
    else if (signal == SIGINT) {
        printf("SIGINT received, exiting...\n");
        pthread_cancel(serial_reader);
        close_serial_port();
        close_tty();
        exit(0);
    }
}

int open_tty(void)
{
    int r, fd;
    struct termios term_setting;

    fd = open("/dev/tty", O_RDWR);

    if (fd < 0) {
        return -1;
    }

    r = tcgetattr(fd, &term_setting);

    if (r != 0) {
        return -2;
    }

    old_term_setting = term_setting;

    term_setting.c_oflag |= (ONLRET);
    term_setting.c_iflag |= (/*IGNBRK |*/ BRKINT | IGNPAR);
    term_setting.c_iflag &= ~(ISTRIP);
    term_setting.c_lflag &= ~(ICANON |/* ISIG |*/ ECHO);
    term_setting.c_lflag |= (ISIG);
    term_setting.c_cflag |= CREAD;
    term_setting.c_cc[VMIN] = 1;
    term_setting.c_cc[VTIME] = 1;
    r = tcsetattr(fd, TCSANOW, &term_setting);

    if (r != 0) {
        return -3;
    }

    return fd;
}

void install_sighandler()
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    sigaddset(&action.sa_mask, SIGUSR1);
    sigaddset(&action.sa_mask, SIGUSR2);
    action.sa_flags = 0;
    action.sa_handler = sig_handler;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
}

int main(int argc, char **argv)
{
    if (argc == 2) {
        port_name = argv[1];
    }

    printf("Using %s as serial device.\n", port_name);

    char ttybuf[255];
    tty_fd = open_tty();

    if (tty_fd < 0) {
        printf("Error opening terminal.\n");
        return (1);
    }

    install_sighandler();

    if (init() < 0) {
        printf("Cannot open port.\r\n");
        exit(1);
    }

    while (1) {
        int n = read(tty_fd, ttybuf, sizeof(ttybuf));
        int i;

        /* check for 0x3 (ctrl-c), clean exit */
        for (i = 0; i < n; i++) {
            if (ttybuf[i] == 0x3) {
                if (i > 0) {
                    write_serial_port(ttybuf, i);
                }

                close_serial_port();
                close_tty();
                system("tset -c");
                return 0;
            }

        }

        write_serial_port(ttybuf, n);
    }

    close_tty();
    close_serial_port();
    return 0;
}
