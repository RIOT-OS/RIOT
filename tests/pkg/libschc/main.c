/*
 * SPDX-FileCopyrightText: 2022 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Tests for the libSCHC package.
 *
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "bitfield.h"
#include "event.h"
#include "event/timeout.h"
#include "event/thread.h"
#include "fmt.h"
#include "kernel_defines.h"
#include "mutex.h"
#include "net/ipv6/addr.h"
#include "od.h"
#include "shell.h"
#include "thread_flags.h"
#include "ztimer.h"

#include "compressor.h"
#include "bit_operations.h"
#include "fragmenter.h"
#include "schc.h"

#define THREAD_FLAG_TX_END  (1U << 4)
#define FRAG_WIN_SIZE_MAX   (8U)
#define TIMEOUT_EVENTS_MAX  (4U)

typedef struct {
    event_t super;
    event_timeout_t timeout;
    void (*timer_task)(void *);
    void *arg;
} _timer_event_t;

static uint32_t _ack_delay = 0;
static unsigned _frag_counter = 0;
static thread_t *_main_thread;
static schc_fragmentation_t _rx_conn, _tx_conn;
static char _line_buf[SHELL_DEFAULT_BUFSIZE];
static _timer_event_t _timer_event;
static BITFIELD(_acks, FRAG_WIN_SIZE_MAX);
static uint8_t _input_buf[128U];
static uint8_t _output_buf[sizeof(_input_buf)];
static uint8_t _input_buf_len;
static uint8_t _output_buf_len;

static ssize_t _copy_input(int argc, char **argv)
{
    bool msn = true;   /* most significant nibble */
    ssize_t start = _input_buf_len;

    if (_input_buf_len >= sizeof(_input_buf)) {
        goto error;
    }
    for (int c = 0; c < argc; c++) {
        char *ptr = argv[c];

        while (*ptr) {
            uint8_t nibble = scn_u32_hex(ptr++, 1);

            if (_input_buf_len >= sizeof(_input_buf)) {
                goto error;
            }
            if (msn) {
                _input_buf[_input_buf_len] = nibble << 4;
            }
            else {
                _input_buf[_input_buf_len++] |= nibble;
            }
            msn = !msn;
        }
    }
    if (!msn) {
        _input_buf_len++;
    }
    return _input_buf_len - start;
error:
    puts("Too many bytes added to input buffer");
    return -ENOBUFS;
}

static int _input(int argc, char **argv)
{
    if (argc > 1 && (strcmp(argv[1], "reset") == 0)) {
        _input_buf_len = 0U;
        puts("Successfully reset input buffer.");
    }
    else if (argc < 2) {
        if (_input_buf_len) {
            od_hex_dump(_input_buf, _input_buf_len, OD_WIDTH_DEFAULT);
        }
        else {
            puts("Input buffer is empty.");
        }
        return 0;
    }
    else if (argc > 2 && (strcmp(argv[1], "add") == 0)) {
        ssize_t size = _copy_input(argc - 2, &argv[2]);
        if (size >= 0) {
            puts("Successfully added to input buffer");
            od_hex_dump(&_input_buf[_input_buf_len - size], size, OD_WIDTH_DEFAULT);
            puts("");
            return 0;
        }
        else {
            return 1;
        }
    }
    else {
        printf("usage: %s {reset|add <hex> ...}\n", argv[0]);
        return 1;
    }
    return 0;
}

static void _output_usage(const char *cmd)
{
    printf("usage: %s [{reset|copy}]\n", cmd);
}

static int _output(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "reset") == 0) {
            _output_buf_len = 0U;
            puts("Successfully reset output buffer.");
        }
        else if (strcmp(argv[1], "copy") == 0) {
            if (_output_buf_len) {
                memcpy(_input_buf, _output_buf, _output_buf_len);
            }
            _input_buf_len = _output_buf_len;
            puts("Successfully copied output buffer to input buffer.");
        }
        else {
            _output_usage(argv[0]);
            return 1;
        }
    }
    else {
        if (_output_buf_len) {
            od_hex_dump(_output_buf, _output_buf_len, OD_WIDTH_DEFAULT);
        }
        else {
            puts("Output buffer is empty.");
        }
    }
    return 0;
}

static int _parse_direction(const char *dir)
{
    if (strcmp(dir, "up") == 0) {
        return UP;
    }
    if (strcmp(dir, "down") == 0) {
        return DOWN;
    }
    if (strcmp(dir, "bi") == 0) {
        return BI;
    }
    return -1;
}

static void _compress_usage(char *cmd)
{
    printf("usage: %s {{up|down|bi} <device id>|init <source addr>}\n", cmd);
}

static int _compress(int argc, char **argv)
{
    if (argc > 2 && (strcmp(argv[1], "init") == 0)) {
        ipv6_addr_t addr;

        if (!ipv6_addr_from_str(&addr, argv[2])) {
            printf("Unable to parse source IPv6 address %s.\n", argv[2]);
            _compress_usage(argv[0]);
            return 1;
        }
        int res = schc_compressor_init(addr.u8);
        /* LCOV_EXCL_START schc_compressor_init always returns 1 */
        if (res != 1) {
            printf("Error initializing compressor with source IPv6 address %s.\n", argv[2]);
            return 1;
        }
        /* LCOV_EXCL_STOP */
        printf("Successfully initialized compressor with source IPv6 address %s.\n", argv[2]);
    }
    else if (argc > 2) {
        int dir = _parse_direction(argv[1]);
        uint32_t device_id = scn_u32_hex(argv[2], 1);

        if ((device_id == 0) || (dir < 0)) {
            _compress_usage(argv[0]);
        }
        if (_input_buf_len == 0) {
            puts("No input buffer defined");
            return 1;
        }
        /* compress packet */
        struct schc_compression_rule_t* comp_rule;
        memset(_output_buf, 0, sizeof(_output_buf));
        schc_bitarray_t bit_arr = SCHC_DEFAULT_BIT_ARRAY(0, _output_buf);

        comp_rule = schc_compress(_input_buf, _input_buf_len, &bit_arr, device_id, (direction)dir);
        if (comp_rule) {
            printf("Used rule %" PRIu32 "/%u to compress to\n", comp_rule->rule_id,
                   comp_rule->rule_id_size_bits);
            od_hex_dump(bit_arr.ptr, bit_arr.len, OD_WIDTH_DEFAULT);
            _output_buf_len = bit_arr.len;
        }
        else if (bit_arr.len > 0) {
            struct schc_device *device = get_device_by_id(device_id);

            assert(device);    /* LCOV_EXCL_BR_LINE hopefully always true */
            printf("Used uncompressed rule %" PRIu32 "/%u to generate\n",
                   device->uncomp_rule_id, device->uncomp_rule_id_size_bits);
            od_hex_dump(bit_arr.ptr, bit_arr.len, OD_WIDTH_DEFAULT);
            _output_buf_len = bit_arr.len;
        }
        else {
            printf("Unable to compress (maybe wrong device ID %" PRIu32 "?)\n", device_id);
        }
    }
    else {
        _compress_usage(argv[0]);
        return 1;
    }
    return 0;
}

static void _decompress_usage(char *cmd)
{
    printf("usage: %s {up|down|bi} <device id>\n", cmd);
}

static int _decompress(int argc, char **argv)
{
    if (argc > 2) {
        int dir = _parse_direction(argv[1]);
        uint32_t device_id = scn_u32_hex(argv[2], 1);

        if ((device_id == 0) || (dir < 0)) {
            _decompress_usage(argv[0]);
            return 1;
        }
        if (_input_buf_len == 0) {
            puts("No input buffer defined");
            return 1;
        }
        schc_bitarray_t bit_arr = SCHC_DEFAULT_BIT_ARRAY(_input_buf_len, _input_buf);
        memset(_output_buf, 0, sizeof(_output_buf));
        uint8_t len = schc_decompress(&bit_arr, _output_buf, device_id, bit_arr.len,
                                      (direction)dir);
        /* LCOV_EXCL_START len == 0 impossible to reach with current version of libSCHC */
        if (len == 0U) {
            puts("Unable to decompress input");
            return 1;
        }
        /* LCOV_EXCL_STOP */
        else {
            puts("Decompressed to");
            od_hex_dump(_output_buf, len, OD_WIDTH_DEFAULT);
        }
    }
    else {
        _decompress_usage(argv[0]);
        return 1;
    }
    return 0;
}

static void _set_ack_usage(char *cmd)
{
    printf("usage: %s <delay ms> <bitmap>\n", cmd);
}

static void _ack_print(void)
{
    printf("ACKs set to %" PRIu32 " ms delay with bitmap ", _ack_delay);
    for (size_t idx = 0; idx < FRAG_WIN_SIZE_MAX; idx++) {
        printf(bf_isset(_acks, idx) ? "1" : "0");
    }
    printf("\n");
}

static int _set_ack(int argc, char **argv)
{
    if (argc > 2) {
        uint32_t delay = scn_u32_dec(argv[1], 10);
        unsigned idx = 0;
        static BITFIELD(tmp, FRAG_WIN_SIZE_MAX) = { 0 };

        static_assert(sizeof(tmp) == sizeof(_acks), "sizeof(tmp) != sizeof(_acks)");
        for (char *bitmap_str = argv[2];
             *bitmap_str;
             bitmap_str++, idx++) {
            if (idx >= FRAG_WIN_SIZE_MAX) {
                printf("%s does not fit fragment window size %u\n", argv[2],
                       FRAG_WIN_SIZE_MAX);
            }
            if (*bitmap_str == '1') {
                bf_set(tmp, idx);
            }
            else if (*bitmap_str == '0') {
                bf_unset(tmp, idx);
            }
            else {
                _set_ack_usage(argv[0]);
                return 1;
            }
        }
        memcpy(_acks, tmp, sizeof(_acks));
        _ack_delay = delay;
        _ack_print();
    }
    else if (argc == 1) {
        _ack_print();
    }
    else {
        _set_ack_usage(argv[0]);
        return 1;
    }
    return 0;
}

static bool _ack_all1(void)
{
#if (FRAG_WIN_SIZE_MAX > 8)
        for (unsigned idx = 0; idx < sizeof(_acks) - 1; idx++) {
            if (_acks[idx] != 0xff) {
                return false;
            }
        }
    }
#endif
    for (unsigned idx = (sizeof(_acks) - 1) * 8; idx < FRAG_WIN_SIZE_MAX; idx++) {
        if (!bf_isset(_acks, idx)) {
            return false;
        }
    }
    return true;
}

static void _timer_cb(event_t *evt)
{
    _timer_event_t *timer_event = container_of(evt, _timer_event_t, super);
    schc_fragmentation_t *conn = timer_event->arg;
    conn->timer_ctx = NULL;
    if (conn->device_id) {  /* prevent firing of expired or canceled timers */
        timer_event->timer_task(timer_event->arg);
    }
}

static void _simulate_ack(schc_fragmentation_t *conn, uint32_t device_id)
{
    if ((_tx_conn.fragmentation_rule->mode != ACK_ALWAYS) &&
        ((_tx_conn.fragmentation_rule->mode != ACK_ON_ERROR) || _ack_all1())) {
        return;
    }
    if (_ack_delay > 0) {
        ztimer_sleep(ZTIMER_MSEC, _ack_delay);
    }

    uint8_t ack[((_tx_conn.fragmentation_rule->rule_id_size_bits / 8 + 1)) +
                DTAG_SIZE_BYTES + BITMAP_SIZE_BYTES];
    uint8_t offset = _tx_conn.fragmentation_rule->rule_id_size_bits;

    uint8_t rule_id[4] = { 0 };
    memset(ack, 0, sizeof(ack));
    little_end_uint8_from_uint32(rule_id, conn->fragmentation_rule->rule_id);
    copy_bits(ack, 0, rule_id, 0, offset);
    copy_bits(ack, offset, conn->ack.dtag, 0, conn->fragmentation_rule->DTAG_SIZE);
    offset += conn->fragmentation_rule->DTAG_SIZE;

    uint8_t window[1] = { conn->window << (8 - conn->fragmentation_rule->WINDOW_SIZE) };
    copy_bits(ack, offset, window, 0, conn->fragmentation_rule->WINDOW_SIZE);
    offset += conn->fragmentation_rule->WINDOW_SIZE;

    if (_ack_all1()) {
        uint8_t c[1] = { 1 << (8 - MIC_C_SIZE_BITS) };
        copy_bits(ack, offset, c, 0, MIC_C_SIZE_BITS);
        offset += MIC_C_SIZE_BITS;
    }
    else {
        uint8_t bitmap[BITMAP_SIZE_BYTES] = { 0 };
        bool set_true = true;
        for (unsigned idx = 0; idx < FRAG_WIN_SIZE_MAX; idx++) {
            if (bf_isset(_acks, idx)) {
                set_bits(bitmap, idx, 1);
            }
            else {
                clear_bits(bitmap, idx, 1);
                if (set_true) {
                    /* set first 0 for next round */
                    bf_set(_acks, idx);
                    set_true = false;
                }
            }
        }
        copy_bits(ack, offset, bitmap, 0, conn->fragmentation_rule->MAX_WND_FCN + 1);
        offset += conn->fragmentation_rule->MAX_WND_FCN + 1; /* TODO must be encoded? */
    }
    uint8_t packet_len = ((offset - 1) / 8) + 1;
    puts("Simulate ACK");
    od_hex_dump(ack, packet_len, OD_WIDTH_DEFAULT);
    schc_input(ack, packet_len, conn, device_id);
}

static uint8_t _tx_cb(uint8_t* data, uint16_t length, uint32_t device_id)
{
    printf("TX Fragment %u on dev 0x%lx\n", ++_frag_counter, (long unsigned)device_id);
    od_hex_dump(data, length, OD_WIDTH_DEFAULT);
    _simulate_ack(&_tx_conn, device_id);
    return 1;
}

static uint8_t _rx_cb(uint8_t* data, uint16_t length, uint32_t device_id)
{
    printf("Packet sent on dev 0x%lx\n", (long unsigned)device_id);
    od_hex_dump(data, length, OD_WIDTH_DEFAULT);
    return 1;
}

static void _tx_end(schc_fragmentation_t *conn)
{
    (void)conn;
    _frag_counter = 0;
    puts("TX End");
    thread_flags_set(_main_thread, THREAD_FLAG_TX_END);
}

/* LCOV_EXCL_START _remove_timer is never called by libSCHC in tests */
static void _free_event(_timer_event_t *evt)
{
    event_timeout_clear(&evt->timeout);
    /* cancel event in case it already was posted */
    event_cancel(evt->timeout.queue, evt->timeout.event);
}

static void _remove_timer(struct schc_fragmentation_t *conn)
{
    _timer_event_t *evt = conn->timer_ctx;
    if (evt) {
        _free_event(evt);
        conn->timer_ctx = NULL;
    }
}
/* LCOV_EXCL_STOP */

static void _rx_end(schc_fragmentation_t *conn)
{
    _output_buf_len = get_mbuf_len(conn);
    mbuf_copy(conn, _output_buf);
    od_hex_dump(_output_buf, _output_buf_len, OD_WIDTH_DEFAULT);
}

static void _reset_timers(void)
{
    _timer_event.super.handler = _timer_cb;
    event_timeout_ztimer_init(&_timer_event.timeout, ZTIMER_MSEC, EVENT_PRIO_HIGHEST,
                              &_timer_event.super);
}

static int _timer(int argc, char **argv)
{
    if ((argc > 1) && (strcmp(argv[1], "reset") == 0)) {
        _reset_timers();
        puts("Reset all timers");
    }
    else {
        printf("Free timers: %u (of %u)\n",
               TIMEOUT_EVENTS_MAX,
               TIMEOUT_EVENTS_MAX);
    }
    return 1;
}

static void _set_schc_timer(schc_fragmentation_t *conn,
                            void (*callback)(void* conn), uint32_t delay, void *arg)
{
    assert(conn->timer_ctx == NULL);    /* LCOV_EXCL_BR_LINE hopefully always true */
    _timer_event.timer_task = callback;
    _timer_event.arg = arg;
    conn->timer_ctx = &_timer_event;
    event_timeout_set(&_timer_event.timeout, delay);
}

static reliability_mode _parse_reliability_mode(const char *mode)
{
    if (strcmp(mode, "no-ack") == 0) {
        return NO_ACK;
    }
    if (strcmp(mode, "ack-always") == 0) {
        return ACK_ALWAYS;
    }
    if (strcmp(mode, "ack-on-error") == 0) {
        return ACK_ON_ERROR;
    }
    if (strcmp(mode, "not-fragmented") == 0) {
        return NOT_FRAGMENTED;
    }
    return 0;
}

static void _fragment_usage(char *cmd)
{
    printf("usage: %s <device id> <MTU> <duty cycle> "
           "{no-ack|ack-always|ack-on-error|not-fragmented}\n", cmd);
}

static int _fragment(int argc, char **argv)
{
    if (argc > 4) {
        uint32_t device_id = scn_u32_hex(argv[1], 1);
        uint32_t mtu = scn_u32_dec(argv[2], 4);
        uint32_t dc = scn_u32_dec(argv[3], 5);
        reliability_mode mode = _parse_reliability_mode(argv[4]);

        if ((device_id == 0) || (mtu == 0) || (mtu > 1280) || (dc == 0) || (mode == 0)) {
            _fragment_usage(argv[0]);
            return 1;
        }
        if (_input_buf_len == 0) {
            puts("No input buffer defined");
            return 1;
        }
        struct schc_fragmentation_rule_t *frag_rule = NULL;
        schc_bitarray_t bit_arr = SCHC_DEFAULT_BIT_ARRAY(_input_buf_len, _input_buf);

        frag_rule = get_fragmentation_rule_by_reliability_mode(mode, device_id);

        if (!frag_rule) {
            printf("No fragmentation rule known for mode %s on device %s\n",
                   argv[4], argv[1]);
            return 1;
        }
        _tx_conn.bit_arr = &bit_arr;
        _tx_conn.device_id = device_id;
        _tx_conn.mtu = mtu;
        _tx_conn.dc = dc;
        _tx_conn.send = _tx_cb;
        _tx_conn.end_tx = _tx_end;
        _tx_conn.fragmentation_rule = frag_rule;
        _tx_conn.post_timer_task = _set_schc_timer;
        _tx_conn.remove_timer_entry = _remove_timer;
        int ret = schc_fragment(&_tx_conn);
        if (ret == -2) {
            puts("No fragmentation needed");
            _frag_counter = 0;
        }
        else {
            assert(ret >= 0);  /* LCOV_EXCL_BR_LINE hopefully always true */
            thread_flags_wait_one(THREAD_FLAG_TX_END);
            puts("Fragmented!");
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "init") == 0)) {
        memset(&_tx_conn, 0, sizeof(_tx_conn));
        int res = schc_fragmenter_init(&_tx_conn);
        /* LCOV_EXCL_START schc_fragmenter_init always returns 1 */
        if (res != 1) {
            puts("Error initializing fragmenter.");
            return 1;
        }
        /* LCOV_EXCL_STOP */
        puts("Fragmenter initialized");
    }
    else {
        _fragment_usage(argv[0]);
        return 1;
    }
    return 0;
}

static void _reassemble_usage(char *cmd)
{
    printf("usage: %s <device id> <timeout>\n", cmd);
}

static int _reassemble(int argc, char **argv)
{
    if (argc > 2) {
        uint32_t device_id = scn_u32_hex(argv[1], 1);
        uint32_t timeout = scn_u32_dec(argv[2], 5);

        if ((device_id == 0) || (timeout == 0)) {
            _reassemble_usage(argv[0]);
            return 1;
        }
        if (_input_buf_len == 0) {
            puts("No input buffer defined");
            return 1;
        }

        schc_bitarray_t bit_arr = SCHC_DEFAULT_BIT_ARRAY(_input_buf_len, _input_buf);
        _rx_conn.bit_arr = &bit_arr;
        /* get active connection and set the correct rule for this connection */
        schc_fragmentation_t *conn = schc_input(_input_buf, _input_buf_len, &_rx_conn, device_id);

        /* if returned value is &_rx_conn: acknowledgement is received, which is handled by the
         * library */
        if (conn != &_rx_conn) {    /* LCOV_EXCL_BR_LINE */
            conn->post_timer_task = _set_schc_timer;
            conn->dc = timeout;

            if (!conn->fragmentation_rule || conn->fragmentation_rule->mode == NOT_FRAGMENTED) {
                /* packet was not fragmented */
                printf("RX Unfragmented on dev 0x%lx\n", (long unsigned)conn->device_id);
                _rx_end(conn); /* final packet arrived */
            } else {
                int ret = schc_reassemble(conn);

                if (ret) {
                    printf("RX Reassembly complete on dev 0x%lx\n",
                           (long unsigned)conn->device_id);
                }
                /* use the connection to reassemble */
                if (ret && (conn->fragmentation_rule->mode == NO_ACK)) {
                    _rx_end(conn); /* final packet arrived */
                }
            }
        }
    }
    else if ((argc > 1) && (strcmp(argv[1], "init") == 0)) {
        memset(&_tx_conn, 0, sizeof(_tx_conn));
        memset(&_rx_conn, 0, sizeof(_rx_conn));
        int res = schc_fragmenter_init(&_tx_conn);
        /* LCOV_EXCL_START schc_fragmenter_init always returns 1 */
        if (res != 1) {
            puts("Error initializing fragmenter.");
            return 1;
        }
        /* LCOV_EXCL_STOP */
        _rx_conn.send = _rx_cb;
        _rx_conn.end_rx = &_rx_end;
        _rx_conn.remove_timer_entry = &_remove_timer;
        puts("Reassembler initialized");
    }
    else {
        _reassemble_usage(argv[0]);
        return 1;
    }
    return 0;
}

extern int unittests(int argc, char **argv);

static const shell_command_t _shell_commands[] = {
    { "input", "Add bytes to input buffer", _input },
    { "output", "Handle output buffer", _output },
    { "compress", "Compress input buffer using libSCHC with preconfigured rules", _compress },
    { "decompress", "Decompress input buffer using libSCHC with preconfigured rules", _decompress },
    { "set_ack", "Set fragments to ACK with libSCHC", _set_ack },
    { "timer", "Check or reset running timers", _timer },
    { "fragment", "Fragment input buffer using libSCHC with preconfigured rules", _fragment },
    { "reassemble", "Reassemble fragment from input buffer using libSCHC with preconfigured rules",
      _reassemble },
    { "unittests", "Run unittests for libSCHC", unittests },
    { NULL, NULL, NULL }
};

int main(void)
{
    _main_thread = thread_get_active();
    memset(_acks, 0xff, sizeof(_acks));
    _reset_timers();
    shell_run(_shell_commands, _line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;   /* LCOV_EXCL_LINE never reached */
}
