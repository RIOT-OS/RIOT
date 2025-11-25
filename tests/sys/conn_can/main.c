/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       main
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "shell.h"
#include "board.h"
#include "periph/gpio.h"
#include "thread.h"

#include "can/can.h"
#include "can/conn/raw.h"
#include "can/conn/isotp.h"
#include "can/device.h"

#include "can/can_trx.h"

#ifdef MODULE_TJA1042
#include "tja1042.h"
tja1042_trx_t tja1042 = { .trx.driver = &tja1042_driver,
                          .stb_pin = TJA1042_STB_PIN
};
#endif

#ifdef MODULE_NCV7356
#include "ncv7356.h"
ncv7356_trx_t ncv7356 = { .trx.driver = &ncv7356_driver,
                          .mode0_pin = NCV7356_MODE0_PIN,
                          .mode1_pin = NCV7356_MODE1_PIN
};
#endif

static can_trx_t *devs[] = {
#ifdef MODULE_TJA1042
    (can_trx_t *)&tja1042,
#endif
#ifdef MODULE_NCV7356
    (can_trx_t *)&ncv7356,
#endif
    NULL,
};

#define THREAD_STACKSIZE   (THREAD_STACKSIZE_MAIN)
#define RECEIVE_THREAD_MSG_QUEUE_SIZE   (8)

#include "timex.h"
#define TEST_CONN_CAN_RECV_TIMEOUT (30 * US_PER_SEC)

#define RCV_THREAD_NUMOF (2)

#define MAX_FILTER      (16)

#define CAN_MSG_RECV        0x400
#define CAN_MSG_BIND_ISOTP  0x401
#define CAN_MSG_RECV_ISOTP  0x402
#define CAN_MSG_CLOSE_ISOTP 0x403
#define CAN_MSG_SEND_ISOTP  0x404

static char thread_stack[RCV_THREAD_NUMOF][THREAD_STACKSIZE];
static kernel_pid_t receive_pid[RCV_THREAD_NUMOF];

static conn_can_raw_t conn[RCV_THREAD_NUMOF];
static struct can_filter filters[RCV_THREAD_NUMOF][MAX_FILTER];

#ifdef MODULE_CAN_ISOTP
#define ISOTP_BUF_SIZE 1024
static uint8_t isotp_buf[RCV_THREAD_NUMOF][ISOTP_BUF_SIZE];

static conn_can_isotp_t conn_isotp[RCV_THREAD_NUMOF];
#endif

static int thread_busy[RCV_THREAD_NUMOF];

static void print_usage(void)
{
    puts("test_can list");
    puts("test_can send ifnum can_id [B1 [B2 [B3 [B4 [B5 [B6 [B7 [B8]]]]]]]]");
    puts("test_can sendrtr ifnum can_id length(0..8)");
    printf("test_can recv ifnum user_id timeout can_id1 [can_id2..can_id%d]\n",
        MAX_FILTER);
    puts("test_can close user_id");
#ifdef MODULE_CAN_ISOTP
    puts("test_can bind_isotp ifnum user_id source_id dest_id");
    puts("test_can send_isotp user_id [B1 [.. [ Bn ]]]");
    puts("test_can recv_isotp user_id timeout");
    puts("test_can close_isotp user_id");
#endif
    puts("test_can get_filter ifnum");
    puts("test_can set_bitrate ifnum bitrate [sample_point]");
    puts("test_can get_bitrate ifnum");
    puts("test_can get_counter ifnum");
    puts("test_can power_up ifnum");
    puts("test_can power_down ifnum");
}

static int _list(int argc, char **argv) {

    (void)argc;
    (void)argv;

    for (int i = 0; i < CAN_DLL_NUMOF; i++) {
        const char *name = raw_can_get_name_by_ifnum(i);
        if (name) {
            printf("CAN #%d: %s\n", i, name);
        }
        else {
            break;
        }
    }

    return 0;
}

static int _send(int argc, char **argv, bool rtr)
{
    if (argc < 5) {
        print_usage();
        return 1;
    }
    struct can_frame frame;
    int ifnum = strtol(argv[2], NULL, 0);
    if (ifnum >= CAN_DLL_NUMOF) {
        puts("Invalid interface number");
        return 1;
    }

    if (rtr) {
        frame.can_id = CAN_RTR_FLAG | strtoul(argv[3], NULL, 16);
        frame.can_dlc = strtoul(argv[4], NULL, 10);
    } else {
        frame.can_id = strtoul(argv[3], NULL, 16);
        frame.can_dlc = argc - 4;
    }
    if (frame.can_dlc > 8) {
        puts("Invalid length");
        return 1;
    }

    if (rtr) {
        for (int i = 0; i < frame.can_dlc; i++) {
            frame.data[i] = 0x0;
        }
    } else {
        for (int i = 0; i < frame.can_dlc; i++) {
            frame.data[i] = strtol(argv[4 + i], NULL, 16);
        }
    }

    conn_can_raw_t conn;
    conn_can_raw_create(&conn, NULL, 0, ifnum, 0);
    int ret = conn_can_raw_send(&conn, &frame, 0);
    if (ret < 0) {
        puts("Error when trying to send");
    }
    return 0;
}

static int _receive(int argc, char **argv)
{
    if (argc < 4) {
        print_usage();
        return 1;
    }
    int res;
    int ifnum = strtol(argv[2], NULL, 0);
    if (ifnum >= CAN_DLL_NUMOF) {
        puts("Invalid interface number");
        return 1;
    }

    int thread_nb = strtol(argv[3], NULL, 0);
    int filt_num = argc - 5;
    if (thread_nb >= RCV_THREAD_NUMOF) {
        printf("Invalid thread number, range=0..%d\n", RCV_THREAD_NUMOF - 1);
        return 1;
    }
    if (thread_busy[thread_nb]) {
        puts("Thread already in use");
        return 1;
    }
    if (filt_num > MAX_FILTER) {
        puts("Too many filters");
        return 1;
    }
    for (int i = 0; i < filt_num; i++) {
        filters[thread_nb][i].can_id = strtoul(argv[5 + i], NULL, 16);
        filters[thread_nb][i].can_mask = 0xffffffff;
    }
    uint32_t timeout = strtoul(argv[4], NULL, 0);
    msg_t msg;
    msg.type = CAN_MSG_RECV;
    msg.content.value = timeout;
    res = conn_can_raw_create(&conn[thread_nb], filters[thread_nb],
                              filt_num, ifnum, 0);
    if (res < 0) {
        puts("Error when setting filters");
        return 1;
    }
    thread_busy[thread_nb] = 1;
    msg_send(&msg, receive_pid[thread_nb]);
    return 0;
}

static int _close(int argc, char **argv)
{
    if (argc < 2) {
        print_usage();
        return 1;
    }
    int thread_nb = strtol(argv[2], NULL, 0);
    if (thread_nb >= RCV_THREAD_NUMOF) {
        printf("Invalid thread number, range=0..%d\n", RCV_THREAD_NUMOF - 1);
        return 1;
    }
    conn_can_raw_close(&conn[thread_nb]);
    thread_busy[thread_nb] = 0;
    return 0;
}

#ifdef MODULE_CAN_ISOTP
static int _bind_isotp(int argc, char **argv)
{
    if (argc < 4) {
        print_usage();
        return 1;
    }
    int ret;
    int ifnum = strtol(argv[2], NULL, 0);
    if (ifnum >= CAN_DLL_NUMOF) {
        puts("Invalid interface number");
        return 1;
    }

    int thread_nb = strtol(argv[3], NULL, 0);
    if (thread_nb >= RCV_THREAD_NUMOF) {
        printf("Invalid thread number, range=0..%d\n", RCV_THREAD_NUMOF - 1);
        return 1;
    }
    if (thread_busy[thread_nb]) {
        puts("Thread already in use");
        return 1;
    }
    struct isotp_options isotp_opt;
    memset(&isotp_opt, 0, sizeof(isotp_opt));
    isotp_opt.tx_id = strtoul(argv[4], NULL, 16);
    isotp_opt.rx_id = strtoul(argv[5], NULL, 16);

#ifdef MODULE_CONN_CAN_ISOTP_MULTI
    conn_can_isotp_init_slave(&conn_isotp[thread_nb], (conn_can_isotp_slave_t *)
        &conn_isotp[thread_nb]);
#endif
    ret = conn_can_isotp_create(&conn_isotp[thread_nb], &isotp_opt, ifnum);
    if (ret == 0) {
        ret = conn_can_isotp_bind(&conn_isotp[thread_nb], NULL);
    }
    if (ret < 0) {
        puts("Error when binding connection");
        return 1;
    }
    thread_busy[thread_nb] = 1;
    return 0;
}

static int _send_isotp(int argc, char **argv)
{
    if (argc < 4) {
        print_usage();
        return 1;
    }
    int thread_nb = strtoul(argv[2], NULL, 0);
    if (thread_nb >= RCV_THREAD_NUMOF) {
        printf("Invalid thread number, range=0..%d\n", RCV_THREAD_NUMOF - 1);
        return 1;
    }
    int len = argc - 3;
    uint8_t data[len];

    for (int i = 0; i < len; i++) {
        data[i] = strtol(argv[3 + i], NULL, 16);
    }

    msg_t msg, reply;
    can_opt_t opt;
    opt.data = data;
    opt.data_len = len;
    msg.type = CAN_MSG_SEND_ISOTP;
    msg.content.ptr = &opt;
    int res = msg_send_receive(&msg, &reply, receive_pid[thread_nb]);
    if (res < 0 || (int)reply.content.value < 0) {
        puts("Error when sending");
        return 1;
    }
    return 0;
}

static int _receive_isotp(int argc, char **argv)
{
    if (argc < 4) {
        print_usage();
        return 1;
    }
    int thread_nb = strtol(argv[2], NULL, 0);
    if (thread_nb >= RCV_THREAD_NUMOF) {
        printf("Invalid thread number, range=0..%d\n", RCV_THREAD_NUMOF - 1);
        return 1;
    }
    uint32_t timeout = strtoul(argv[3], NULL, 0);

    msg_t msg;
    msg.type = CAN_MSG_RECV_ISOTP;
    msg.content.value = timeout;
    msg_send(&msg, receive_pid[thread_nb]);

    return 0;
}

static int _close_isotp(int argc, char **argv)
{
    if (argc < 2) {
        print_usage();
        return 1;
    }
    int thread_nb = strtol(argv[2], NULL, 0);
    if (thread_nb >= RCV_THREAD_NUMOF) {
        printf("Invalid thread number, range=0..%d\n", RCV_THREAD_NUMOF - 1);
        return 1;
    }
    conn_can_isotp_close(&conn_isotp[thread_nb]);
    thread_busy[thread_nb] = 0;
    return 0;
}
#endif /* MODULE_CAN_ISOTP */

static int _get_filter(int argc, char **argv)
{
    if (argc < 3) {
        print_usage();
        return 1;
    }
    int res;
    int ifnum = strtol(argv[2], NULL, 0);
    struct can_filter filters[32];
    can_opt_t opt;
    opt.data = (void *)filters;
    opt.data_len = sizeof(filters);
    opt.opt = CANOPT_RX_FILTERS;
    res = raw_can_get_can_opt(ifnum, &opt);
    if (res < 0) {
        puts("Error when reading filters");
    }
    else if (res == 0) {
        puts("No filter set");
    }
    else {
        for (unsigned int i = 0; i < res / sizeof(filters[0]); i++) {
            printf("Filter %d: 0x%" PRIx32"\n", i, filters[i].can_id);
            printf("Mask %d: 0x%" PRIx32"\n", i, filters[i].can_mask);
        }
    }
    return 0;
}

static int _set_bitrate(int argc, char **argv)
{
    if (argc < 4) {
        print_usage();
        return 1;
    }

    int ifnum = strtol(argv[2], NULL, 0);
    if (ifnum >= CAN_DLL_NUMOF) {
        printf("Invalid ifnum %d\n", ifnum);
        return 1;
    }
    uint32_t bitrate = strtoul(argv[3], NULL, 0);
    uint32_t sample_point = 0;

    int ret;
    if (argc > 4) {
        sample_point = strtoul(argv[4], NULL, 0);
    }
    printf("Setting bitrate=%" PRIu32 ", sample point=%" PRIu32 "\n",
           bitrate, sample_point);
    ret = raw_can_set_bitrate(ifnum, bitrate, sample_point);
    if (ret < 0) {
        printf("Error when setting bitrate: res=%d\n", ret);
        return 1;
    }
    else if (ret == 1) {
        puts("Bitrate/sample_point cannot be reached");
    }

    puts("Bittimings successfully set");

    return 0;
}

static int _get_bitrate(int argc, char **argv)
{
    if (argc < 3) {
        print_usage();
        return 1;
    }

    int ifnum = strtol(argv[2], NULL, 0);
    struct can_bittiming bittiming;
    can_opt_t opt;
    opt.data = &bittiming;
    opt.data_len = sizeof(bittiming);
    opt.opt = CANOPT_BITTIMING;

    int ret = raw_can_get_can_opt(ifnum, &opt);
    if (ret < 0) {
        printf("Error when getting bitrate: res=%d\n", ret);
        return 1;
    }

    printf("Bitrate read: bitrate=%" PRIu32 ", sample_point=%" PRIu32
           "\nbrp=%" PRIu32 "phase-seg1=%" PRIu32
           ", phase-seg2=%" PRIu32 ", sjw=%" PRIu32 "\n", bittiming.bitrate,
           bittiming.sample_point, bittiming.brp, bittiming.phase_seg1,
           bittiming.phase_seg2, bittiming.sjw);
    return 0;
}

static int _get_counter(int argc, char **argv)
{
    if (argc < 3) {
        print_usage();
        return 1;
    }

    int res = 0;
    int ifnum = strtol(argv[2], NULL, 0);
    if (ifnum >= CAN_DLL_NUMOF) {
        puts("Invalid interface number");
        return 1;
    }

    uint16_t cnt;
    can_opt_t opt;
    opt.data = &cnt;
    opt.data_len = sizeof(cnt);
    opt.opt = CANOPT_TEC;

    int ret = raw_can_get_can_opt(ifnum, &opt);
    if (ret < 0) {
        printf("Error when getting TEC: res=%d\n", ret);
        res = 1;
    }
    else {
        printf("TEC=%" PRIu16, cnt);
    }

    opt.opt = CANOPT_REC;

    ret = raw_can_get_can_opt(ifnum, &opt);
    if (ret < 0) {
        printf("\nError when getting REC: res=%d\n", ret);
        res = 1;
    }
    else {
        printf(", REC=%" PRIu16 "\n", cnt);
    }

    return res;
}

static int _power_up(int argc, char **argv)
{
    if (argc < 3) {
        print_usage();
        return 1;
    }

    int res = 0;
    int ifnum = strtol(argv[2], NULL, 0);
    if (ifnum >= CAN_DLL_NUMOF) {
        puts("Invalid interface number");
        return 1;
    }

    int ret = raw_can_power_up(ifnum);
    if (ret < 0) {
        printf("Error when powering up: res=%d\n", ret);
        res = 1;
    }

    return res;
}

static int _power_down(int argc, char **argv)
{
    if (argc < 3) {
        print_usage();
        return 1;
    }

    int res = 0;
    int ifnum = strtol(argv[2], NULL, 0);
    if (ifnum >= CAN_DLL_NUMOF) {
        puts("Invalid interface number");
        return 1;
    }

    int ret = raw_can_power_down(ifnum);
    if (ret < 0) {
        printf("Error when powering up: res=%d\n", ret);
        res = 1;
    }

    return res;
}

static int _can_handler(int argc, char **argv)
{
    if (argc < 2) {
        print_usage();
        return 1;
    }
    else if (strncmp(argv[1], "list", 5) == 0) {
        return _list(argc, argv);
    }
    else if (strncmp(argv[1], "send", 5) == 0) {
        return _send(argc, argv, false);
    }
    else if (strncmp(argv[1], "sendrtr", 8) == 0) {
        return _send(argc, argv, true);
    }
    else if (strncmp(argv[1], "recv", 5) == 0) {
        return _receive(argc, argv);
    }
    else if (strncmp(argv[1], "close", 6) == 0) {
        return _close(argc, argv);
    }
#ifdef MODULE_CAN_ISOTP
    else if (strncmp(argv[1], "bind_isotp", 11) == 0) {
        return _bind_isotp(argc, argv);
    }
    else if (strncmp(argv[1], "send_isotp", 11) == 0) {
        return _send_isotp(argc, argv);
    }
    else if (strncmp(argv[1], "recv_isotp", 11) == 0) {
        return _receive_isotp(argc, argv);
    }
    else if (strncmp(argv[1], "close_isotp", 12) == 0) {
        return _close_isotp(argc, argv);
    }
#endif
    else if (strncmp(argv[1], "get_filter", 10) == 0) {
        return _get_filter(argc, argv);
    }
    else if (strncmp(argv[1], "set_bitrate", 11) == 0) {
        return _set_bitrate(argc, argv);
    }
    else if (strncmp(argv[1], "get_bitrate", 11) == 0) {
        return _get_bitrate(argc, argv);
    }
    else if (strncmp(argv[1], "get_counter", 11) == 0) {
        return _get_counter(argc, argv);
    }
    else if (strncmp(argv[1], "power_up", 9) == 0) {
        return _power_up(argc, argv);
    }
    else if (strncmp(argv[1], "power_down", 11) == 0) {
        return _power_down(argc, argv);
    }
    else {
        printf("unknown command: %s\n", argv[1]);
        return 1;
    }
}

static void *_receive_thread(void *args)
{
    int thread_nb = (intptr_t)args;
    struct can_frame frame;
    msg_t msg, msg_queue[RECEIVE_THREAD_MSG_QUEUE_SIZE];

    /* setup the device layers message queue */
    msg_init_queue(msg_queue, RECEIVE_THREAD_MSG_QUEUE_SIZE);

    printf("%d: launching receive_thread\n", thread_nb);

    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
        case CAN_MSG_RECV:
        {
            int ret;
            while ((ret = conn_can_raw_recv(&conn[thread_nb], &frame,
                   msg.content.value))
                   == sizeof(struct can_frame)) {
                printf("%d: %-8s %" PRIx32 "  [%x] ",
                       thread_nb,
                       raw_can_get_name_by_ifnum(conn[thread_nb].ifnum),
                       frame.can_id,
                       frame.can_dlc);
                for (int i = 0; i < frame.can_dlc; i++) {
                    printf(" %02X", frame.data[i]);
                }
                printf("\n");
            }
            printf("%d: recv terminated: ret=%d\n", thread_nb, ret);
            conn_can_raw_close(&conn[thread_nb]);
            thread_busy[thread_nb] = 0;
            break;
        }
#ifdef MODULE_CAN_ISOTP
        case CAN_MSG_RECV_ISOTP:
        {
            int ret;
            while ((ret = conn_can_isotp_recv(&conn_isotp[thread_nb],
                   isotp_buf[thread_nb],
                   ISOTP_BUF_SIZE, msg.content.value))
                   <= ISOTP_BUF_SIZE && ret >= 0) {
                printf("%d: %-8s ISOTP [%d] ",
                       thread_nb,
                       raw_can_get_name_by_ifnum(conn_isotp[thread_nb].ifnum),
                       ret);
                for (int i = 0; i < ret; i++) {
                    printf(" %02X", isotp_buf[thread_nb][i]);
                }
                printf("\n");
            }
            printf("%d: recv terminated: ret=%d\n", thread_nb, ret);
            break;
        }
        case CAN_MSG_SEND_ISOTP:
        {
            msg_t reply;
            can_opt_t *opt = msg.content.ptr;
            int ret = conn_can_isotp_send(&conn_isotp[thread_nb], opt->data,
                opt->data_len, 0);
            reply.type = msg.type;
            reply.content.value = ret;
            msg_reply(&msg, &reply);
            break;
        }
#endif /* MODULE_CAN_ISOTP */
        default:
            printf("%d: _receive_thread: received unknown message\n",
                thread_nb);
            break;
        }
    }

    return NULL;
}

static int init(int argc, char **argv) {

    if (argc < 2) {
        puts("usage: init [trx_id]");
        return 1;
    }

    unsigned trx = atoi(argv[1]);
    if (trx >= ARRAY_SIZE(devs)) {
        puts("Invalid trx_id");
        return 1;
    }

    int res = can_trx_init(devs[trx]);
    if (res < 0) {
        printf("Error when initializing trx: %d\n", res);
        return 1;
    }

    puts("Trx successfully initialized");
    return 0;
}

static int set_mode(int argc, char **argv) {

    if (argc < 3) {
        puts("usage: set_mode [trx_id] [mode]");
        puts("modes:");
        puts("\t0: normal mode");
        puts("\t1: silent mode");
        puts("\t2: standby mode");
        puts("\t3: high-speed mode (SW CAN only)");
        puts("\t4: high-voltage wakeup mode (SW CAN only)");
        return 1;
    }
    unsigned trx = atoi(argv[1]);
    unsigned mode = atoi(argv[2]);
    if ((trx >= ARRAY_SIZE(devs)) ||
            (mode > TRX_HIGH_VOLTAGE_WAKE_UP_MODE)) {
        puts("Invalid trx_id or mode");
        return 1;
    }

    int res = can_trx_set_mode(devs[trx], mode);
    if (res < 0) {
        printf("Error when setting mode: %d\n", res);
        return 1;
    }

    puts("Mode successfully set");
    return 0;
}

static const shell_command_t _commands[] = {
    {"test_can", "Test CAN functions", _can_handler},
    { "init", "initialize a can trx", init },
    { "set_mode", "set a can trx mode", set_mode },
    { NULL, NULL, NULL},
};

int main(void)
{
    for (intptr_t i = 0; i < RCV_THREAD_NUMOF; i++) {
        receive_pid[i] = thread_create(thread_stack[i], THREAD_STACKSIZE,
                                       THREAD_PRIORITY_MAIN - 1,
                                       0, _receive_thread,
                                       (void*)i, "receive_thread");
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
