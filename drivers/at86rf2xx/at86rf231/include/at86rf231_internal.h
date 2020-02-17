/*
 * Copyright (C) 2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       AT86RF231 internal interface
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF231_INTERNAL_H
#define AT86RF231_INTERNAL_H

#include "at86rf2xx.h"
#include "at86rf2xx_internal_common.h"
#include "at86rf2xx_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum value to configure transmission power
 *          for AT86RF231 transceivers
 */
#define AT86RF231_TXPOWER_MAX               (20)
/**
 * @brief   Transmission power offset
 *          for AT86RF231 transceivers
 */
#define AT86RF231_TXPOWER_OFF               (17)
/**
 * @brief Minimum transmission power in dbm
 */
#define AT86RF231_TXPOWER_DBM_MIN           (-AT86RF231_TXPOWER_OFF)
/**
 * @brief Maximum transmisson power in dbm
 */
#define AT86RF231_TXPOWER_DBM_MAX           (AT86RF231_TXPOWER_MAX - \
                                             AT86RF231_TXPOWER_OFF)
/**
 * @brief   Maximum receiver sensibility in dbm
 *          for AT86RF231 transceivers
 */
#define AT86RF231_RX_SENSITIVITY_DBM_MAX    (-49)
/**
 * @brief   Minimum receiver sensibility in dbm
 *          for AT86RF231 transceivers
 */
#define AT86RF231_RX_SENSITIVITY_DBM_MIN    (-101)
/**
 * @brief   AT86RF231 transceiver partnumber
 */
#define AT86RF231_PARTNUM                   (0x03)
/**
 * @brief   AT86RF231 RSSI base value
 */
#define AT86RF231_RSSI_BASE_VAL             (-91)
/**
 * @brief   AT86RF231 minimum channel
 */
#define AT86RF231_CHANNEL_MIN               (IEEE802154_CHANNEL_MIN)
/**
 * @brief   AT86RF231 maximum channel
 */
#define AT86RF231_CHANNEL_MAX               (IEEE802154_CHANNEL_MAX)
/**
 * @brief   AT86RF231 default channel
 */
#define AT86RF231_DEFAULT_CHANNEL           (IEEE802154_DEFAULT_CHANNEL)
/* Only page 0 is supported in the 2.4 GHz band */

/**
 * @brief   State transition delay from SLEEP to TRX_OFF
 *          for AT86RF231 transceivers
 */
#define AT86RF231_WAKEUP_DELAY              (380U)
/**
 * @brief   Delay until the state machine has settled
 *          to TRX_OFF after reset
 */
#define AT86RF231_RESET_DELAY               (37U)

/**
 * @brief AT86RF231 phy mode
 */
typedef enum at86rf231_phy_mode {
    AT86RF231_OQPSK_250     = (0b00000000),
    AT86RF231_OQPSK_500     = (0b00000001),
    AT86RF231_OQPSK_1000    = (0b00000010),
    AT86RF231_OQPSK_2000    = (0b00000011)
} at86rf231_phy_mode_t;

/**
 * @brief Frame buffer structure of AT86RF231
 */
typedef struct {
    uint8_t phr;        /**< PHR */
    uint8_t *payload;   /**< Pointer to payload buffer */
    uint16_t fcs;       /**< checksum */
    uint8_t lqi;        /**< LQI */
} at86rf231_fb_t;

/**
 * @name Wrapper functions for AT86RF231
 */
static inline
uint8_t at86rf231_get_status(const at86rf231_t *dev)
{
    return at86rf2xx_get_status((const at86rf2xx_t *)dev);
}

static inline
void at86rf231_get_addr_short(const at86rf231_t *dev, network_uint16_t *addr)
{
    at86rf2xx_get_addr_short((const at86rf2xx_t *)dev, addr);
}

static inline
void at86rf231_set_addr_short(at86rf231_t *dev, network_uint16_t *addr)
{
    at86rf2xx_set_addr_short((at86rf2xx_t *)dev, addr);
}

static inline
void at86rf231_get_addr_long(const at86rf231_t *dev, eui64_t *addr)
{
    at86rf2xx_get_addr_long((const at86rf2xx_t *)dev, addr);
}

static inline
void at86rf231_set_addr_long(at86rf231_t *dev, eui64_t *addr)
{
    at86rf2xx_set_addr_long((at86rf2xx_t *)dev, addr);
}

static inline
uint16_t at86rf231_get_pan(const at86rf231_t *dev)
{
    return at86rf2xx_get_pan((const at86rf2xx_t *)dev);
}

static inline
void at86rf231_set_pan(at86rf231_t *dev, uint16_t pan)
{
    at86rf2xx_set_pan((at86rf2xx_t *)dev, pan);
}

static inline
uint16_t at86rf231_get_channel(const at86rf231_t *dev)
{
    return at86rf2xx_get_channel((const at86rf2xx_t *)dev);
}

static inline
uint8_t at86rf231_get_frame_max_retries(const at86rf231_t *dev)
{
    return at86rf2xx_get_frame_max_retries((const at86rf2xx_t *)dev);
}

static inline
void at86rf231_set_frame_max_retries(const at86rf231_t *dev, uint8_t max)
{
    at86rf2xx_set_frame_max_retries((const at86rf2xx_t *)dev, max);
}

static inline
uint8_t at86rf231_get_csma_max_retries(const at86rf231_t *dev)
{
    return at86rf2xx_get_csma_max_retries((const at86rf2xx_t *)dev);
}

static inline
void at86rf231_set_csma_max_retries(const at86rf231_t *dev, int8_t retries)
{
    at86rf2xx_set_csma_max_retries((const at86rf2xx_t *)dev, retries);
}

static inline
void at86rf231_set_csma_backoff_exp(const at86rf231_t *dev,
                                    uint8_t min, uint8_t max)
{
    at86rf2xx_set_csma_backoff_exp((const at86rf2xx_t *)dev, min, max);
}

static inline
void at86rf231_set_csma_seed(const at86rf231_t *dev, const uint8_t entropy[2])
{
    at86rf2xx_set_csma_seed((const at86rf2xx_t *)dev, entropy);
}

static inline
void at86rf231_set_slotted_operation(const at86rf231_t *dev, bool slotted)
{
    at86rf2xx_set_slotted_operation((const at86rf2xx_t *)dev, slotted);
}

static inline
void at86rf231_set_irq_mode(const at86rf231_t *dev, bool polling,
                            bool polarity)
{
    at86rf2xx_set_irq_mode((const at86rf2xx_t *)dev, polling, polarity);
}

static inline
void at86rf231_set_frame_type_filter(const at86rf231_t *dev,
                                     bool reserved_ft, bool filter_ft)
{
    at86rf2xx_set_frame_type_filter((const at86rf2xx_t *)dev,
                                    reserved_ft, filter_ft);
}

static inline
void at86rf231_set_clock_output(const at86rf231_t *dev, bool immediately,
                                char mode)
{
    at86rf2xx_set_clock_output((const at86rf2xx_t *)dev, immediately, mode);
}

static inline
void at86rf231_set_external_timestamping(const at86rf231_t *dev,
                                         bool timestamping)
{
    at86rf2xx_set_external_timestamping((const at86rf2xx_t *)dev, timestamping);
}

static inline
void at86rf231_set_frame_buffer_protection(const at86rf231_t *dev, bool protect)
{
    at86rf2xx_set_frame_buffer_protection((const at86rf2xx_t *)dev, protect);
}

static inline
void at86rf231_get_random(const at86rf231_t *dev, uint8_t *data, size_t len)
{
    at86rf2xx_get_random((at86rf2xx_t *)dev, data, len);
}
/** @} */

/**
 * @brief Check if the device is actually an AT86RF231
 *
 * @param[in] dev           device to be checked
 *
 * @return                  0 on success
 * @return                  -ENOTSUP on mismatch
 */
int at86rf231_validate(const at86rf231_t *dev);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in,out] dev       device to change state of
 * @param[in] state         the targeted new state
 *
 * @return                  the previous state before the new state was set
 */
uint8_t at86rf231_set_state(at86rf231_t *dev, uint8_t state);

/**
 * @brief   Perform hard reset
 *
 * @param[in] dev           device to be reset
 */
void at86rf231_hardware_reset(at86rf231_t *dev);

/**
 * @brief   Perform soft reset
 *
 * @param[in] dev           device to be reset
 */
void at86rf231_reset(at86rf231_t *dev);

/**
 * @brief   Get the configured transmission power of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured transmission power in dBm
 */
int16_t at86rf231_get_txpower(const at86rf231_t *dev);

/**
 * @brief   Set the transmission power of the given device [in dBm]
 *
 * If the device does not support the exact dBm value given, it will set a value
 * as close as possible to the given value. If the given value is larger or
 * lower then the maximal or minimal possible value, the min or max value is
 * set, respectively.
 *
 * @param[in] dev           device to write to
 * @param[in] dbm           transmission power in dBm
 */
void at86rf231_set_txpower(at86rf231_t *dev, int16_t dbm);

/**
 * @brief   Get the configured receiver sensitivity of the given device [in dBm]
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured receiver sensitivity in dBm
 */
int16_t at86rf231_get_rxsensitivity(const at86rf231_t *dev);

/**
 * @brief   Set the receiver sensitivity of the given device [in dBm]
 *
 * If the device does not support the exact dBm value given, it will set a value
 * as close as possible to the given value. If the given value is larger or
 * lower then the maximal or minimal possible value, the min or max value is
 * set, respectively.
 *
 * @param[in] dev           device to write to
 * @param[in] dbm           rx sensitivity in dBm
 */
void at86rf231_set_rxsensitivity(const at86rf231_t *dev, int16_t dbm);

/**
 * @brief   Perform one manual channel clear assessment (CCA)
 *
 * The CCA mode and threshold level depends on the current transceiver settings.
 *
 * @param[in]  dev          device to use
 *
 * @return                  true if channel is determined clear
 * @return                  false if channel is determined busy
 */
bool at86rf231_cca(at86rf231_t *dev);

/**
 * @brief   Get the CCA threshold value
 *
 * @param[in] dev           device to read value from
 *
 * @return                  the current CCA threshold value
 */
int8_t at86rf231_get_cca_threshold(const at86rf231_t *dev);

/**
 * @brief   Set the CCA threshold value
 *
 * @param[in] dev           device to write to
 * @param[in] thresh        the new CCA threshold value
 */
void at86rf231_set_cca_threshold(const at86rf231_t *dev, int8_t thresh);

/**
 * @brief   Get the latest ED level measurement
 *
 * @param[in] dev           device to read value from
 *
 * @return                  the last ED level
 */
int8_t at86rf231_get_ed_level(const at86rf231_t *dev);

/**
 * @brief   Set channel and channel page of the given device
 *
 * If @p channel is less than the minimum channel, the minimum channel
 * is selected.
 * If @p channel is greater than the maximum channel, the maximum
 * channel is selected.
 *
 * @param[in,out] dev       device to write to
 * @param[in] channel       channel to set
 */
void at86rf231_set_channel(at86rf231_t *dev, uint8_t channel);

/**
 * @brief   Configure PHY layer (modulation, data rate)
 *
 * @param[in] dev           device to be configured
 * @param[in] mode          phy profile
 */
void at86rf231_configure_phy(const at86rf231_t *dev,
                             at86rf231_phy_mode_t mode);

/**
 * @brief Read PHR field (Rx frame length) from frame buffer
 *
 * @param[in] dev           device
 *
 * @return                  PHR
 */
uint8_t at86rf231_fb_read_phr(const at86rf231_t *dev);

/**
 * @brief   Read and parse full frame buffer content
 *
 * @param[in] dev           device
 * @param[out] fb           frame buffer structure
 * @param[out] buf          payload buffer
 * @param[in] buf_size      size of payload buffer
 *
 * @return                  payload length on success
 * @return                  -ENOBUFS if not enough space in @p buf
 */
ssize_t at86rf231_fb_read(const at86rf231_t *dev, at86rf231_fb_t *fb,
                          void *buf, size_t buf_size);
#ifdef __cplusplus
}
#endif

#endif /* AT86RF231_INTERNAL_H */
