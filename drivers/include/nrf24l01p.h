/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_nrf24l01p NRF24L01+ driver interface
 * @ingroup     drivers_netdev
 *
 * @brief       Low-level driver for nrf24l01+ transceiver
 *
 * @{
 * @file
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 */

#ifndef NRF24L01P_H
#define NRF24L01P_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include "periph/gpio.h"
#include "periph/spi.h"

/**
 * @brief   Structure that represents the hardware setup of the nrf24l01+ transceiver.
 */
typedef struct {
    spi_t spi;           /**< SPI device to initialize */
    gpio_t ce;           /**< GPIO pin to initialize as chip enable */
    gpio_t cs;           /**< GPIO pin to initialize as chip select */
    gpio_t irq;          /**< GPIO pin to initialize as interrupt request */
    unsigned listener;   /**< Place to store an ID in */
} nrf24l01p_t;

/**
 * @brief   Defines the address width of the nrf24l01+ transceiver.
 */
typedef enum {
    NRF24L01P_AW_3BYTE, /**< address width is 3 Byte */
    NRF24L01P_AW_4BYTE, /**< address width is 4 Byte */
    NRF24L01P_AW_5BYTE  /**< address width is 5 Byte */
} nrf24l01p_aw_t;

/**
 * @brief   Defines the RF datarate.
 */
typedef enum {
    NRF24L01P_DR_250KBS,/**< datarate is 250 kbps */
    NRF24L01P_DR_1MBS,  /**< datarate is 1 Mbps */
    NRF24L01P_DR_2MBS   /**< datarate is 2 Mbps */
} nrf24l01p_dr_t;

/**
 * @brief   Defines the RF power level.
 */
typedef enum {
    NRF24L01P_PWR_N18DBM = 0,   /**< power is -18dBm */
    NRF24L01P_PWR_N12DBM,       /**< power is -12dBm */
    NRF24L01P_PWR_N6DBM,        /**< power is - 6dBm */
    NRF24L01P_PWR_0DBM          /**< power is   0dBm */
} nrf24l01p_pwr_t;

/**
 * @brief   Defines the datapipe on which the receiver searches for packets.
 */
typedef enum {
    NRF24L01P_PIPE0 = 0,/**< RX pipe 0 */
    NRF24L01P_PIPE1,    /**< RX pipe 1 */
    NRF24L01P_PIPE2,    /**< RX pipe 2 */
    NRF24L01P_PIPE3,    /**< RX pipe 3 */
    NRF24L01P_PIPE4,    /**< RX pipe 4 */
    NRF24L01P_PIPE5     /**< RX pipe 5 */
} nrf24l01p_rx_pipe_t;

/**
 * @brief   Defines the error detection encoding scheme for the nrf24l01p transceiver.
 */
typedef enum {
    NRF24L01P_CRC_1BYTE = 0,    /**< encoding scheme generates 1 Byte redundancy */
    NRF24L01P_CRC_2BYTE,        /**< encoding scheme generates 2 Bytes redundancy */
} nrf24l01p_crc_t;

/**
 * @brief   Defines the automatic retransmission delay defined from end of transmission
 * to start of next treansmission.
 */
typedef enum {
    NRF24L01P_RETR_250US = 0,   /**< retransmit delay is 250us */
    NRF24L01P_RETR_500US,       /**< retransmit delay is 500us */
    NRF24L01P_RETR_750US,       /**< retransmit delay is 750us */
    NRF24L01P_RETR_1000US,      /**< retransmit delay is 1000us */
    NRF24L01P_RETR_1250US,      /**< retransmit delay is 1250us */
    NRF24L01P_RETR_1500US,      /**< retransmit delay is 1500us */
    NRF24L01P_RETR_1750US,      /**< retransmit delay is 1750us */
    NRF24L01P_RETR_2000US,      /**< retransmit delay is 2000us */
    NRF24L01P_RETR_2250US,      /**< retransmit delay is 2250us */
    NRF24L01P_RETR_2500US,      /**< retransmit delay is 2500us */
    NRF24L01P_RETR_2750US,      /**< retransmit delay is 2750us */
    NRF24L01P_RETR_3000US,      /**< retransmit delay is 3000us */
    NRF24L01P_RETR_3250US,      /**< retransmit delay is 3250us */
    NRF24L01P_RETR_3500US,      /**< retransmit delay is 3500us */
    NRF24L01P_RETR_3750US,      /**< retransmit delay is 3750us */
    NRF24L01P_RETR_4000US,      /**< retransmit delay is 4000us */
} nrf24l01p_retransmit_delay_t;

/**
 * @brief   Defines states for the nrf24l01+ transceiver
 */
typedef enum {
    RCV_PKT_NRF24L01P = 0,  /**< transceiver received data */
} nrf24l01p_rx_event_t ;

/**
* @brief   Read one register of the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] reg    Register address to read from.
* @param[in] answer Byte to read.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_read_reg(const nrf24l01p_t *dev, char reg, char *answer);

/**
* @brief   Write one register to the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] reg    Register address to write to.
* @param[in] write  Byte to write.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_write_reg(const nrf24l01p_t *dev, char reg, char write);

/**
* @brief   Initialize the nrf24l01+ transceiver.
*
* @ note
* This function initializes the transceiver so that it is ready to use.
*
* @param[in] dev    Transceiver device to use.
* @param[in] spi    SPI device to use.
* @param[in] ce     GPIO pin to use for chip enable.
* @param[in] csn    GPIO pin to use for chip select.
* @param[in] irq    GPIO pin to use for interrupt request.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_init(nrf24l01p_t *dev, spi_t spi, gpio_t ce, gpio_t csn, gpio_t irq);

/**
* @brief   Power on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_on(const nrf24l01p_t *dev);

/**
* @brief   Power off the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_off(const nrf24l01p_t *dev);

/**
* @brief   Transmit payload laying in TX FIFO of the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
*
*/
void nrf24l01p_transmit(const nrf24l01p_t *dev);

/**
* @brief   Read payload from RX FIFO of the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] answer Buffer to receive bytes to.
* @param[in] size   Number of bytes to transfer. For nrf24l01+ in general 32.
*
* @return           Number of bytes that were transferred.
* @return           -1 on error.
*/
int nrf24l01p_read_payload(const nrf24l01p_t *dev, char *answer, unsigned int size);

/**
* @brief   Register a given ID to the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pid    ID to register.
*
*/
void nrf24l01p_register(nrf24l01p_t *dev, unsigned int *pid);

/**
* @brief   Enable dynamic payload for the pipe on give nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pipe   RX pipe for which dynamic payload is enabled
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_enable_dynamic_payload(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe);

/**
* @brief   Enable dynamic ack for the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_enable_dynamic_ack(const nrf24l01p_t *dev);

/**
* @brief   Unregister the nrf24l01+ transceiver from his ID.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pid    Actual ID to unregister.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_unregister(nrf24l01p_t *dev, unsigned int pid);

/**
* @brief   Get ID from the nrf24l01p transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pid    Transceiver ID.
*
*/
void nrf24l01p_get_id(const nrf24l01p_t *dev, unsigned int *pid);

/**
* @brief   Start searching packets while in RX mode.
*
* @param[in] dev    Transceiver device to use.
*
*/
void nrf24l01p_start(const nrf24l01p_t *dev);

/**
* @brief   Stop searching packets while in RX mode.
*
* @param[in] dev    Transceiver device to use.
*
*/
void nrf24l01p_stop(const nrf24l01p_t *dev);

/**
* @brief   Preload TX FIFO with payload to transmit.
*
* @param[in] dev    Transceiver device to use.
* @param[in] data   Buffer to preload.
* @param[in] size   Number of bytes in buffer. For nrf24l01+ e.g. 32
*
* @return           Number of bytes that were transferred.
* @return           -1 on error.
*/
int nrf24l01p_preload(const nrf24l01p_t *dev, char *data, unsigned int size);

/**
* @brief   Set the RF channel for the nrf24l01+ transceiver.
*
* @note
* To ensure non-overlapping channels in 2Mbps mode, don't use directly
* neighbouring channels in this mode.
*
* @param[in] dev    Transceiver device to use.
* @param[in] chan   Buffer to preload.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_set_channel(const nrf24l01p_t *dev, uint8_t chan);

/**
* @brief   Set the address width for the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] aw     Address width (type nrf24l01p_aw_t).
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_set_address_width(const nrf24l01p_t *dev, nrf24l01p_aw_t aw);

/**
* @brief   Set the RX payload width for the nrf24l01+ transceiver
*
* @ note
* This function sets the payload width for one packet. If the maximum of 32 bytes is
* exeeded, this value is set to 32.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pipe   RX pipe to set the payload width.
* @param[in] width  Number of bytes per packet in RX payload.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_set_payload_width(const nrf24l01p_t *dev,
                                nrf24l01p_rx_pipe_t pipe, uint8_t width);

/**
* @brief   Set the TX address for the nrf24l01+ transceiver (byte array).
*
* @note
* You can either use this function and give it a pointer to a byte array which
* holds the address to set, or use "nrf24l01p_set_tx_address_long" which requires
* a uint64_t which holds the address in the LSBs.
*
* @param[in] dev    Transceiver device to use.
* @param[in] saddr  Byte array which holds the TX address.
* @param[in] length Number of bytes in address array.
*
* @return           Address length on success.
* @return           -1 on error.
*/
int nrf24l01p_set_tx_address(const nrf24l01p_t *dev, const uint8_t *saddr, unsigned int length);

/**
* @brief   Set the TX address for the nrf24l01+ transceiver (long int).
*
* @param[in] dev    Transceiver device to use.
* @param[in] saddr  Long integer which holds the TX address in LSBs.
* @param[in] length Number of relevant bytes in uint64_t.
*
* @return           Address length on success.
* @return           -1 on error.
*/
int nrf24l01p_set_tx_address_long(const nrf24l01p_t *dev, uint64_t saddr, unsigned int length);

/**
* @brief   Set the RX address for the nrf24l01+ transceiver (byte array).
*
* @note
* You can either use this function and give it a pointer to a byte array which
* holds the address to set, or use "nrf24l01p_set_rx_address_long" which requires
* a uint64_t which holds the address in the LSBs.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pipe   RX pipe to set the address.
* @param[in] saddr  Byte array which holds the RX address.
* @param[in] length Number of bytes in address array.
*
* @return           Address length on success.
* @return           -1 on error.
*/
int nrf24l01p_set_rx_address(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe, const uint8_t *saddr, unsigned int length);

/**
* @brief   Set the RX address for the nrf24l01+ transceiver (long int).
*
* @param[in] dev    Transceiver device to use.
* @param[in] pipe   RX pipe to set the address.
* @param[in] saddr  Long integer which holds the RX address in LSBs.
* @param[in] length Number of relevant bytes in uint64_t.
*
* @return           Address length on success.
* @return           -1 on error.
*/
int nrf24l01p_set_rx_address_long(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe, uint64_t saddr, unsigned int length);

/**
* @brief   Get the TX address for the nrf24l01+ transceiver (long int).
*
* @param[in] dev    Transceiver device to use.
*
* @return           TX address of the nrf24l01+ transceiver.
* @return           -1 on error.
*/
uint64_t nrf24l01p_get_tx_address_long(const nrf24l01p_t *dev);

/**
* @brief   Get the RX address for the nrf24l01+ transceiver (long int).
*
* @param[in] dev    Transceiver device to use.
* @param[in] pipe   RX pipe to get the address from.
*
* @return           RX address of the nrf24l01+ transceiver.
* @return           -1 on error.
*/
uint64_t nrf24l01p_get_rx_address_long(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe);

/**
* @brief   Get the TX address for the nrf24l01+ transceiver (long int).
*
* @note
* If you chose 2Mbps you should not allocate directly neighboring RF channels.
*
* @param[in] dev    Transceiver device to use.
* @param[in] dr     Datarate (of type nrf24l01p_dr_t).
*
* @return           1 on success.
* @return           -1 on error.
*/
int nrf24l01p_set_datarate(const nrf24l01p_t *dev, nrf24l01p_dr_t dr);

/**
* @brief   Get the status (register) of the nrf24l01+ transceiver device.
*
* @param[in] dev    Transceiver device to use.s of the.
*
* @return           Value of the status register.
*/
int nrf24l01p_get_status(const nrf24l01p_t *dev);

/**
* @brief   Set the transmit power for the nrf24l01+ transceiver device.
*
* @note
* This function rounds the input values to the nearest possible setting.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pwr    TX power for the nrf24l01p transceiver.
*
* @return           1 on success.
* @return           -1 on error.
*/
int nrf24l01p_set_power(const nrf24l01p_t *dev, int pwr);

/**
* @brief   Get the transmit power for the nrf24l01+ transceiver device.
*
* @param[in] dev    Transceiver device to use.
*
* @return           TX power value of the nrf24l01+ transceiver.
*/
int nrf24l01p_get_power(const nrf24l01p_t *dev);

/**
* @brief   Set the nrf24l01+ into TX mode.
*
* @param[in] dev    Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_set_txmode(const nrf24l01p_t *dev);

/**
* @brief   Set the nrf24l01+ into RX mode.
*
* @param[in] dev    Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_set_rxmode(const nrf24l01p_t *dev);

/**
* @brief   Reset all interrupts on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
*
* @return           1 on success.
* @return           -1 on error.
*/
int nrf24l01p_reset_all_interrupts(const nrf24l01p_t *dev);

/**
* @brief   Reset interrupts on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] intrs  Interrupt mask to reset
*
* @return           1 on success.
* @return           -1 on error.
*/
int nrf24l01p_reset_interrupts(const nrf24l01p_t *dev, char intrs);

/**
* @brief   Mask one interrupt on the nrf24l01+ transceiver.
*
* @note
* There are three interrupts on the nrf24l01+ which can be masked:
* "MASK_RX_DR", "MASK_TX_DS" and "MASK_MAX_RT". Theay are defined
* in "include/nrf24l01p_settings.h".
*
* @param[in] dev    Transceiver device to use.
* @param[in] intr   Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_mask_interrupt(const nrf24l01p_t *dev, char intr);

/**
* @brief   Unmask one interrupt on the nrf24l01+ transceiver.
*
* @note
* There are three interrupts on the nrf24l01+ which can be unmasked:
* "MASK_RX_DR", "MASK_TX_DS" and "MASK_MAX_RT". Theay are defined
* in "include/nrf24l01p_settings.h".
*
* @param[in] dev    Transceiver device to use.
* @param[in] intr   Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_unmask_interrupt(const nrf24l01p_t *dev, char intr);

/**
* @brief   Enable RX datapipe on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pipe   RX pipe to enable.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_enable_pipe(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe);

/**
* @brief   Disable RX datapipe on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pipe   RX pipe to disable.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_disable_pipe(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe);

/**
* @brief   Disable CRC error detection on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
*
* @return           0.
*/
int nrf24l01p_disable_crc(const nrf24l01p_t *dev);

/**
* @brief   Enable CRC error detection on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
* @param[in] crc    Length of cyclic redundancy check (type nrf24l01p_crc_t).
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_enable_crc(const nrf24l01p_t *dev, nrf24l01p_crc_t crc);

/**
* @brief   Setup and enable automatic ACK and retransmission on the nrf24l01+ transceiver.
*
* @note
* This function enables automatic acknowledgement for a given RX data pipe and also sets up the
* mautomatic retransmission behavior.
*
* @param[in] dev    Transceiver device to use.
* @param[in] pipe   RX pipe to setup auto ack.
* @param[in] delay_retrans    Automatic retransmission delay
                              (type nrf24l01p_retransmit_delay_t)
* @param[in] count_retrans    Auto retransmit count.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_setup_auto_ack(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe, nrf24l01p_retransmit_delay_t delay_retrans, char count_retrans);

/**
* @brief   Disable automatic ACK on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_disable_all_auto_ack(const nrf24l01p_t *dev);

/**
* @brief   Flush TX FIFO on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_flush_tx_fifo(const nrf24l01p_t *dev);

/**
* @brief   Flush RX FIFO on the nrf24l01+ transceiver.
*
* @param[in] dev    Transceiver device to use.
*
* @return           0 on success.
* @return           -1 on error.
*/
int nrf24l01p_flush_rx_fifo(const nrf24l01p_t *dev);

/**
* @brief   Callback that is called when interrupt occurs on interrupt
* pin from the nrf24l01+ transceiver.
*
* @param[in] arg    Used to pass transceiver device "dev".
*/
void nrf24l01p_rx_cb(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_H */
/** @} */
