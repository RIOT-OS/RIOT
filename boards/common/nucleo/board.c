/*
 * Copyright (C) 2016 OTA keys
 *               2016-2017 Inria
 *               2014-2017 Freie Universität Berlin
 *               2015 Lari Lehtomäki
 *               2015 TriaGnoSys GmbH
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
 * @brief
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Lari Lehtomäki <lari@lehtomaki.fi>
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      José Alamos <jialamos@uc.cl>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

#if defined(AUTO_INIT_LED0)
    gpio_init(LED0_PIN, GPIO_OUT);
#endif
#if defined(LED1_PIN)
    gpio_init(LED1_PIN, GPIO_OUT);
#endif
#if defined(LED2_PIN)
    gpio_init(LED2_PIN, GPIO_OUT);
#endif
}
