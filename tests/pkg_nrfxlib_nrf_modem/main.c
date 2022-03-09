/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Nordic nrf_modem example application for nRF9160-based MCUs
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "board.h"
#include "cpu.h"
#include "msg.h"
#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "tlsf.h"
#include "ztimer.h"
#include "ztimer/periodic.h"

#include "nrf_modem_os.h"
#include "nrf_modem_riot.h"
#include "nrf_modem.h"
#include "nrf_modem_gnss.h"
#include "nrf_modem_at.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define LED_BLINK_DELAY 500 /* ms */
#define CONFIG_GNSS_MSG_QUEUE 8
#define CONFIG_SHMEM_SIZE 8192 /* bytes */

char gnss_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t gnss_pid = KERNEL_PID_UNDEF;

struct nrf_modem_gnss_nmea_data_frame nmea_data;

static int _led_blink(void* arg)
{
    (void)arg;
#ifdef LED0_TOGGLE
    LED0_TOGGLE;
#endif
    return 0;
}

static void _gnss_event_handler(int event)
{
    msg_t msg;
    switch (event) {

        case NRF_MODEM_GNSS_EVT_NMEA:
            nrf_modem_gnss_read(&nmea_data,
                         sizeof(struct nrf_modem_gnss_nmea_data_frame),
                         NRF_MODEM_GNSS_DATA_NMEA);
            /* Send data to the thread */
            msg.type = NRF_MODEM_GNSS_EVT_NMEA;
            msg.content.ptr = &nmea_data;
            msg_send_int(&msg, gnss_pid);
                         break;
        case NRF_MODEM_GNSS_EVT_FIX:
            msg.type = NRF_MODEM_GNSS_EVT_FIX;
            msg_send_int(&msg, gnss_pid);
            break;
        case NRF_MODEM_GNSS_EVT_BLOCKED:
        case NRF_MODEM_GNSS_EVT_UNBLOCKED: /* Falls-through */
            break;
        default:
            DEBUG("Unhandled event:%d\n", event);
            break;
    }
}

static int _gnss_init(void)
{
    int ret;
    uint8_t system_mask = NRF_MODEM_GNSS_SYSTEM_GPS_MASK;

    /* Initialize GNSS service */
    ret = nrf_modem_gnss_init();
    if (ret != 0) {
        printf("Cannot initialize GNSS:%d\n", ret);
    }
    puts("[gnss_thread]: GNSS Initialized");

    /* Define handler which will process GNSS events */
    ret = nrf_modem_gnss_event_handler_set(_gnss_event_handler);
    if (ret != 0) {
        DEBUG("Cannot set GNSS handler:%d\n", ret);
        return -1;
    }

    /* Unsure GNSS is stop before configuring it */
    ret = nrf_modem_gnss_stop();
    if (ret != 1) {
        DEBUG("Cannot stop GNSS service:%d\n", ret);
        return -1;
    }

    /* Configure GNSS service before start */
    ret = nrf_modem_gnss_fix_interval_set(1);
    if (ret != 0) {
        DEBUG("Cannot set fix internal:%d\n", ret);
        return -1;
    }

    ret = nrf_modem_gnss_fix_retry_set(0);
    if (ret != 0) {
        DEBUG("Cannot set fix retry:%d\n", ret);
        return -1;
    }
    ret = nrf_modem_gnss_system_mask_set(system_mask);
    if (ret != 0) {
        DEBUG("Cannot select system mask:%d\n", ret);
        return -1;
    }

    ret = nrf_modem_gnss_nmea_mask_set(
                                        NRF_MODEM_GNSS_NMEA_GLL_MASK |
                                        NRF_MODEM_GNSS_NMEA_GSA_MASK |
                                        NRF_MODEM_GNSS_NMEA_GSV_MASK |
                                        NRF_MODEM_GNSS_NMEA_GGA_MASK |
                                        NRF_MODEM_GNSS_NMEA_RMC_MASK);
    if (ret != 0) {
        DEBUG_PUTS("NMEA set mask failed\n");
        return -1;
    }
    /* Start GNSS service */
    ret = nrf_modem_gnss_start();
    if (ret != 0) {
        DEBUG("Cannot start GNSS service:%d\n", ret);
        return -1;
    }
    puts("[gnss_thread]: Starting GNSS service...");
    return 0;
}

/* Threads definitions */

void *gnss_thread(void *arg)
{
    (void)arg;
    struct nrf_modem_gnss_nmea_data_frame *nmea_data;
    ztimer_periodic_t led_timer;
    bool gnss_print = false;
    int ret;
    puts("[gnss_thread]: starting...");

    msg_t msgq[CONFIG_GNSS_MSG_QUEUE];
    msg_init_queue(msgq, CONFIG_GNSS_MSG_QUEUE);
    /* Initialize GNSS */
    ret = _gnss_init();
    if (ret != 0) {
        puts("[gnss_thread]: GNSS init failed\n");
        return NULL;
    }
    /* If board has a LED, blinks it while waiting for GPS fix event */
    ztimer_periodic_init(ZTIMER_MSEC, &led_timer, _led_blink, NULL,
                         LED_BLINK_DELAY);
    ztimer_periodic_start(&led_timer);
    puts("[gnss_thread]: waiting for fix event...");
    while (1)
    {
        msg_t msg;
        msg_receive(&msg);

        switch (msg.type) {
            case NRF_MODEM_GNSS_EVT_FIX:
                puts("[gnss_thread]: GNSS event FIX");
                ztimer_periodic_stop(&led_timer);
                gnss_print = true;
                break;
            case NRF_MODEM_GNSS_EVT_NMEA:
                nmea_data = (struct nrf_modem_gnss_nmea_data_frame*) msg.content.ptr;
                if (gnss_print) {
                    printf("[gnss_thread]: NMEA:%s\n", nmea_data->nmea_str);
                }
                break;
            default:
                printf("[gnss_thread]: unhandled msg type:%d\n", msg.type);
        }
    }
    /* Should never reach here */
    return NULL;
}

static int _at_cmd(int argc, char **argv)
{
    int err;
    uint8_t buffer[64];

    if (argc < 2) {
        puts("at: Missing argument");
        return -1;
    }

    err = nrf_modem_at_cmd(buffer, sizeof(buffer), argv[1], 1);
    /* Display modem answer */
    printf("%s\n", buffer);
    return err;
}

/* Shell commands */
static const shell_command_t shell_commands[] = {
    { "at", "Send an AT command to the modem", _at_cmd},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("nRF modem test application\n");

    /* Create GNSS thread */
    gnss_pid = thread_create(gnss_stack, sizeof(gnss_stack),
                             THREAD_PRIORITY_MAIN - 1,
                             THREAD_CREATE_WOUT_YIELD,
                             gnss_thread, NULL, "gnss_thread");

    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    /* define own shell commands */
    shell_run_once(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
