/*
 * SPDX-FileCopyrightText: 2017 SKF AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

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
#include "net/ieee802154/radio.h"

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
 * @brief   CCA modes
 */
#define KW41ZRF_CCATYPE_MODE1               (1U)  /**< CCA Mode 1 - Energy */
#define KW41ZRF_CCATYPE_MODE2               (2U)  /**< CCA Mode 2 - Carrier Sense */
#define KW41ZRF_CCATYPE_MODE3               (3U)  /**< CCA Mode 3 - Combined */

/**
 * @brief Maximum number of boot retries for the KW41Z radio
 */
#define KW41ZRF_MAX_BOOT_RETRIES    (9)

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
    /**
     * @name   device specific fields
     * @{
     */
    uint32_t rf_osc_en_idle;                /**< RF_OSC_EN bits setting when RF module is in standby */
    int16_t tx_power;                       /**< The current tx-power setting of the device */
    ieee802154_filter_mode_t filter_mode;   /**< Current frame filter mode */
    uint8_t cca_busy;                       /**< CCA busy flag */
    ieee802154_tx_status_t tx_status;       /**< Status of a transmission */
    uint8_t pm_blocked;                     /**< true if we have blocked a low power mode in the CPU */
    /** @} */
} kw41zrf_t;

/**
 * @brief   Setup KW41Z in order to be used with the IEEE 802.15.4 Radio HAL
 *
 * @note    This functions MUST be called before @ref kw41zrf_init.
 *
 * @param[in] hal  pointer to the HAL descriptor associated to the device.
 */
void kw41zrf_hal_setup(ieee802154_dev_t *hal);

/**
 * @brief   Initialize the given KW41ZRF device
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int kw41zrf_init(void);

/**
 * @brief   Reset radio hardware and restore default settings
 *
 * @return 0 on success
 * @return <0 on initialization failure
 */
int kw41zrf_reset_hardware(kw41zrf_t *dev);


/**
 * @brief Print ZLL IRQSTS register state
 */
void kw41zrf_print_irq_state(void);

/**
 * @brief Print ZLL PHY_CTRL register state
 */
void kw41zrf_print_phy_ctrl(void);

/**
 * @brief Print ZLL SEQ_CTRL_STS register state
 */
void kw41zrf_print_seq_ctrl_sts(void);

#ifdef __cplusplus
}
#endif

/** @} */
