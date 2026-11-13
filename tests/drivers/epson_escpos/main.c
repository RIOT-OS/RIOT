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
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "epson_escpos.h"
#include "epson_escpos_params.h"

#include "fmt.h"
#include "shell.h"

#if defined(MODULE_LWIP_IPV4) && defined(ESCPOS_IPV4_ADDR)
#  include "lwip/ip4_addr.h"
#  include "lwip/netif.h"
#  include "lwip/tcpip.h"
#endif

static epson_escpos_t _dev;

#if defined(MODULE_EPSON_ESCPOS_TCP)
static epson_escpos_tcp_t _tcp;
#elif defined(MODULE_EPSON_ESCPOS_UART)
static epson_escpos_uart_t _uart;
#endif

static int _cmd_open(int argc, char **argv)
{
    int res;
#if defined(MODULE_EPSON_ESCPOS_TCP)
    if (argc > 1) {
        epson_escpos_tcp_params_t params = {
            .host = argv[1],
            .port = CONFIG_EPSON_ESCPOS_TCP_PORT,
        };

        /* split "<host>:<port>" */
        char *port = strchr(argv[1], ':');
        if (port) {
            char *end;
            unsigned long num = strtoul(port + 1, &end, 10);
            if ((*end != '\0') || (num == 0) || (num > UINT16_MAX)) {
                printf("error: invalid port: %s\n", port + 1);
                return 1;
            }
            *port = '\0';
            params.port = num;
        }

        res = epson_escpos_tcp_setup(&_tcp, &params);
        if (res < 0) {
            printf("error: %s (%d)\n", strerror(-res), res);
            return 1;
        }
    }
#else
    (void)argc;
    (void)argv;
#endif
    res = epson_escpos_open(&_dev);
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    return 0;
}
SHELL_COMMAND(open, "open connection to the printer [host[:port]]", _cmd_open);

static int _cmd_close(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    epson_escpos_close(&_dev);
    return 0;
}
SHELL_COMMAND(close, "close connection to the printer", _cmd_close);

static int _cmd_text(int argc, char **argv)
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
    if (res == 0) {
        res = epson_escpos_flush(&_dev);
    }
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    return 0;
}
SHELL_COMMAND(text, "print text without newline", _cmd_text);

static int _cmd_print(int argc, char **argv)
{
    if (_cmd_text(argc, argv)) {
        return 1;
    }
    int res = epson_escpos_ln(&_dev, 1);
    if (res == 0) {
        res = epson_escpos_flush(&_dev);
    }
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    return 0;
}
SHELL_COMMAND(print, "print text followed by a newline", _cmd_print);

static int _cmd_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int res = epson_escpos_hw(&_dev, EPSON_ESCPOS_HW_INIT);
    if (res == 0) {
        res = epson_escpos_flush(&_dev);
    }
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    return 0;
}
SHELL_COMMAND(init, "initialize the printer (ESC @)", _cmd_init);

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
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    return 0;
}
SHELL_COMMAND(feed, "print and feed <n> lines", _cmd_feed);

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
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    return 0;
}
SHELL_COMMAND(cut, "cut the paper [part] [nofeed]", _cmd_cut);

static int _cmd_style(int argc, char **argv)
{
    int res;

    if ((argc == 2) && !strcmp(argv[1], "reset")) {
        res = epson_escpos_set_default(&_dev);
        if (res < 0) {
            printf("error: %s (%d)\n", strerror(-res), res);
            return 1;
        }
        return 0;
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
    int val_bool = scn_bool_str(val);

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
    else if (val_bool < 0) {
        goto invalid;
    }
    else if (!strcmp(key, "bold")) {
        style.fields = EPSON_ESCPOS_STYLE_BOLD;
        style.bold = val_bool;
    }
    else if (!strcmp(key, "invert")) {
        style.fields = EPSON_ESCPOS_STYLE_INVERT;
        style.invert = val_bool;
    }
    else if (!strcmp(key, "smooth")) {
        style.fields = EPSON_ESCPOS_STYLE_SMOOTH;
        style.smooth = val_bool;
    }
    else if (!strcmp(key, "flip")) {
        style.fields = EPSON_ESCPOS_STYLE_FLIP;
        style.flip = val_bool;
    }
    else {
        printf("error: unknown style \"%s\"\n", key);
        return 1;
    }

    res = epson_escpos_set(&_dev, &style);
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    return 0;

invalid:
    printf("error: invalid value \"%s\" for %s\n", val, key);
    return 1;
}
SHELL_COMMAND(style, "set text style", _cmd_style);

static int _cmd_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    static const char *paper[] = { "empty", "near end", "ok" };

    int online = epson_escpos_is_online(&_dev);
    if (online < 0) {
        printf("error: %s (%d)\n", strerror(-online), online);
        return 1;
    }
    int res = epson_escpos_paper_status(&_dev);
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    printf("printer is %s, paper: %s\n", online ? "online" : "offline", paper[res]);
    return 0;
}
SHELL_COMMAND(status, "query the printer status", _cmd_status);

static int _cmd_demo(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    static const epson_escpos_style_t title = {
        .fields = EPSON_ESCPOS_STYLE_ALIGN | EPSON_ESCPOS_STYLE_SIZE | EPSON_ESCPOS_STYLE_BOLD,
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

    /* separator over the full width of font A */
    char line[UINT8_MAX + 1];
    unsigned columns = _dev.profile->columns[EPSON_ESCPOS_FONT_A];
    memset(line, '-', columns);
    line[columns] = '\0';

    int res = 0;
    res += epson_escpos_hw(&_dev, EPSON_ESCPOS_HW_INIT);
    res += epson_escpos_set(&_dev, &title);
    res += epson_escpos_textln(&_dev, "RIOT OS");
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_set(&_dev, &center);
    res += epson_escpos_textln(&_dev, "ESC/POS driver test");
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_textln(&_dev, line);
    res += epson_escpos_text(&_dev, "You are running RIOT on a(n) ");
    res += epson_escpos_set(&_dev, &bold);
    res += epson_escpos_text(&_dev, RIOT_BOARD);
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_textln(&_dev, " board.");
    res += epson_escpos_text(&_dev, "This board features a(n) ");
    res += epson_escpos_set(&_dev, &bold);
    res += epson_escpos_text(&_dev, RIOT_CPU);
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_textln(&_dev, " CPU.");
    res += epson_escpos_textln(&_dev, line);
    res += epson_escpos_textln(&_dev, "normal text");
    res += epson_escpos_set(&_dev, &bold);
    res += epson_escpos_textln(&_dev, "bold text");
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_set(&_dev, &underline);
    res += epson_escpos_textln(&_dev, "underlined text");
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_set(&_dev, &invert);
    res += epson_escpos_textln(&_dev, " inverted text ");
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_set(&_dev, &font_b);
    res += epson_escpos_textln(&_dev, "font B text");
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_set(&_dev, &big);
    res += epson_escpos_textln(&_dev, "3x3");
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_set(&_dev, &right);
    res += epson_escpos_textln(&_dev, "right aligned");
    res += epson_escpos_set_default(&_dev);
    res += epson_escpos_cut(&_dev, EPSON_ESCPOS_CUT_PART, true);
    res += epson_escpos_flush(&_dev);

    if (res < 0) {
        printf("error printing the demo\n");
        return 1;
    }
    return 0;
}
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
    int res = epson_escpos_tcp_setup(&_tcp, &epson_escpos_tcp_params[0]);
    if (res < 0) {
        printf("error: %s (%d)\n", strerror(-res), res);
        return 1;
    }
    epson_escpos_init(&_dev, &epson_escpos_tcp_transport, &_tcp,
                      EPSON_ESCPOS_PARAM_PROFILE);
    printf("printer: %s\n", CONFIG_EPSON_ESCPOS_TCP_HOST);
#elif defined(MODULE_EPSON_ESCPOS_UART)
    epson_escpos_uart_setup(&_uart, &epson_escpos_uart_params[0]);
    epson_escpos_init(&_dev, &epson_escpos_uart_transport, &_uart,
                      EPSON_ESCPOS_PARAM_PROFILE);
#endif

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
