/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_nrf51822qfaa
 * @{
 *
 * @file
 * @brief           CPU specific radio function definitions
 *
 * @author          Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author          Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __NRF_RADIO_H
#define __NRF_RADIO_H

#define NRF_RADIO_DEFAULT_MODE              (NRF_RADIO_MODE_BLE1MBIT)
#define NRF_RADIO_DEFAULT_CHANNEL           (123U)
#define NRF_RADIO_DEFAULT_TXPOWER           (NRF_RADIO_PWR_M12DBM)      /* chosen arbitrarily */

#define NRF_RADIO_DEFAULT_BASEADDR_LENGTH   (2)                     /* 2 byte base address */
#define NRF_RADIO_DEFAULT_BASEADDR          (0x5555)
#define NRF_RADIO_DEFAULT_PREFIX            (0x07060504)

#define NRF_RADIO_MAX_PACKET_SIZE           (250U)
#define NRF_RADIO_BUFSIZE                   (NRF_RADIO_MAX_PACKET_SIZE + 1)     /* 255 max! */


typedef enum {
    NRF_RADIO_MODE_1MBIT    = 0,
    NRF_RADIO_MODE_2MBIT    = 1,
    NRF_RADIO_MODE_250KBIT  = 2,
    NRF_RADIO_MODE_BLE1MBIT = 3
} nrf_radio_mode_t;

typedef enum {
    NRF_RADIO_PWR_P4DBM     = 0x0f,
    NRF_RADIO_PWR_0DBM      = 0x00,
    NRF_RADIO_PWR_M4DBM     = 0xfc,
    NRF_RADIO_PWR_M8DBM     = 0xf8,
    NRF_RADIO_PWR_M12DBM    = 0xf4,
    NRF_RADIO_PWR_M16DBM    = 0xf0,
    NRF_RADIO_PWR_M20DBM    = 0xec,
    NRF_RADIO_PWR_M30DBM    = 0xd8
} nrf_radio_power_t;


/**
 * @brief Initialize the NRF51822 RADIO device
 *
 * @return          0 on success
 * @return          -1 on error
 */
int nrf_radio_init(void);

int nrf_radio_send(uint8_t addr, char *data, int size);
int nrf_radio_receive(uint8_t addr, char *data, int maxsize);

void nrf_radio_set_mode(nrf_radio_mode_t);
void nrf_radio_set_channel(int channel);
void nrf_radio_set_power(nrf_radio_power_t power);

void nrf_radio_poweron(void);
void nrf_radio_poweroff(void);

#endif /* __NRF_RADIO_H */
/** @} */
