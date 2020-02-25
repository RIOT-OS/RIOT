/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
 *               2019 OvGU Magdeburg
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
 * @brief       Internal common interfaces for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 */

#ifndef AT86RF2XX_INTERNAL_COMMON_H
#define AT86RF2XX_INTERNAL_COMMON_H

#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get the configured channel number of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set channel number
 */
static inline
uint8_t at86rf2xx_get_channel(const at86rf2xx_t *dev)
{
    return dev->base.netdev.chan;
}

/**
 * @brief   Get the configured PAN ID of the given device
 *
 * @param[in] dev           device to read from
 *
 * @return                  the currently set PAN ID
 */
static inline
uint16_t at86rf2xx_get_pan(const at86rf2xx_t *dev)
{
    return dev->base.netdev.pan;
}

/**
 * @brief   Get the currently configured Tx power [in dbm]
 *
 * @param[in] dev           device
 *
 * @return                  Tx power in dbm
 */
static inline
int16_t at86rf2xx_get_txpower(const at86rf2xx_t *dev)
{
    return dev->base.netdev.txpower;
}

/**
 * @brief Check if the device has the expected part number @p part
 *
 * @param[in] dev           device to be checked
 *
 * @return                  0 on success
 * @return                  -ENOTSUP on mismatch
 */
int at86rf2xx_validate(const at86rf2xx_t *dev, uint8_t part);

/**
 * @brief   Perform initial state transition from P_ON to TRX_OFF
 * @param[in]   dev         device to power on
 */
void at86rf2xx_power_on(at86rf2xx_t * dev);

/**
 * @brief   Convenience function for reading the status of the given device
 *
 * @param[in] dev       device to read the status from
 *
 * @return              internal status of the given device
 */
uint8_t at86rf2xx_get_status(const at86rf2xx_t *dev);

/**
 * @brief   Get the short address of the given device
 *
 * @param[in]   dev         device to read from
 * @param[out]  addr        the short address will be stored here
 *
 * @return                  the currently set (2-byte) short address
 */
void at86rf2xx_get_addr_short(const at86rf2xx_t *dev, network_uint16_t *addr);

/**
 * @brief   Set the short address of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] addr          (2-byte) short address to set
 */
void at86rf2xx_set_addr_short(at86rf2xx_t *dev, const network_uint16_t *addr);

/**
 * @brief   Get the configured long address of the given device
 *
 * @param[in]   dev         device to read from
 * @param[out]  addr        the long address will be stored here
 *
 * @return                  the currently set (8-byte) long address
 */
void at86rf2xx_get_addr_long(const at86rf2xx_t *dev, eui64_t *addr);

/**
 * @brief   Set the long address of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] addr          (8-byte) long address to set
 */
void at86rf2xx_set_addr_long(at86rf2xx_t *dev, const eui64_t *addr);

/**
 * @brief   Set the PAN ID of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] pan           PAN ID to set
 */
void at86rf2xx_set_pan(at86rf2xx_t *dev, uint16_t pan);

/**
 * @brief   Get the maximum number of retransmissions
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retransmissions
 */
uint8_t at86rf2xx_get_frame_max_retries(const at86rf2xx_t *dev);

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
void at86rf2xx_set_frame_max_retries(const at86rf2xx_t *dev, uint8_t max);

/**
 * @brief   Get the maximum number of channel access attempts per frame (CSMA)
 *
 * @param[in] dev           device to read from
 *
 * @return                  configured number of retries
 */
uint8_t at86rf2xx_get_csma_max_retries(const at86rf2xx_t *dev);

/**
 * @brief   Set the maximum number of channel access attempts per frame (CSMA)
 *
 * This setting specifies the number of attempts to access the channel to
 * transmit a frame. If the channel is busy @p retries times, then frame
 * transmission fails.
 * Valid values: 0 to 5, -1 means CSMA disabled
 * If @p retries > 5, the maximum value 5 is taken.
 *
 * @param[in] dev           device to write to
 * @param[in] retries       the maximum number of retries
 */
void at86rf2xx_set_csma_max_retries(const at86rf2xx_t *dev, int8_t retries);

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
void at86rf2xx_set_csma_backoff_exp(const at86rf2xx_t *dev,
                                    uint8_t min, uint8_t max);

/**
 * @brief   Set seed for CSMA random backoff
 *
 * @param[in] dev           device to write to
 * @param[in] entropy       11 bit of entropy as seed for random backoff
 */
void at86rf2xx_set_csma_seed(const at86rf2xx_t *dev, const uint8_t entropy[2]);

/**
 * @brief   Configure frame type filtering
 *
 * If @p reserved_ft == 0, @p filter_ft does not matter
 *
 * @param[in] dev           device
 * @param[in] reserved_ft   enable / disable reserved frame types
 * @param[in] filter_ft     apply address filter to reserved frame types
 */
void at86rf2xx_set_frame_type_filter(const at86rf2xx_t *dev,
                                     bool reserved_ft, bool filter_ft);

/**
 * @brief   Enable / disable external timestamping over pin 10 (DIG2)
 *
 * @param[in] dev           device
 * @param[in] timestamping  enable / disable timestamping
 */
void at86rf2xx_set_external_timestamping(const at86rf2xx_t *dev,
                                         bool timestamping);

/**
 * @brief   Enable / disable frame buffer protection
 *
 * A received frame is automatically protected against overwriting:
 * - in Basic Operating Mode, if its FCS is valid
 * - in Extended Operating Mode, if an IRQ_3 (TRX_END) is generated.
 *
 * Else another received frame could override a frame that has not been
 * read completely.
 *
 * @param[in] dev           device
 * @param[in] protect       enable / disable protection
 */
void at86rf2xx_set_frame_buffer_protection(const at86rf2xx_t *dev,
                                           bool protect);

/**
 * @brief   Read random data from the RNG
 *
 * @note    According to the data sheet this function only works properly in
 *          Basic Operation Mode. However, sporadic testing has shown that even
 *          in Extended Operation Mode this returns random enough data to be
 *          used as a seed for @ref sys_random if no cryptographically secure
 *          randomness is required.
 *          Any further use-case needs to be evaluated, especially if
 *          crypto-relevant randomness is required.
 *
 * @param[in] dev       device to configure
 * @param[out] data     buffer to copy the random data to
 * @param[in]  len      number of random bytes to store in data
 */
void at86rf2xx_get_random(const at86rf2xx_t *dev, uint8_t *data, size_t len);

/**
 * @brief   Set the state of the given device (trigger a state change)
 *
 * @param[in,out] dev       device to change state of
 * @param[in] state         the targeted new state
 *
 * @return                  the previous state before the new state was set
 */
uint8_t at86rf2xx_set_state(at86rf2xx_t *dev, uint8_t state);

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
bool at86rf2xx_cca(at86rf2xx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_INTERNAL_COMMON_H */
