/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 *               2015 Lari Lehtomäki
 *               2015 TriaGnoSys GmbH
 *               2016-2017 Inria
 *               2016-2017 OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nucleo
 * @{
 *
 * @file
 * @brief       Board initialization code for all Nucleo boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Lari Lehtomäki <lari@lehtomaki.fi>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      José Alamos <jialamos@uc.cl>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(EN3V3_PIN, GPIO_OUT);

}
