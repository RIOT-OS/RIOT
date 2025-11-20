/*
 * SPDX-FileCopyrightText: 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file    Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @brief   Native CPU entry code
 * @ingroup cpu_native
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <dlfcn.h>
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <features.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "byteorder.h"
#include "irq.h"
#include "kernel_init.h"
#include "native_internal.h"
#include "periph/init.h"
#include "periph/pm.h"
#include "test_utils/expect.h"
#include "tty_uart.h"

#define ENABLE_DEBUG 0
#include "debug.h"
#define DEBUG_STARTUP(...) DEBUG("[native] startup: " __VA_ARGS__)

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

#ifdef MODULE_NETDEV_TAP
#include "netdev_tap_params.h"

netdev_tap_params_t netdev_tap_params[NETDEV_TAP_MAX];
#endif
#ifdef MODULE_MTD_NATIVE
#include "board.h"
#include "mtd_native.h"
#endif
#ifdef MODULE_PERIPH_CAN
#include "candev_linux.h"
#endif
#ifdef MODULE_PERIPH_SPIDEV_LINUX
#include "spidev_linux.h"
#endif
#ifdef MODULE_PERIPH_GPIO_LINUX
#include "gpiodev_linux.h"
#endif
#ifdef MODULE_NATIVE_CLI_EUI_PROVIDER
#include "native_cli_eui_provider.h"
#endif
#ifdef MODULE_SOCKET_ZEP
#include "socket_zep_params.h"

socket_zep_params_t socket_zep_params[SOCKET_ZEP_MAX];
#endif
#ifdef MODULE_PERIPH_EEPROM
#include "eeprom_native.h"
extern char eeprom_file[EEPROM_FILEPATH_MAX_LEN];
#endif

static const char short_opts[] = ":hi:s:deEoc:"
#ifdef MODULE_PERIPH_GPIO_LINUX
    "g:"
#endif
#ifdef MODULE_MTD_NATIVE
    "m:"
#endif
#ifdef MODULE_PERIPH_CAN
    "n:"
#endif
#ifdef MODULE_SOCKET_ZEP
    "z:"
#endif
#ifdef MODULE_NATIVE_CLI_EUI_PROVIDER
    "U:"
#endif
#ifdef MODULE_PERIPH_SPIDEV_LINUX
    "p:"
#endif
#ifdef MODULE_NETDEV_TAP
    "w:"
#endif
    "";

#if __GLIBC__
/* glibc and Apple's libSystem pass argc, argv, and envp to init_fini handlers */
# define _HAVE_INIT_FINIT_PROGRAM_ARGUMENTS 1
#else
/* otherwise, not guaranteed */
# define _HAVE_INIT_FINIT_PROGRAM_ARGUMENTS 0
#endif

static const struct option long_opts[] = {
    { "help", no_argument, NULL, 'h' },
    { "id", required_argument, NULL, 'i' },
    { "seed", required_argument, NULL, 's' },
    { "daemonize", no_argument, NULL, 'd' },
    { "stderr-pipe", no_argument, NULL, 'e' },
    { "stderr-noredirect", no_argument, NULL, 'E' },
    { "stdout-pipe", no_argument, NULL, 'o' },
    { "uart-tty", required_argument, NULL, 'c' },
#ifdef MODULE_PERIPH_GPIO_LINUX
    { "gpio", required_argument, NULL, 'g' },
#endif
#ifdef MODULE_MTD_NATIVE
    { "mtd", required_argument, NULL, 'm' },
#endif
#ifdef MODULE_PERIPH_CAN
    { "can", required_argument, NULL, 'n' },
#endif
#ifdef MODULE_SOCKET_ZEP
    { "zep", required_argument, NULL, 'z' },
#endif
#ifdef MODULE_NATIVE_CLI_EUI_PROVIDER
    { "eui64", required_argument, NULL, 'U' },
#endif
#ifdef MODULE_PERIPH_SPIDEV_LINUX
    { "spi", required_argument, NULL, 'p' },
#endif
#ifdef MODULE_PERIPH_EEPROM
    { "eeprom", required_argument, NULL, 'M' },
#endif
    { NULL, 0, NULL, '\0' },
};

/**
 * @brief   initialize _native_null_in_pipe to allow for reading from stdin
 *
 * @param[in] stdintype _STDIOTYPE_STDIO to to just initialize pipe, any other
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
 * @param[in,out]   argv    an argument vector
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

#if defined(MODULE_NETDEV_TAP)
    for (int i = 0; i < NETDEV_TAP_MAX; i++) {
        real_printf(" <tap interface %d>", i + 1);
    }
#endif
    real_printf(" [-i <id>] [-d] [-e|-E] [-o] [-c <tty>]");
#ifdef MODULE_PERIPH_GPIO_LINUX
    real_printf(" [-g <gpiochip>]");
#endif
    real_printf(" [-i <id>] [-d] [-e|-E] [-o] [-c <tty>]");
#if defined(MODULE_SOCKET_ZEP) && (SOCKET_ZEP_MAX > 0)
    real_printf(" -z [[<laddr>:<lport>,]<raddr>:<rport>]");
    for (int i = 0; i < SOCKET_ZEP_MAX - 1; i++) {
        /* for further interfaces the local address must be different so we omit
         * the braces (marking them as optional) to be 100% clear on that */
        real_printf(" -z <laddr>:<lport>,<raddr>:<rport>");
    }
#endif
#ifdef MODULE_NATIVE_CLI_EUI_PROVIDER
    real_printf(" [--eui64 <eui64> …]");
#endif
#ifdef MODULE_PERIPH_SPIDEV_LINUX
    real_printf(" [-p <b>:<d>:<spidev>]");
#endif

    real_printf("\n\n");

    real_printf("help: %s -h\n", _progname);

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
"        times (up to UART_NUMOF)\n"
#ifdef MODULE_PERIPH_GPIO_LINUX
"    -g <gpio>, --gpio=<gpio>\n"
"        specify gpiochip device for GPIO access.\n"
"        This argument can be used multiple times.\n"
"        Example: --gpio=/dev/gpiochip0 uses gpiochip0 for port 0\n"
#endif
#if defined(MODULE_SOCKET_ZEP) && (SOCKET_ZEP_MAX > 0)
"    -z [<laddr>:<lport>,]<raddr>:<rport> --zep=[<laddr>:<lport>,]<raddr>:<rport>\n"
"        provide a ZEP interface with an (optional) local address and port\n"
"        (<laddr>:<lport>) and a remote address and port (<raddr>:<rport>).\n"
"        The ZEP interface connects to the remote address and may listen\n"
"        on a local address.\n"
"        Required to be provided SOCKET_ZEP_MAX times\n"
#endif
#ifdef MODULE_NATIVE_CLI_EUI_PROVIDER
"    -U <eui64>, --eui64=<eui64>\n"
"        provide a ZEP interface with EUI-64 (MAC address)\n"
"        This argument can be provided multiple times\n"
#endif
    );
#ifdef MODULE_MTD_NATIVE
    real_printf(
"    -m <mtd>, --mtd=<mtd>\n"
"       specify the file name of mtd emulated device\n");
#endif
#if defined(MODULE_PERIPH_CAN)
    real_printf(
"    -n <ifnum>:<ifname>, --can <ifnum>:<ifname>\n"
"        specify CAN interface <ifname> to use for CAN device #<ifnum>\n"
"        max number of CAN device: %d\n", CAN_DLL_NUMOF);
#endif
#ifdef MODULE_PERIPH_SPIDEV_LINUX
    real_printf(
"    -p <b>:<d>:<spidev>, --spi=<b>:<d>:<spidev>\n"
"        specify Linux SPI device to use for CS line d on bus b (in RIOT)\n"
"        Example: --spi=0:1:/dev/spidev0.0 will assign the file spidev0.0 to\n"
"                 SPI_DEV(0) and SPI_HWCS(1).\n"
"        Supports up to %d buses with %d CS lines each.\n", SPI_NUMOF, SPI_MAXCS
    );
#endif
#ifdef MODULE_PERIPH_EEPROM
    real_printf(
"    -M <eeprom> , --eeprom=<eeprom>\n"
"        Specify the file path where the EEPROM content is stored\n"
"        Example: --eeprom=/tmp/riot_native.eeprom\n");
#endif
#ifdef MODULE_NETDEV_TAP
    real_printf(
"    -w <tap>\n"
"        Add a tap interface as a wireless interface\n");
#endif
    real_exit(status);
}

#ifdef MODULE_SOCKET_ZEP
static void _parse_ep_str(char *ep_str, char **addr, char **port)
{
    /* read endpoint string in reverse, the last chars are the port and decimal
     * numbers, then a colon, then the address (potentially containing colons,
     * that's why we read in reverse) */
    for (int i = strlen(ep_str) - 1; (i >= 0) && (*port == NULL); i--) {
        if (((ep_str[i] < '0') || (ep_str[i] > '9')) && (ep_str[i] != ':')) {
            usage_exit(EXIT_FAILURE);
        }
        if ((ep_str[i] == ':') && (i >= (int)sizeof("[]"))) {
            /* found port delimiter, but we need to make sure it isn't delivered
             * like :<port>. Two characters for either hostname or IP address
             * seems reasonable especially considering, that we need to
             * remove the [] around IPv6 addresses */
            *port = &ep_str[i + 1];
            if ((ep_str[0] == '[') && (ep_str[i - 1] == ']')) {
                /* addr is in the format [<addr>], strip [] */
                *addr = &ep_str[1];
                ep_str[i - 1] = '\0';
            }
            else if ((ep_str[0] == '[') || (ep_str[i - 1] == ']')) {
                /* unbalanced brackets */
                usage_exit(EXIT_FAILURE);
            }
            else {
                *addr = ep_str;
            }
            ep_str[i] = '\0';
        }
    }
    if (*port == NULL) {
        usage_exit(EXIT_FAILURE);
    }
}

static void _zep_params_setup(char *zep_str, int zep)
{
    char *save_ptr, *first_ep, *second_ep;

    /* reboot uses execve() so we need to preserve argv */
    zep_str = strdup(zep_str);

    if ((first_ep = strtok_r(zep_str, ",", &save_ptr)) == NULL) {
        usage_exit(EXIT_FAILURE);
    }
    second_ep = strtok_r(NULL, ",", &save_ptr);
    if (second_ep == NULL) {
        socket_zep_params[zep].local_addr = NULL;
        socket_zep_params[zep].local_port = NULL;
        _parse_ep_str(first_ep, &socket_zep_params[zep].remote_addr,
                      &socket_zep_params[zep].remote_port);
    }
    else {
        _parse_ep_str(first_ep, &socket_zep_params[zep].local_addr,
                      &socket_zep_params[zep].local_port);
        _parse_ep_str(second_ep, &socket_zep_params[zep].remote_addr,
                      &socket_zep_params[zep].remote_port);
    }
}

#endif

/** @brief Initialization function pointer type */
typedef void (*init_func_t)(int argc, char **argv, char **envp);
#ifdef __APPLE__
/* Taken from the sources of Apple's dyld launcher
 * https://github.com/opensource-apple/dyld/blob/3f928f32597888c5eac6003b9199d972d49857b5/src/dyldInitialization.cpp#L85-L104
 */
/* Find the extents of the __DATA __mod_init_func section */
extern init_func_t __init_array_start __asm("section$start$__DATA$__mod_init_func");
extern init_func_t __init_array_end   __asm("section$end$__DATA$__mod_init_func");
#else
/* Linker script provides pointers to the beginning and end of the init array */
extern init_func_t __init_array_start;
extern init_func_t __init_array_end;
#endif

static void _reset_handler(void)
{
    pm_reboot();
}

__attribute__((constructor)) static void startup(int argc, char **argv, char **envp)
{
    _native_init_syscalls();

    /* Passing argc, argv, and envp to init_fini handlers is a glibc
     * extension. If we are not running glibc, we parse /proc/self/cmdline
     * to populate argc and argv by hand */
    if (!_HAVE_INIT_FINIT_PROGRAM_ARGUMENTS) {
        const size_t bufsize = 4096;
        const size_t argc_max = 32;
        size_t cmdlen = 0;
        char *cmdline = malloc(bufsize);
        argv = calloc(argc_max, sizeof(char *));
        argc = 0;
        envp = NULL;
        expect(cmdline != NULL);
        int cmdfd = real_open("/proc/self/cmdline", O_RDONLY);
        expect(cmdfd != -1);
        ssize_t count;
        do {
            count = real_read(cmdfd, cmdline + cmdlen, bufsize - cmdlen);
            if (count < 0) {
                if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
                    continue;
                }
                expect(0);
            }
            cmdlen += count;
        } while (count > 0);
        real_close(cmdfd);
        cmdline = realloc(cmdline, cmdlen);

        char *argpos = cmdline;
        while ((size_t)argc < argc_max) {
            if (argpos == cmdline + cmdlen) {
                argv[argc] = NULL;
                break;
            }
            size_t len = strlen(argpos);

            argv[argc++] = argpos;
            argpos += len + 1;
        }
        expect((size_t)argc < argc_max);
        argv = realloc(argv, sizeof(char *) * (argc + 1));
    } else {
        /* must be */
        assert(argc > 0);
        assert(argv);
        assert(argv[0]);
    }

    _native_argv = argv;
    _progname = argv[0];
    _native_pid = real_getpid();

    /* will possibly be overridden via option below: */
    _native_id = _native_pid;

    int c, opt_idx = 0, uart = 0;
#ifdef MODULE_NETDEV_TAP
    unsigned taps = 0;
    memset(netdev_tap_params, 0, sizeof(netdev_tap_params));
#endif
#ifdef MODULE_SOCKET_ZEP
    unsigned zeps = 0;
#endif
    bool dmn = false, force_stderr = false;
    _stdiotype_t stderrtype = _STDIOTYPE_STDIO;
    _stdiotype_t stdouttype = _STDIOTYPE_STDIO;
    _stdiotype_t stdintype = _STDIOTYPE_STDIO;

    while ((c = getopt_long(argc, argv, short_opts, long_opts, &opt_idx)) >= 0) {
        switch (c) {
            case 0:
                /* fall through to 'h' */
            case 'h':
                usage_exit(EXIT_SUCCESS);
                break;
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
#ifdef MODULE_PERIPH_GPIO_LINUX
            case 'g':
                if (gpio_linux_setup(optarg) < 0) {
                    usage_exit(EXIT_FAILURE);
                }
                break;
#endif
            case 'o':
                stdouttype = _STDIOTYPE_FILE;
                break;
            case 'c':
                tty_uart_setup(uart++, optarg);
                break;
#ifdef MODULE_MTD_NATIVE
            case 'm':
                ((mtd_native_dev_t *)mtd_dev_get(0))->fname = strndup(optarg, PATH_MAX - 1);
                break;
#endif
#if defined(MODULE_PERIPH_CAN)
            case 'n':{
                int i;
                i = atol(optarg);
                if (i >= (int)CAN_DLL_NUMOF) {
                    usage_exit(EXIT_FAILURE);
                }
                while ((*optarg != ':') && (*optarg != '\0')) {
                    optarg++;
                }
                if (*optarg == '\0') {
                    usage_exit(EXIT_FAILURE);
                }
                optarg++;
                strncpy(candev_conf[i].interface_name, optarg,
                        CAN_MAX_SIZE_INTERFACE_NAME);
                }
                break;
#endif
#ifdef MODULE_SOCKET_ZEP
            case 'z':
                _zep_params_setup(optarg, zeps++);
                break;
#endif
#ifdef MODULE_NATIVE_CLI_EUI_PROVIDER
            case 'U':
                native_cli_add_eui64(optarg);
                break;
#endif
#ifdef MODULE_PERIPH_SPIDEV_LINUX
            case 'p': {
                    long bus = strtol(optarg, &optarg, 10);
                    if (*optarg != ':') {
                        usage_exit(EXIT_FAILURE);
                    }
                    long cs = strtol(++optarg, &optarg, 10);
                    if (*optarg != ':') {
                        usage_exit(EXIT_FAILURE);
                    }
                    if (spidev_linux_setup(bus, cs, ++optarg) < 0) {
                        usage_exit(EXIT_FAILURE);
                    }
                }
                break;
#endif
#ifdef MODULE_PERIPH_EEPROM
            case 'M': {
                strncpy(eeprom_file, optarg, EEPROM_FILEPATH_MAX_LEN);
                break;
            }
#endif
#ifdef MODULE_NETDEV_TAP
            case 'w':
                netdev_tap_params[taps].tap_name = &argv[optind - 1];
                netdev_tap_params[taps].wired = false;
                ++taps;
                break;
#endif
            default:
                usage_exit(EXIT_FAILURE);
                break;
        }
    }
#ifdef MODULE_SOCKET_ZEP
    if (zeps != SOCKET_ZEP_MAX) {
        /* not enough ZEPs given */
        usage_exit(EXIT_FAILURE);
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

    /* startup is a constructor which is being called from the init_array during
     * C runtime initialization, this is normally used for code which must run
     * before launching main(), such as C++ global object constructors etc.
     * However, this function (startup) misbehaves a bit when we call
     * kernel_init below, which does not return until there is an abort or a
     * power off command.
     * We need all C++ global constructors and other initializers to run before
     * we enter the normal application code, which may depend on global objects
     * having been initialized properly. Therefore, we iterate through the
     * remainder of the init_array and call any constructors which have been
     * placed after startup in the initialization order.
     */
    init_func_t *init_array_ptr = &__init_array_start;
    DEBUG("__init_array_start: %p\n", (void *)init_array_ptr);
    while (init_array_ptr < &__init_array_end) {
        /* Skip everything which has already been run */
        if ((*init_array_ptr) == startup) {
            /* Found ourselves, move on to calling the rest of the constructors */
            DEBUG_STARTUP("%18p - myself\n", (void *)init_array_ptr);
            ++init_array_ptr;
            break;
        }
        DEBUG_STARTUP("%18p - skip\n", (void *)init_array_ptr);
        ++init_array_ptr;
    }
    while (init_array_ptr < &__init_array_end) {
        /* call all remaining constructors */
        DEBUG_STARTUP("%18p - call\n", (void *)init_array_ptr);
        (*init_array_ptr)(argc, argv, envp);
        ++init_array_ptr;
    }
    DEBUG_STARTUP("done, __init_array_end: %p\n", (void *)init_array_ptr);

    native_cpu_init();
    native_interrupt_init();
#ifdef MODULE_NETDEV_TAP
    for (unsigned i = 0; taps < NETDEV_TAP_MAX; ++taps, ++i) {
        if (argv[optind + i] == NULL) {
            break;
        }
        netdev_tap_params[taps].tap_name = &argv[optind + i];
        netdev_tap_params[taps].wired = true;
    }
#endif

#ifdef MODULE_PERIPH_EEPROM
    eeprom_native_read();
#endif

    periph_init();
    board_init();

    native_register_interrupt(SIGUSR1, _reset_handler);

    /* initialize stdio after signal setup */
    early_init();

    puts("RIOT native hardware initialization complete.\n");
    irq_enable();
    kernel_init();
}
