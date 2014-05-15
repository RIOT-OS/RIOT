/**
 * Native CPU entry code
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
#include "tap.h"

int (*real_printf)(const char *format, ...);
int (*real_getpid)(void);
int _native_null_in_pipe[2];
int _native_null_out_file;
const char *_progname;
char **_native_argv;
pid_t _native_pid;

/**
 * initialize _native_null_in_pipe to allow for reading from stdin
 * @param stdiotype: "stdio" (only initialize pipe) or any string
 * (redirect stdin to _native_null_in_pipe)
 */
void _native_null_in(char *stdiotype)
{

    if (pipe(_native_null_in_pipe) == -1) {
        err(1, "_native_null_in(): pipe()");
    }

    if (strcmp(stdiotype, "stdio") == 0) {
        return;
    }

    if (dup2(_native_null_in_pipe[0], STDIN_FILENO) == -1) {
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
        if ((stdout_outfile = open("/dev/null", O_WRONLY)) == -1) {
            err(EXIT_FAILURE, "_native_log_stdout: open");
        }
    }
    else if (strcmp(stdouttype, "file") == 0) {
        char stdout_logname[255];
        snprintf(stdout_logname, sizeof(stdout_logname), "/tmp/riot.stdout.%d", _native_pid);
        if ((stdout_outfile = creat(stdout_logname, 0666)) == -1) {
            err(EXIT_FAILURE, "_native_log_stdout: open");
        }
    }
    else {
        errx(EXIT_FAILURE, "_native_log_stdout: unknown log type");
    }

    if (dup2(stdout_outfile, STDOUT_FILENO) == -1) {
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
        if ((stderr_outfile = open("/dev/null", O_WRONLY)) == -1) {
            err(EXIT_FAILURE, "_native_log_stderr: open");
        }
    }
    else if (strcmp(stderrtype, "file") == 0) {
        char stderr_logname[255];
        snprintf(stderr_logname, sizeof(stderr_logname), "/tmp/riot.stderr.%d", _native_pid);
        if ((stderr_outfile = creat(stderr_logname, 0666)) == -1) {
            err(EXIT_FAILURE, "_native_log_stderr: open");
        }
    }
    else {
        errx(EXIT_FAILURE, "_native_log_stderr: unknown log type");
    }

    if (dup2(stderr_outfile, STDERR_FILENO) == -1) {
        err(EXIT_FAILURE, "_native_log_stderr: dup2(STDERR_FILENO)");
    }
}

void daemonize(void)
{
    if ((_native_pid = fork()) == -1) {
        err(EXIT_FAILURE, "daemonize: fork");
    }

    if (_native_pid > 0) {
        real_printf("RIOT pid: %d\n", _native_pid);
        exit(EXIT_SUCCESS);
    }
}

void usage_exit(void)
{
    real_printf("usage: %s", _progname);

#ifdef MODULE_NATIVENET
    real_printf(" <tap interface>");
#endif

#ifdef MODULE_UART0
    real_printf(" [-t <port>|-u]");
#endif

    real_printf(" [-d] [-e|-E] [-o]\n");

    real_printf(" help: %s -h\n", _progname);

    real_printf("\nOptions:\n\
-h      help\n\
-d      daemonize\n\
-e      redirect stderr to file\n\
-E      do not redirect stderr (i.e. leave sterr unchanged despite socket/daemon io)\n\
-o      redirect stdout to file when not attached to socket\n");

#ifdef MODULE_UART0
    real_printf("\
-u      redirect stdio to UNIX socket\n\
-t      redirect stdio to TCP socket\n");
#endif

    real_printf("\n\
The order of command line arguments matters.\n");
    exit(EXIT_FAILURE);

}

__attribute__((constructor)) static void startup(int argc, char **argv)
{
    /* get system read/write/printf */
    *(void **)(&real_read) = dlsym(RTLD_NEXT, "read");
    *(void **)(&real_write) = dlsym(RTLD_NEXT, "write");
    *(void **)(&real_malloc) = dlsym(RTLD_NEXT, "malloc");
    *(void **)(&real_realloc) = dlsym(RTLD_NEXT, "realloc");
    *(void **)(&real_free) = dlsym(RTLD_NEXT, "free");
    *(void **)(&real_printf) = dlsym(RTLD_NEXT, "printf");
    *(void **)(&real_getpid) = dlsym(RTLD_NEXT, "getpid");

    _native_argv = argv;
    _progname = argv[0];
    _native_pid = real_getpid();

    int argp = 1;
    char *stderrtype = "stdio";
    char *stdouttype = "stdio";
    char *stdiotype = "stdio";
#ifdef MODULE_UART0
    char *ioparam = NULL;
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
        else if (strcmp("-d", arg) == 0) {
            daemonize();
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
        else if (strcmp("-t", arg) == 0) {
            stdiotype = "tcp";
            if (argp+1 < argc) {
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
        }
#endif
        else {
            usage_exit();
        }
    }

    _native_log_stderr(stderrtype);
    _native_log_stdout(stdouttype);
    _native_null_in(stdiotype);

#ifdef MODULE_UART0
    _native_init_uart0(stdiotype, ioparam);
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
