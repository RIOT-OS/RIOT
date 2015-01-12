/*
 * Copyright (C) 2014-2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_mulle
 * @{
 *
 * @file
 * @brief  Device I/O mappings for the Mulle platform.
 *
 * @author Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <stddef.h> /* for NULL */
#include "devopttab.h"
#include "devicemap.h"
#include "devio-uart.h"
#include "devio-null.h"
#include "cpu.h"
#include "periph_conf.h"
#include "periph/uart.h"

#if UART_0_EN
static const devoptab_t dotab_uart0 = {
    "UART0", /* name */
    1, /* isatty */
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, /* Character device, 0777 perms (crwxrwxrwx) */
    devnull_open_r,
    devnull_close_r,
    uart0_write_r,
    NULL, /* Not yet implemented */
    NULL, /* No seeking on UART */
    NULL, /* No fstat on UART */
};
#endif

#if UART_1_EN
static const devoptab_t dotab_uart1 = {
    "UART1", /* name */
    1, /* isatty */
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, /* Character device, 0777 perms (crwxrwxrwx) */
    devnull_open_r,
    devnull_close_r,
    uart1_write_r,
    NULL, /* Not yet implemented */
    NULL, /* No seeking on UART */
    NULL, /* No fstat on UART */
};
#endif

#if UART_2_EN
static const devoptab_t dotab_uart2 = {
    "UART2", /* name */
    1, /* isatty */
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, /* Character device, 0777 perms (crwxrwxrwx) */
    devnull_open_r,
    devnull_close_r,
    uart2_write_r,
    NULL, /* Not yet implemented */
    NULL, /* No seeking on UART */
    NULL, /* No fstat on UART */
};
#endif

#if UART_3_EN
static const devoptab_t dotab_uart3 = {
    "UART3", /* name */
    1, /* isatty */
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, /* Character device, 0777 perms (crwxrwxrwx) */
    devnull_open_r,
    devnull_close_r,
    uart3_write_r,
    NULL, /* Not yet implemented */
    NULL, /* No seeking on UART */
    NULL, /* No fstat on UART */
};
#endif

#if UART_4_EN
static const devoptab_t dotab_uart4 = {
    "UART4", /* name */
    1, /* isatty */
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO, /* Character device, 0777 perms (crwxrwxrwx) */
    devnull_open_r,
    devnull_close_r,
    uart4_write_r,
    NULL, /* Not yet implemented */
    NULL, /* No seeking on UART */
    NULL, /* No fstat on UART */
};
#endif

static const devoptab_t dotab_stdin = {
    "stdin", /* name */
    1, /* isatty */
    S_IFCHR | S_IRUSR | S_IRGRP | S_IROTH, /* Character device, 0444 perms (cr--r--r--) */
    devnull_open_r,
    devnull_close_r,
    NULL,
    NULL, /* Not yet implemented */
    NULL, /* No seeking on UART */
    NULL, /* No fstat on UART */
};

static const devoptab_t dotab_stdout = {
    "stdout", /* name */
    1, /* isatty */
    S_IFCHR | S_IWUSR | S_IWGRP | S_IWOTH, /* Character device, 0222 perms (c-w--w--w-) */
    devnull_open_r,
    devnull_close_r,
#if UART_0_EN
    uart0_write_r,
#else
    NULL, /* No stdout */
#endif
    NULL, /* Not yet implemented */
    NULL, /* No seeking on UART */
    NULL, /* No fstat on UART */
};



/* This table maps the standard streams to device operations table entries. */
const devoptab_t *devoptab_list[MAX_OPEN_DEVICES] = { 0 };

static const devoptab_name_t devoptab_names[] = {
#if UART_0_EN
    {"UART0", &dotab_uart0},
#endif
#if UART_1_EN
    {"UART1", &dotab_uart1},
#endif
#if UART_2_EN
    {"UART2", &dotab_uart2},
#endif
#if UART_3_EN
    {"UART3", &dotab_uart3},
#endif
#if UART_4_EN
    {"UART4", &dotab_uart4},
#endif
};

const devoptab_name_list_t devoptab_name_list = {
    sizeof(devoptab_names) / sizeof(devoptab_names[0]), /* len */
    &devoptab_names[0], /* data */
};

void devicemap_init(void)
{
    /* Set up stdin, stdout and stderr */
    devoptab_list[STDIN_FILENO] = &dotab_stdin;
    devoptab_list[STDOUT_FILENO] = &dotab_stdout;
    devoptab_list[STDERR_FILENO] = &dotab_stdout;
}
