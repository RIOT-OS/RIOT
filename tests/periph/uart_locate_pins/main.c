/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @defgroup    tests_periph_uart_locate_pins
 * @{
 *
 * @file
 * @brief       Test application to figure out pin mapping via UART
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */
#include <stdint.h>

#include "container.h"
#include "fmt.h"
#include "macros/units.h"
#include "periph/gpio.h"
#include "periph/timer.h"
#include "soft_uart.h"

#ifndef UART_SYMBOL_RATE
#  define UART_SYMBOL_RATE 9600
#endif

/* Adapt this list of pins to detect as needed. You may want to not use all
 * pins, e.g. pins connected to crystals, etc. may not react friendly to be
 * used as output */
static const struct {
    uint8_t port_num;
    uint8_t pin_num;
} pins[] = {
    {
        .port_num = 0,
        .pin_num = 0,
    },
};

soft_uart_conf_t soft_uart_config[] = {
    {
        .tx_pin = GPIO_UNDEF,
        .rx_pin = GPIO_UNDEF,
        .tx_timer = TIMER_DEV(0),
        .timer_freq = MHZ(1),
    },
};

int main(void)
{
    while (1) {
        for (unsigned i = 0; i < ARRAY_SIZE(pins); i++) {
            gpio_t pin = GPIO_PIN(pins[i].port_num, pins[i].pin_num);
            soft_uart_config[0].tx_pin = pin;
            soft_uart_init(0, UART_SYMBOL_RATE, NULL, NULL);
            static char buf[32];
            char *pos = buf;
            *pos++ = 'P';
            pos += fmt_u16_dec(pos, pins[i].port_num);
            *pos++ = '.';
            pos += fmt_u16_dec(pos, pins[i].pin_num);
            *pos++ = ' ';
            *pos++ = '/';
            *pos++ = ' ';
            *pos++ = 'P';
            *pos++ = (char)((int)'A' + (int)pins[i].port_num);
            pos += fmt_u16_dec(pos, pins[i].pin_num);
            *pos++ = '\n';

            soft_uart_write(0, (void *)buf, (size_t)pos - (size_t)buf);
            soft_uart_poweroff(0);
        }
    }
    return 0;
}
