/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Implementation of public functions for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include "kernel_defines.h"
#include "byteorder.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#if IS_USED(MODULE_AT86RF2XX_AES_SPI)
#include "at86rf2xx_aes.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

void at86rf2xx_disable_clock_output(at86rf2xx_t *dev)
{
#if AT86RF2XX_IS_PERIPH
    (void) dev;
#else
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_CTRL_0);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_CTRL);
    tmp &= ~(AT86RF2XX_TRX_CTRL_0_MASK__CLKM_SHA_SEL);
    tmp |= (AT86RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF);
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_CTRL_0, tmp);
#endif
}

void at86rf2xx_enable_smart_idle(at86rf2xx_t *dev)
{
#if AT86RF2XX_SMART_IDLE_LISTENING
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_RPC);
    tmp |= AT86RF2XX_TRX_RPC_MASK__RX_RPC__SMART_IDLE;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_RPC, tmp);
    at86rf2xx_set_rxsensitivity(dev, RSSI_BASE_VAL);
#else
    (void) dev;
#endif
}

void at86rf2xx_disable_smart_idle(at86rf2xx_t *dev)
{
#if AT86RF2XX_SMART_IDLE_LISTENING
    uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_RPC);
    tmp &= ~AT86RF2XX_TRX_RPC_MASK__RX_RPC__SMART_IDLE;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__TRX_RPC, tmp);
#else
    (void) dev;
#endif
}

bool at86rf2xx_cca(at86rf2xx_t *dev)
{
    uint8_t reg;
    uint8_t old_state = at86rf2xx_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    /* Disable RX path */
    uint8_t rx_syn = at86rf2xx_reg_read(dev, AT86RF2XX_REG__RX_SYN);

    reg = rx_syn | AT86RF2XX_RX_SYN__RX_PDT_DIS;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RX_SYN, reg);
    /* Manually triggered CCA is only possible in RX_ON (basic operating mode) */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_ON);
    /* Perform CCA */
    reg = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_CC_CCA);
    reg |= AT86RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__PHY_CC_CCA, reg);
    /* Spin until done (8 symbols + 12 µs = 128 µs + 12 µs for O-QPSK)*/
    do {
        reg = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATUS);
    } while ((reg & AT86RF2XX_TRX_STATUS_MASK__CCA_DONE) == 0);
    /* return true if channel is clear */
    bool ret = !!(reg & AT86RF2XX_TRX_STATUS_MASK__CCA_STATUS);
    /* re-enable RX */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__RX_SYN, rx_syn);
    /* Step back to the old state */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
    at86rf2xx_set_state(dev, old_state);
    return ret;
}
