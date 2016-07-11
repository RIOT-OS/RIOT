/**
 * Native CPU entry code
 *
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#include <dlfcn.h>
#else
#include <dlfcn.h>
#endif

#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "kernel_init.h"
#include "cpu.h"
#include "irq.h"

#include "board_internal.h"
#include "native_internal.h"
#include "tty_uart.h"

typedef enum {
    _STDIOTYPE_STDIO = 0,   /**< leave intact */
    _STDIOTYPE_NULL,        /**< redirect to "/dev/null" */
    _STDIOTYPE_FILE,        /**< redirect to file */
} _stdiotype_t;

int _native_null_in_pipe[2];
int _native_null_out_file;
const char *_progname;
char **_native_argv;
pid_t _native_pid;
pid_t _native_id;
unsigned _native_rng_seed = 0;
int _native_rng_mode = 0;
const char *_native_unix_socket_path = NULL;

#ifdef MODULE_NETDEV2_TAP
#include "netdev2_tap_params.h"

netdev2_tap_params_t netdev2_tap_params[NETDEV2_TAP_MAX];
#endif

#ifdef MODULE_MTD_NATIVE
#include "board.h"
#include "mtd_native.h"
#endif

static const char short_opts[] = ":hi:s:deEoc:"
#ifdef MODULE_MTD_NATIVE
    "m:"
#endif
    "";
static const struct option long_opts[] = {
    { "help", no_argument, NULL, 'h' },
    { "id", required_argument, NULL, 'i' },
    { "seed", required_argument, NULL, 's' },
    { "daemonize", no_argument, NULL, 'd' },
    { "stderr-pipe", no_argument, NULL, 'e' },
    { "stderr-noredirect", no_argument, NULL, 'E' },
    { "stdout-pipe", no_argument, NULL, 'o' },
    { "uart-tty", required_argument, NULL, 'c' },
#ifdef MODULE_MTD_NATIVE
    { "mtd", required_argument, NULL, 'm' },
#endif
    { NULL, 0, NULL, '\0' },
};

/**
 * @brief   initialize _native_null_in_pipe to allow for reading from stdin
 *
 * @param[in] stdiotype _STDIOTYPE_STDIO to to just initialize pipe, any other
 *                      value to also redirect stdin to that pipe
 */
void _native_input(_stdiotype_t stdintype)
{
    if (real_pipe(_native_null_in_pipe) == -1) {
        err(EXIT_FAILURE, "_native_null_in(): pipe()");
    }

    if (stdintype == _STDIOTYPE_STDIO) {
        return;
    }

    if (real_dup2(_native_null_in_pipe[0], STDIN_FILENO) == -1) {
        err(EXIT_FAILURE, "_native_null_in: dup2(STDIN_FILENO)");
    }
}

/**
 * @brief   set up output redirection
 *
 * @param[in] stdiotype The type of redirection
 * @param[in] output    Output file. May be either `STDOUT_FILENO` for stdout or
 *                      `STDERR_FILENO` for stderr
 *
 * @return The new file descriptor of the redirection
 * @return -1 if the file descriptor did not change from the standard one
 */
int _native_log_output(_stdiotype_t stdiotype, int output)
{
    int outfile;

    assert((output == STDERR_FILENO) || (output == STDOUT_FILENO));

    switch (stdiotype) {
        case _STDIOTYPE_STDIO:
            return -1;
        case _STDIOTYPE_NULL:
            if ((outfile = real_open("/dev/null", O_WRONLY)) == -1) {
                err(EXIT_FAILURE, "_native_log_output: open");
            }
            break;
        case _STDIOTYPE_FILE: {
            /* 20 should suffice for 64-bit PIDs ;-) */
            char logname[sizeof("/tmp/riot.stderr.") + 20];

            snprintf(logname, sizeof(logname), "/tmp/riot.std%s.%d",
                     (output == STDOUT_FILENO) ? "out": "err", _native_pid);
            if ((outfile = real_creat(logname, 0666)) == -1) {
                err(EXIT_FAILURE, "_native_log_output: open");
            }
            break;
        }
        default:
            errx(EXIT_FAILURE, "_native_log_output: unknown log type");
            break;
    }
    if (real_dup2(outfile, output) == -1) {
        err(EXIT_FAILURE, "_native_log_output: dup2(output)");
    }
    return outfile;
}

void daemonize(void)
{
    if ((_native_pid = real_fork()) == -1) {
        err(EXIT_FAILURE, "daemonize: fork");
    }

    if (_native_pid > 0) {
        real_printf("RIOT pid: %d\n", _native_pid);
        real_exit(EXIT_SUCCESS);
    }
    else {
        _native_pid = real_getpid();

        /* detach from current working directory */
        if (real_chdir("/") == -1) {
            err(EXIT_FAILURE, "daemonize: chdir");
        }

        /* detach from process group */
        if (real_setsid() == -1) {
            err(EXIT_FAILURE, "daemonize: setsid");
        }

        /* set umask */
        real_umask(0);
    }
}

/**
 * Remove any -d options from an argument vector.
 *
 * @param[in][out]  argv    an argument vector
 */
static void filter_daemonize_argv(char **argv)
{
    int idx = 0;
    for (char **narg = argv; *narg != NULL; narg++, idx++) {
        if (strcmp("-d", narg[0]) == 0) {
            char **xarg = narg;
            do {
                xarg[0] = xarg[1];
            } while (*xarg++ != NULL);
            if (optind > 1) {
                /* adapt optind if changed */
                optind--;
            }
            narg--; /* rescan current item to filter out double args */
        }
    }
}

void usage_exit(int status)
{
    real_printf("usage: %s", _progname);

#if defined(MODULE_NETDEV2_TAP)
    for (int i = 0; i < NETDEV2_TAP_MAX; i++) {
        real_printf(" <tap interface %d>", i + 1);
    }
#endif

    real_printf(" [-i <id>] [-d] [-e|-E] [-o] [-c <tty>]\n");

    real_printf(" help: %s -h\n\n", _progname);

    real_printf("\nOptions:\n"
"    -h, --help\n"
"        print this help message\n"
"    -i <id>, --id=<id>\n"
"        specify instance id (set by config module)\n"
"    -s <seed>, --seed=<seed>\n"
"        specify srandom(3) seed (/dev/random is used instead of random(3) if\n"
"        the option is omitted)\n"
"    -d, --daemonize\n"
"        daemonize native instance\n"
"    -e, --stderr-pipe\n"
"        redirect stderr to file\n"
"    -E, --stderr-noredirect\n"
"        do not redirect stderr (i.e. leave sterr unchanged despite\n"
"        daemon/socket io)\n"
"    -o, --stdout-pipe\n"
"        redirect stdout to file (/tmp/riot.stdout.PID) when not attached\n"
"        to socket\n"
"    -c <tty>, --uart-tty=<tty>\n"
"        specify TTY device for UART. This argument can be used multiple\n"
"        times (up to UART_NUMOF)\n");
#ifdef MODULE_MTD_NATIVE
    real_printf(
"    -m <mtd>, --mtd=<mtd>\n"
"       specify the file name of mtd emulated device\n");
#endif
    real_exit(status);
}

__attribute__((constructor)) static void startup(int argc, char **argv)
{
    _native_init_syscalls();

    _native_argv = argv;
    _progname = argv[0];
    _native_pid = real_getpid();

    /* will possibly be overridden via option below: */
    _native_id = _native_pid;

    int c, opt_idx = 0, uart = 0;
    bool dmn = false, force_stderr = false;
    _stdiotype_t stderrtype = _STDIOTYPE_STDIO;
    _stdiotype_t stdouttype = _STDIOTYPE_STDIO;
    _stdiotype_t stdintype = _STDIOTYPE_STDIO;

    while ((c = getopt_long(argc, argv, short_opts, long_opts, &opt_idx)) >= 0) {
        switch (c) {
            case 0:
            case 'h':
                usage_exit(EXIT_SUCCESS);
            case 'i':
                _native_id = atol(optarg);
                break;
            case 's':
                _native_rng_seed = atol(optarg);
                _native_rng_mode = 1;
                break;
            case 'd':
                dmn = true;
                break;
            case 'e':
                if (force_stderr) {
                    /* -e and -E are mutually exclusive */
                    usage_exit(EXIT_FAILURE);
                }
                stderrtype = _STDIOTYPE_FILE;
                break;
            case 'E':
                if (stderrtype == _STDIOTYPE_FILE) {
                    /* -e and -E are mutually exclusive */
                    usage_exit(EXIT_FAILURE);
                }
                force_stderr = true;
                break;
            case 'o':
                stdouttype = _STDIOTYPE_FILE;
                break;
            case 'c':
                tty_uart_setup(uart++, optarg);
                break;
#ifdef MODULE_MTD_NATIVE
            case 'm':
                mtd0.fname = strndup(optarg, PATH_MAX - 1);
                break;
#endif
            default:
                usage_exit(EXIT_FAILURE);
        }
    }
#ifdef MODULE_NETDEV2_TAP
    for (int i = 0; i < NETDEV2_TAP_MAX; i++) {
        if (argv[optind + i] == NULL) {
            /* no tap parameter left */
            usage_exit(EXIT_FAILURE);
        }
    }
#endif

    if (dmn) {
        filter_daemonize_argv(_native_argv);
        if (stderrtype == _STDIOTYPE_STDIO) {
            stderrtype = _STDIOTYPE_NULL;
        }
        if (stdouttype == _STDIOTYPE_STDIO) {
            stdouttype = _STDIOTYPE_NULL;
        }
        if (stdintype == _STDIOTYPE_STDIO) {
            stdintype = _STDIOTYPE_NULL;
        }
        daemonize();
    }
    if (force_stderr) {
        stderrtype = _STDIOTYPE_STDIO;
    }

    _native_log_output(stderrtype, STDERR_FILENO);
    _native_null_out_file = _native_log_output(stdouttype, STDOUT_FILENO);
    _native_input(stdintype);

    native_cpu_init();
    native_interrupt_init();
#ifdef MODULE_NETDEV2_TAP
    for (int i = 0; i < NETDEV2_TAP_MAX; i++) {
        netdev2_tap_params[i].tap_name = &argv[optind + i];
    }
#endif

    board_init();

    puts("RIOT native hardware initialization complete.\n");
    irq_enable();
    kernel_init();
}
