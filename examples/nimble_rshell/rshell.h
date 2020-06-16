/*
 * Copyright (C) 2020 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef RSHELL_H_INCLUDED
#define RSHELL_H_INCLUDED

#include "pipe.h"
#include "shell.h"
#include "ringbuffer.h"

#ifndef RSHELL_STDIN_BUF_LEN
#define RSHELL_STDIN_BUF_LEN       (16)
#endif

#ifndef RSHELL_STDOUT_BUF_LEN
#define RSHELL_STDOUT_BUF_LEN       (32)
#endif

typedef struct rshell_s rshell_t;
typedef void (*rshell_tx_cb_t)(const char*, uint8_t, void*);

/**
 * @brief BLE shell struct
 */
struct rshell_s {
    rshell_tx_cb_t tx_cb;
    void *tx_cb_arg;
    const shell_command_t *shell_commands;

    char stdin_pipe_buf[RSHELL_STDIN_BUF_LEN];
    char stdout_pipe_buf[RSHELL_STDOUT_BUF_LEN];
    ringbuffer_t stdin_ringbuffer;
    ringbuffer_t stdout_ringbuffer;
    pipe_t stdin_pipe;
    pipe_t stdout_pipe;
};

int rshell_accept_client(rshell_t *ctx);

int rshell_disconnect_client(rshell_t *ctx);

int rshell_rx_cb(rshell_t *ctx, const char *data, uint8_t len);

int rshell_init(rshell_t *ctx, rshell_tx_cb_t tx_cb, void *tx_cb_arg,
                                        const shell_command_t *shell_commands);

#endif /* RSHELL_H_INCLUDED */
