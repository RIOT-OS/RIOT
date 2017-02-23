/*
 * Copyright (C) 2016 Cr0s
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
 * @brief       Test application for SX1276 modem driver
 *
 * @author      Cr0s
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "xtimer.h"
#include "lpm.h"
#include "periph/rtc.h"

#include "common.h"

#include "board.h"

#include "sx1276_internal.h"
#include "sx1276_params.h"
#include "sx1276_netdev.h"
#include "net/gnrc/netdev2.h"
#include "net/netdev2.h"

#define GNRC_LORA_MSG_QUEUE 16

static netdev2_t *nd;
int random(int argc, char **argv)
{
    printf("random: number from sx1276: %u\n", (unsigned int) sx1276_random((sx1276_t*) nd));
    init_configs((sx1276_t*) nd);

    return 0;
}

int regs(int argc, char **argv)
{
    if (argc <= 1) {
        puts("usage: get <all | allinline | regnum>");
        return -1;
    }

    if (strcmp(argv[1], "all") == 0) {
        puts("- listing all registers -");
        uint16_t i = 0;
        uint8_t reg = 0, data = 0;
        uint8_t j = 0;

        /* Listing registers map*/
        puts("Reg   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
        for (i = 0; i <= 7; i++) {
            printf("0x%02X ", i << 4);

            for (j = 0; j <= 15; j++, reg++) {
                data = sx1276_reg_read(&sx1276, reg);

                printf("%02X ", data);
            }

            puts("");
        }

        puts("-done-");

        return 0;
    }
    if (strcmp(argv[1], "allinline") == 0) {
        puts("- listing all registers in one line -");
        uint16_t reg;
        uint8_t data = 0;

        /* Listing registers map*/
        for (reg = 0; reg < 256; reg++) {
            data = sx1276_reg_read(&sx1276, (uint8_t) reg);

            printf("%02X ", data);
        }

        puts("-done-");

        return 0;
    }
    else {
        long int num = 0;

        /* Register number in hex */
        if (strstr(argv[1], "0x") != NULL) {
            num = strtol(argv[1], NULL, 16);
        }
        else {
            num = atoi(argv[1]);
        }

        if (num >= 0 && num <= 255) {
            printf("[regs] 0x%02X = 0x%02X\n", (uint8_t) num, sx1276_reg_read(&sx1276, (uint8_t) num));
        }
        else {
            puts("regs: invalid register number specified");
            return -1;
        }
    }

    return 0;
}

int tx_test(int argc, char **argv)
{
    if (argc <= 1) {
        puts("tx_test: payload is not specified");
        return -1;
    }

    printf("tx_test: sending \"%s\" payload (%d bytes)\n", argv[1], strlen(argv[1]) + 1);

     
    struct iovec vec[1];
    vec[0].iov_base = argv[1];
    vec[0].iov_len = strlen(argv[1]) + 1;
    nd->driver->send(nd, vec, 1); 
    
    xtimer_usleep(10000); /* wait for the chip */

    puts("tx_test: sended");

    return 0;
}

int regs_set(int argc, char **argv)
{
    if (argc <= 2) {
        puts("usage: set <num> <value>");
        return -1;
    }

    long num, val;

    // Register number in hex
    if (strstr(argv[1], "0x") != NULL) {
        num = strtol(argv[1], NULL, 16);
    }
    else {
        num = atoi(argv[1]);
    }

    // Register value in hex
    if (strstr(argv[2], "0x") != NULL) {
        val = strtol(argv[2], NULL, 16);
    }
    else {
        val = atoi(argv[2]);
    }

    sx1276_reg_write(&sx1276, (uint8_t) num, (uint8_t) val);

    return 0;

}

int config_channel(int argc, char **argv)
{
    if(argc < 2)
        return -1;
    uint32_t chan;
    if(strstr(argv[1], "get") != NULL)
    {
        nd->driver->get(nd, NETOPT_CHANNEL, &chan, sizeof(uint32_t));
        printf("Channel: %i\n", (int) chan);
    }
    else
    {
        if(argc < 3)
            return -1;
        chan = atoi(argv[2]);
        nd->driver->set(nd, NETOPT_CHANNEL, &chan, sizeof(uint32_t));
        printf("Channel set\n");
    }
    return 0;
}

int rx_test(int argc, char **argv)
{
    nd->driver->set(nd, NETOPT_LORA_SINGLE_RECEIVE, false, sizeof(uint8_t));
    sx1276_set_rx(&sx1276, 0);

    return 0;
}

int lora_setup(int argc, char **argv) {
	if (argc < 4) {
		return -1;
	}

	int bw = atoi(argv[1]);
	int sf = atoi(argv[2]);
	int cr = atoi(argv[3]);

	sx1276_lora_bandwidth_t lora_bw;

	switch (bw) {
	case 125:
		lora_bw = SX1276_BW_125_KHZ;
		break;

	case 250:
		lora_bw = SX1276_BW_250_KHZ;
		break;

	case 500:
		lora_bw = SX1276_BW_500_KHZ;
		break;

	default:
		puts("lora_setup invalid bandwidth value passed");
		return -1;
	}

	sx1276_lora_spreading_factor_t lora_sf;
	if (sf < 7 || sf > 12) {
		puts("lora_setup: invalid spreading factor value passed");
		return -1;
	}

	lora_sf = (sx1276_lora_spreading_factor_t) sf;

	sx1276_lora_coding_rate_t lora_cr;
	if (cr < 5 || cr > 8) {
		puts("lora_setup: invalid coding rate value passed");
		return -1;
	}

	lora_cr = (sx1276_lora_coding_rate_t) (cr - 5);

    netdev2_t *netdev = (netdev2_t*) &sx1276;
    netdev->driver->set(netdev, NETOPT_LORA_BANDWIDTH, &lora_bw, sizeof(sx1276_lora_bandwidth_t));
    netdev->driver->set(netdev, NETOPT_LORA_SPREADING_FACTOR, &lora_sf, sizeof(sx1276_lora_spreading_factor_t));
    netdev->driver->set(netdev, NETOPT_LORA_CODING_RATE, &lora_cr, sizeof(sx1276_lora_coding_rate_t));

	puts("lora_setup: configuration is set");

	return 0;
}

static const shell_command_t shell_commands[] = {
    { "random", "Get random number from sx1276", random },
    { "get", "<all | num> - gets value of registers of sx1276, all or by specified number from 0 to 255", regs },
    { "set", "<num> <value> - sets value of register with specified number", regs_set },
    { "tx_test", "<payload> Send test payload string", tx_test },
    { "rx_test", "Start rx test", rx_test },
    { "channel", "Set frequency (in Hz)", config_channel },
	{ "lora_setup", "<BW (125, 250, 512)> <SF (7..12)> <CR 4/(5,6,7,8)> - sets up LoRa modulation settings", lora_setup},

    { NULL, NULL, NULL }
};

void *_event_loop(void *arg)
{
    static msg_t _msg_q[GNRC_LORA_MSG_QUEUE];
    msg_t msg, reply;
    netdev2_t *netdev = (netdev2_t*) arg;
    msg_init_queue(_msg_q, GNRC_LORA_MSG_QUEUE);

    gnrc_netapi_opt_t *opt;
    int res;

    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case NETDEV2_MSG_TYPE_EVENT:
                netdev->driver->isr(netdev);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                //gnrc_pktsnip_t *pkt = msg.content.ptr;
                //gnrc_netdev2->send(gnrc_netdev2, pkt);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                /* read incoming options */
                opt = msg.content.ptr;
                /* set option for device driver */
                res = netdev->driver->set(netdev, opt->opt, opt->data, opt->data_len);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                /* read incoming options */
                opt = msg.content.ptr;
                /* get option from device driver */
                res = netdev->driver->get(netdev, opt->opt, opt->data, opt->data_len);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            default:
                break;
        }
    }
    return NULL;
}

static char message[32];
static void _event_cb(netdev2_t *dev, netdev2_event_t event)
{
    msg_t msg;
    msg.type = NETDEV2_MSG_TYPE_EVENT;
    kernel_pid_t *pid = (kernel_pid_t*) dev->context;
    size_t len;
    struct netdev2_radio_rx_info rx_info;
    switch(event)
    {
        case NETDEV2_EVENT_ISR:
            msg_send(&msg, *pid);
            break;
        case NETDEV2_EVENT_TX_COMPLETE:
            printf("TX DONE\n");
            break;
        case NETDEV2_EVENT_RX_COMPLETE:
            len = dev->driver->recv(dev, NULL, 5, &rx_info);
            dev->driver->recv(dev, message, len, NULL);
            printf("%s\n. {RSSI: %i, SNR: %i}", message, rx_info.rssi, (signed int) rx_info.lqi);
        default:
            break;
    }
}

#define SX1276_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT)

static char stack[SX1276_MAC_STACKSIZE];

int main(void)
{
    xtimer_init();

    memcpy(&sx1276.params, sx1276_params, sizeof(sx1276_params));
    netdev2_t *netdev = (netdev2_t*) &sx1276;
    nd = netdev;
    netdev->driver = &sx1276_driver;
    netdev->driver->init(netdev);
    netdev->event_callback = _event_cb;

    kernel_pid_t pid;
    pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 5, THREAD_CREATE_STACKTEST,
                     _event_loop, (void *) netdev, "asd");
    netdev->context = &pid;
    /* start the shell */
    puts("Initialization successful - starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
