/*
 * Copyright (C) 2018 Baptiste Cartier
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdint.h>

#ifndef CC430_RADIO_H
#define CC430_RADIO_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------------------------------
 *                                Defines
 * -------------------------------------------------------------------------
 */


#define CONF_REG_SIZE   47     /* There are 47 8-bit configuration registers */
#define cc430_radio_POWER_OUTPUT_MINUS_30DBM       0x03
#define cc430_radio_POWER_OUTPUT_MINUS_12DBM       0x25
#define cc430_radio_POWER_OUTPUT_MINUS_6DBM        0x2D
#define cc430_radio_POWER_OUTPUT_0DBM              0x8D
#define cc430_radio_POWER_OUTPUT_10DBM             0xC3
#define cc430_radio_POWER_OUTPUT_MAX               0xC0
#define cc430_radio_POWER_OUTPUT_DEFAULT_8_8DBM    0xC6

#define RADIOINSTRW_MINUS_30DBM              0x7E03
#define RADIOINSTRW_MINUS_12DBM              0x7E25
#define RADIOINSTRW_MINUS_6DBM               0x7E2D
#define RADIOINSTRW_0DBM                     0x7E8D
#define RADIOINSTRW_10DBM                    0x7EC3
#define RADIOINSTRW_MAX                      0x7EC0
#define RADIOINSTRW_DEFAULT_8_8DBM           0x7EC6

#define TX_TO_IDLE_TIME     10      // TX to IDLE, no calibration: ~1us   => 0.3us *10 = 3us
#define RX_TO_IDLE_TIME     2       // RX to IDLE, no calibration: ~0.1us => 0.3*2 = 0.6us
#define IDLE_TO_RX_TIME     300     // IDLE to RX, no calibration: 75.1us => 0.3*300 = 90us


void cc430_radio_delay_rf(volatile unsigned long p);
uint8_t cc430_radio_strobe(uint8_t strobe);
uint8_t cc430_radio_read_single_reg(uint8_t addr);
void cc430_radio_write_single_reg(uint8_t addr, uint8_t value);
void cc430_radio_read_burst_reg(uint8_t addr, void *buffer, uint8_t count);
void cc430_radio_write_burst_reg(uint8_t addr, void *buffer, uint8_t count);
void cc430_radio_reset_radio_core(void);
int8_t cc430_radio_write_pa_table(uint8_t value);
void cc430_radio_transmit(void *buffer, uint8_t length);
void cc430_radio_receive(void *buffer, uint8_t *length);
void cc430_radio_receive_on(void);
void cc430_radio_receive_off(void);
void cc430_radio_write_rf_reg(const uint8_t SmartRFSetting[][2], uint8_t size);


#ifdef __cplusplus
}
#endif

#endif /* CC430_RADIO_H */