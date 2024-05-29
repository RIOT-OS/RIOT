#include <stdio.h>
#include <msg.h>
#include <shell.h>
#include <errno.h>
#include "native_internal.h"
#include "async_read.h"

#define ENABLE_DEBUG 0
#include "debug.h"

extern void async_shell_on_char(char ch);

static void io_signal_handler(int fd, void *arg) {
    (void) arg;

    while (1) {
        char c;
        int status = real_read(fd, &c, 1);

        if (status == 1) {
            async_shell_on_char(c);
        } else {
            if (status == -1 && errno != EAGAIN) {
                DEBUG("error: cannot read from fd\n");
            }

            async_shell_on_char('\0');
            break;
        }
    }

    native_async_read_continue(fd);
}

static bool init_shell_done = false;
static msg_t msg_queue[16];

int async_shell_init(void) {
    if (!init_shell_done) {
        init_shell_done = true;
    } else {
        DEBUG("error: already initialized\n");
        return 1;
    }

    // we need a message queue for the thread running the shell in order to
    // receive potentially fast incoming networking packets
    msg_init_queue(msg_queue, sizeof(msg_queue) / sizeof(msg_queue[0]));

    native_async_read_setup();
    native_async_read_add_handler(0, NULL, io_signal_handler);

    return 0;
}

size_t async_shell_bufsize(void) {
    return SHELL_DEFAULT_BUFSIZE;
}

void async_shell_prompt(char *tag, bool highlight) {
    char *tg = tag == NULL ? "" : tag;

    if (highlight) {
        printf("\033[01;31m%s>\033[00m ", tg);
    } else {
        printf("%s> ", tg);
    }
}

// for RIOT-c app commands
static const shell_command_t app_commands[] = {
    //{ "gcoap", "CoAP example", gcoap_cli_cmd },
    { NULL, NULL, NULL }
};

const shell_command_t * async_shell_app_commands(void) {
    return app_commands;
}