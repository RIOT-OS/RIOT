/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Attilio Dona' <@attiliodona>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph_conf.h"

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

static unsigned long bitrate[] = {
		[SPI_CLK_100KHZ] = 100000,
		[SPI_CLK_400KHZ] = 400000,
		[SPI_CLK_1MHZ] = 1000000,
		[SPI_CLK_5MHZ] = 5000000,
		[SPI_CLK_10MHZ] = 10000000
};

#if 0
/**
 * @brief   array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[] =  {
#if SPI_0_EN
    [SPI_0] = MUTEX_INIT,
#endif
#if SPI_1_EN
    [SPI_1] = MUTEX_INIT,
#endif
};
#endif

static mutex_t lock = MUTEX_INIT;

int spi_init_master(spi_t dev, spi_mode_t conf, spi_clk_t speed)
{
	// cc3200 has only one SPI for external use
    if (dev >= SPI_UNDEF || dev != SPI_DEV) {
        return -1;
    }

    MAP_PRCMPeripheralClkEnable(PRCM_GSPI, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_05 for SPI0 GSPI_CLK
    //
    MAP_PinTypeSPI(digital_pin_to_pin_num[SPI_0_PIN_SCK], PIN_MODE_7);

    //
    // Configure PIN_06 for SPI0 GSPI_MISO
    //
    MAP_PinTypeSPI(digital_pin_to_pin_num[SPI_0_PIN_MISO], PIN_MODE_7);

    //
    // Configure PIN_07 for SPI0 GSPI_MOSI
    //
    MAP_PinTypeSPI(digital_pin_to_pin_num[SPI_0_PIN_MOSI], PIN_MODE_7);

    //
    // Configure PIN_08 for SPI0 GSPI_CS
    //
    //MAP_PinTypeSPI(digital_pin_to_pin_num[SPI_0_PIN_CS], PIN_MODE_7);

    //
    // Reset SPI
    //
    MAP_SPIReset(GSPI_BASE);

    //
    // Configure SPI interface
    //
    // see:
    //  e2e.ti.com/support/wireless_connectivity/f/968/p/359727/1265934#1265934
    //
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                     bitrate[speed],SPI_MODE_MASTER,conf,
                     (SPI_HW_CTRL_CS |
                     SPI_4PIN_MODE |
                     SPI_TURBO_OFF |
                     SPI_CS_ACTIVELOW |
                     SPI_WL_8));

    //
    // Enable SPI for communication
    //
    MAP_SPIEnable(GSPI_BASE);

    /* configure SPI mode */

    return 0;
}

int spi_init_slave(spi_t dev, spi_mode_t conf, char (*cb)(char data))
{
    /* actually not implemented */
    return -1;
}


int spi_conf_pins(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }

    //
    // Configure PIN_06 for SPI0 GSPI_MISO
    //
    MAP_PinTypeSPI(digital_pin_to_pin_num[SPI_0_PIN_MISO], PIN_MODE_7);

    return 0;
}


int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    if (bus >= SPI_UNDEF) {
        return -1;
    }
    mutex_lock(&lock);
    return 0;
}

void spi_release(spi_t bus)
{
    if (bus >= SPI_UNDEF) {
        return;
    }
    mutex_unlock(&lock);
}

#if 0
unsigned char spi_single_byte(spi_t dev, char out) {

	if (dev >= SPI_NUMOF) {
        return -1;
    }
}
#endif

uint8_t spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont, uint8_t out)
{
    spi_transfer_bytes(bus, cs, cont, &out, NULL, 1);
    return 0;
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont, const void *out, void *in, size_t len)
{
    if (bus >= SPI_UNDEF) {
        return;
    }

    //MAP_SPITransfer(GSPI_BASE, (unsigned char*)out, (unsigned char*)in, length, SPI_CS_ENABLE|SPI_CS_DISABLE);
    MAP_SPITransfer(GSPI_BASE, (unsigned char*)out, (unsigned char*)in, len, 0);
}

uint8_t spi_transfer_reg(spi_t bus, spi_cs_t cs, uint8_t reg, uint8_t out)
{
    if (bus >= SPI_UNDEF) {
        return -1;
    }
    //MAP_SPITransfer(GSPI_BASE, &reg, 0, 1, SPI_CS_ENABLE);
    MAP_SPITransfer(GSPI_BASE, &reg, 0, 1, 0);

    //if (MAP_SPITransfer(GSPI_BASE, (unsigned char*)&out, (unsigned char*)in, 1, SPI_CS_DISABLE)) {
    if (MAP_SPITransfer(GSPI_BASE, (unsigned char*)&out, 0, 1, 0)) {
    	return -1;
    }
    return 1; // success transfer
}

void spi_transfer_regs(spi_t bus, spi_cs_t cs, uint8_t reg, const void *out, void *in, size_t len)
{
    if (bus >= SPI_UNDEF) {
       return;
    }
    //MAP_SPITransfer(GSPI_BASE, &reg, 0, 1, SPI_CS_ENABLE);
    MAP_SPITransfer(GSPI_BASE, &reg, 0, 1, 0);
    //if(MAP_SPITransfer(GSPI_BASE, (unsigned char*)&out, (unsigned char*)in, length, SPI_CS_DISABLE)) {
    if(MAP_SPITransfer(GSPI_BASE, (unsigned char*)out, (unsigned char*)in, len, 0)) {
    	return;
    }
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    /* spi slave is not implemented */
}


#endif /* SPI_NUMOF */
