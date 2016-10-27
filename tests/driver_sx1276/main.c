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

#include "sx1276_regs_lora.h"
#include "sx1276_regs_fsk.h"

void init_configs(void)
{
    sx1276_lora_settings_t settings;

    settings.bandwidth = SX1276_BW_125_KHZ;
    settings.coderate = SX1276_CR_4_5;
    settings.datarate = SX1276_SF12;
    settings.crc_on = true;
    settings.freq_hop_on = false;
    settings.hop_period = 0;
    settings.implicit_header = false;
    settings.iq_inverted = false;
    settings.low_datarate_optimize = false;
    settings.payload_len = 0;
    settings.power = 14;
    settings.preamble_len = LORA_PREAMBLE_LENGTH;
    settings.rx_continuous = true;
    settings.tx_timeout = 1000 * 1000 * 30; // 30 sec
    settings.rx_timeout = LORA_SYMBOL_TIMEOUT;

    sx1276_configure_lora(&sx1276, &settings);

    sx1276_set_channel(&sx1276, 868500000);
}

void event_handler_thread(void *arg, sx1276_event_type_t event_type)
{
	sx1276_rx_packet_t *packet = (sx1276_rx_packet_t *) &sx1276._internal.last_packet;

	switch (event_type) {
		case SX1276_RX_DONE:
			printf("RX: %u bytes: '%s' | RSSI: %d\n",
				   packet->size,
				   packet->content,
				   packet->rssi_value);

			break;

		case SX1276_RX_ERROR_CRC:
			puts("sx1276: RX CRC failed");
			break;

		case SX1276_TX_DONE:
			puts("sx1276: transmission done.");
			break;

		case SX1276_RX_TIMEOUT:
			puts("sx1276: RX timeout");
			break;

		case SX1276_TX_TIMEOUT:
			puts("sx1276: TX timeout");
			break;

		default:
			printf("sx1276: received event #%d\n", (int) event_type);
			break;
	}
}

void init_radio(void)
{
    sx1276.nss_pin = SX1276_SPI_NSS;
    sx1276.spi = SX1276_SPI;

    sx1276.dio0_pin = SX1276_DIO0;
    sx1276.dio1_pin = SX1276_DIO1;
    sx1276.dio2_pin = SX1276_DIO2;
    sx1276.dio3_pin = SX1276_DIO3;

    sx1276.dio4_pin = (gpio_t) NULL;
    sx1276.dio5_pin = (gpio_t) NULL;
    sx1276.reset_pin = (gpio_t) SX1276_RESET;

    sx1276_settings_t settings;
    settings.channel = RF_FREQUENCY;
    settings.modem = SX1276_MODEM_LORA;
    settings.state = SX1276_RF_IDLE;

    sx1276.settings = settings;

    sx1276.sx1276_event_cb = event_handler_thread;

    /* Launch initialization of driver and device */
    puts("init_radio: initializing driver...");
    sx1276_init(&sx1276);

    /* Configure the device */
    init_configs();

    /* Put chip into sleep */
    sx1276_set_sleep(&sx1276);

    puts("init_radio: sx1276 initialization done");
}

int random(int argc, char **argv)
{
    printf("random: number from sx1276: %u\n", (unsigned int) sx1276_random(&sx1276));
    init_configs();

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

    sx1276_send(&sx1276, (uint8_t *) argv[1], strlen(argv[1]) + 1);

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

int rx_test(int argc, char **argv)
{
    sx1276_set_rx(&sx1276, 1000 * 1000 * 5); // 5 sec timeout

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

	sx1276_configure_lora_bw(&sx1276, lora_bw);
	sx1276_configure_lora_sf(&sx1276, lora_sf);
	sx1276_configure_lora_cr(&sx1276, lora_cr);

	puts("lora_setup: configuration is set");

	return 0;
}

static const shell_command_t shell_commands[] = {
    { "random", "Get random number from sx1276", random },
    { "get", "<all | num> - gets value of registers of sx1276, all or by specified number from 0 to 255", regs },
    { "set", "<num> <value> - sets value of register with specified number", regs_set },
    { "tx_test", "<payload> Send test payload string", tx_test },
    { "rx_test", "Start rx test", rx_test },
	{ "lora_setup", "<BW (125, 250, 512)> <SF (7..12)> <CR 4/(5,6,7,8)> - sets up LoRa modulation settings", lora_setup},

    { NULL, NULL, NULL }
};

void sx1276_reset(sx1276_t *sx1276);
void sx1276_set_op_mode(sx1276_t *sx1276, int opmode);

int main(void)
{
    xtimer_init();
    init_radio();

    /* start the shell */
    puts("Initialization successful - starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
