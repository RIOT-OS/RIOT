/**
 * Native uart0 implementation
 *
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup native_board
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/select.h>

#include "cpu.h"
#include "board_uart0.h"
#include "thread.h"

#include "native_internal.h"
#include "native_fildes.h"
#include "board_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int _native_uart_sock;
int _native_uart_conn;
_native_fd_t _native_uart_fd = -1;

int _native_replay_enabled;
FILE *_native_replay_buffer;

fd_set _native_uart_rfds;

/* uart API */

int uart0_puts(char *astring, int length)
{
    int nwritten, offset;

    nwritten = 0;
    offset = 0;

    while (
            (length - offset > 0) && (
                (nwritten = _native_write(
                               STDOUT_FILENO,
                               astring+offset,
                               length-offset)
                ) > 0)
          ) {
        offset += nwritten;
    }
    if (nwritten == -1) {
        err(EXIT_FAILURE, "uart0_puts: write");
    }
    else if ((length > 0) && (nwritten == 0)) {
        /* XXX: handle properly */
        errx(EXIT_FAILURE, "uart0_puts: Could not write to stdout. I don't know what to do now.");
    }

    return length;
}

/* internal */

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

#ifndef UART_TCPPORT
#define UART_TCPPORT "4711"
#endif
int init_tcp_socket(char *tcpport)
{
    struct addrinfo hints, *info, *p;
    int i, s = -1;
    if (tcpport == NULL) {
        tcpport = UART_TCPPORT;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((i = real_getaddrinfo(NULL, tcpport, &hints, &info)) != 0) {
        errx(EXIT_FAILURE,
                "init_uart_socket: getaddrinfo: %s", real_gai_strerror(i));
    }

    for (p = info; p != NULL; p = p->ai_next) {
        if ((s = real_socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            warn("init_uart_socket: socket");
            continue;
        }

        i = 1;
        if (real_setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) == -1) {
            err(EXIT_FAILURE, "init_uart_socket: setsockopt");
        }

        if (real_bind(s, p->ai_addr, p->ai_addrlen) == -1) {
            real_close(s);
            warn("init_uart_socket: bind");
            continue;
        }

        break;
    }
    if (p == NULL)  {
        errx(EXIT_FAILURE, "init_uart_socket: failed to bind\n");
    }
    real_freeaddrinfo(info);

    if (real_listen(s, 1) == -1) {
        err(EXIT_FAILURE, "init_uart_socket: listen");
    }

    return s;
}

int init_unix_socket(void)
{
    int s;
    struct sockaddr_un sa;

    if ((s = real_socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        err(EXIT_FAILURE, "init_unix_socket: socket");
    }

    sa.sun_family = AF_UNIX;

    if (_native_unix_socket_path != NULL) {
        snprintf(sa.sun_path, sizeof(sa.sun_path), "%s", _native_unix_socket_path);
    }
    else {
        snprintf(sa.sun_path, sizeof(sa.sun_path), "/tmp/riot.tty.%d", _native_pid);
    }
    real_unlink(sa.sun_path); /* remove stale socket */
    if (real_bind(s, (struct sockaddr *)&sa, SUN_LEN(&sa)) == -1) {
        err(EXIT_FAILURE, "init_unix_socket: bind");
    }

    if (real_listen(s, 5) == -1) {
        err(EXIT_FAILURE, "init_unix_socket: listen");
    }

    return s;
}

void handle_uart_in(void)
{
    char buf[42];
    int nread;

    DEBUG("handle_uart_in\n");

    nread = _native_read(STDIN_FILENO, buf, sizeof(buf));
    if (nread == -1) {
        err(EXIT_FAILURE, "handle_uart_in(): read()");
    }
    else if (nread == 0) {
        /* end of file / socket closed */
        if (_native_uart_conn != 0) {
            if (_native_null_out_file != -1) {
                if (real_dup2(_native_null_out_file, STDOUT_FILENO) == -1) {
                    err(EXIT_FAILURE, "handle_uart_in: dup2(STDOUT_FILENO)");
                }
            }
            if (real_dup2(_native_null_in_pipe[0], STDIN_FILENO) == -1) {
                err(EXIT_FAILURE, "handle_uart_in: dup2(STDIN_FILENO)");
            }
            _native_uart_conn = 0;
            warnx("closed stdio");
        }
        else {
            errx(EXIT_FAILURE, "handle_uart_in: unhandled situation!");
        }
    }
    for (int pos = 0; pos < nread; pos++) {
        uart0_handle_incoming(buf[pos]);
    }
    uart0_notify_thread();

    thread_yield();
}

void handle_uart_sock(void)
{
    int s;
    socklen_t t;
    struct sockaddr remote;

    t = sizeof(remote);

    _native_syscall_enter();
    if ((s = real_accept(_native_uart_sock, &remote, &t)) == -1) {
        err(EXIT_FAILURE, "handle_uart_sock: accept");
    }
    else {
        warnx("handle_uart_sock: successfully accepted socket");
        if (_native_uart_fd != -1) {
            _native_fd_remove(_native_uart_fd);
        }
        _native_uart_fd = _native_fd_add(s);
    }

    if (real_dup2(s, STDOUT_FILENO) == -1) {
        err(EXIT_FAILURE, "handle_uart_sock: dup2()");
    }
    if (real_dup2(s, STDIN_FILENO) == -1) {
        err(EXIT_FAILURE, "handle_uart_sock: dup2()");
    }

    /* play back log from last position */
    if (_native_replay_enabled) {
        warnx("handle_uart_sock: replaying buffer");
        size_t nread;
        char buf[200];
        while ((nread = real_fread(buf, 1, sizeof(buf), _native_replay_buffer)) != 0) {
            int nwritten;
            int pos = 0;
            while ((nwritten = real_write(STDOUT_FILENO, &buf[pos], nread)) != -1) {
                nread -= nwritten;
                pos += nwritten;
                if (nread == 0) {
                    break;
                }
            }
            if (nwritten == -1) {
                err(EXIT_FAILURE, "handle_uart_sock: write");
            }
        }
        if (real_feof(_native_replay_buffer) != 0) {
            real_clearerr(_native_replay_buffer);
        }
        else if (real_ferror(_native_replay_buffer) != 0) {
            err(EXIT_FAILURE, "handle_uart_sock(): fread()");
        }
    }

    _native_syscall_leave();

    _native_uart_conn = s;
}

#ifdef MODULE_UART0
void _native_handle_uart0_input(void)
{
    if (FD_ISSET(STDIN_FILENO, &_native_rfds)) {
        handle_uart_in();
    }
    else if ((_native_uart_sock != -1) && (FD_ISSET(_native_uart_sock, &_native_rfds))) {
        handle_uart_sock();
    }
    else {
        DEBUG("_native_handle_uart0_input - nothing to do\n");
    }
}

int _native_set_uart_fds(void)
{
    DEBUG("_native_set_uart_fds\n");
    FD_SET(STDIN_FILENO, &_native_rfds);
    if (_native_uart_sock == -1) {
        return (STDIN_FILENO);
    }
    else {
        FD_SET(_native_uart_sock, &_native_rfds);
        return ((STDIN_FILENO > _native_uart_sock) ? STDIN_FILENO : _native_uart_sock);
    }
}
#endif

void _native_init_uart0(char *stdiotype, char *ioparam, int replay)
{
    _native_replay_enabled = replay;

    if (_native_replay_enabled) {
        char stdout_logname[255];
        snprintf(stdout_logname, sizeof(stdout_logname), "/tmp/riot.stdout.%d", _native_pid);
        if ((_native_replay_buffer = real_fopen(stdout_logname, "r+")) == NULL) {
            err(EXIT_FAILURE, "_native_init_uart0: fdopen(_native_null_out_file)");
        }

        int fd = real_fileno(_native_replay_buffer);
        if (fd == -1) {
            err(EXIT_FAILURE, "_native_init_uart0: fileno(_native_replay_buffer)");
        }
        (void) _native_fd_add(fd); /* _native_replay_buffer is not touched again */
    }

    if (strcmp(stdiotype, "tcp") == 0) {
        _native_uart_sock = init_tcp_socket(ioparam);
    }
    else if (strcmp(stdiotype, "unix") == 0) {
        _native_uart_sock = init_unix_socket();
    }
    else if (strcmp(stdiotype, "stdio") == 0) {
        _native_uart_sock = -1;
        _native_uart_conn = 1;
    }
    else if (strcmp(stdiotype, "null") == 0) {
        _native_uart_sock = -1;
        _native_uart_conn = 0;
    }
    else {
        errx(EXIT_FAILURE, "_native_init_uart0: unknown stdio type");
    }

    if (_native_uart_sock != -1) {
        (void) _native_fd_add(_native_uart_sock); /* don't need this fd */
    }

    puts("RIOT native uart0 initialized.");
}
