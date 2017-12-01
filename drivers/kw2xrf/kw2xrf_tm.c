/*
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 *
 * @file
 * @brief       Testing function of kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#include "kw2xrf.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_tm.h"

#ifdef KW2XRF_TESTMODE

static inline void enable_xcvr_test_mode(kw2xrf_t *dev)
{
    uint8_t reg;

    kw2xrf_read_iregs(dev, MKW2XDMI_DTM_CTRL1, &reg, 1);
    reg |= MKW2XDMI_DTM_CTRL1_DTM_EN;
    kw2xrf_write_iregs(dev, MKW2XDMI_DTM_CTRL1, &reg, 1);

    kw2xrf_read_iregs(dev, MKW2XDMI_TESTMODE_CTRL, &reg, 1);
    reg |= MKW2XDMI_TESTMODE_CTRL_CONTINUOUS_EN | MKW2XDMI_TESTMODE_CTRL_IDEAL_PFC_EN;
    kw2xrf_write_iregs(dev, MKW2XDMI_TESTMODE_CTRL, &reg, 1);
}

static inline void disable_xcvr_test_mode(kw2xrf_t *dev)
{
    uint8_t reg;

    kw2xrf_read_iregs(dev, MKW2XDMI_DTM_CTRL1, &reg, 1);
    reg &= MKW2XDMI_DTM_CTRL1_DTM_EN;
    kw2xrf_write_iregs(dev, MKW2XDMI_DTM_CTRL1, &reg, 1);

    kw2xrf_read_iregs(dev, MKW2XDMI_TESTMODE_CTRL, &reg, 1);
    reg &= ~(MKW2XDMI_TESTMODE_CTRL_CONTINUOUS_EN | MKW2XDMI_TESTMODE_CTRL_IDEAL_PFC_EN);
    kw2xrf_write_iregs(dev, MKW2XDMI_TESTMODE_CTRL, &reg, 1);
}

int kw2xrf_set_test_mode(kw2xrf_t *dev, uint8_t mode)
{
    uint8_t reg = 0;
    uint8_t buf[2];

    kw2xrf_abort_sequence(dev);
    disable_xcvr_test_mode(dev);
    kw2xrf_set_channel(dev, dev->netdev.chan);

    switch(mode) {
        case NETOPT_RF_TESTMODE_IDLE:
            reg = 0;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);

            kw2xrf_set_sequence(dev, XCVSEQ_IDLE);
            break;

        case NETOPT_RF_TESTMODE_CRX:
            /* set continuous RX mode */
            reg = 0;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            /* set data length */
            reg = 127;
            kw2xrf_write_iregs(dev, MKW2XDMI_DUAL_PAN_DWELL, &reg, 1);

            kw2xrf_set_sequence(dev, XCVSEQ_RECEIVE);
            break;

        case KW2XRF_TM_CTX_PREAMBLE:
            /* set continuous TX mode, transmit 10101010 pattern */
            reg = 0;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            buf[0] = 1;
            buf[1] = 0xaa;
            kw2xrf_write_fifo(dev, buf, buf[0] + 1);

            kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
            break;

        case NETOPT_RF_TESTMODE_CTX_CW:
            /* set continuous TX mode, transmit unmodulated carrier */
            reg = MKW2XDMI_TX_MODE_CTRL_DTS0;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            /* fix pll frequency for cw mode */
            uint16_t pll_frac = kw2xrf_read_dreg(dev, MKW2XDM_PLL_FRAC0_LSB);
            pll_frac |= ((uint16_t)kw2xrf_read_dreg(dev, MKW2XDM_PLL_FRAC0_MSB) << 8);
            pll_frac -= 0x400;

            kw2xrf_write_dreg(dev, MKW2XDM_PLL_FRAC0_LSB, (uint8_t)pll_frac);
            kw2xrf_write_dreg(dev, MKW2XDM_PLL_FRAC0_MSB, (uint8_t)(pll_frac >> 8));

            kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
            break;

        case KW2XRF_TM_CTX_NM1:
            /* set continuous TX mode */
            reg = MKW2XDMI_TX_MODE_CTRL_DTS0;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
            break;

        case KW2XRF_TM_CTX_NM0:
            /* set continuous TX mode */
            reg = MKW2XDMI_TX_MODE_CTRL_DTS1;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
            break;

        case KW2XRF_TM_CTX_2MHZ:
            /* set continuous TX mode */
            reg = MKW2XDMI_TX_MODE_CTRL_DTS1 | MKW2XDMI_TX_MODE_CTRL_DTS0;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
            break;

        case KW2XRF_TM_CTX_200KHZ:
            /* set continuous TX mode */
            reg = MKW2XDMI_TX_MODE_CTRL_DTS2;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
            break;

        case KW2XRF_TM_CTX_1MBPS_PRBS9:
            /* set continuous TX mode, transmit PRBS9 pattern */
            reg = MKW2XDMI_TX_MODE_CTRL_DTS2 | MKW2XDMI_TX_MODE_CTRL_DTS0;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
            break;

        case KW2XRF_TM_CTX_EXT:
            /* set continuous TX mode */
            reg = MKW2XDMI_TX_MODE_CTRL_DTS2 | MKW2XDMI_TX_MODE_CTRL_DTS1;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
            break;

        case NETOPT_RF_TESTMODE_CTX_PRBS9:
            /* set continuous TX mode, transmit PRBS9 pattern */
            reg = MKW2XDMI_TX_MODE_CTRL_DTS2 | MKW2XDMI_TX_MODE_CTRL_DTS1
                | MKW2XDMI_TX_MODE_CTRL_DTS0;
            kw2xrf_write_iregs(dev, MKW2XDMI_TX_MODE_CTRL, &reg, 1);
            enable_xcvr_test_mode(dev);

            kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
            break;

    }
    return 1;
}

#endif
/** @} */
