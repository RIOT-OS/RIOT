/*
 * Copyright (C) 2016 MUTEX NZ Ltd.
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#pragma once

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Low-level radio driver for the CC2538
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 * @author      Ian Martin <ian@locicontrols.com>
 */


#include <stdbool.h>

#include "board.h"
#include "cc2538_rfcore.h"

#include "net/ieee802154.h"
#include "kernel_defines.h"

#include "net/ieee802154/radio.h"

#include "net/netopt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CC2538_AUTOCRC_LEN          (2)
#define CC2538_RF_FIFO_SIZE         (128)
#define CC2538_PACKET_LENGTH_SIZE   (1)
#define CC2538_LENGTH_BYTE_MASK     (0x7F) /**< Mask for the length byte in the packet */

#define CC2538_RF_MAX_DATA_LEN (CC2538_RF_FIFO_SIZE - CC2538_PACKET_LENGTH_SIZE)

/* TODO: Move these to sys/include/net/ieee802154.h somehow */
/* IEEE 802.15.4 defined constants (2.4 GHz logical channels) */
#define IEEE802154_MIN_FREQ         (2405) /**< Min. frequency (2405 MHz) */
#define IEEE802154_MAX_FREQ         (2480) /**< Max. frequency (2480 MHz) */

#define IEEE802154_CHANNEL_SPACING  (5)    /**< Channel spacing in MHz  */

#define IEEE802154_CHAN2FREQ(chan)  ( IEEE802154_MIN_FREQ + ((chan) - IEEE802154_CHANNEL_MIN) * IEEE802154_CHANNEL_SPACING )
#define IEEE802154_FREQ2CHAN(freq)  ( IEEE802154_CHANNEL_MIN + ((freq) - IEEE802154_MIN_FREQ) / IEEE802154_CHANNEL_SPACING )
/* /TODO */

#define CC2538_MIN_FREQ             (2394)
#define CC2538_MAX_FREQ             (2507)

#define CC2538_RF_POWER_DEFAULT     (CONFIG_IEEE802154_DEFAULT_TXPOWER)    /**< Default output power in dBm */
#define CC2538_RF_CHANNEL_DEFAULT   (CONFIG_IEEE802154_DEFAULT_CHANNEL)

#define OUTPUT_POWER_MIN            (-24)  /**< Min output power in dBm */
#define OUTPUT_POWER_MAX            (7)    /**< Max output power in dBm */
#define NUM_POWER_LEVELS            ( OUTPUT_POWER_MAX - OUTPUT_POWER_MIN + 1 )

#define CC2538_CORR_VAL_MIN         (50U)
#define CC2538_CORR_VAL_MAX         (110U)
#define CC2538_CORR_VAL_MASK        (0x7F)

#define CC2538_CRC_BIT_MASK         (0x80)

#define CC2538_CCA_THR_MASK         (0x000000FF)    /**< CCA Threshold mask */

#define CC2538_CCA_MODE_MASK        (0x18)          /**< CCA Mode mask */
#define CC2538_CCA_MODE_POS         (3U)            /**< CCA Mode pos */

#define CC2538_CSP_SKIP_INST_MASK   (0x70)          /**< CSP Skip instruction mask */
#define CC2538_CSP_SKIP_INST_POS    (4U)            /**< CSP Skip instruction pos */

#define CC2538_CSP_SKIP_N_MASK      (0x08)          /**< CSP Skip condition negation mask */

#define CC2538_CSP_SKIP_COND_CCA    (0x00)          /**< CSP Skip condition is valid CCA */
#define CC2538_CSP_SKIP_COND_CSPZ   (0x06)          /**< CSP Skip condition is CSPZ is 0 */
#define CC2538_CSP_SKIP_COND_RSSI   (0x07)          /**< CSP Skip condition is valid RSSI */

#define CC2538_SFR_MTMSEL_MASK      (0x7)           /**< MAC Timer selection mask */
#define CC2538_SFR_MTMSEL_TIMER_P   (0x2)           /**< Selects Timer period */
#define CC2538_MCTRL_SYNC_MASK      (0x2)           /**< Sync MAC Timer to external clock */
#define CC2538_MCTRL_RUN_MASK       (0x1)           /**< Run MAC Timer */

#define CC2538_CSP_MCU_CTRL_MASK    (0x1)           /**< MCU Ctrl mask */

#define CC2538_CSP_INCMAXY_MAX_MASK (0x7)           /**< CSP INCMAXY instruction (increment Register CSPX
                                                         without exceeding CSPY) */

#define CC2538_RXENABLE_RXON_MASK   (0x80)          /**< RX on mask */

#define CC2538_RSSI_OFFSET          (-73)  /**< Signal strength offset value */
#define CC2538_RF_SENSITIVITY       (-97)  /**< dBm typical, normal conditions */

#define CC2538_ACCEPT_FT_0_BEACON        (1 << 3) /**< enable or disable the BEACON filter */
#define CC2538_ACCEPT_FT_1_DATA          (1 << 4) /**< enable or disable the DATA filter */
#define CC2538_ACCEPT_FT_2_ACK           (1 << 5) /**< enable or disable the ACK filter */
#define CC2538_ACCEPT_FT_3_CMD           (1 << 6) /**< enable or disable the CMD filter */
#define CC2538_STATE_SFD_WAIT_RANGE_MIN  (0x03U)  /**< min range value of SFD wait state */
#define CC2538_STATE_SFD_WAIT_RANGE_MAX  (0x06U)  /**< max range value of SFD wait state */
#define CC2538_FRMCTRL1_PENDING_OR_MASK  (0x04)   /**< mask for enabling or disabling the
                                                       frame pending bit */
#define CC2538_FRMCTRL0_RX_MODE_DIS      (0xC)    /**< mask for disabling RX Chain during
                                                       CCA */

#define RFCORE_ASSERT(expr) (void)( (expr) || RFCORE_ASSERT_failure(#expr, __FUNCTION__, __LINE__) )

#if DEVELHELP
#define RFCORE_WAIT_UNTIL(expr) while (!(expr)) {                                        \
    DEBUG("RFCORE_WAIT_UNTIL(%s) at line %u in %s()\n", #expr, __LINE__, __FUNCTION__);  \
    thread_yield();                                                                      \
}
#else
#define RFCORE_WAIT_UNTIL(expr) while (!(expr)) thread_yield()
#endif

#define RFCORE_FLUSH_RECEIVE_FIFO() rfcore_strobe(ISFLUSHRX)

#define ABS_DIFF(x, y)          ( ((x) < (y))? ((y) - (x)) : ((x) - (y)) )
#define BOOLEAN(x)              ( (x) != 0 )
#define NOT(x)                  ( (x) == 0 )
#define GET_BYTE(buffer, index) ( (unsigned char*)(buffer) )[index]

#define BIT(n) ( 1 << (n) )

enum {
    FSM_STATE_IDLE           =  0,
    FSM_STATE_RX_CALIBRATION =  2,
    FSM_STATE_TX_CALIBRATION = 32,
};

/**
 * @brief RFCORE_XREG_RFERRM bits
 */
enum {
    STROBE_ERR = BIT(6),
    TXUNDERF   = BIT(5),
    TXOVERF    = BIT(4),
    RXUNDERF   = BIT(3),
    RXOVERF    = BIT(2),
    RXABO      = BIT(1),
    NLOCK      = BIT(0),
};

/**
 * @brief RFCORE_XREG_FRMCTRL0 bits
 */
enum {
    SET_RXENMASK_ON_TX  = BIT(0),
    IGNORE_TX_UNDERF    = BIT(1),
    PENDING_OR          = BIT(2),
};

/**
 * @brief RFCORE_XREG_FRMCTRL1 bits
 */
enum {
    ENERGY_SCAN      = BIT(4),
    AUTOACK          = BIT(5),
    AUTOCRC          = BIT(6),
    APPEND_DATA_MODE = BIT(7),
};

/**
 * @brief RFCORE_XREG_RFIRQM0 / RFCORE_XREG_RFIRQF0 bits
 */
enum {
    ACT_UNUSED       = BIT(0),
    SFD              = BIT(1), /**< Start of frame event */
    FIFOP            = BIT(2),
    SRC_MATCH_DONE   = BIT(3),
    SRC_MATCH_FOUND  = BIT(4),
    FRAME_ACCEPTED   = BIT(5),
    RXPKTDONE        = BIT(6), /**< End of frame event */
    RXMASKZERO       = BIT(7),
};

/**
 * @brief RFCORE_XREG_RFIRQM1 / RFCORE_XREG_RFIRQF1 bits
 */
enum {
    TXACKDONE        = BIT(0),
    TXDONE           = BIT(1),
    RF_IDLE          = BIT(2),
    CSP_MANINT       = BIT(3),
    CSP_STOP         = BIT(4),
    CSP_WAIT         = BIT(5),
};

/**
 * @brief Values for use with CCTEST_OBSSELx registers.
 */
enum {
    rfc_obs_sig0 = 0,
    rfc_obs_sig1 = 1,
    rfc_obs_sig2 = 2,
};

/**
 * @brief Values for RFCORE_XREG_RFC_OBS_CTRLx registers.
 */
enum {
    constant_value_0 = 0x00, /**< Constant value 0 */
    constant_value_1 = 0x01, /**< Constant value 1*/
    rfc_sniff_data   = 0x08, /**< Data from packet sniffer. Sample data
                                  on rising edges of sniff_clk.*/
    rfc_sniff_clk    = 0x09, /**< 250kHz clock for packet sniffer data.*/
    rssi_valid       = 0x0c, /**< Pin is high when the RSSI value has
                                  been updated at least once since RX was
                                  started. Cleared when leaving RX.*/
    demod_cca        = 0x0d, /**< Clear channel assessment. See FSMSTAT1
                                  register for details on how to configure
                                  the behavior of this signal. */
    sampled_cca      = 0x0e, /**< A sampled version of the CCA bit from
                                  demodulator. The value is updated whenever
                                  a SSAMPLECCA or STXONCCA strobe is issued.*/
    sfd_sync         = 0x0f, /**< Pin is high when a SFD has been received
                                  or transmitted. Cleared when leaving
                                  RX/TX respectively. Not to be confused
                                  with the SFD exception.*/
    tx_active        = 0x10, /**< Indicates that FFCTRL is in one of the TX
                                  states. Active-high.*/
    rx_active        = 0x11, /**< Indicates that FFCTRL is in one of the
                                  RX states. Active-high. */
    ffctrl_fifo      = 0x12, /**< Pin is high when one or more bytes are
                                  in the RXFIFO. Low during RXFIFO overflow. */
    ffctrl_fifop     = 0x13, /**< Pin is high when the number of bytes
                                  in the RXFIFO exceeds the programmable
                                  threshold or at least  one complete
                                  frame is in the RXFIFO. Also highduring
                                  RXFIFO overflow. Not to be confused with
                                  the FIFOP exception.*/
    packet_done      = 0x14, /**< A complete frame has been received.
                                  I.e., the number of bytes set by the
                                  frame-length field has been received.*/
    rfc_xor_rand_i_q = 0x16, /**< XOR between I and Q random outputs.
                                  Updated at 8 MHz.*/
    rfc_rand_q       = 0x17, /**< Random data output from the Q channel
                                  of the receiver. Updated at 8 MHz.*/
    rfc_rand_i       = 0x18, /**< Random data output from the I channel
                                  of the receiver. Updated at 8 MHz */
    lock_status      = 0x19, /**< 1 when PLL is in lock, otherwise 0 */
    pa_pd            = 0x20, /**< Power amplifier power-down signal */
    lna_pd           = 0x2a, /**< LNA power-down signal*/
    disabled         = 0xff, /**< disabled */
};

/**
 * @name    RF CORE observable signals settings
 */
#ifndef CONFIG_CC2538_RF_OBS_0
#define CONFIG_CC2538_RF_OBS_0      tx_active
#endif
#ifndef CONFIG_CC2538_RF_OBS_1
#define CONFIG_CC2538_RF_OBS_1      rx_active
#endif
#ifndef CONFIG_CC2538_RF_OBS_2
#define CONFIG_CC2538_RF_OBS_2      rssi_valid
#endif

/* Default configuration for cc2538dk or similar */
#ifndef CONFIG_CC2538_RF_OBS_SIG_0_PCX
#define CONFIG_CC2538_RF_OBS_SIG_0_PCX  0   /* PC0 = LED_1 (red) */
#endif
#ifndef CONFIG_CC2538_RF_OBS_SIG_1_PCX
#define CONFIG_CC2538_RF_OBS_SIG_1_PCX  1   /* PC0 = LED_2 (red) */
#endif
#ifndef CONFIG_CC2538_RF_OBS_SIG_2_PCX
#define CONFIG_CC2538_RF_OBS_SIG_2_PCX  2   /* PC0 = LED_3 (red) */
#endif
#if ((CONFIG_CC2538_RF_OBS_SIG_2_PCX > 7) || \
     (CONFIG_CC2538_RF_OBS_SIG_1_PCX > 7) || \
     (CONFIG_CC2538_RF_OBS_SIG_0_PCX > 7))
#error "CONFIG_CC2538_RF_OBS_SIG_X_PCX must be between 0-7 (PC0-PC7)"
#endif

/**
 * @brief   Device descriptor for CC2538 transceiver
 */
typedef struct {
    uint8_t state;                /**< current state of the radio */
} cc2538_rf_t;

/**
 * @brief   Setup CC2538 in order to be used with the IEEE 802.15.4 Radio HAL
 *
 * @note    This functions MUST be called before @ref cc2538_init.
 *
 * @param[in] hal  pointer to the HAL descriptor associated to the device.
 */
void cc2538_rf_hal_setup(ieee802154_dev_t *hal);

/**
 * @brief   Enable CC2538 RF IRQs.
 */
static inline void cc2538_rf_enable_irq(void)
{
    RFCORE_XREG_RFIRQM1 = TXDONE | CSP_STOP | TXACKDONE;
    RFCORE_XREG_RFIRQM0 = RXPKTDONE | SFD;
}

/**
 * @brief   Disable CC2538 RF IRQs.
 */
static inline void cc2538_rf_disable_irq(void)
{
    RFCORE_XREG_RFIRQM1 = 0;
    RFCORE_XREG_RFIRQM0 = 0;
}

/**
 * @brief   IRQ handler for RF events
 *
 */
void cc2538_irq_handler(void);

/**
 * @brief   Trigger a clear channel assessment
 *
 * @return  True if channel is clear
 * @return  False if channel is busy
 */
bool cc2538_channel_clear(void);

/**
 * @brief   Get the configured long address of the device
 *
 * @param[out] addr The currently set (8-byte) long address
 */
void cc2538_get_addr_long(uint8_t *addr);

/**
 * @brief   Get the configured short address of the device
 *
 * @param[out] addr The currently set (2-byte) short address
 */
void cc2538_get_addr_short(uint8_t *addr);

/**
 * @brief   Get the configured channel number of the device
 *
 * @return  The currently set channel number
 */
unsigned int cc2538_get_chan(void);

/**
 * @brief   Check if device is in monitor (promiscuous) mode
 *
 * @return  True if device is in monitor mode
 * @return  False if device is not in monitor mode
 */
bool cc2538_get_monitor(void);

/**
 * @brief   Get the configured PAN ID of the device
 *
 * @return  The currently set PAN ID
 */
uint16_t cc2538_get_pan(void);

/**
 * @brief   Get the configured transmission power of the device
 *
 * @return  The currently configured transmission power in dBm
 */
int cc2538_get_tx_power(void);

/**
 * @brief   Initialise the CC2538 radio hardware
 *
 */
void cc2538_init(void);

/**
 * @brief   Check if device is active
 *
 * @return  True if device is active
 * @return  False if device is not active
 */
bool cc2538_is_on(void);

/**
 * @brief   Deactivate the CC2538 radio device
 *
 */
void cc2538_off(void);

/**
 * @brief   Activate the CC2538 radio device
 *
 */
bool cc2538_on(void);

/**
 * @brief   Setup a CC2538 radio device
 *
 * @param[out] dev          Device descriptor
 */
void cc2538_setup(cc2538_rf_t *dev);

/**
 * @brief   Set the short address of the device
 *
 * @param[in] addr          (2-byte) short address to set
 */
void cc2538_set_addr_short(const uint8_t *addr);

/**
 * @brief   Set the long address of the device
 *
 * @param[in] addr          (8-byte) short address to set
 */
void cc2538_set_addr_long(const uint8_t *addr);

/**
 * @brief   Set the channel number of the device
 *
 * @param[in] chan          Channel number to set
 */
void cc2538_set_chan(unsigned int chan);

/**
 * @brief   Set the frequency of the device
 *
 * @param[in] MHz          Frequency to set in MHz
 */
void cc2538_set_freq(unsigned int MHz);

/**
 * @brief   Enable/disable monitor (promiscuous) mode for the device
 *
 * @param[in] mode          True for enable, false for disable
 */
void cc2538_set_monitor(bool mode);

/**
 * @brief   Set the PAN ID of the device
 *
 * @param[in] pan           PAN ID to set
 */
void cc2538_set_pan(uint16_t pan);

/**
 * @brief   Set the state of the device
 *
 * @param[out] dev          Device descriptor
 * @param[in]  state        State to set device to
 */
void cc2538_set_state(cc2538_rf_t *dev, netopt_state_t state);

/**
 * @brief   Set the transmission power for the device
 *
 * @param[in] dBm          Transmission power to set in dBm
 */
void cc2538_set_tx_power(int dBm);

#ifdef __cplusplus
}
#endif

/** @} */
