/*
 * Copyright (C)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_cc2538rf CC2538 Transceiver based drivers
 * @ingroup     drivers_netdev
 *
 * This module contains drivers for radio devices in Texas Instruments CC2538 series.
 * The driver is aimed to work with all devices of this series.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for CC2538 Transceiver based drivers
 *
 * @author      Anon Mall <anon.mall@dai-labor.de>
 */

#ifndef CC2538RF_H_
#define CC2538RF_H_

#include <stdint.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/gnrc/netdev.h"
#include "cc2538rf.h"
#include "cc2538_rfcore.h"

#ifdef __cplusplus
extern "C" {
#endif

/*Some configuration*/
#ifdef CC2538_RF_CONF_AUTOACK
#define CC2538_RF_AUTOACK CC2538_RF_CONF_AUTOACK
#else
#define CC2538_RF_AUTOACK 1
#endif /* CC2538_RF_CONF_AUTOACK */

#define CC2538RF_CCA_THRES                    (0xF8) /** CCA Threshold */
#define RFCORE_XREG_FRMFILT0_FRAME_FILTER_EN  (0x01) /** enables frame filtering */
#define CC2538RF_MAX_PACKET_LEN               (127) /** cc2538rf core max packet length */
#define CC2538RF_TX_POWER                     (0xD5) /* ToDo: Determine value */
#define CC2538RF_DEFAULT_CHANNEL              (15) /** CC2538rf default channel */
#define CC2538RF_DEFAULT_PANID                (0x777) /** CC2538rf PAN ID */
#define IEEE_ADDR_LOCATION_PRIMARY   0x00280028 /**< Primary IEEE address location */
#define IEEE_ADDR_LOCATION_SECONDARY 0x0027FFCC /**< Secondary IEEE address location */


#define RSSI_OFFSET    73 /** offset in db, which has to be subtracted from the RSSI register */

/**
 * @brief   FRMCTRL0 Flags
 * @{
 */
#define RFCORE_XREG_FRMCTRL0_AUTOCRC          (0x40) /** AUTO CRC XREG FRMCTRL0 */
#define RFCORE_XREG_FRMCTRL0_AUTOACK          (0x20) /** AUTO ACK XREG FRMCTRL0 */
#define RFCORE_XREG_FRMCTRL0_APPENDMODE       (0x80) /** appends RSSI values to rx buffer */

/**
 * @brief   Internal device option flags
 * @{
 */
#define CC2538RF_OPT_AUTOACK        (0x0001)       /**< auto ACKs active */
#define CC2538RFRF2XX_OPT_CSMA      (0x0002)       /**< CSMA active */
#define CC2538RF_OPT_PROMISCUOUS    (0x0004)       /**< promiscuous mode
                                                     *   active */
#define CC2538RF_OPT_PRELOADING     (0x0008)       /**< preloading enabled */
#define CC2538RF_OPT_TELL_TX_START  (0x0010)       /**< notify MAC layer on TX
                                                     *   start */
#define CC2538RF_OPT_TELL_TX_END    (0x0020)       /**< notify MAC layer on TX
                                                     *   finished */
#define CC2538RF_OPT_TELL_RX_START  (0x0040)       /**< notify MAC layer on RX
                                                     *   start */
#define CC2538RF_OPT_TELL_RX_END    (0x0080)       /**< notify MAC layer on RX
                                                     *   finished */
#define CC2538RF_OPT_RAWDUMP        (0x0100)       /**< pass RAW frame data to
                                                     *   upper layer */
#define CC2538RF_OPT_SRC_ADDR_LONG  (0x0200)       /**< send data using long
                                                     *   source address */
#define CC2538RF_OPT_USE_SRC_PAN    (0x0400)       /**< do not compress source
                                                     *   PAN ID */
/** @} */


/**
 * @brief   Flags for device internal states (see datasheet)
 * @{
 */
#define CC2538RF_STATE_TRX_OFF      (0x08)     /**< idle */
#define CC2538RF_STATE_PLL_ON       (0x09)     /**< ready to transmit */
#define CC2538RF_STATE_SLEEP        (0x0f)     /**< sleep mode */
#define CC2538RF_STATE_BUSY_RX_AACK (0x11)     /**< busy receiving data */
#define CC2538RF_STATE_BUSY_TX_ARET (0x12)     /**< busy transmitting data */
#define CC2538RF_STATE_RX_AACK_ON   (0x16)     /**< wait for incoming data */
#define CC2538RF_STATE_TX_ARET_ON   (0x19)     /**< ready for sending data */
#define CC2538RF_STATE_IN_PROGRESS  (0x1f)     /**< ongoing state conversion */
/** @} */

/**
 * @brief   Flags for FSMSTAT
 * @{
 */
#define RFCORE_XREG_FSMSTAT1_RX_ACTIVE        (0x1) /**< Status signal - TX states */
#define RFCORE_XREG_FSMSTAT1_TX_ACTIVE        (0x2) /**< Status signal - RX states */
#define RFCORE_XREG_FSMSTAT1_LOCKSTATUS       (0x4) /**< Status signal - Lock Status */
#define RFCORE_XREG_FSMSTAT1_SIMPLED_CCA      (0x8) /**< Status signal - Simpled CCA Sample */
#define RFCORE_XREG_FSMSTAT1_CCA              (0x10) /**< Status signal - Clear Channel */
#define RFCORE_XREG_FSMSTAT1_SFD              (0x20) /**< Status signal - SFD was sent or received */
#define RFCORE_XREG_FSMSTAT1_FIFO             (0x40) /**< Status signal - FIFOP */
#define RFCORE_XREG_FSMSTAT1_FIFOP            (0x80) /**< Status signal - FIFO */
/** @} */

/**
 * @brief   RF Interrupt FLAGS
 * @{
 */
/* RFCORE_SFR_RFIRQF1 */
#define CC2538RF_IRQFLAG_TXACKDONE   (0x0001)       /**< TX ACK DONE*/
#define CC2538RF_IRQFLAG_TXDONE      (0x0002)       /**< TX DONE */
#define CC2538RF_IRQFLAG_RFIDLE      (0x0004)       /**< RF IDLE */
#define CC2538RF_IRQFLAG_CSPMANINT   (0x0008)       /**< CSP Manual Interrupt */
#define CC2538RF_IRQFLAG_CSPSTOP     (0x0010)       /**< CSP Stop */
#define CC2538RF_IRQFLAG_CSPWAIT     (0x0020)       /**< CSP Wait */

/* RFCORE_SFR_RFIRQF0 */
#define CC2538RF_IRQFLAG_SFD         (0x0002)       /**< SFD has been trans or recv */
#define CC2538RF_IRQFLAG_FIFOP       (0x0004)       /**< FIFO Operation */
#define CC2538RF_IRQFLAG_SRCMATDONE  (0x0008)       /**< Src Matching Done */
#define CC2538RF_IRQFLAG_SRCMATFOUND (0x0010)       /**< Src Matching Found */
#define CC2538RF_IRQFLAG_FRAMEACC    (0x0020)       /**< Frame Accepted */
#define CC2538RF_IRQFLAG_RXPKTDONE   (0x0040)       /**< RX Paket Done */
#define CC2538RF_IRQFLAG_RXMASKZERO  (0x0080)       /**< RXMASK has gone all zero */
/** @} */


/**
 * @brief   RF Interrupt MASKS
 * @{
 */
/* RFCORE_SFR_RFIRQM1 */
#define CC2538RF_IRQMASK_TXACKDONE   (0x0001)       /**< TX ACK DONE*/
#define CC2538RF_IRQMASK_TXDONE      (0x0002)       /**< TX DONE */
#define CC2538RF_IRQMASK_RFIDLE      (0x0004)       /**< RF IDLE */
#define CC2538RF_IRQMASK_CSPMANINT   (0x0008)       /**< CSP Manual Interrupt */
#define CC2538RF_IRQMASK_CSPSTOP     (0x0010)       /**< CSP Stop */
#define CC2538RF_IRQMASK_CSPWAIT     (0x0020)       /**< CSP Wait */

/* RFCORE_SFR_RFIRQM0 */
#define CC2538RF_IRQMASK_UNUSED      (0x0001)       /**< Unused */
#define CC2538RF_IRQMASK_SFD         (0x0002)       /**< SFD sent or recv */
#define CC2538RF_IRQMASK_FIFOP       (0x0004)       /**< FIFO Operation */
#define CC2538RF_IRQMASK_SRCMATDONE  (0x0008)       /**< Src Matching Done */
#define CC2538RF_IRQMASK_SRCMATFOUND (0x0010)       /**< Src Matching Found */
#define CC2538RF_IRQMASK_FRMACCPT    (0x0020)       /**< RX Frame Accepted */
#define CC2538RF_IRQMASK_RXPKTDONE   (0x0040)       /**< RX Packet done */
#define CC2538RF_IRQMASK_RXMASKZERO  (0x0080)       /**< RXMASK has gone all zero */


/* RFCORE_XREG_RFERRM */
#define CC2538RF_RFERRM_NLOCK     (0x0001) /**< Frequency synthesizer lock error */
#define CC2538RF_RFERRM_RXABO     (0x0002) /**< Frame RX was aborted */
#define CC2538RF_RFERRM_RXOVERF   (0x0004) /**< RX FIFO overflowed */
#define CC2538RF_RFERRM_RXUNDERF  (0x0008) /**< RX FIFO underflowed */
#define CC2538RF_RFERRM_TXOVERF   (0x0010) /**< TX FIFO overflowed */
#define CC2538RF_RFERRM_TXUNDERF  (0x0020) /**< TX FIFO underflowed */
#define CC2538RF_RFERRM_STROBEERR (0x0040) /**< Strobe error */
#define CC2538RF_RFERRM_RFERRM    (0x007F) /**< RF error interrupt mask */
/** @} */

/* CSP OPCODES */
#define CC2538_RF_CSP_OP_ISRXON                0xE3
#define CC2538_RF_CSP_OP_ISTXON                0xE9
#define CC2538_RF_CSP_OP_ISTXONCCA             0xEA
#define CC2538_RF_CSP_OP_ISRFOFF               0xEF
#define CC2538_RF_CSP_OP_ISFLUSHRX             0xED
#define CC2538_RF_CSP_OP_ISFLUSHTX             0xEE
#define CC2538_RF_CSP_OP_ISCLEAR               0xFF
#define CC2538_RF_CSP_OP_ISSTART               0xE1


/* CSP MASK */
#define CC2538RF_CSP_RUNNING         (0x20)


/*Channel Values*/
#define CC2538RF_CHANNEL_MIN            11
#define CC2538RF_CHANNEL_MAX            26
#define CC2538RF_CHANNEL_SPACING         5

typedef enum {
    CC2538RF_FREQ_915MHZ,       /**< frequency 915MHz enabled */
    CC2538RF_FREQ_868MHZ,       /**< frequency 868MHz enabled */
} cc2538rf_freq_t;

/**
 * @brief   Device descriptor for cc2538rf radio devices
 */
typedef struct {
    /* netdev fields */
    const gnrc_netdev_driver_t *driver; /**< pointer to the devices interface */
    gnrc_netdev_event_cb_t event_cb;    /**< netdev event callback */
    kernel_pid_t mac_pid;               /**< the driver's thread's PID */
    gnrc_nettype_t proto;               /**< protocol the radio expects */
    uint8_t state;                      /**< current state of the radio */
    uint8_t seq_nr;                     /**< sequence number to use next */
    uint8_t frame_len;                  /**< length of the current TX frame */
    uint16_t pan;                       /**< currently used PAN ID */
    uint8_t chan;                       /**< currently used channel */
    uint8_t addr_short[2];              /**< the radio's short address */
    uint8_t addr_long[8];               /**< the radio's long address */
    uint16_t options;                   /**< state of used options */
    uint8_t idle_state;                 /**< state to return to after sending */
} cc2538rf_t;

/**
 * @brief struct holding all params needed for device initialization
 */
typedef struct cc2538rf_params {
    //spi_t spi;              /**< SPI bus the device is connected to */
    //spi_speed_t spi_speed;  /**< SPI speed to use */
    //gpio_t cs_pin;          /**< GPIO pin connected to chip select */
    //gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    //gpio_t sleep_pin;       /**< GPIO pin connected to the sleep pin */
    //gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} cc2538rf_params_t;

/**
 * @brief   Initialize a given cc2538rf device
 *
 * @param[out] dev          device descriptor
 * @param[in] spi           SPI bus the device is connected to
 * @param[in] spi_speed     SPI speed to use
 * @param[in] cs_pin        GPIO pin connected to chip select
 * @param[in] int_pin       GPIO pin connected to the interrupt pin
 * @param[in] sleep_pin     GPIO pin connected to the sleep pin
 * @param[in] reset_pin     GPIO pin connected to the reset pin
 *
 * @return                  0 on success
 * @return                  <0 on error
int cc2538rf_init(cc2538rf_t *dev, spi_t spi, spi_speed_t spi_speed,
                   gpio_t cs_pin, gpio_t int_pin,
                   gpio_t sleep_pin, gpio_t reset_pin);
 */
 int cc2538rf_init(cc2538rf_t *dev);

/**
 * @brief   Trigger a hardware reset and configure radio with default values
 *
 * @param[in] dev           device to reset
 */
void cc2538rf_reset(cc2538rf_t *dev);

/**
 * @brief   Trigger a clear channel assessment
 *
 * @param[in] dev           device to use
 *
 * @return                  true if channel is clear
 * @return                  false if channel is busy
 */
bool cc2538rf_cca(cc2538rf_t *dev);

/**
 * @brief   Get the short address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (2-byte) short address
 */
uint16_t cc2538rf_get_addr_short(cc2538rf_t *dev);

/**
 * @brief   Set the short address of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] addr          (2-byte) short address to set
 */
void cc2538rf_set_addr_short(cc2538rf_t *dev, uint16_t addr);

/**
 * @brief   Get the configured long address of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set (8-byte) long address
 */
uint64_t cc2538rf_get_addr_long(cc2538rf_t *dev);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void cc2538rf_set_addr_long(cc2538rf_t *dev, uint64_t addr);

/**
 * @brief   Get the configured channel of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel
 */
uint8_t cc2538rf_get_chan(cc2538rf_t *dev);

/**
 * @brief   Set the channel of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] chan          channel to set
 */
void cc2538rf_set_chan(cc2538rf_t *dev, uint8_t chan);

/**
 * @brief   Get the configured frequency of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set frequency
 */
cc2538rf_freq_t cc2538rf_get_freq(cc2538rf_t *dev);

/**
 * @brief   Set the frequency of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] chan          frequency to set
 */
void cc2538rf_set_freq(cc2538rf_t *dev, cc2538rf_freq_t freq);

/**
 * @brief   Get the configured PAN ID of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set PAN ID
 */
uint16_t cc2538rf_get_pan(cc2538rf_t *dev);

/**
 * @brief   Set the PAN ID of the given device
 *
 * @param[in] dev           device to write to
 * @param[in] pan           PAN ID to set
 */
void cc2538rf_set_pan(cc2538rf_t *dev, uint16_t pan);

/**
 * @brief   Get the configured transmission power of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured transmission power in dBm
 */
int16_t cc2538rf_get_txpower(cc2538rf_t *dev);

/**
 * @brief   Set the transmission power of the given device [in dBm]
 *
 * If the device does not support the exact dBm value given, it will set a value
 * as close as possible to the given value. If the given value is larger or
 * lower then the maximal or minimal possible value, the min or max value is
 * set, respectively.
 *
 * @param[in] dev           device to write to
 * @param[in] txpower       transmission power in dBm
 */
void cc2538rf_set_txpower(cc2538rf_t *dev, int16_t txpower);

/**
 * @brief   Get the maximum number of retransmissions
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retransmissions
 */
uint8_t cc2538rf_get_max_retries(cc2538rf_t *dev);

/**
 * @brief   Set the maximum number of retransmissions
 *
 * This setting specifies the number of attempts to retransmit a frame, when it
 * was not acknowledged by the recipient, before the transaction gets cancelled.
 * The maximum value is 7.
 *
 * @param[in] dev           device to write to
 * @param[in] max           the maximum number of retransmissions
 */
void cc2538rf_set_max_retries(cc2538rf_t *dev, uint8_t max);

/**
 * @brief   Get the maximum number of channel access attempts per frame (CSMA)
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retries
 */
uint8_t cc2538rf_get_csma_max_retries(cc2538rf_t *dev);

/**
 * @brief   Set the maximum number of channel access attempts per frame (CSMA)
 *
 * This setting specifies the number of attempts to access the channel to
 * transmit a frame. If the channel is busy @p retries times, then frame
 * transmission fails.
 * Valid values: 0 to 5, -1 means CSMA disabled
 *
 * @param[in] dev           device to write to
 * @param[in] max           the maximum number of retries
 */
void cc2538rf_set_csma_max_retries(cc2538rf_t *dev, int8_t retries);

/**
 * @brief   Set the min and max backoff exponent for CSMA/CA
 *
 * - Maximum BE: 0 - 8
 * - Minimum BE: 0 - [max]
 *
 * @param[in] dev           device to write to
 * @param[in] min           the minimum BE
 * @param[in] max           the maximum BE
 */
void cc2538rf_set_csma_backoff_exp(cc2538rf_t *dev, uint8_t min, uint8_t max);

/**
 * @brief   Set seed for CSMA random backoff
 *
 * @param[in] dev           device to write to
 * @param[in] entropy       11 bit of entropy as seed for random backoff
 */
void cc2538rf_set_csma_seed(cc2538rf_t *dev, uint8_t entropy[2]);

/**
 * @brief   Enable or disable driver specific options
 *
 * @param[in] dev           device to set/clear option flag for
 * @param[in] option        option to enable/disable
 * @param[in] state         true for enable, false for disable
 */
void cc2538rf_set_option(cc2538rf_t *dev, uint16_t option, bool state);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in] dev           device to change state of
 * @param[in] state         the targeted new state
 */
void cc2538rf_set_state(cc2538rf_t *dev, uint8_t state);

/**
 * @brief   Reset the internal state machine to TRX_OFF mode.
 *
 * This will force a transition to TRX_OFF regardless of whether the transceiver
 * is currently busy sending or receiving. This function is used to get back to
 * a known state during driver initialization.
 *
 * @param[in] dev           device to operate on
 */
void cc2538rf_reset_state_machine(cc2538rf_t *dev);

/**
 * @brief   Convenience function for simply sending data
 *
 * @note This function ignores the PRELOADING option
 *
 * @param[in] dev           device to use for sending
 * @param[in] data          data to send (must include IEEE802.15.4 header)
 * @param[in] len           length of @p data
 *
 * @return                  number of bytes that were actually send
 * @return                  0 on error
 */
size_t cc2538rf_send(cc2538rf_t *dev, uint8_t *data, size_t len);

/**
 * @brief   Prepare for sending of data
 *
 * This function puts the given device into the TX state, so no receiving of
 * data is possible after it was called.
 *
 * @param[in] dev            device to prepare for sending
 */
void cc2538rf_tx_prepare(cc2538rf_t *dev);

/**
 * @brief   Load chunks of data into the transmit buffer of the given device
 *
 * @param[in] dev           device to write data to
 * @param[in] data          buffer containing the data to load
 * @param[in] len           number of bytes in @p buffer
 * @param[in] offset        offset used when writing data to internal buffer
 *
 * @return                  offset + number of bytes written
 */
size_t cc2538rf_tx_load(uint8_t *data, size_t len,
                         size_t offset);

/**
 * @brief   Trigger sending of data previously loaded into transmit buffer
 *
 * @param[in] dev           device to trigger
 */
void cc2538rf_tx_exec(void);

/**
 * @brief   Flushes TX Fifo
 *
 */
void cc2538rf_flush_tx_fifo(void);

/**
 * @brief   Read the length of a received packet
 *
 * @param dev               device to read from
 *
 * @return                  overall length of a received packet in byte
 */
size_t cc2538rf_rx_len(cc2538rf_t *dev);

/**
 * @brief   Is called from the interrupt handler to parse data and send it to the stack
 *
 */
void cc2538rf_rx_read(void);

/**
 * @brief   Flushes RX Fifo
 *
 */
void cc2538rf_flush_rx_fifo(void);



/**
 * @brief  Interrupt handler for RX/TX Interrupts
 */
//void cc2538rf_isr_rfcoretx_handler(void);


/**
 * @brief interrupt handler
 */
//void isr_rfcoretx(void);

#ifdef __cplusplus
}
#endif

#endif /* cc2538rf_H_ */
/** @} */
