/*
 * Copyright (C) 2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_axavior
 * @{
 *
 * @file
 * @brief       Board specific implementations for the AxAvior board
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "nvram.h"
#include "xtimer.h"
#include "nvram-spi.h"

#include "periph/spi.h"

static void leds_init(void);
static int nvram_init(void);

static nvram_t axavior_nvram_dev;
nvram_t *axavior_nvram = &axavior_nvram_dev;
static nvram_spi_params_t nvram_spi_params = {
        .spi = EXTFLASH_SPI,
        .cs = EXTFLASH_CS,
        .address_count = EXTFLASH_ADDRESS_COUNT,
};

void board_init(void)
{
    /* disable the JTAG interface because we use SWD */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    leds_init();

    /* initialize the NVRAM */
    nvram_init();
}

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LEDs initialization is hard-coded in this function. As the LED is soldered
 * onto the board it is fixed to its CPU pins.
 *
 * The LEDs are connected to the following pin:
 * - Green:     PB3
 */
static void leds_init(void)
{
    gpio_init(LED_GREEN_GPIO, GPIO_DIR_OUT, GPIO_NOPULL);
    LED_GREEN_ON;
}

/**
 * @brief Initialize the on-module SPI flash chip
 */
static int nvram_init(void)
{
    union {
        uint32_t u32;
        uint8_t  u8[sizeof(uint32_t)];
    } rec;
    rec.u32 = 0;

    if (spi_init_master(EXTFLASH_SPI, SPI_CONF_FIRST_RISING, SPI_SPEED_5MHZ) != 0) {
        return -1;
    }

    if (nvram_spi_init(axavior_nvram, &nvram_spi_params, EXTFLASH_CAPACITY) != 0) {
        return -2;
    }

    if (axavior_nvram->read(axavior_nvram, &rec.u8[0], 0, sizeof(rec.u32)) != sizeof(rec.u32)) {
        return -3;
    }

    return 0;
}
