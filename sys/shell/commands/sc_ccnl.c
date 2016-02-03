/*
 * Copyright (C) 2015, 2016  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands to interact with the CCN-Lite stack
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "random.h"
#include "sched.h"
#include "net/gnrc/netif.h"
#include "ccn-lite-riot.h"
#include "ccnl-pkt-ndntlv.h"

#define BUF_SIZE (64)

/**
 * Maximum number of Interest retransmissions
 */
#define CCNL_INTEREST_RETRIES   (3)

#define MAX_ADDR_LEN            (8U)

static unsigned char _int_buf[BUF_SIZE];
static unsigned char _cont_buf[BUF_SIZE];

static const char *_default_content = "Start the RIOT!";
static unsigned char _out[CCNL_MAX_PACKET_SIZE];

/* check for one-time initialization */
static bool started = false;

/* usage for open command */
static void _open_usage(void)
{
    puts("ccnl <interface>");
}

int _ccnl_open(int argc, char **argv)
{
    /* check if already running */
    if (started) {
        puts("Already opened an interface for CCN!");
        return -1;
    }

    /* check if parameter is given */
    if (argc != 2) {
        _open_usage();
        return -1;
    }

    /* check if given number is a valid netif PID */
    int pid = atoi(argv[1]);
    if (!gnrc_netif_exist(pid)) {
        printf("%i is not a valid interface!\n", pid);
        return -1;
    }

    ccnl_start();

    /* set the relay's PID, configure the interface to interface to use CCN
     * nettype */
    if (ccnl_open_netif(pid, GNRC_NETTYPE_CCN) < 0) {
        puts("Error registering at network interface!");
        return -1;
    }

    started = true;

    return 0;
}


static void _content_usage(char *argv)
{
    printf("usage: %s <URI> [content]\n"
            "%% %s /riot/peter/schmerzl             (default content)\n"
            "%% %s /riot/peter/schmerzl RIOT\n",
            argv, argv, argv);
}

int _ccnl_content(int argc, char **argv)
{
    char *body = (char*) _default_content;
    int arg_len = strlen(_default_content) + 1;
    int offs = CCNL_MAX_PACKET_SIZE;
    if (argc < 2) {
        _content_usage(argv[0]);
        return -1;
    }

    if (argc > 2) {
        char buf[BUF_SIZE];
        memset(buf, ' ', BUF_SIZE);
        char *buf_ptr = buf;
        for (int i = 2; (i < argc) && (buf_ptr < (buf + BUF_SIZE)); i++) {
            arg_len = strlen(argv[i]);
            if ((buf_ptr + arg_len) > (buf + BUF_SIZE)) {
                arg_len = (buf + BUF_SIZE) - buf_ptr;
            }
            strncpy(buf_ptr, argv[i], arg_len);
            buf_ptr += arg_len + 1;
        }
        *buf_ptr = '\0';
        body = buf;
        arg_len = strlen(body);
    }

    int suite = CCNL_SUITE_NDNTLV;

    struct ccnl_prefix_s *prefix = ccnl_URItoPrefix(argv[1], suite, NULL, NULL);

    arg_len = ccnl_ndntlv_prependContent(prefix, (unsigned char*) body, arg_len, NULL, NULL, &offs, _out);

    unsigned char *olddata;
    unsigned char *data = olddata = _out + offs;

    int len;
    unsigned typ;

    if (ccnl_ndntlv_dehead(&data, &arg_len, (int*) &typ, &len) ||
        typ != NDN_TLV_Data) {
        return -1;
    }

    struct ccnl_content_s *c = 0;
    struct ccnl_pkt_s *pk = ccnl_ndntlv_bytes2pkt(typ, olddata, &data, &arg_len);
    c = ccnl_content_new(&ccnl_relay, &pk);
    ccnl_content_add2cache(&ccnl_relay, c);
    c->flags |= CCNL_CONTENT_FLAGS_STATIC;

    return 0;
}

static struct ccnl_face_s *_intern_face_get(char *addr_str)
{
    /* initialize address with 0xFF for broadcast */
    size_t addr_len = MAX_ADDR_LEN;
    uint8_t relay_addr[MAX_ADDR_LEN];
    memset(relay_addr, UINT8_MAX, MAX_ADDR_LEN);

    addr_len = gnrc_netif_addr_from_str(relay_addr, sizeof(relay_addr), addr_str);
    if (addr_len == 0) {
        printf("Error: %s is not a valid link layer address\n", addr_str);
        return NULL;
    }

    sockunion sun;
    sun.sa.sa_family = AF_PACKET;
    memcpy(&(sun.linklayer.sll_addr), relay_addr, addr_len);
    sun.linklayer.sll_halen = addr_len;
    sun.linklayer.sll_protocol = htons(ETHERTYPE_NDN);

    /* TODO: set correct interface instead of always 0 */
    struct ccnl_face_s *fibface = ccnl_get_face_or_create(&ccnl_relay, 0, &sun.sa, sizeof(sun.linklayer));

    return fibface;
}

static int _intern_fib_add(char *pfx, char *addr_str)
{
    int suite = CCNL_SUITE_NDNTLV;
    struct ccnl_prefix_s *prefix = ccnl_URItoPrefix(pfx, suite, NULL, 0);
    if (!prefix) {
        puts("Error: prefix could not be created!");
        return -1;
    }

    struct ccnl_face_s *fibface = _intern_face_get(addr_str);
    if (fibface == NULL) {
        return -1;
    }
    fibface->flags |= CCNL_FACE_FLAGS_STATIC;

    if (ccnl_fib_add_entry(&ccnl_relay, prefix, fibface) != 0) {
        printf("Error adding (%s : %s) to the FIB\n", pfx, addr_str);
        return -1;
    }

    return 0;
}

static void _interest_usage(char *arg)
{
    printf("usage: %s <URI> [relay]\n"
            "%% %s /riot/peter/schmerzl                     (classic lookup)\n",
            arg, arg);
}

int _ccnl_interest(int argc, char **argv)
{
    if (argc < 2) {
        _interest_usage(argv[0]);
        return -1;
    }

    if (argc > 2) {
        if (_intern_fib_add(argv[1], argv[2]) < 0) {
            _interest_usage(argv[0]);
            return -1;
        }
    }

    memset(_int_buf, '\0', BUF_SIZE);
    memset(_cont_buf, '\0', BUF_SIZE);
    for (int cnt = 0; cnt < CCNL_INTEREST_RETRIES; cnt++) {
        gnrc_netreg_entry_t _ne;
        /* register for content chunks */
        _ne.demux_ctx =  GNRC_NETREG_DEMUX_CTX_ALL;
        _ne.pid = sched_active_pid;
        gnrc_netreg_register(GNRC_NETTYPE_CCN_CHUNK, &_ne);

        ccnl_send_interest(CCNL_SUITE_NDNTLV, argv[1], NULL, _int_buf, BUF_SIZE);
        if (ccnl_wait_for_chunk(_cont_buf, BUF_SIZE, 0) > 0) {
            gnrc_netreg_unregister(GNRC_NETTYPE_CCN_CHUNK, &_ne);
            printf("Content received: %s\n", _cont_buf);
            return 0;
        }
        gnrc_netreg_unregister(GNRC_NETTYPE_CCN_CHUNK, &_ne);
    }
    printf("Timeout! No content received in response to the Interest for %s.\n", argv[1]);

    return -1;
}

static void _ccnl_fib_usage(char *argv)
{
    printf("usage: %s [<action> <options>]\n"
           "prints the FIB if called without parameters:\n"
           "%% %s\n"
           "<action> may be one of the following\n"
           "  * \"add\" - adds an entry to the FIB, requires a prefix and a next-hop address, e.g.\n"
           "            %s add /riot/peter/schmerzl ab:cd:ef:01:23:45:67:89\n"
           "  * \"del\" - deletes an entry to the FIB, requires a prefix or a next-hop address, e.g.\n"
           "            %s del /riot/peter/schmerzl\n"
           "            %s del ab:cd:ef:01:23:45:67:89\n",
            argv, argv, argv, argv, argv);
}

int _ccnl_fib(int argc, char **argv)
{
    if (argc < 2) {
        ccnl_fib_show(&ccnl_relay);
    }
    else if ((argc == 3) && (strncmp(argv[1], "del", 3) == 0)) {
        int suite = CCNL_SUITE_NDNTLV;
        if (strchr(argv[2], '/')) {
            struct ccnl_prefix_s *prefix = ccnl_URItoPrefix(argv[2], suite, NULL, 0);
            if (!prefix) {
                puts("Error: prefix could not be created!");
                return -1;
            }
            int res = ccnl_fib_rem_entry(&ccnl_relay, prefix, NULL);
            free_prefix(prefix);
            return res;
        }
        else {
            struct ccnl_face_s *face = _intern_face_get(argv[2]);
            if (face == NULL) {
                printf("There is no face for address %s\n", argv[1]);
                return -1;
            }
            int res = ccnl_fib_rem_entry(&ccnl_relay, NULL, face);
            return res;
        }
    }
    else if ((argc == 4) && (strncmp(argv[1], "add", 3) == 0)) {
        if (_intern_fib_add(argv[2], argv[3]) < 0) {
            _ccnl_fib_usage(argv[0]);
            return -1;
        }
    }
    else {
        _ccnl_fib_usage(argv[0]);
        return -1;
    }
    return 0;
}
