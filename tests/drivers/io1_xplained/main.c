/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the Atmel IO1 Xplained extension
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "xtimer.h"
#include "board.h"

#include "periph/gpio.h"

#include "at30tse75x.h"
#include "sdcard_spi.h"
#include "io1_xplained.h"
#include "io1_xplained_params.h"

#define DELAY_1S   (1U) /* 1 seconds delay between each test */

static io1_xplained_t dev;

static void _sd_card_cid(void)
{
    puts("SD Card CID info:");
    printf("MID: %d\n", dev.sdcard.cid.MID);
    printf("OID: %c%c\n", dev.sdcard.cid.OID[0], dev.sdcard.cid.OID[1]);
    printf("PNM: %c%c%c%c%c\n",
           dev.sdcard.cid.PNM[0], dev.sdcard.cid.PNM[1], dev.sdcard.cid.PNM[2],
           dev.sdcard.cid.PNM[3], dev.sdcard.cid.PNM[4]);
    printf("PRV: %u\n", dev.sdcard.cid.PRV);
    printf("PSN: %" PRIu32 "\n", dev.sdcard.cid.PSN);
    printf("MDT: %u\n", dev.sdcard.cid.MDT);
    printf("CRC: %u\n", dev.sdcard.cid.CID_CRC);
    puts("+----------------------------------------+\n");
}

int main(void)
{
    float temperature;

    puts("IO1 Xplained extention test application\n");
    puts("+-------------Initializing------------+\n");

    if (io1_xplained_init(&dev, &io1_xplained_params[0]) != IO1_XPLAINED_OK) {
        puts("[Error] Cannot initialize the IO1 Xplained extension\n");
        return 1;
    }

    puts("Initialization successful");
    puts("\n+--------Starting tests --------+");
    while (1) {
        /* Get temperature in degrees celsius */
        at30tse75x_get_temperature(&dev.temp, &temperature);
        printf("Temperature [°C]: %.2f\n"
               "+-------------------------------------+\n",
               temperature);
        xtimer_sleep(DELAY_1S);

        /* Card detect pin is inverted */
        if (!gpio_read(IO1_SDCARD_SPI_PARAM_DETECT)) {
            _sd_card_cid();
            xtimer_sleep(DELAY_1S);
        }

        uint16_t light;
        io1_xplained_read_light_level(&light);
        printf("Light level: %i\n"
               "+-------------------------------------+\n",
               light);
        xtimer_sleep(DELAY_1S);

        /* set led */
        gpio_set(IO1_LED_PIN);
        xtimer_sleep(DELAY_1S);

        /* clear led */
        gpio_clear(IO1_LED_PIN);
        xtimer_sleep(DELAY_1S);

        /* toggle led */
        gpio_toggle(IO1_LED_PIN);
        xtimer_sleep(DELAY_1S);

        /* toggle led again */
        gpio_toggle(IO1_LED_PIN);
        xtimer_sleep(DELAY_1S);
    }

    return 0;
}
