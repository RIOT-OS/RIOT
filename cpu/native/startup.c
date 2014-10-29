/**
 * Native CPU entry code
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup arch
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#include <dlfcn.h>
#else
#include <dlfcn.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "kernel_internal.h"
#include "cpu.h"

#include "board_internal.h"
#include "native_internal.h"
#include "native_fildes.h"
#include "tap.h"

int _native_null_in_pipe[2];
int _native_null_out_file;
const char *_progname;
char **_native_argv;
pid_t _native_pid;
pid_t _native_id;
unsigned _native_rng_seed = 0;
int _native_rng_mode = 0;
const char *_native_unix_socket_path = NULL;

/**
 * initialize _native_null_in_pipe to allow for reading from stdin
 * @param stdiotype: "stdio" (only initialize pipe) or any string
 * (redirect stdin to _native_null_in_pipe)
 */
void _native_null_in(char *stdiotype)
{
    if (real_pipe(_native_null_in_pipe) == -1) {
        err(EXIT_FAILURE, "_native_null_in(): pipe()");
    }
    else {
        (void) _native_fd_add(_native_null_in_pipe[0]); /* don't need this fd */
        (void) _native_fd_add(_native_null_in_pipe[1]); /* don't need this fd */
    }

    if (strcmp(stdiotype, "stdio") == 0) {
        return;
    }

    if (real_dup2(_native_null_in_pipe[0], STDIN_FILENO) == -1) {
        err(EXIT_FAILURE, "_native_null_in: dup2(STDIN_FILENO)");
    }
}

/**
 * set up stdout redirection
 *
 * @param stdouttype: "stdio" (leave intact), "null" (redirect to
 * /dev/null) or "file" (redirect to /tmp/riot.stdout.PID)
 */
void _native_log_stdout(char *stdouttype)
{
    int stdout_outfile;

    if (strcmp(stdouttype, "stdio") == 0) {
        _native_null_out_file = -1;
        return;
    }
    else if (strcmp(stdouttype, "null") == 0) {
        if ((stdout_outfile = real_open("/dev/null", O_WRONLY)) == -1) {
            err(EXIT_FAILURE, "_native_log_stdout: open");
        }
        else {
            (void) _native_fd_add(stdout_outfile); /* don't need this fd */
        }
    }
    else if (strcmp(stdouttype, "file") == 0) {
        char stdout_logname[255];
        snprintf(stdout_logname, sizeof(stdout_logname), "/tmp/riot.stdout.%d", _native_pid);
        if ((stdout_outfile = real_creat(stdout_logname, 0666)) == -1) {
            err(EXIT_FAILURE, "_native_log_stdout: open");
        }
        else {
            (void) _native_fd_add(stdout_outfile); /* don't need this fd */
        }
    }
    else {
        errx(EXIT_FAILURE, "_native_log_stdout: unknown log type");
    }

    if (real_dup2(stdout_outfile, STDOUT_FILENO) == -1) {
        err(EXIT_FAILURE, "_native_log_stdout: dup2(STDOUT_FILENO)");
    }
    _native_null_out_file = stdout_outfile;
}

/**
 * set up stderr redirection
 *
 * @param stderrtype: "stdio" (leave intact), "null" (redirect to
 * /dev/null) or "file" (redirect to /tmp/riot.stdout.PID)
 */
void _native_log_stderr(char *stderrtype)
{
    int stderr_outfile;

    if (strcmp(stderrtype, "stdio") == 0) {
        return;
    }
    else if (strcmp(stderrtype, "null") == 0) {
        if ((stderr_outfile = real_open("/dev/null", O_WRONLY)) == -1) {
            err(EXIT_FAILURE, "_native_log_stderr: open");
        }
        else {
            (void) _native_fd_add(stderr_outfile); /* don't need this fd */
        }
    }
    else if (strcmp(stderrtype, "file") == 0) {
        char stderr_logname[255];
        snprintf(stderr_logname, sizeof(stderr_logname), "/tmp/riot.stderr.%d", _native_pid);
        if ((stderr_outfile = real_creat(stderr_logname, 0666)) == -1) {
            err(EXIT_FAILURE, "_native_log_stderr: open");
        }
        else {
            (void) _native_fd_add(stderr_outfile); /* don't need this fd */
        }
    }
    else {
        errx(EXIT_FAILURE, "_native_log_stderr: unknown log type");
    }

    if (real_dup2(stderr_outfile, STDERR_FILENO) == -1) {
        err(EXIT_FAILURE, "_native_log_stderr: dup2(STDERR_FILENO)");
    }
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
    }
}

/**
 * Remove any -d options from an argument vector.
 *
 * @param[in][out]  argv    an argument vector
 *
 * @return                  1 if "-d" was found, 0 otherwise
 */
static int filter_daemonize_argv(char **argv)
{
    int ret = 0;
    for (char **narg = argv; *narg != NULL; narg++) {
        if (strcmp("-d", narg[0]) == 0) {
            ret = 1;
            char **xarg = narg;
            do {
                xarg[0] = xarg[1];
            } while (*xarg++ != NULL);
            narg--; /* rescan current item to filter out double args */
        }
    }
    return ret;
}

void usage_exit(void)
{
    real_printf("usage: %s", _progname);

#ifdef MODULE_NATIVENET
    real_printf(" <tap interface>");
#endif

#ifdef MODULE_UART0
    real_printf(" [-t <port>|-u [path]] [-r]");
#endif

    real_printf(" [-i <id>] [-d] [-e|-E] [-o]\n");

    real_printf(" help: %s -h\n", _progname);

    real_printf("\nOptions:\n\
-h          help\n");

#ifdef MODULE_UART0
    real_printf("\
-t <port>   redirect stdio to TCP socket listening on <port>\n\
-u <path>   redirect stdio to UNIX socket (<path> if given,\n\
            /tmp/riot.tty.PID otherwise)\n\
-r          replay missed output when (re-)attaching to socket\n\
            (implies -o)\n");
#endif
    real_printf("\
-i <id>     specify instance id (set by config module)\n\
-s <seed>   specify srandom(3) seed (/dev/random is used instead of\n\
            random(3) if the option is omitted)\n\
-d          daemonize\n\
-e          redirect stderr to file\n\
-E          do not redirect stderr (i.e. leave sterr unchanged despite\n\
            daemon/socket io)\n\
-o          redirect stdout to file (/tmp/riot.stdout.PID) when not attached\n\
            to socket\n");

    real_printf("\n\
The order of command line arguments matters.\n");
    real_exit(EXIT_FAILURE);

}

__attribute__((constructor)) static void startup(int argc, char **argv)
{
    _native_init_syscalls();

    _native_argv = argv;
    _progname = argv[0];
    _native_pid = real_getpid();

    /* will possibly be overridden via option below: */
    _native_id = _native_pid;

    int argp = 1;
    char *stderrtype = "stdio";
    char *stdouttype = "stdio";
    char *stdiotype = "stdio";
#ifdef MODULE_UART0
    char *ioparam = NULL;
    int replay = 0;
#endif

#ifdef MODULE_NATIVENET
    if (
            (argc < 2)
            || (
                (strcmp("-h", argv[argp]) == 0)
                || (strcmp("--help", argv[argp]) == 0)
               )
       ) {
        usage_exit();
    }
    argp++;
#endif

    for (; argp < argc; argp++) {
        char *arg = argv[argp];
        if ((strcmp("-h", arg) == 0) || (strcmp("--help", arg) == 0)) {
            usage_exit();
        }
        else if (strcmp("-i", arg) == 0) {
            if (argp + 1 < argc) {
                argp++;
            }
            else {
                usage_exit();
            }
            _native_id = atol(argv[argp]);
        }
        else if (strcmp("-s", arg) == 0) {
            if (argp + 1 < argc) {
                argp++;
            }
            else {
                usage_exit();
            }
            _native_rng_seed = atol(argv[argp]);
            _native_rng_mode = 1;
        }
        else if (strcmp("-d", arg) == 0) {
            if (strcmp(stdiotype, "stdio") == 0) {
                stdiotype = "null";
            }
            if (strcmp(stdouttype, "stdio") == 0) {
                stdouttype = "null";
            }
            if (strcmp(stderrtype, "stdio") == 0) {
                stderrtype = "null";
            }
        }
        else if (strcmp("-e", arg) == 0) {
            stderrtype = "file";
        }
        else if (strcmp("-E", arg) == 0) {
            stderrtype = "stdio";
        }
        else if (strcmp("-o", arg) == 0) {
            stdouttype = "file";
        }
#ifdef MODULE_UART0
        else if (strcmp("-r", arg) == 0) {
            stdouttype = "file";
            replay = 1;
        }
        else if (strcmp("-t", arg) == 0) {
            stdiotype = "tcp";
            if (argp + 1 < argc) {
                ioparam = argv[++argp];
            }
            else {
                usage_exit();
            }
            if (strcmp(stdouttype, "stdio") == 0) {
                stdouttype = "null";
            }
            if (strcmp(stderrtype, "stdio") == 0) {
                stderrtype = "null";
            }
        }
        else if (strcmp("-u", arg) == 0) {
            stdiotype = "unix";
            if (strcmp(stdouttype, "stdio") == 0) {
                stdouttype = "null";
            }
            if (strcmp(stderrtype, "stdio") == 0) {
                stderrtype = "null";
            }

            /* parse optional path */
            if ((argp + 1 < argc) && (argv[argp + 1][0] != '-')) {
                _native_unix_socket_path = argv[++argp];
            }
        }
#endif
        else {
            usage_exit();
        }
    }

    if (filter_daemonize_argv(_native_argv)) {
        daemonize();
    }

    _native_log_stderr(stderrtype);
    _native_log_stdout(stdouttype);
    _native_null_in(stdiotype);

#ifdef MODULE_UART0
    _native_init_uart0(stdiotype, ioparam, replay);
#endif

    native_hwtimer_pre_init();
    native_cpu_init();
    native_interrupt_init();
#ifdef MODULE_NATIVENET
    tap_init(argv[1]);
#endif

    board_init();

    puts("RIOT native hardware initialization complete.\n");
    kernel_init();
}
