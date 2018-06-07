/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_kw41zrf KW41Z radio-driver
 * @ingroup     drivers_netdev
 * @brief       Device driver for the NXP KW41Z, KW21Z in-cpu transceiver
 * @{
 *
 * @file
 * @brief       Interface definition for the kw41zrf driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef KW41ZRF_H
#define KW41ZRF_H

#include <stdint.h>

#include "mutex.h"
#include "board.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/gnrc/nettype.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum packet length
 */
#define KW41ZRF_MAX_PKT_LENGTH           (IEEE802154_FRAME_LEN_MAX)

/**
 * @brief   Default PAN ID used after initialization
 */
#define KW41ZRF_DEFAULT_PANID            (IEEE802154_DEFAULT_PANID)

/**
 * @brief   Default channel used after initialization
 *
 * @{
 */
#ifndef KW41ZRF_DEFAULT_CHANNEL
#define KW41ZRF_DEFAULT_CHANNEL          (IEEE802154_DEFAULT_CHANNEL)
#endif
/** @} */

/**
 * @brief   Allowed range of channels
 *
 * @{
 */
#define KW41ZRF_MIN_CHANNEL              (11U)
#define KW41ZRF_MAX_CHANNEL              (26U)
/** @} */

/**
 * @brief   Default TX_POWER in dbm used after initialization
 */
#define KW41ZRF_DEFAULT_TX_POWER         (IEEE802154_DEFAULT_TXPOWER)

/**
 * @brief   Maximum output power of the kw41z device in dBm
 */
#define KW41ZRF_OUTPUT_POWER_MAX       (4)

/**
 * @brief   Minimum output power of the kw41z device in dBm
 */
#define KW41ZRF_OUTPUT_POWER_MIN       (-19)

/**
 * @brief   Internal device option flags
 *
 * `0x00ff` is reserved for general IEEE 802.15.4 flags
 * (see @ref netdev_ieee802154_t)
 *
 * @{
 */
enum kw41zrf_opt {
    KW41ZRF_OPT_SRC_ADDR_LONG = NETDEV_IEEE802154_SRC_MODE_LONG, /**< legacy define */
    KW41ZRF_OPT_RAWDUMP = NETDEV_IEEE802154_RAW,                 /**< legacy define */
    KW41ZRF_OPT_ACK_REQ = NETDEV_IEEE802154_ACK_REQ,             /**< legacy define */

    KW41ZRF_OPT_CSMA          = (0x0100), /**< use CSMA/CA algorithm for sending */
    KW41ZRF_OPT_PROMISCUOUS   = (0x0200), /**< promiscuous mode active */
    KW41ZRF_OPT_PRELOADING    = (0x0400), /**< preloading enabled */
    KW41ZRF_OPT_TELL_TX_START = (0x0800), /**< notify MAC layer on TX start */
    KW41ZRF_OPT_TELL_TX_END   = (0x1000), /**< notify MAC layer on TX finished */
    KW41ZRF_OPT_TELL_RX_START = (0x2000), /**< notify MAC layer on RX start */
    KW41ZRF_OPT_TELL_RX_END   = (0x4000), /**< notify MAC layer on RX finished */
    KW41ZRF_OPT_AUTOACK       = (0x8000), /**< enable automatic sending of
                                           *   ACKs for incoming packet */
};
/** @} */

/**
 * @brief ISR callback function type
 */
typedef void (*kw41zrf_cb_t)(void *arg);

/**
 * @brief   Device descriptor for KW41ZRF radio devices
 *
 * @extends netdev_ieee802154_t
 */
typedef struct {
    netdev_ieee802154_t netdev; /**< netdev parent struct */
    /**
     * @name   device specific fields
     * @{
     */
    thread_t *thread;           /**< Network driver thread, for providing feedback from IRQ handler */
    uint32_t tx_warmup_time;    /**< TX warmup time, in event timer ticks */
    uint32_t rx_warmup_time;    /**< RX warmup time, in event timer ticks */
    uint32_t rf_osc_en_idle;    /**< RF_OSC_EN bits setting when RF module is in standby */
    uint8_t max_retrans;        /**< Maximum number of frame retransmissions
                                 *   when no Ack frame is received (macMaxFrameRetries) */
    uint8_t csma_max_backoffs;  /**< Maximum number of CSMA backoffs when
                                 *   waiting for channel clear (macMaxCsmaBackoffs) */
    uint8_t csma_min_be;        /**< Minimum backoff exponent (macMinBe) */
    uint8_t csma_max_be;        /**< Maximum backoff exponent (macMaxBe) */
    int16_t tx_power;           /**< The current tx-power setting of the device */
    uint8_t idle_seq;           /**< state to return to after sending */
    uint8_t cca_result;         /**< Used for passing CCA result from ISR to user */
    uint8_t csma_be;            /**< Counter used internally by send implementation */
    uint8_t csma_num_backoffs;  /**< Counter used internally by send implementation */
    uint8_t num_retrans;        /**< Counter used internally by send implementation */
    bool pm_blocked;            /**< true if we have blocked a low power mode in the CPU */
    bool recv_blocked;          /**< blocks moving to XCVSEQ_RECEIVE to prevent
                                 *   overwriting the RX buffer before the higher
                                 *   layers have copied it to system RAM */
    /** @} */
} kw41zrf_t;

/**
 * @brief   Setup an KW41ZRF based device state
 *
 * @param[out] dev          device descriptor
 */
void kw41zrf_setup(kw41zrf_t *dev);

/**
 * @brief   Initialize the given KW41ZRF device
 *
 * @param[out] dev          device descriptor
 * @param[in]  cb           irq callback
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int kw41zrf_init(kw41zrf_t *dev, kw41zrf_cb_t cb);

/**
 * @brief   Configure radio with default values
 *
 * @param[in] dev           device to reset
 */
void kw41zrf_reset_phy(kw41zrf_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* KW41ZRF_H */
/** @} */
