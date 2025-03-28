/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef KW41ZRF_H
#define KW41ZRF_H

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
 * @defgroup drivers_kw41zrf_config     KW41Z, KW21Z in-cpu transceiver driver compile configuration
 * @ingroup config_drivers_netdev
 * @{
 */
/**
 * @brief   Default channel used after initialization
 */
#ifndef KW41ZRF_DEFAULT_CHANNEL
#define KW41ZRF_DEFAULT_CHANNEL          (CONFIG_IEEE802154_DEFAULT_CHANNEL)
#endif

/**
 * @brief   Default CCA threshold
 */
#ifndef KW41ZRF_DEFAULT_CCA_THRESHOLD
#define KW41ZRF_DEFAULT_CCA_THRESHOLD    (-60)
#endif

/**
 * @brief   Default LQI compensation
 */
#ifndef KW41ZRF_DEFAULT_LQI_COMPENSATION
#define KW41ZRF_DEFAULT_LQI_COMPENSATION (102)
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
#define KW41ZRF_DEFAULT_TX_POWER         (CONFIG_IEEE802154_DEFAULT_TXPOWER)

/**
 * @brief   Maximum output power of the kw41z device in dBm
 */
#define KW41ZRF_OUTPUT_POWER_MAX       (4)

/**
 * @brief   Minimum output power of the kw41z device in dBm
 */
#define KW41ZRF_OUTPUT_POWER_MIN       (-19)

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
    int16_t tx_power;           /**< The current tx-power setting of the device */
    uint8_t flags;              /**< Internal driver option flags */
    uint8_t max_retrans;        /**< Maximum number of frame retransmissions
                                 *   when no Ack frame is received (macMaxFrameRetries) */
    uint8_t csma_max_backoffs;  /**< Maximum number of CSMA backoffs when
                                 *   waiting for channel clear (macMaxCsmaBackoffs) */
    uint8_t csma_min_be;        /**< Minimum backoff exponent (macMinBe) */
    uint8_t csma_max_be;        /**< Maximum backoff exponent (macMaxBe) */
    uint8_t idle_seq;           /**< state to return to after sending */
    uint8_t cca_result;         /**< Used for passing CCA result from ISR to user */
    uint8_t csma_be;            /**< Counter used internally by send implementation */
    uint8_t csma_num_backoffs;  /**< Counter used internally by send implementation */
    uint8_t num_retrans;        /**< Counter used internally by send implementation */
    uint32_t backoff_delay;     /**< CSMA delay for the current TX operation */
    uint32_t tx_timeout;        /**< Used to timeout waiting for ACK during TRX */
    uint8_t pm_blocked;         /**< true if we have blocked a low power mode in the CPU */
    uint8_t recv_blocked;       /**< blocks moving to XCVSEQ_RECEIVE to prevent
                                 *   overwriting the RX buffer before the higher
                                 *   layers have copied it to system RAM */
    /** @} */
} kw41zrf_t;

/**
 * @brief   Setup an KW41ZRF based device state
 *
 * @param[out] dev          device descriptor
 * @param[in]  index        index of @p params in a global parameter struct array.
 *                          If initialized manually, pass a unique identifier instead.
 */
void kw41zrf_setup(kw41zrf_t *dev, uint8_t index);

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
 * @brief   Reset radio hardware and restore default settings
 *
 * @param[in] dev           device to reset
 *
 * @return 0 on success
 * @return <0 on initialization failure
 */
int kw41zrf_reset(kw41zrf_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* KW41ZRF_H */
