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

/**
 * @name cc110x raw low-level interface
 * @internal
 * @{
 */
char *cc110x_get_marc_state(cc110x_t *dev);
const char *cc110x_state_to_text(uint8_t state);
int cc110x_rd_set_mode(cc110x_t *dev, int mode);
uint8_t cc110x_get_buffer_pos(cc110x_t *dev);
void cc110x_isr_handler(cc110x_t *dev, void(*callback)(void*), void*arg);
void cc110x_set_base_freq_raw(cc110x_t *dev, const char* freq_array);
void cc110x_setup_rx_mode(cc110x_t *dev);
void cc110x_switch_to_pwd(cc110x_t *dev);
void cc110x_switch_to_rx(cc110x_t *dev);
void cc110x_wakeup_from_rx(cc110x_t *dev);
void cc110x_write_register(cc110x_t *dev, uint8_t r, uint8_t value);

extern const char cc110x_default_conf[];
extern const uint8_t cc110x_default_conf_size;
extern const uint8_t cc110x_pa_table[];

#ifdef MODULE_CC110X_HOOKS
void cc110x_hooks_init(void);
void cc110x_hook_rx(void);
void cc110x_hook_tx(void);
void cc110x_hook_off(void);
#endif
/* @} */

#ifdef __cplusplus
}
#endif

#endif /* CC110X_INTERFACE_H */
/** @} */
