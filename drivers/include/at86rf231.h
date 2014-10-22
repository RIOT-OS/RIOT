/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_at86rf231
 * @ingroup     drivers
 * @brief       Device driver for the Atmel AT86RF231 radio
 * @{
 *
 * @file
 * @brief       Interface definition for the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef AT86RF231_H_
#define AT86RF231_H_

#include <stdio.h>
#include <stdint.h>

#include "kernel_types.h"
#include "board.h"
#include "radio/types.h"
#include "ieee802154_frame.h"
#include "at86rf231/at86rf231_settings.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AT86RF231_MAX_PKT_LENGTH    127
#define AT86RF231_MAX_DATA_LENGTH   118

/**
 *  Structure to represent a at86rf231 packet.
 */
typedef struct __attribute__((packed))      /* TODO: do we need the packed here? it leads to an
                                               unaligned pointer -> trouble for M0 systems... */
{
    /* @{ */
    uint8_t length;             /** < the length of the frame of the frame including fcs*/
    ieee802154_frame_t frame;   /** < the ieee802154 frame */
    int8_t rssi;                /** < the rssi value */
    uint8_t crc;                /** < 1 if crc was successfull, 0 otherwise */
    uint8_t lqi;                /** < the link quality indicator */
    /* @} */
}
at86rf231_packet_t;

extern volatile kernel_pid_t transceiver_pid;

#define AT_DRIVER_STATE_DEFAULT     (0)
#define AT_DRIVER_STATE_SENDING     (1)

/**
 * @brief To keep state inside of at86rf231 driver
 * @details This variable is used to determine if a TRX_END IRQ from
 *          the radio transceiver has to be interpreted as end of
 *          sending or reception.
 */
extern uint8_t driver_state;

void at86rf231_init(kernel_pid_t tpid);
/* void at86rf231_reset(void); */
void at86rf231_rx(void);
void at86rf231_rx_handler(void);
void at86rf231_rx_irq(void *arg);

int16_t at86rf231_send(at86rf231_packet_t *packet);

int8_t at86rf231_set_channel(uint8_t channel);
uint8_t at86rf231_get_channel(void);

uint16_t at86rf231_set_pan(uint16_t pan);
uint16_t at86rf231_get_pan(void);

radio_address_t at86rf231_set_address(radio_address_t address);
radio_address_t at86rf231_get_address(void);
uint64_t at86rf231_get_address_long(void);
uint64_t at86rf231_set_address_long(uint64_t address);

void at86rf231_switch_to_rx(void);

void at86rf231_set_monitor(uint8_t mode);

enum {
    RF86RF231_MAX_TX_LENGTH = 125,
    RF86RF231_MAX_RX_LENGTH = 127,
    RF86RF231_MIN_CHANNEL = 11,
    RF86RF231_MAX_CHANNEL = 26
};

extern at86rf231_packet_t at86rf231_rx_buffer[AT86RF231_RX_BUF_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* AT86RF231_H_ */
/** @} */
