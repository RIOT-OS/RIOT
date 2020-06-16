/*
 * Copyright (C) 2020 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* needed for fdopen */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "rshell.h"

#include "log.h"
#include "msg.h"
#include "vfs.h"
#include "shell.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SERVER_MSG_QUEUE_SIZE   (8)

static char sending_stack[1024];
static char shell_thread_stack[1600];

static msg_t _shell_msg_queue[SERVER_MSG_QUEUE_SIZE];

#define BLE_MAX_PAYLOAD_SIZE (20) /* 46 max to avoid fragmenting */

static ssize_t _stdout_write(vfs_file_t *filp, const void *src, size_t nbytes)
{
    rshell_t *ctx = filp->private_data.ptr;

    DEBUG("[telnet] _stdout_write() %u bytes\n", nbytes);

    return pipe_write(&ctx->stdout_pipe, src, nbytes);
}

static ssize_t _stdin_read(vfs_file_t *filp, void *dest, size_t nbytes)
{
    rshell_t *ctx = filp->private_data.ptr;

    ssize_t bytes_read = pipe_read(&ctx->stdin_pipe, dest, nbytes);

    DEBUG("[telnet] _stdout_read() %u bytes\n", bytes_read);
    return bytes_read;
}

static vfs_file_ops_t _stdout_ops = {
    .write = _stdout_write,
};

static vfs_file_ops_t _stdin_ops = {
    .read = _stdin_read,
};

static void *_shell_thread(void *arg)
{
    rshell_t *ctx = arg;

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_shell_msg_queue, SERVER_MSG_QUEUE_SIZE);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run_once(ctx->shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    char cd[] = {'C' - '@', 'D' - '@'};
    pipe_write(&ctx->stdout_pipe, &cd, sizeof(cd));

    fclose(stdin);
    fclose(stdout);

    return NULL;
}

static void *_sending_thread(void *arg)
{
    rshell_t *ctx = arg;

    char buffer[BLE_MAX_PAYLOAD_SIZE];
    while (1) {
        ssize_t bytes_read = pipe_read(&ctx->stdout_pipe, buffer, sizeof(buffer));
        DEBUG("[telnet] _sending_thread() %u bytes\n", bytes_read);

        /* exit on EOT */
        for(ssize_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == ('D' - '@')) {
                return NULL;
            }
        }

        ctx->tx_cb(buffer, bytes_read, ctx->tx_cb_arg);
    }

    return NULL;
}

int rshell_disconnect_client(rshell_t *ctx)
{
    (void)ctx;

    /* send EOT to shell to make it quit */
    char cd[] = {'C' - '@', 'D' - '@'};
    pipe_write(&ctx->stdin_pipe, &cd, sizeof(cd));

    /* and send to sending thread to make it quit */
    pipe_write(&ctx->stdout_pipe, &cd, sizeof(cd));

    return 0;
}

int rshell_accept_client(rshell_t *ctx)
{
    /* sending thread */
    if (thread_create(sending_stack, sizeof(sending_stack),
                                            THREAD_PRIORITY_MAIN - 1,
                                            THREAD_CREATE_STACKTEST,
                                            _sending_thread, ctx,
                                            "BLE shell TX"
                     ) <= KERNEL_PID_UNDEF) {
        LOG_ERROR("[telnet] error initializing thread");
        return 1;
    }

    /* set up pipe buffers for stdin and stdout */
    ringbuffer_init(&ctx->stdin_ringbuffer, ctx->stdin_pipe_buf,
                                                sizeof(ctx->stdin_pipe_buf));
    ringbuffer_init(&ctx->stdout_ringbuffer, ctx->stdout_pipe_buf,
                                                sizeof(ctx->stdout_pipe_buf));
    pipe_init(&ctx->stdin_pipe, &ctx->stdin_ringbuffer, NULL);
    pipe_init(&ctx->stdout_pipe, &ctx->stdout_ringbuffer, NULL);

    /* bind to file descriptors for stdin/stdout over the remote link */
    int in_fd = vfs_bind(VFS_ANY_FD, O_RDONLY, &_stdin_ops, ctx);
    int out_fd = vfs_bind(VFS_ANY_FD, O_WRONLY, &_stdout_ops, ctx);
    DEBUG("[telnet] in fd: %i\n", in_fd);
    DEBUG("[telnet] out fd: %i\n", out_fd);

    /* store the current stdin stdout */
    FILE *stdin_real = stdin;
    FILE *stdout_real = stdout;
    DEBUG("[telnet] stdin_real: 0x%lx\n", (uint32_t)stdin_real);
    DEBUG("[telnet] stdout_real: 0x%lx\n", (uint32_t)stdout_real);


    /* set stdin/stdout to the remote link */
    stdin = (FILE *)fdopen(in_fd, "r");
    stdout = (FILE *)fdopen(out_fd, "w");

    thread_create(shell_thread_stack, sizeof(shell_thread_stack),
                                        THREAD_PRIORITY_MAIN,
                                        THREAD_CREATE_STACKTEST,
                                        _shell_thread,
                                        ctx, "BLE shell");

    /* set them back as before */
    stdin = stdin_real;
    stdout = stdout_real;

    return 0;
}

int rshell_rx_cb(rshell_t *ctx, const char *data, uint8_t len)
{
    for (int i = 0; i < len; i++) {
        /* don't pass ^D */
        if (data[i] == 'D' - '@') {
            continue;
        }

        pipe_write(&ctx->stdin_pipe, data + i, 1);
    }
    return 0;
}

int rshell_init(rshell_t* ctx, rshell_tx_cb_t tx_cb, void *tx_cb_arg,
                                        const shell_command_t *shell_commands)
{
    ctx->tx_cb = tx_cb;
    ctx->tx_cb_arg = tx_cb_arg;
    ctx->shell_commands = shell_commands;
    return 0;
}
