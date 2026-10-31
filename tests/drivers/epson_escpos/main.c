/*
 * SPDX-FileCopyrightText: 2026 Christopher Büchse
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the Epson ESC/POS receipt printer driver
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "epson_escpos.h"

/* the test printer is a TM-T88IV */
#ifndef EPSON_ESCPOS_PARAM_PROFILE
#  define EPSON_ESCPOS_PARAM_PROFILE    (&epson_escpos_profile_tm_t88iv)
#endif

#include "epson_escpos_params.h"
#include "shell.h"

#if defined(MODULE_LWIP_IPV4) && defined(ESCPOS_IPV4_ADDR)
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#endif

static epson_escpos_t _dev;

#if defined(MODULE_EPSON_ESCPOS_TCP)
static epson_escpos_tcp_t _tcp;
#elif defined(MODULE_EPSON_ESCPOS_UART)
static epson_escpos_uart_t _uart;
#elif defined(MODULE_EPSON_ESCPOS_DUMMY)
static epson_escpos_dummy_t _dummy;
static uint8_t _dummy_buf[512];
#endif

static int _check(int res)
{
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    return 0;
}

/* join argv[1..argc-1] with spaces and print it */
static int _print_args(int argc, char **argv, bool newline)
{
    int res = 0;
    for (int i = 1; (res == 0) && (i < argc); i++) {
        if (i > 1) {
            res = epson_escpos_text(&_dev, " ");
        }
        if (res == 0) {
            res = epson_escpos_text(&_dev, argv[i]);
        }
    }
    if ((res == 0) && newline) {
        res = epson_escpos_ln(&_dev, 1);
    }
    if (res == 0) {
        res = epson_escpos_flush(&_dev);
    }
    return _check(res);
}

static int _cmd_open(int argc, char **argv)
{
#if defined(MODULE_EPSON_ESCPOS_TCP)
    if (argc > 1) {
        epson_escpos_tcp_params_t params = {
            .host = argv[1],
            .port = CONFIG_EPSON_ESCPOS_TCP_PORT,
        };
        if (_check(epson_escpos_tcp_setup(&_tcp, &params))) {
            return 1;
        }
    }
#else
    (void)argc;
    (void)argv;
#endif
    return _check(epson_escpos_open(&_dev));
}

static int _cmd_close(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    epson_escpos_close(&_dev);
    return 0;
}

static int _cmd_print(int argc, char **argv)
{
    return _print_args(argc, argv, true);
}

static int _cmd_text(int argc, char **argv)
{
    return _print_args(argc, argv, false);
}

static int _cmd_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int res = epson_escpos_hw(&_dev, EPSON_ESCPOS_HW_INIT);
    if (res == 0) {
        res = epson_escpos_flush(&_dev);
    }
    return _check(res);
}

static int _cmd_feed(int argc, char **argv)
{
    unsigned n = (argc > 1) ? (unsigned)atoi(argv[1]) : 1;
    if (n > 255) {
        puts("error: n must be 0-255");
        return 1;
    }
    int res = epson_escpos_print_and_feed(&_dev, n);
    if (res == 0) {
        res = epson_escpos_flush(&_dev);
    }
    return _check(res);
}

static int _cmd_cut(int argc, char **argv)
{
    epson_escpos_cut_t mode = EPSON_ESCPOS_CUT_FULL;
    bool feed = true;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "part")) {
            mode = EPSON_ESCPOS_CUT_PART;
        }
        else if (!strcmp(argv[i], "nofeed")) {
            feed = false;
        }
        else {
            printf("usage: %s [part] [nofeed]\n", argv[0]);
            return 1;
        }
    }

    int res = epson_escpos_cut(&_dev, mode, feed);
    if (res == 0) {
        res = epson_escpos_flush(&_dev);
    }
    return _check(res);
}

static int _parse_bool(const char *s)
{
    if (!strcmp(s, "on") || !strcmp(s, "1")) {
        return 1;
    }
    if (!strcmp(s, "off") || !strcmp(s, "0")) {
        return 0;
    }
    return -1;
}

static int _cmd_style(int argc, char **argv)
{
    if ((argc == 2) && !strcmp(argv[1], "reset")) {
        return _check(epson_escpos_set_default(&_dev));
    }
    if (argc != 3) {
        printf("usage: %s reset\n"
               "       %s align left|center|right\n"
               "       %s font a|b\n"
               "       %s size normal|2h|2w|2x|<w>x<h>\n"
               "       %s underline 0|1|2\n"
               "       %s density 0-8\n"
               "       %s bold|invert|smooth|flip on|off\n",
               argv[0], argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }

    epson_escpos_style_t style = { 0 };
    const char *key = argv[1];
    const char *val = argv[2];
    int b = _parse_bool(val);

    if (!strcmp(key, "align")) {
        style.fields = EPSON_ESCPOS_STYLE_ALIGN;
        if (!strcmp(val, "left")) {
            style.align = EPSON_ESCPOS_ALIGN_LEFT;
        }
        else if (!strcmp(val, "center")) {
            style.align = EPSON_ESCPOS_ALIGN_CENTER;
        }
        else if (!strcmp(val, "right")) {
            style.align = EPSON_ESCPOS_ALIGN_RIGHT;
        }
        else {
            goto invalid;
        }
    }
    else if (!strcmp(key, "font")) {
        style.fields = EPSON_ESCPOS_STYLE_FONT;
        if (!strcmp(val, "a")) {
            style.font = EPSON_ESCPOS_FONT_A;
        }
        else if (!strcmp(val, "b")) {
            style.font = EPSON_ESCPOS_FONT_B;
        }
        else {
            goto invalid;
        }
    }
    else if (!strcmp(key, "size")) {
        style.fields = EPSON_ESCPOS_STYLE_SIZE;
        if (!strcmp(val, "normal")) {
            style.size = EPSON_ESCPOS_SIZE_NORMAL;
        }
        else if (!strcmp(val, "2h")) {
            style.size = EPSON_ESCPOS_SIZE_2H;
        }
        else if (!strcmp(val, "2w")) {
            style.size = EPSON_ESCPOS_SIZE_2W;
        }
        else if (!strcmp(val, "2x")) {
            style.size = EPSON_ESCPOS_SIZE_2X;
        }
        else if ((strlen(val) == 3) && (val[1] == 'x')) {
            style.size = EPSON_ESCPOS_SIZE_CUSTOM;
            style.width = val[0] - '0';
            style.height = val[2] - '0';
        }
        else {
            goto invalid;
        }
    }
    else if (!strcmp(key, "underline")) {
        style.fields = EPSON_ESCPOS_STYLE_UNDERLINE;
        style.underline = atoi(val);
    }
    else if (!strcmp(key, "density")) {
        style.fields = EPSON_ESCPOS_STYLE_DENSITY;
        style.density = atoi(val);
    }
    else if (b < 0) {
        goto invalid;
    }
    else if (!strcmp(key, "bold")) {
        style.fields = EPSON_ESCPOS_STYLE_BOLD;
        style.bold = b;
    }
    else if (!strcmp(key, "invert")) {
        style.fields = EPSON_ESCPOS_STYLE_INVERT;
        style.invert = b;
    }
    else if (!strcmp(key, "smooth")) {
        style.fields = EPSON_ESCPOS_STYLE_SMOOTH;
        style.smooth = b;
    }
    else if (!strcmp(key, "flip")) {
        style.fields = EPSON_ESCPOS_STYLE_FLIP;
        style.flip = b;
    }
    else {
        printf("error: unknown style \"%s\"\n", key);
        return 1;
    }

    return _check(epson_escpos_set(&_dev, &style));

invalid:
    printf("error: invalid value \"%s\" for %s\n", val, key);
    return 1;
}

static int _cmd_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    static const char *paper[] = { "empty", "near end", "ok" };

    int online = epson_escpos_is_online(&_dev);
    if (_check(online)) {
        return 1;
    }
    int res = epson_escpos_paper_status(&_dev);
    if (_check(res)) {
        return 1;
    }
    printf("printer is %s, paper: %s\n", online ? "online" : "offline",
           paper[res]);
    return 0;
}

static int _cmd_demo(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    static const epson_escpos_style_t title = {
        .fields = EPSON_ESCPOS_STYLE_ALIGN | EPSON_ESCPOS_STYLE_SIZE |
                  EPSON_ESCPOS_STYLE_BOLD,
        .align = EPSON_ESCPOS_ALIGN_CENTER,
        .size = EPSON_ESCPOS_SIZE_2X,
        .bold = true,
    };
    static const epson_escpos_style_t bold = {
        .fields = EPSON_ESCPOS_STYLE_BOLD, .bold = true,
    };
    static const epson_escpos_style_t underline = {
        .fields = EPSON_ESCPOS_STYLE_UNDERLINE,
        .underline = EPSON_ESCPOS_UNDERLINE_1DOT,
    };
    static const epson_escpos_style_t invert = {
        .fields = EPSON_ESCPOS_STYLE_INVERT, .invert = true,
    };
    static const epson_escpos_style_t font_b = {
        .fields = EPSON_ESCPOS_STYLE_FONT, .font = EPSON_ESCPOS_FONT_B,
    };
    static const epson_escpos_style_t big = {
        .fields = EPSON_ESCPOS_STYLE_SIZE, .size = EPSON_ESCPOS_SIZE_CUSTOM,
        .width = 3, .height = 3,
    };
    static const epson_escpos_style_t center = {
        .fields = EPSON_ESCPOS_STYLE_ALIGN, .align = EPSON_ESCPOS_ALIGN_CENTER,
    };
    static const epson_escpos_style_t right = {
        .fields = EPSON_ESCPOS_STYLE_ALIGN, .align = EPSON_ESCPOS_ALIGN_RIGHT,
    };

    int res = 0;
#define RUN(x) do { if (res == 0) { res = (x); } } while (0)
    RUN(epson_escpos_hw(&_dev, EPSON_ESCPOS_HW_INIT));
    RUN(epson_escpos_set(&_dev, &title));
    RUN(epson_escpos_textln(&_dev, "RIOT OS"));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_set(&_dev, &center));
    RUN(epson_escpos_textln(&_dev, "ESC/POS driver test"));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_textln(&_dev, "------------------------------------------"));
    RUN(epson_escpos_text(&_dev, "You are running RIOT on a(n) "));
    RUN(epson_escpos_set(&_dev, &bold));
    RUN(epson_escpos_text(&_dev, RIOT_BOARD));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_textln(&_dev, " board."));
    RUN(epson_escpos_text(&_dev, "This board features a(n) "));
    RUN(epson_escpos_set(&_dev, &bold));
    RUN(epson_escpos_text(&_dev, RIOT_CPU));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_textln(&_dev, " CPU."));
    RUN(epson_escpos_textln(&_dev, "------------------------------------------"));
    RUN(epson_escpos_textln(&_dev, "normal text"));
    RUN(epson_escpos_set(&_dev, &bold));
    RUN(epson_escpos_textln(&_dev, "bold text"));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_set(&_dev, &underline));
    RUN(epson_escpos_textln(&_dev, "underlined text"));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_set(&_dev, &invert));
    RUN(epson_escpos_textln(&_dev, " inverted text "));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_set(&_dev, &font_b));
    RUN(epson_escpos_textln(&_dev, "font B text"));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_set(&_dev, &big));
    RUN(epson_escpos_textln(&_dev, "3x3"));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_set(&_dev, &right));
    RUN(epson_escpos_textln(&_dev, "right aligned"));
    RUN(epson_escpos_set_default(&_dev));
    RUN(epson_escpos_cut(&_dev, EPSON_ESCPOS_CUT_PART, true));
    RUN(epson_escpos_flush(&_dev));
#undef RUN
    return _check(res);
}

#ifdef MODULE_EPSON_ESCPOS_DUMMY
static int _cmd_dump(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    epson_escpos_flush(&_dev);
    for (size_t i = 0; i < _dummy.len; i++) {
        printf("%02x%c", _dummy_buf[i], ((i % 16) == 15) ? '\n' : ' ');
    }
    puts("");
    _dummy.len = 0;
    return 0;
}

SHELL_COMMAND(dump, "dump and clear the dummy output buffer", _cmd_dump);
#endif

SHELL_COMMAND(open, "open connection to the printer [host[:port]]", _cmd_open);
SHELL_COMMAND(close, "close connection to the printer", _cmd_close);
SHELL_COMMAND(print, "print text followed by a newline", _cmd_print);
SHELL_COMMAND(text, "print text without newline", _cmd_text);
SHELL_COMMAND(init, "initialize the printer (ESC @)", _cmd_init);
SHELL_COMMAND(feed, "print and feed <n> lines", _cmd_feed);
SHELL_COMMAND(cut, "cut the paper [part] [nofeed]", _cmd_cut);
SHELL_COMMAND(style, "set text style", _cmd_style);
SHELL_COMMAND(status, "query the printer status", _cmd_status);
SHELL_COMMAND(demo, "print a demo receipt", _cmd_demo);

#if defined(MODULE_LWIP_IPV4) && defined(ESCPOS_IPV4_ADDR)
static void _setup_ipv4(void)
{
    ip4_addr_t ip, netmask, gw;

    if (!ip4addr_aton(ESCPOS_IPV4_ADDR, &ip) ||
        !ip4addr_aton(ESCPOS_IPV4_NETMASK, &netmask) ||
        !ip4addr_aton(ESCPOS_IPV4_GW, &gw)) {
        puts("error: invalid IPv4 configuration");
        return;
    }

    LOCK_TCPIP_CORE();
    if (netif_default) {
        netif_set_addr(netif_default, &ip, &netmask, &gw);
        printf("IPv4 address: %s/%s gw %s\n", ESCPOS_IPV4_ADDR,
               ESCPOS_IPV4_NETMASK, ESCPOS_IPV4_GW);
    }
    else {
        puts("error: no network interface");
    }
    UNLOCK_TCPIP_CORE();
}
#endif

int main(void)
{
    puts("Epson ESC/POS receipt printer driver test");

#if defined(MODULE_LWIP_IPV4) && defined(ESCPOS_IPV4_ADDR)
    _setup_ipv4();
#endif

#if defined(MODULE_EPSON_ESCPOS_TCP)
    if (_check(epson_escpos_tcp_setup(&_tcp, &epson_escpos_tcp_params[0]))) {
        return 1;
    }
    epson_escpos_init(&_dev, &epson_escpos_tcp_transport, &_tcp,
                      EPSON_ESCPOS_PARAM_PROFILE);
    printf("printer: %s\n", EPSON_ESCPOS_PARAM_TCP_HOST);
#elif defined(MODULE_EPSON_ESCPOS_UART)
    epson_escpos_uart_setup(&_uart, &epson_escpos_uart_params[0]);
    epson_escpos_init(&_dev, &epson_escpos_uart_transport, &_uart,
                      EPSON_ESCPOS_PARAM_PROFILE);
#elif defined(MODULE_EPSON_ESCPOS_DUMMY)
    epson_escpos_dummy_setup(&_dummy, _dummy_buf, sizeof(_dummy_buf));
    epson_escpos_init(&_dev, &epson_escpos_dummy_transport, &_dummy,
                      EPSON_ESCPOS_PARAM_PROFILE);
#endif

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
