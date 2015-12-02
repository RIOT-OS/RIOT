/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc112x
 * @{
 * @file
 * @brief       Basic functionality of cc112x driver
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdint.h>
#include "board.h"
#include "periph/cpuid.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "xtimer.h"
#include "cpu.h"
#include "log.h"

#include "cc112x.h"
#include "./include/cc112x-defaultsettings.h"
#include "cc112x-defines.h"
#include "./include/cc112x-interface.h"
#include "cc112x-internal.h"
#include "cc112x-spi.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Internal function prototypes */
#ifndef CC112X_DONT_RESET
static void _power_up_reset(cc112x_t *dev);
#endif

int cc112x_setup(cc112x_t *dev, const cc112x_params_t *params)
{
	int ret = 0;

    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    dev->params = *params;

    /* Configure chip-select */
    ret += gpio_init(dev->params.reset, GPIO_DIR_OUT, GPIO_PULLUP);
    ret += gpio_init(dev->params.cs, GPIO_DIR_OUT, GPIO_PULLUP);

    gpio_set(dev->params.cs);

    /* Configure GPIO1 don't need to be configured. SPI configuration does it. */
    /* Configure SPI */
    ret += spi_acquire(dev->params.spi);
    ret += spi_init_master(dev->params.spi, SPI_CONF_FIRST_RISING, SPI_SPEED_400KHZ);
    ret += spi_release(dev->params.spi);

#ifndef CC112X_DONT_RESET
    /* reset device*/
    _power_up_reset(dev);
#endif

    /* Resetting device strobe */
    cc112x_strobe(dev, CC112X_SRES);

    /* set default state */
    dev->radio_state = RADIO_IDLE;

    /* Write configuration to configuration registers */
    for(uint16_t a = 0 ; a < registerSettingsCount ; ++a){
    	cc112x_write_reg(dev, preferredSettings[a].addr, preferredSettings[a].data);
    }

    /* Perform self calibration */
    uint8_t original_fs_cal2;
	uint8_t calResults_for_vcdac_start_high[3];
	uint8_t calResults_for_vcdac_start_mid[3];
	uint8_t marcstate;
	uint8_t writeByte;

	/* 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00) */
	cc112x_write_reg(dev, CC112X_FS_VCO2, 0);

	/* 2) Start with high VCDAC (original VCDAC_START + 2): */
	original_fs_cal2 = cc112x_read_reg(dev, CC112X_FS_CAL2);
	writeByte = original_fs_cal2 + VCDAC_START_OFFSET;
	cc112x_write_reg(dev, CC112X_FS_CAL2, writeByte);

	/* 3) Calibrate and wait for calibration to be done (radio back in IDLE state) */
	cc112x_strobe(dev, CC112X_SCAL);

	do {
		marcstate = cc112x_read_reg(dev, CC112X_MARCSTATE);
	} while (marcstate != 0x41);

	/* 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with high VCDAC_START value */
	calResults_for_vcdac_start_high[FS_VCO2_INDEX] = cc112x_read_reg(dev, CC112X_FS_VCO2);
	calResults_for_vcdac_start_high[FS_VCO4_INDEX] = cc112x_read_reg(dev, CC112X_FS_VCO4);
	calResults_for_vcdac_start_high[FS_CHP_INDEX] = cc112x_read_reg(dev, CC112X_FS_CHP);

	/* 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00) */
	writeByte = 0x00;
	cc112x_write_reg(dev, CC112X_FS_VCO2, writeByte);

	/* 6) Continue with mid VCDAC (original VCDAC_START): */
	writeByte = original_fs_cal2;
	cc112x_write_reg(dev, CC112X_FS_CAL2, writeByte);

	/* 7) Calibrate and wait for calibration to be done (radio back in IDLE state) */
	cc112x_strobe(dev, CC112X_SCAL);

	do {
		marcstate = cc112x_read_reg(dev, CC112X_MARCSTATE);
	} while (marcstate != 0x41);

	/* 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with mid VCDAC_START value */
	calResults_for_vcdac_start_mid[FS_VCO2_INDEX] = cc112x_read_reg(dev, CC112X_FS_VCO2);
	calResults_for_vcdac_start_mid[FS_VCO4_INDEX] = cc112x_read_reg(dev, CC112X_FS_VCO4);
	calResults_for_vcdac_start_mid[FS_CHP_INDEX] = cc112x_read_reg(dev, CC112X_FS_CHP);

	/* 9) Write back highest FS_VCO2 and corresponding FS_VCO and FS_CHP result */
	if (calResults_for_vcdac_start_high[FS_VCO2_INDEX]
		> calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
		writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
		cc112x_write_reg(dev, CC112X_FS_VCO2, writeByte);
		writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
		cc112x_write_reg(dev, CC112X_FS_VCO4, writeByte);
		writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
		cc112x_write_reg(dev, CC112X_FS_CHP, writeByte);
	} else {
		writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
		cc112x_write_reg(dev, CC112X_FS_VCO2, writeByte);
		writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
		cc112x_write_reg(dev, CC112X_FS_VCO4, writeByte);
		writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
		cc112x_write_reg(dev, CC112X_FS_CHP, writeByte);
	}

    /* Set default channel number */
//    cc112x_set_channel(dev, CC112X_DEFAULT_CHANNEL);

    /* set default node id */
#ifdef CPUID_ID_LEN
    if (CPUID_ID_LEN>0) {
        char cpuid[CPUID_ID_LEN];
        cpuid_get(cpuid);
        cc112x_set_address(dev, (uint8_t) cpuid[0]);
    }
#endif

    LOG_INFO("cc112x: initialized with address=%u and channel=%i\n",
            (unsigned)dev->radio_address,
            dev->radio_channel);


    return ret;
}

uint8_t cc112x_set_address(cc112x_t *dev, uint8_t address)
{
    DEBUG("%s:%s:%u setting address %u\n", RIOT_FILE_RELATIVE, __func__,
            __LINE__, (unsigned)address);
    if (!(address < MIN_UID) || (address > MAX_UID)) {
        if (dev->radio_state != RADIO_UNKNOWN) {
            cc112x_write_register(dev, CC112X_DEV_ADDR, address);
            dev->radio_address = address;
            return address;
        }
    }

    return 0;
}

void cc112x_set_monitor(cc112x_t *dev, uint8_t mode)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    /* If mode true no address check is performed, otherwise
     * address checking is enabled, broadcast frames address
     * is set to 0x00.
     */
    cc112x_write_register(dev, CC112X_PKT_CFG1, mode ? 0x01 : 0x21);
}

void cc112x_setup_rx_mode(cc112x_t *dev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    /* Stay in RX mode until end of packet */
    cc112x_write_reg(dev, CC112X_RFEND_CFG1, 0x0E);
    cc112x_switch_to_rx(dev);
}

void cc112x_switch_to_rx(cc112x_t *dev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    gpio_irq_disable(dev->params.gpio2);

    /* Flush RX buffer */
    cc112x_strobe(dev, CC112X_SIDLE);
    cc112x_strobe(dev, CC112X_SFRX);

    dev->radio_state = RADIO_RX;

    /* Assert on received sync word */
    cc112x_write_reg(dev, CC112X_IOCFG2, 0x6);
    /* Go into receiving mode */
    cc112x_strobe(dev, CC112X_SRX);

    gpio_irq_enable(dev->params.gpio2);
}

void cc112x_wakeup_from_rx(cc112x_t *dev)
{
    if (dev->radio_state != RADIO_RX) {
        return;
    }

    DEBUG("cc112x: switching to idle mode\n");

	/* Go into transmit mode then back to IDLE mode */
	cc112x_strobe(dev, CC112X_SIDLE);
    dev->radio_state = RADIO_IDLE;
}

void cc112x_switch_to_pwd(cc112x_t *dev)
{
    DEBUG("cc112x: switching to powerdown mode\n");
    cc112x_wakeup_from_rx(dev);
    cc112x_strobe(dev, CC112X_SPWD);
    dev->radio_state = RADIO_PWD;
}

int16_t cc112x_set_channel(cc112x_t *dev, uint8_t channr)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    if (channr > CC112X_MAX_CHANNR && channr < CC112X_MIN_CHANNR) {
        return -1;
    }

    uint64_t freq = channr * CC112X_CHANNEL_WIDTH + CC112X_CHANNEL0_FREQ;
    uint64_t regval;
    regval = (uint64_t)((uint64_t)(262144)*(uint64_t)(freq))/(uint64_t)(CC112X_XTAL_FREQ);

    cc112x_write_reg(dev, CC112X_FREQ0, (uint8_t)(regval&0xff));
    cc112x_write_reg(dev, CC112X_FREQ1, (uint8_t)((regval&0xff00)>>8));
    cc112x_write_reg(dev, CC112X_FREQ2, (uint8_t)((regval&0xff0000)>>16));

    dev->radio_channel = channr;

    return channr;
}

//int16_t cc112x_set_power(cc112x_t *dev, int16_t power)
//{
//
//}

#ifndef CC112X_DONT_RESET
static void _power_up_reset(cc112x_t *dev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    gpio_set(dev->params.reset);
    xtimer_usleep(1000);
    gpio_clear(dev->params.reset);
    xtimer_usleep(1000);
    gpio_set(dev->params.reset);
    xtimer_usleep(RESET_WAIT_TIME);
    cc112x_cs(dev);
}
#endif

void cc112x_write_register(cc112x_t *dev, uint8_t r, uint8_t value)
{
    /* Save old radio state */
    uint8_t old_state = dev->radio_state;

    /* Wake up from RX (no effect if in other mode) */
    cc112x_wakeup_from_rx(dev);
    cc112x_write_reg(dev, r, value);

    /* Have to put radio back to RX if old radio state
     * was RX, otherwise no action is necessary */
    if (old_state == RADIO_RX) {
        cc112x_switch_to_rx(dev);
    }
}

int cc112x_rd_set_mode(cc112x_t *dev, cc112x_radio_mode_t mode)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    int result;

    /* Get current radio mode */
    if ((dev->radio_state == RADIO_UNKNOWN) || (dev->radio_state == RADIO_PWD)) {
        result = RADIO_MODE_OFF;
    }
    else {
        result = RADIO_MODE_ON;
    }

    switch(mode) {
        case RADIO_MODE_ON:
            DEBUG("cc112x: switching to RX mode\n");
            cc112x_setup_rx_mode(dev);          /* Set chip to desired mode */
            break;

        case RADIO_MODE_OFF:
            gpio_irq_disable(dev->params.gpio2); /* Disable interrupts */
            cc112x_switch_to_pwd(dev);          /* Set chip to power down mode */
            break;

        case RADIO_MODE_GET:
            /* do nothing, just return current mode */
        default:
            /* do nothing */
            break;
    }

    /* Return previous mode */
    return result;
}

bool cc112x_test(cc112x_t *dev){
	 /* FIFO and strobe test */
	while(MARC_PIN_IDLE != cc112x_get_marc_state(dev)){
		cc112x_strobe(dev, CC112X_SIDLE);
	}
//	gpio_irq_enable(dev->params.gpio2);
//	printf("MARCSTATE - %x\n", cc112x_read_reg(dev, CC112X_MARCSTATE));
//	printf("PARTNUM - %x\n", cc112x_read_reg(dev, CC112X_PARTNUMBER));
//	printf("MARC_STATUS_0 - %x\n", cc112x_read_reg(dev, CC112X_MARC_STATUS0));
//	printf("MARC_STATUS_1 - %x\n", cc112x_read_reg(dev, CC112X_MARC_STATUS1));
//	cc112x_strobe(dev, CC112X_SFTX);

	printf("Adding 8 bytes...");
	for(uint32_t a = 0 ; a < 0x08 ; ++a){
		cc112x_write_reg(dev, CC112X_SINGLE_TXFIFO, 8+a);
	}

	if(8 == cc112x_read_reg(dev, CC112X_TXLAST)){
		puts(" OK");
	} else {
		puts(" ERROR");
		printf("TXFIFO - %d\n", cc112x_read_reg(dev, CC112X_FIFO_NUM_TXBYTES));
		printf("TXLAST - %d\n", cc112x_read_reg(dev, CC112X_TXLAST));
		printf("TXfirst - %d\n", cc112x_read_reg(dev, CC112X_TXFIRST));
		return false;
	}
//	printf("TXFIFO - %d\n", cc112x_read_reg(dev, CC112X_FIFO_NUM_TXBYTES));
//	printf("TXLAST - %d\n", cc112x_read_reg(dev, CC112X_TXLAST));
//	printf("TXfirst - %d\n", cc112x_read_reg(dev, CC112X_TXFIRST));

	printf("Adding additional 8 bytes...");
	for(uint32_t a = 0 ; a < 0x08 ; ++a){
		cc112x_write_reg(dev, CC112X_SINGLE_TXFIFO, 1+a);
	}

	/* Read the whole registers */
//	char in = 0xff;
//	uint32_t cpsr;
//	printf("DATA:\n");
//	for(uint8_t a = 0 ; a < 0x10 ; a++){
//		spi_acquire(dev->params.spi);
//		cpsr = disableIRQ();
//		cc112x_cs(dev);
//		spi_transfer_byte( dev->params.spi, 0x3E | CC112X_READ | CC112X_SINGLE, NULL );
//		spi_transfer_byte( dev->params.spi, a, &in);
//		spi_transfer_byte( dev->params.spi, CC112X_NOBYTE, &in);
//		gpio_set(dev->params.cs);
//		restoreIRQ(cpsr);
//		spi_release(dev->params.spi);
//		printf(" %x", in);
//	}
//	puts(" ");

	if((16 == cc112x_read_reg(dev, CC112X_TXLAST)) && (0 == cc112x_read_reg(dev, CC112X_TXFIRST))){
		puts(" OK");
	} else {
		puts(" ERROR");
		printf("TXFIFO - %d\n", cc112x_read_reg(dev, CC112X_FIFO_NUM_TXBYTES));
		printf("TXLAST - %d\n", cc112x_read_reg(dev, CC112X_TXLAST));
		printf("TXfirst - %d\n", cc112x_read_reg(dev, CC112X_TXFIRST));
		return false;
	}

	printf("STX - transmitting 8+1 bytes...");
	cc112x_strobe(dev, CC112X_STX);

	xtimer_usleep(500000);

	if((16 == cc112x_read_reg(dev, CC112X_TXLAST)) && (9 == cc112x_read_reg(dev, CC112X_TXFIRST))){
		puts(" OK");
	} else {
		puts(" ERROR");
		printf("TXFIFO - %d\n", cc112x_read_reg(dev, CC112X_FIFO_NUM_TXBYTES));
		printf("TXLAST - %d\n", cc112x_read_reg(dev, CC112X_TXLAST));
		printf("TXfirst - %d\n", cc112x_read_reg(dev, CC112X_TXFIRST));
		return false;
	}

	printf("SFTX - flushing data...");
	cc112x_strobe(dev, CC112X_SFTX);

	if(0 == cc112x_read_reg(dev, CC112X_TXLAST) && 0 == cc112x_read_reg(dev, CC112X_TXFIRST)){
		puts(" OK");
	} else {
		puts(" ERROR");
		printf("TXFIFO - %d\n", cc112x_read_reg(dev, CC112X_FIFO_NUM_TXBYTES));
		printf("TXLAST - %d\n", cc112x_read_reg(dev, CC112X_TXLAST));
		printf("TXfirst - %d\n", cc112x_read_reg(dev, CC112X_TXFIRST));
		return false;
	}

	printf("Additional write test...");

	for(uint32_t a = 0 ; a < 0x16 ; ++a){
		cc112x_write_reg(dev, CC112X_SINGLE_TXFIFO, 10+a);
	}

	if(22 == cc112x_read_reg(dev, CC112X_TXLAST) && 0 == cc112x_read_reg(dev, CC112X_TXFIRST)){
		puts(" OK");
	} else {
		puts(" ERROR");
		printf("TXFIFO - %d\n", cc112x_read_reg(dev, CC112X_FIFO_NUM_TXBYTES));
		printf("TXLAST - %d\n", cc112x_read_reg(dev, CC112X_TXLAST));
		printf("TXfirst - %d\n", cc112x_read_reg(dev, CC112X_TXFIRST));
		return false;
	}

//	printf("DATA:\n");
//	for(uint8_t a = 0 ; a < 0x10 ; a++){
//		spi_acquire(dev->params.spi);
//		cpsr = disableIRQ();
//		cc112x_cs(dev);
//		spi_transfer_byte( dev->params.spi, 0x3E | CC112X_READ | CC112X_SINGLE, NULL );
//		spi_transfer_byte( dev->params.spi, a, &in);
//		spi_transfer_byte( dev->params.spi, CC112X_NOBYTE, &in);
//		gpio_set(dev->params.cs);
//		restoreIRQ(cpsr);
//		spi_release(dev->params.spi);
//		printf(" %x", in);
//	}

	puts(" ");

	return true;
}

cc112x_marc_state cc112x_get_marc_state(cc112x_t *dev){
	uint8_t state = cc112x_read_reg(dev, CC112X_MARCSTATE);
	printf("%x %x\n", state, (state&(0x60))>>5);
	cc112x_marc_state ret;
	switch((state&(0x60))>>5){
	case 0x00:
		ret = MARC_PIN_SETTLING;
		break;
	case 0x01:
		ret = MARC_PIN_TX;
		break;
	case 0x02:
		ret = MARC_PIN_IDLE;
		break;
	case 0x03:
		ret = MARC_PIN_RX;
		break;
	default:
		break;
	}
	return ret;
}
