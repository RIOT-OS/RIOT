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
 * @brief       Internal interfaces for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 */

#ifndef AT86RF2XX_INTERNAL_H
#define AT86RF2XX_INTERNAL_H

#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Minimum reset pulse width, refer p.190. We use 62us so
 *          that it is at least one tick on platforms with coarse xtimers
 */
#define AT86RF2XX_RESET_PULSE_WIDTH     (62U)

/**
 * @brief   The typical transition time to TRX_OFF after reset pulse is 26 us,
 *          refer to figure 7-8, p. 44. We use 62 us so that it is at least one
 *          tick on platforms that use a 16384 Hz oscillator or have slow start
 *          up times due to parasitic capacitance on the oscillator
 */
#define AT86RF2XX_RESET_DELAY           (62U)

/**
 * @brief   Default TX power (0dBm)
 */
#define AT86RF2XX_DEFAULT_TXPOWER       (IEEE802154_DEFAULT_TXPOWER)

/**
 * @brief   Initialize device
 *
 * @param[out] dev      device to be initialized
 */
void at86rf2xx_setup(at86rf2xx_t *dev);

/**
 * @brief   Trigger a hardware and software reset and configure radio with
 *          default values
 *
 * @param[in,out] dev       device to reset
 */
void at86rf2xx_reset(at86rf2xx_t *dev);

/**
 * @brief   Set the channel number of the given device
 *
 * @param[in,out] dev       device to write to
 * @param[in] channel       channel number to set
 */
void at86rf2xx_set_channel(at86rf2xx_t *dev, uint8_t channel);

/**
 * @brief   Read frame retransmission counter
 *
 * @param[in] dev           device from which to read retransmission counter
 *
 * @return                  retransmission counter
 */
uint8_t at86rf2xx_get_frame_retries(const at86rf2xx_t *dev);

/**
 * @brief   Read channel access retries
 *
 * @param[in] dev           device from which to read CSMA retries
 *
 * @return                  CSMA retries
 */
uint8_t at86rf2xx_get_csma_retries(const at86rf2xx_t *dev);

/**
 * @brief   Get CCA_ED_THRES value
 *
 * @param[in] dev           device to read value from
 *
 * @return                    CCA_ED_THRES value
 */
uint8_t at86rf2xx_get_cca_threshold(const at86rf2xx_t *dev);

/**
 * @brief   Set the CCA_ED_THRES value value
 *
 * @param[in] dev           device to write to
 * @param[in] thresh        the CCA_ED_THRES value value
 */
void at86rf2xx_set_cca_threshold(const at86rf2xx_t *dev, uint8_t thresh);

/**
 * @brief   Get PHY_ED_LEVEL register value
 *
 * @param[in] dev           device to read value from
 *
 * @return                  PHY_ED_LEVEL register value
 */
uint8_t at86rf2xx_get_ed_level(const at86rf2xx_t* dev);

/**
 * @brief   Configure interrupt logic
 *
 * @param[in] dev           device
 * @param[in] polling       Indicate IRQ in IRQ_STATUS register
 * @param[in] polarity      0: IRQ active high; 1: IRQ active low
 */
void at86rf2xx_set_irq_mode(const at86rf2xx_t *dev,
                            bool polling, bool polarity);

/**
 * @brief   Enable / disable slottedt ACK transmissions in RX_AACK
 *          via pin 11 (SLP_TR)
 *
 * @param[in] dev           device
 * @param[in] slotted       enable / disable
 */
void at86rf2xx_set_slotted_operation(const at86rf2xx_t *dev, bool slotted);

/**
 * @brief   Configure clock output over pin 17 CLKM
 *
 * @param[in] dev           device
 * @param[in] immediately   show change immediately
 * @param[in] mode          clock mode
 */
void at86rf2xx_set_clock_output(const at86rf2xx_t *dev,
                                bool immediately, char mode);

/**
 * @brief   Enable or disable driver specific options
 *
 * @param[in,out] dev       device to set/clear option flag for
 * @param[in] option        option to enable/disable
 * @param[in] state         true for enable, false for disable
 */
void at86rf2xx_set_option(at86rf2xx_t *dev, uint16_t option, bool state);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_INTERNAL_H */
/** @} */
