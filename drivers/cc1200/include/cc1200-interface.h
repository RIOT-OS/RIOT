/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc1200
 * @{
 *
 * @file
 * @brief       internal declarations for cc1200 driver
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
 */

#ifndef CC1200_INTERFACE_H
#define CC1200_INTERFACE_H

#include <stdint.h>
#include "cc1200.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name cc1200 raw low-level interface
 * @internal
 * @{
 */
char *cc1200_get_marc_state(cc1200_t *dev);
char *cc1200_state_to_text(uint8_t state);
int cc1200_rd_set_mode(cc1200_t *dev, int mode);
uint8_t cc1200_get_buffer_pos(cc1200_t *dev);
void cc1200_isr_handler(cc1200_t *dev, void(*callback)(void*), void*arg);
void cc1200_set_base_freq_raw(cc1200_t *dev, const char* freq_array);
void cc1200_setup_rx_mode(cc1200_t *dev);
void cc1200_switch_to_pwd(cc1200_t *dev);
void cc1200_switch_to_rx(cc1200_t *dev);
void cc1200_wakeup_from_rx(cc1200_t *dev);
void cc1200_write_register(cc1200_t *dev, uint16_t r, uint8_t value);

extern const char cc1200_default_conf[];
extern const uint8_t cc1200_default_conf_size;
extern const uint8_t cc1200_pa_table[];

#ifdef MODULE_CC1200_HOOKS
void cc1200_hooks_init(void);
void cc1200_hook_rx(void);
void cc1200_hook_tx(void);
void cc1200_hook_off(void);
#endif
/* @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC1200_INTERFACE_H */
