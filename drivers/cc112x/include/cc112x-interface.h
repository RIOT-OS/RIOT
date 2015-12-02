/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       internal declarations for cc110x driver
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CC110X_INTERFACE_H
#define CC110X_INTERFACE_H

#include <stdint.h>
#include "cc110x.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MARC_PIN_SETTLING,
    MARC_PIN_IDLE,
    MARC_PIN_TX,
    MARC_PIN_RX
} cc112x_marc_state;

/**
 * @name cc112x raw low-level interface
 * @internal
 * @{
 */
cc112x_marc_state cc112x_get_marc_state(cc112x_t *dev);
char *cc112x_state_to_text(uint8_t state);
int cc112x_rd_set_mode(cc112x_t *dev, cc112x_radio_mode_t mode);
uint8_t cc112x_get_buffer_pos(cc112x_t *dev);
void cc112x_isr_handler(cc112x_t *dev, void(*callback)(void*), void*arg);
void cc112x_setup_rx_mode(cc112x_t *dev);
void cc112x_switch_to_pwd(cc112x_t *dev);
void cc112x_switch_to_rx(cc112x_t *dev);
void cc112x_wakeup_from_rx(cc112x_t *dev);
void cc112x_write_register(cc112x_t *dev, uint8_t r, uint8_t value);

/* @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC110X_INTERFACE_H */
