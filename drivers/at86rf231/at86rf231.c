/*
 * Copyright (C) 2013-2015 INRIA/FU Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf231
 * @{
 *
 * @file
 * @brief       Driver implementation of the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include "at86rf231.h"
#include "at86rf231_spi.h"
#include "at86rf231/at86rf231_settings.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "kernel_types.h"
#include "crash.h"
#include "transceiver.h"
#include "hwtimer.h"
#include "config.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef AT86RF231_SPI_SPEED
#define SPI_SPEED    SPI_SPEED_5MHZ
#else
#define SPI_SPEED    AT86RF231_SPI_SPEED
#endif

#define POWER_ON_DELAY_uS      (330)
#define WAKE_UP_DELAY_uS       (500)
#define PLL_START_DELAY_uS     (160)
#define TX_BEGIN_DELAY_uS       (16)
#define TX_FINISH_DELAY_uS      (32)
#define CCA_MEASURE_DELAY_uS   (140)

#define _MAX_RETRIES    (100)

static uint16_t radio_pan;
static uint8_t  radio_channel;
static uint16_t radio_address;
static uint64_t radio_address_long;
static bool radio_monitor;
static int radio_tx_power;
static int radio_cca_threshold;

static volatile unsigned long sfd_count, rxed_pkts;

static volatile bool at86rf231_active;
static volatile bool at86rf231_pll_locked;
static volatile bool at86rf231_cca_done;


netdev_802154_raw_packet_cb_t at86rf231_raw_packet_cb;
netdev_rcv_data_cb_t at86rf231_data_packet_cb;

/* default source address length for sending in number of byte */
static size_t _default_src_addr_len = 2;


uint8_t  driver_state;

void at86rf231_gpio_spi_interrupts_init(void);
void at86rf231_reset(void);


static const char * MODE_LABEL [] = {
    "P_ON/CMD_NOP",            // the initial state, or the empty command
    "BUSY_RX",
    "BUSY_TX/CMD_TX_START",    // the now-TXing state, or the start-TX command
    "CMD_FORCE_TRX_OFF",       // command only
    "CMD_FORCE_PLL_ON",        // command only
    "?",                       // 0x05 is not a valid mode
    "(CMD_)RX_ON",             // both a state *and* the command to reach it
    "?",                       // 0x07 is not a valid mode
    "(CMD_)TRX_OFF",           // both a state *and* the command to reach it
    "(CMD_)PLL_ON",            // both a state *and* the command to reach it
    "?", "?", "?", "?", "?",   // 0x0A--Ox0E are not valid modes
    "SLEEP",                   // can normally never appear on SPI...
    "?",                       // 0x10 is not a valid mode
    "BUSY_RX_AACK",
    "BUSY_TX_ARET",
    "?", "?", "?",             // 0x13--0x15 are not valid modes
    "(CMD_)RX_AACK_ON",        // both a state *and* the command to reach it
    "?", "?",                  // 0x17--0x18 are not valid modes
    "(CMD_)TX_ARET_ON",        // both a state *and* the command to reach it
    "?", "?",                  // 0x1A--0x1B are not valid modes
    "RX_ON_NOCLK",
    "RX_AACK_ON_NOCLK",
    "BUSY_RX_AACK_NOCLK",
    "STATE_TRANSITION_IN_PROGRESS"  // when transceiver is between two state
};
/* expected mode (state) for the AT86RF23x radio transceiver */
uint8_t expected_mode;
/* This function ensures that the radio transceiver is in the expected state */
void assert_expected_radio_mode(void)
{
    uint8_t current_mode = at86rf231_current_mode();

    /* states are identical: everything's OK */
    if (expected_mode == current_mode) return;

    /* check for "compatible" modes: i.e. modes that can be automatically
       switched without indicating an anomaly */
    switch (expected_mode) {
    case AT86RF231_TRX_STATUS__BUSY_RX:
    case AT86RF231_TRX_STATUS__RX_ON:
    case AT86RF231_TRX_STATUS__RX_ON_NOCLK:
        if ((current_mode == AT86RF231_TRX_STATUS__BUSY_RX) ||
            (current_mode == AT86RF231_TRX_STATUS__RX_ON) ||
            (current_mode == AT86RF231_TRX_STATUS__RX_ON_NOCLK))
            return;
    case AT86RF231_TRX_STATUS__BUSY_TX:
    case AT86RF231_TRX_STATUS__PLL_ON:
        if ((current_mode == AT86RF231_TRX_STATUS__BUSY_TX) ||
            (current_mode == AT86RF231_TRX_STATUS__PLL_ON))
            return;
    case AT86RF231_TRX_STATUS__SLEEP:
        if ((current_mode == AT86RF231_TRX_STATUS__P_ON) ||
            (current_mode == AT86RF231_TRX_STATUS__SLEEP))
            return;
    case AT86RF231_TRX_STATUS__BUSY_RX_AACK:
    case AT86RF231_TRX_STATUS__RX_AACK_ON:
    case AT86RF231_TRX_STATUS__RX_AACK_ON_NOCLK:
    case AT86RF231_TRX_STATUS__BUSY_RX_AACK_NOCLK:
        if ((current_mode == AT86RF231_TRX_STATUS__BUSY_RX_AACK) ||
            (current_mode == AT86RF231_TRX_STATUS__RX_AACK_ON) ||
            (current_mode == AT86RF231_TRX_STATUS__RX_AACK_ON_NOCLK) ||
            (current_mode == AT86RF231_TRX_STATUS__BUSY_RX_AACK_NOCLK))
            return;
    case AT86RF231_TRX_STATUS__BUSY_TX_ARET:
    case AT86RF231_TRX_STATUS__TX_ARET_ON:
        if ((current_mode == AT86RF231_TRX_STATUS__BUSY_TX_ARET) ||
            (current_mode == AT86RF231_TRX_STATUS__TX_ARET_ON))
            return;
    }

    /* if we arrive here, radio transceiver is in an inconsistent state */
    char errmsg[80];
    sprintf(errmsg, "AT86RF231 in wrong mode (%d [%s] instead of %d [%s])",
            current_mode,  MODE_LABEL[current_mode],
            expected_mode, MODE_LABEL[expected_mode]);
    core_panic(0x231, errmsg);
}
#if 1
#define ASSERT_EXPECTED_RADIO_MODE assert_expected_radio_mode()
#else
#define ASSERT_EXPECTED_RADIO_MODE  /* empty macro */
#endif
/* This function handles putting AT86RF23x from one mode (state) to another;
   since this is quite a complex process, every transceiver mode change
   should be done through this "public" function. */
netdev_802154_tx_status_t at86rf231_change_mode(uint8_t target_mode)
{
    ASSERT_EXPECTED_RADIO_MODE;
    uint8_t current_mode = at86rf231_current_mode();
    uint8_t wanted_status;
    unsigned long wait_delay_us = 1;

    switch (target_mode) {
    case AT86RF231_TRX_STATE__NOP:
       // nothing to check nor to wait for
        return NETDEV_802154_TX_STATUS_OK;

    case AT86RF231_TRX_STATE__TX_START:
        // start packet transmission
        switch (current_mode) {
        case AT86RF231_TRX_STATUS__PLL_ON:
            // "basic" transmission mode
            wanted_status = AT86RF231_TRX_STATUS__BUSY_TX;
            break;
        case AT86RF231_TRX_STATUS__TX_ARET_ON:
            // "advanced" (CSMA & wait-for-ACK) transmission mode
            wanted_status = AT86RF231_TRX_STATUS__BUSY_TX_ARET;
            break;
        default:
            // cannot start a transmission in current mode
            printf("Cannot start a transmission in current mode (0x%2.2x, %s)",
                   current_mode, MODE_LABEL[current_mode]);
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
        }
        wait_delay_us = TX_BEGIN_DELAY_uS;
        break;

    case AT86RF231_TRX_STATE__FORCE_TRX_OFF:
        // force transceiver to "idle" mode
        switch (current_mode) {
        case AT86RF231_TRX_STATUS__TRX_OFF:
            // already there
            return NETDEV_802154_TX_STATUS_OK;
        case AT86RF231_TRX_STATUS__P_ON:
            // delay after transceiver RESET
            wait_delay_us = POWER_ON_DELAY_uS;
            break;
        case AT86RF231_TRX_STATUS__BUSY_TX:
        case AT86RF231_TRX_STATUS__BUSY_TX_ARET:
        case AT86RF231_TRX_STATUS__BUSY_RX_AACK_NOCLK:
        case AT86RF231_TRX_STATUS__BUSY_RX_AACK:
        case AT86RF231_TRX_STATUS__BUSY_RX:
            // delay to terminate ongoing transmission
            wait_delay_us = TX_FINISH_DELAY_uS;
            break;
        default:
            // you should normally be able to FORCE_TRX_OFF
            // from any other mode (except SLEEP) with minimal delay
            break;
            // wake-up case not handled here
        }
        wanted_status = AT86RF231_TRX_STATUS__TRX_OFF;
        break;

    case AT86RF231_TRX_STATE__FORCE_PLL_ON:
        // force transceiver to "active", ready-to-send state
        switch (current_mode) {
        case AT86RF231_TRX_STATUS__PLL_ON:
            // already there
            return NETDEV_802154_TX_STATUS_OK;
        case AT86RF231_TRX_STATUS__BUSY_TX:
        case AT86RF231_TRX_STATUS__BUSY_TX_ARET:
        case AT86RF231_TRX_STATUS__BUSY_RX_AACK:
        case AT86RF231_TRX_STATUS__BUSY_RX:
            // delay to terminate ongoing transmission
            wait_delay_us = TX_FINISH_DELAY_uS;
            break;
        case AT86RF231_TRX_STATUS__RX_ON:
        case AT86RF231_TRX_STATUS__RX_AACK_ON:
        case AT86RF231_TRX_STATUS__TX_ARET_ON:
            // modes that can switch to PLL_ON with minimal (1us) delay
            break;
        default:
            // other modes cannot force tranceiver to PLL_ON mode
            printf("Cannot force PLL_ON in current mode (0x%2.2x, %s)",
                   current_mode, MODE_LABEL[current_mode]);
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
        }
        wanted_status = AT86RF231_TRX_STATUS__PLL_ON;
        break;

    case AT86RF231_TRX_STATE__RX_ON:
        // go to active listen state
        switch (current_mode) {
        case AT86RF231_TRX_STATUS__RX_ON:
            // already there
            return NETDEV_802154_TX_STATUS_OK;
        case AT86RF231_TRX_STATUS__TRX_OFF:
            // delay to start PLL from "idle" mode
            wait_delay_us = PLL_START_DELAY_uS;
            break;
        case AT86RF231_TRX_STATUS__PLL_ON:
            // modes that can switch to RX_ON with minimal (1us) delay
            break;
        default:
            // other modes cannot switch directly to RX_ON mode
            printf("Cannot switch to RX_ON in current mode (0x%2.2x, %s)",
                   current_mode, MODE_LABEL[current_mode]);
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
        }
        wanted_status = AT86RF231_TRX_STATUS__RX_ON;
        break;

    case AT86RF231_TRX_STATE__TRX_OFF:
        // go to "idle" mode
        switch (current_mode) {
        case AT86RF231_TRX_STATUS__TRX_OFF:
            // already there
            return NETDEV_802154_TX_STATUS_OK;
        case AT86RF231_TRX_STATUS__P_ON:
            // delay after transceiver RESET
            wait_delay_us = POWER_ON_DELAY_uS;
            break;
        case AT86RF231_TRX_STATUS__PLL_ON:
        case AT86RF231_TRX_STATUS__RX_ON:
        case AT86RF231_TRX_STATUS__RX_AACK_ON:
        case AT86RF231_TRX_STATUS__TX_ARET_ON:
            // modes that can switch to TRX_OFF with minimal (1us) delay
            break;
        default:
            // other modes cannot switch directly to "idle" mode
            printf("Cannot switch to TRX_OFF in current mode (0x%2.2x, %s)",
                   current_mode, MODE_LABEL[current_mode]);
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
        }
        wanted_status = AT86RF231_TRX_STATUS__TRX_OFF;
        break;

    case AT86RF231_TRX_STATE__PLL_ON:
        // go to "active", ready-to-send state
        switch (current_mode) {
        case AT86RF231_TRX_STATUS__PLL_ON:
            // already there
            return NETDEV_802154_TX_STATUS_OK;
        case AT86RF231_TRX_STATUS__TRX_OFF:
            // delay to start PLL from "idle" mode
            wait_delay_us = PLL_START_DELAY_uS;
            break;
        case AT86RF231_TRX_STATUS__RX_ON:
        case AT86RF231_TRX_STATUS__RX_AACK_ON:
        case AT86RF231_TRX_STATUS__TX_ARET_ON:
            // modes that can switch to PLL_ON with minimal (1us) delay
            break;
        default:
            // other modes cannot switch directly to PLL_ON mode
            printf("Cannot switch to PLL_ON in current mode (0x%2.2x, %s)",
                   current_mode, MODE_LABEL[current_mode]);
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
        }
        wanted_status = AT86RF231_TRX_STATUS__PLL_ON;
        break;

    case AT86RF231_TRX_STATE__RX_AACK_ON:
        // go to advanced, "automatic-ACK-return" active listen state
        switch (current_mode) {
        case AT86RF231_TRX_STATUS__RX_AACK_ON:
            // already there
            return NETDEV_802154_TX_STATUS_OK;
        case AT86RF231_TRX_STATUS__TRX_OFF:
            // delay to start PLL from "idle" mode
            wait_delay_us = PLL_START_DELAY_uS;
            break;
        case AT86RF231_TRX_STATUS__PLL_ON:
            // mode that can switch to RX_AACK_ON with minimal (1us) delay
            break;
        case AT86RF231_TRX_STATUS__P_ON:
        case AT86RF231_TRX_STATUS__SLEEP:
            // WARNING: THESE ARE NOT MODES THAT SHOULD BE ENCOUNTERED WHEN
            // GOING INTO RX MODE, BUT FOR AN UNKNOWN REASON, THE RADIO
            // CHIP REGULARLY REPORTS BEING IN THIS MODE (WHILE ACTUALLY
            // IN PLL_ON MODE): THIS IS A WORKAROUND!!!
            printf("WARNING, MAY BE IN ANORMAL MODE (%2.2x, %s)\n",
                   current_mode, MODE_LABEL[current_mode]);
            break;
        default:
            // other modes cannot switch directly to RX_AACK_ON mode
            printf("Cannot switch to RX_AACK_ON in current mode (0x%2.2x, %s)",
                   current_mode, MODE_LABEL[current_mode]);
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
        }
        wanted_status = AT86RF231_TRX_STATUS__RX_AACK_ON;
        break;

    case AT86RF231_TRX_STATE__TX_ARET_ON:
        // go to advanced, "CSMA & wait-for-ACK" ready-to-send state
        switch (current_mode) {
        case AT86RF231_TRX_STATUS__TX_ARET_ON:
            // already there
            return NETDEV_802154_TX_STATUS_OK;
        case AT86RF231_TRX_STATUS__TRX_OFF:
            // delay to start PLL from "idle" mode
            wait_delay_us = PLL_START_DELAY_uS;
            break;
        case AT86RF231_TRX_STATUS__PLL_ON:
            // mode that can switch to TX_ARET_ON with minimal (1us) delay
            break;
        default:
            // other modes cannot switch directly to TX_ARET_ON mode
            printf("Cannot switch to TX_ARET_ON in current mode (0x%2.2x, %s)",
                   current_mode, MODE_LABEL[current_mode]);
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
        }
        wanted_status = AT86RF231_TRX_STATUS__TX_ARET_ON;
        break;

    default:
        // unknown command!
        printf("Invalid transceiver state (%u) demanded", target_mode);
        return NETDEV_802154_TX_STATUS_INVALID_PARAM;
    }

    if (wait_delay_us > 1) {
        hwtimer_wait(HWTIMER_TICKS(wait_delay_us) + 1);
    } else {
        hwtimer_spin(1);
    }

    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, target_mode);
    int countdown = _MAX_RETRIES;
    do {
        hwtimer_spin(1);
        current_mode = at86rf231_current_mode();
        if (!--countdown) {
            printf("at86rf231 : ERROR : could not enter mode %s!\n",
                  MODE_LABEL[target_mode]);
            printf("            (Current mode == %u, %s)\n",
                  current_mode, MODE_LABEL[current_mode]);
            return NETDEV_802154_TX_STATUS_ERROR;
        }
    } while (current_mode != wanted_status);

    expected_mode = wanted_status;

    return NETDEV_802154_TX_STATUS_OK;
}


/* update the memorized config. values with actual registers' contents */
static const int TX_POW_TO_DBM[] = {
    4, 3, 3, 3, 2, 2, 1, 0, -1, -2, -3, -4, -6, -8, -12, -17
};
void at86rf231_update_config_values(void)
{
    ASSERT_EXPECTED_RADIO_MODE;
    radio_address =  at86rf231_reg_read(AT86RF231_REG__SHORT_ADDR_0)
                  | (at86rf231_reg_read(AT86RF231_REG__SHORT_ADDR_1) << 8);
    radio_address_long =  at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_0)
            | ((uint64_t) at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1) << 8)
            | ((uint64_t) at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_2) << 16)
            | ((uint64_t) at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_3) << 24)
            | ((uint64_t) at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_4) << 32)
            | ((uint64_t) at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_5) << 40)
            | ((uint64_t) at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_6) << 48)
            | ((uint64_t) at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_7) << 56);
    radio_pan =  at86rf231_reg_read(AT86RF231_REG__PAN_ID_0)
              | (at86rf231_reg_read(AT86RF231_REG__PAN_ID_1) << 8);
    radio_channel = at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA)
                  & AT86RF231_PHY_CC_CCA_MASK__CHANNEL;
    radio_monitor = (at86rf231_reg_read(AT86RF231_REG__XAH_CTRL_1)
                     & AT86RF231_XAH_CTRL_1__AACK_PROM_MODE);
    uint8_t pow = at86rf231_reg_read(AT86RF231_REG__PHY_TX_PWR)
                & AT86RF231_PHY_TX_PWR_MASK__TX_PWR;
    radio_tx_power = TX_POW_TO_DBM[pow];
    uint8_t val = at86rf231_reg_read(AT86RF231_REG__CCA_THRES)
                & AT86RF231_CCA_THRES_MASK__CCA_ED_THRES;
    radio_cca_threshold = AT86RF231_RSSI_BASE_VAL + 2 * val;
}

/* rebuilds the configuration after going out of sleep mode */
void at86rf231_set_cfg_registers(void)
{
    ASSERT_EXPECTED_RADIO_MODE;
    at86rf231_reg_write(AT86RF231_REG__IRQ_MASK,
                        (AT86RF231_IRQ_STATUS_MASK__TRX_UR
                       | AT86RF231_IRQ_STATUS_MASK__CCA_ED_DONE
                       | AT86RF231_IRQ_STATUS_MASK__TRX_END
                       | AT86RF231_IRQ_STATUS_MASK__RX_START
                       | AT86RF231_IRQ_STATUS_MASK__PLL_UNLOCK
                       | AT86RF231_IRQ_STATUS_MASK__PLL_LOCK));
    uint8_t reg = at86rf231_reg_read(AT86RF231_REG__TRX_CTRL_1);
    reg |= AT86RF231_TRX_CTRL_1_MASK__IRQ_MASK_MODE;
    at86rf231_reg_write(AT86RF231_REG__TRX_CTRL_1, reg);
    at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);

    at86rf231_set_address(radio_address);
    at86rf231_set_address_long(radio_address_long);
    at86rf231_set_pan(radio_pan);
    at86rf231_set_channel(radio_channel);
    at86rf231_set_monitor(radio_monitor);
    at86rf231_set_tx_power(radio_tx_power);
    at86rf231_set_cca_threshold(radio_cca_threshold);
}


/* debug function: show the transceiver's registers contents */
void at86rf231_print_status(void)
{
    printf("> == AT86RF transceiver status ==\n");
    ASSERT_EXPECTED_RADIO_MODE;
    uint8_t mode = at86rf231_current_mode();
    printf("> Current mode = 0x%2.2x (%s).\n", mode, MODE_LABEL[mode]);

    uint8_t irqStat = at86rf231_reg_read(AT86RF231_REG__IRQ_MASK);
    printf("> IRQ mask == 0x%2.2x.\n", irqStat);

    irqStat = at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);
    if (irqStat == 0) {
        printf("> No at86rf231 IRQ.\n");
    } else {
        printf("> at86rf231 IRQ!\n");
        printf("> Pending:\n");
        if (irqStat & AT86RF231_IRQ_STATUS_MASK__BAT_LOW) {
            printf("  -- BAT_LOW\n");
        }
        if (irqStat & AT86RF231_IRQ_STATUS_MASK__TRX_UR) {
            printf("  -- TRX_UR\n");
        }
        if (irqStat & AT86RF231_IRQ_STATUS_MASK__AMI) {
            printf("  -- AMI\n");
        }
        if (irqStat & AT86RF231_IRQ_STATUS_MASK__CCA_ED_DONE) {
            printf("  -- CCA_ED_DONE\n");
        }
        if (irqStat & AT86RF231_IRQ_STATUS_MASK__TRX_END) {
            printf("  -- TRX_END\n");
        }
        if (irqStat & AT86RF231_IRQ_STATUS_MASK__RX_START) {
            printf("  -- RX_START\n");
        }
        if (irqStat & AT86RF231_IRQ_STATUS_MASK__PLL_UNLOCK) {
            printf("  -- PLL_UNLOCK\n");
        }
        if (irqStat & AT86RF231_IRQ_STATUS_MASK__PLL_LOCK) {
            printf("  -- PLL_LOCK\n");
        }
    }
    printf("> Address: %4.4x\n",
               at86rf231_reg_read(AT86RF231_REG__SHORT_ADDR_0)
            | (at86rf231_reg_read(AT86RF231_REG__SHORT_ADDR_1) << 8));
    printf("> PAN ID: %4.4x\n",
               at86rf231_reg_read(AT86RF231_REG__PAN_ID_0)
            | (at86rf231_reg_read(AT86RF231_REG__PAN_ID_1) << 8));
    printf("> Long address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                          at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_0),
                          at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1),
                          at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_2),
                          at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_3),
                          at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_4),
                          at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_5),
                          at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_6),
                          at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_7));
    printf("> Channel: %u\n", at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA)
                     & AT86RF231_PHY_CC_CCA_MASK__CHANNEL);
    printf("> Monitor mode: %d\n", (at86rf231_reg_read(AT86RF231_REG__XAH_CTRL_1)
                     & AT86RF231_XAH_CTRL_1__AACK_PROM_MODE));
    uint8_t pow = at86rf231_reg_read(AT86RF231_REG__PHY_TX_PWR)
                & AT86RF231_PHY_TX_PWR_MASK__TX_PWR;
    printf("> TX power: %d dBm\n", TX_POW_TO_DBM[pow]);
    uint8_t val = at86rf231_reg_read(AT86RF231_REG__CCA_THRES)
                & AT86RF231_CCA_THRES_MASK__CCA_ED_THRES;
    printf("> CCA threshold: %d dBm\n", AT86RF231_RSSI_BASE_VAL + 2 * val);

    printf("> CSMA seed 0: %2.2x\n", at86rf231_reg_read(AT86RF231_REG__CSMA_SEED_0));
    printf("> CSMA seed 1: %2.2x\n", at86rf231_reg_read(AT86RF231_REG__CSMA_SEED_1) & 0x07);
    printf("> SFD seen: %lu\n", sfd_count);
    printf("> packets seen: %lu\n", rxed_pkts);
}



#ifdef MODULE_TRANSCEIVER
void at86rf231_init(kernel_pid_t tpid)
{
    transceiver_pid = tpid;
    at86rf231_initialize(NULL);
}
#endif

int at86rf231_initialize(netdev_t *dev)
{
    at86rf231_gpio_spi_interrupts_init();

    hwtimer_wait(HWTIMER_TICKS(POWER_ON_DELAY_uS));

    at86rf231_reset();

    /*
     * TODO: configure security
     */
#ifdef MODULE_CONFIG
    at86rf231_set_pan(sysconfig.radio_pan_id);
#else
    at86rf231_set_pan(0x0001);
#endif

    at86rf231_update_config_values();

    return 0;
}

int at86rf231_on(void)
{
    /* negate SLP_TR line */
    gpio_clear(AT86RF231_SLEEP);

    /* busy wait for TRX_OFF state */
    hwtimer_wait(HWTIMER_TICKS(WAKE_UP_DELAY_uS));
    int countdown = _MAX_RETRIES;
    do {
        hwtimer_spin(1);
        if (--countdown <= 0) {
            at86rf231_print_status();
#ifdef RESET_TRANSCEIVER_ON_ERROR
            printf("at86rf231 (ON) : ERROR : could not wake up transceiver!\n");
            printf("Resetting transceiver\n");
            at86rf231_reset();
#else
            core_panic(0x0231,
                   "at86rf231 (ON) : ERROR : could not wake up transceiver!\n");
            /* we won't go past here... */
#endif
        }
    } while ( (at86rf231_current_mode() != AT86RF231_TRX_STATUS__TRX_OFF)
           && (!at86rf231_active) );

    /* Read IRQ status register to clear it */
    at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);

    /* re-enable IRQs from radio transceiver */
    gpio_irq_enable(AT86RF231_INT);

    /* transceiver is active again */
    at86rf231_active = true;
    at86rf231_cca_done = false;

    /* we are now supposed to be in TRX_OFF mode */
    expected_mode = AT86RF231_TRX_STATUS__TRX_OFF;

    /* change into reception mode */
//    at86rf231_set_cfg_registers();
    at86rf231_switch_to_rx();

    DEBUG("AT86RF231 transceiver is up and running.\n");

    return 1;
}

void at86rf231_off(void)
{
    /* Send a FORCE TRX OFF command */
    netdev_802154_tx_status_t res =
           at86rf231_change_mode(AT86RF231_TRX_STATE__FORCE_TRX_OFF);
    if (res != NETDEV_802154_TX_STATUS_OK) {
        core_panic(0x0231,
                   "at86rf231 (OFF) : ERROR : could not enter TRX_OFF mode");
    }

    /* assert SLP_TR line */
    gpio_set(AT86RF231_SLEEP);

    /* disable IRQs from radio transceiver */
    gpio_irq_disable(AT86RF231_INT);

    /* update our driver's flags */
    at86rf231_active = false;
    at86rf231_pll_locked = false;
    DEBUG("AT86RF231 transceiver goes off.\n");

    /* the SPI will send 0 to any request as long as transceiver is asleep */
    expected_mode = 0;
}

int at86rf231_is_on(void)
{
    return at86rf231_active;
}

void at86rf231_switch_to_rx(void)
{
    /* Wait for transceiver to be in a "switchable" state */
    char errmsg[80];
    uint8_t current_mode;
    bool ready = false;
    do {
        current_mode = at86rf231_current_mode();
        switch (current_mode) {
        case AT86RF231_TRX_STATUS__RX_AACK_ON:
        case AT86RF231_TRX_STATUS__BUSY_RX_AACK:
            // we are already in (advanced) RX mode!
            return;
        case AT86RF231_TRX_STATUS__TRX_OFF:
        case AT86RF231_TRX_STATUS__RX_ON:
        case AT86RF231_TRX_STATUS__TX_ARET_ON:
        case AT86RF231_TRX_STATUS__PLL_ON:
            // ok, ready to continue
            ready = true;
            break;
        case AT86RF231_TRX_STATUS__BUSY_RX:
        case AT86RF231_TRX_STATUS__BUSY_TX:
        case AT86RF231_TRX_STATUS__BUSY_TX_ARET:
            // continue to wait...
            ready = false;
            break;
        case AT86RF231_TRX_STATUS__P_ON:
        case AT86RF231_TRX_STATUS__SLEEP:
            // WARNING: THESE ARE NOT MODES THAT SHOULD BE ENCOUNTERED WHEN
            // GOING INTO RX MODE, BUT FOR AN UNKNOWN REASON, THE RADIO
            // CHIP REGULARLY REPORTS BEING IN THIS MODE (WHILE ACTUALLY
            // IN PLL_ON MODE): THIS IS A WORKAROUND!!!
            printf("[RX] WARNING, MAY BE IN ANORMAL MODE (%2.2x, %s)\n",
                   current_mode, MODE_LABEL[current_mode]);
            ready = true;
            break;
        default:
            // we are in a mode we shouldn't be!
             sprintf(errmsg,
                    "at86rf231 : (RX) ERROR : in an incorrect state (%s)",
                    MODE_LABEL[current_mode]);
            core_panic(0x0231, errmsg);
        }
    } while (!ready);

    /* First return to TRX_OFF state */
    netdev_802154_tx_status_t res =
           at86rf231_change_mode(AT86RF231_TRX_STATE__FORCE_TRX_OFF);
    if (res != NETDEV_802154_TX_STATUS_OK) {
        core_panic(0x0231,
                   "at86rf231 : (RX) ERROR : could not force TRX_OFF mode");
    }

    /* Then enter RX_AACK_ON state */
    res = at86rf231_change_mode(AT86RF231_TRX_STATE__RX_AACK_ON);
    if (res != NETDEV_802154_TX_STATUS_OK) {
        core_panic(0x0231,
                   "at86rf231 : (RX) ERROR : could not enter RX_AACK_ON mode");
    }
}

#ifndef MODULE_OPENWSN
void at86rf231_fberror_irq(void)
{
#ifdef RESET_TRANSCEIVER_ON_ERROR
    /* When frame buffer corruption occurs, reset the transceiver */
    printf("at86rf231: frame buffer corruption detected!");
    printf("Resetting transceiver\n");
    at86rf231_reset();
#else
    printf("at86rf231: WARNING! frame buffer corruption detected!\n");
#endif
#if 0
    /* When frame buffer corruption occurs, halt/reset the system */
    core_panic(0x0231,
               "at86rf231: frame buffer corruption detected!");
#endif
}

void at86rf233_sfd_irq(void)
{
    sfd_count++;
}

void at86rf231_irq(void)
{
    ASSERT_EXPECTED_RADIO_MODE;
    /* Check the kind of the received interrupt(s) */
    uint8_t irqStat = at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);
#if ENABLE_DEBUG
    DEBUG("at86rf231 IRQ!\n");
    DEBUG("Pending:\n");
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__BAT_LOW) {
        DEBUG("-> BAT_LOW\n");
    }
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__TRX_UR) {
        DEBUG("-> TRX_UR\n");
    }
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__AMI) {
        DEBUG("-> AMI\n");
    }
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__CCA_ED_DONE) {
        DEBUG("-> CCA_ED_DONE\n");
    }
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__TRX_END) {
        DEBUG("-> TRX_END\n");
    }
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__RX_START) {
        DEBUG("-> RX_START\n");
    }
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__PLL_UNLOCK) {
        DEBUG("-> PLL_UNLOCK\n");
    }
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__PLL_LOCK) {
        DEBUG("-> PLL_LOCK\n");
    }
    DEBUG("\n");
#endif

    /* Handle SFD recognitions */
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__RX_START) {
        at86rf233_sfd_irq();
    }

    /* Handle completed transmissions */
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__TRX_END) {
        /* check if we are in sending state */
        if (driver_state == AT_DRIVER_STATE_SENDING) {
            /* TX done, listen again */
            at86rf231_switch_to_rx();
            /* clear internal state */
            driver_state = AT_DRIVER_STATE_DEFAULT;
        } else {
            /* process received frame */
printf("RX\n");
            at86rf231_rx_handler();
        }
    }

    /* Handle Frame Buffer access violations */
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__TRX_UR) {
        at86rf231_fberror_irq();
    }

    /* Catch signal of transceiver activation / CCA completion */
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__CCA_ED_DONE) {
        at86rf231_cca_done = true;
        at86rf231_active = true;
    }

    /* Be aware of the PLL status */
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__PLL_UNLOCK) {
        DEBUG("**** WARNING! PLL UNLOCKED!!! ****\n");
        at86rf231_pll_locked = false;
    }
    if (irqStat & AT86RF231_IRQ_STATUS_MASK__PLL_LOCK) {
        DEBUG("** PLL lock!\n");
        at86rf231_pll_locked = true;
    }

    /* other interrupts don't interest us (and shouldn't be enabled) */
}
#endif /* MODULE_OPENWSN */


int at86rf231_add_raw_recv_callback(netdev_t *dev,
                                    netdev_802154_raw_packet_cb_t recv_cb)
{
    (void)dev;

    if (at86rf231_raw_packet_cb == NULL){
        at86rf231_raw_packet_cb = recv_cb;
        return 0;
    }

    return -ENOBUFS;
}

int at86rf231_rem_raw_recv_callback(netdev_t *dev,
                                    netdev_802154_raw_packet_cb_t recv_cb)
{
    (void)dev;

    at86rf231_raw_packet_cb = NULL;
    return 0;
}

int at86rf231_add_data_recv_callback(netdev_t *dev,
                                     netdev_rcv_data_cb_t recv_cb)
{
    (void)dev;

    if (at86rf231_data_packet_cb == NULL){
        at86rf231_data_packet_cb = recv_cb;
        return 0;
    }

    return -ENOBUFS;
}

int at86rf231_rem_data_recv_callback(netdev_t *dev,
                                     netdev_rcv_data_cb_t recv_cb)
{
    (void)dev;

    at86rf231_data_packet_cb = NULL;
    return 0;
}

radio_address_t at86rf231_set_address(radio_address_t address)
{
    radio_address = address;

    at86rf231_reg_write(AT86RF231_REG__SHORT_ADDR_0,
                        (uint8_t)(0x00FF & radio_address));
    at86rf231_reg_write(AT86RF231_REG__SHORT_ADDR_1,
                        (uint8_t)(radio_address >> 8));

    return radio_address;
}

radio_address_t at86rf231_get_address(void)
{
    return radio_address;
}

uint64_t at86rf231_set_address_long(uint64_t address)
{
    radio_address_long = address;

    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_0,
                        (uint8_t)(0x00000000000000FF & radio_address_long));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_1,
                        (uint8_t)(0x000000000000FF00 & radio_address_long >> 8));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_2,
                        (uint8_t)(0x0000000000FF0000 & radio_address_long >> 16));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_3,
                        (uint8_t)(0x00000000FF000000 & radio_address_long >> 24));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_4,
                        (uint8_t)(0x000000FF00000000 & radio_address_long >> 32));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_5,
                        (uint8_t)(0x0000FF0000000000 & radio_address_long >> 40));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_6,
                        (uint8_t)(0x00FF000000000000 & radio_address_long >> 48));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_7,
                        (uint8_t)(0xFF00000000000000 & radio_address_long >> 56));

    return radio_address_long;
}

uint64_t at86rf231_get_address_long(void)
{
    return radio_address_long;
}

uint16_t at86rf231_set_pan(uint16_t pan)
{
    radio_pan = pan;

    at86rf231_reg_write(AT86RF231_REG__PAN_ID_0,
                        (uint8_t)(0x00FF & radio_pan));
    at86rf231_reg_write(AT86RF231_REG__PAN_ID_1,
                        (uint8_t)(radio_pan >> 8));

    return radio_pan;
}

uint16_t at86rf231_get_pan(void)
{
    return radio_pan;
}

int at86rf231_set_channel(unsigned int channel)
{
    if (channel < AT86RF231_MIN_CHANNEL || channel > AT86RF231_MAX_CHANNEL) {
#if DEVELHELP
        puts("[at86rf231] channel out of range!");
#endif
        return -1;
    }

    radio_channel = channel;

    at86rf231_reg_write(AT86RF231_REG__PHY_CC_CCA,
                        (radio_channel & AT86RF231_PHY_CC_CCA_MASK__CHANNEL));

    return radio_channel;
}

unsigned int at86rf231_get_channel(void)
{
    return radio_channel;
}

void at86rf231_set_monitor(int mode)
{
    radio_monitor = mode;
    /* read register */
    uint8_t tmp = at86rf231_reg_read(AT86RF231_REG__XAH_CTRL_1);
    /* set promicuous mode depending on *mode* */
    if (mode) {
        at86rf231_reg_write(AT86RF231_REG__XAH_CTRL_1,
                            (tmp | AT86RF231_XAH_CTRL_1__AACK_PROM_MODE));
    }
    else {
        at86rf231_reg_write(AT86RF231_REG__XAH_CTRL_1,
                            (tmp & (~AT86RF231_XAH_CTRL_1__AACK_PROM_MODE)));
    }
}

int at86rf231_get_monitor(void)
{
    return radio_monitor;
}

static uint8_t DBM_TO_TX_POW[] = {
    0xF, 0xE, 0xE, 0xE, 0xE, 0xE, 0xD, 0xD, 0xD, 0xD, 0xC,
    0xC, 0xB, 0xB, 0xA, 0x9, 0x8, 0x7, 0x6, 0x5, 0x3, 0x0
};
int at86rf231_set_tx_power(int dbm)
{
    if (dbm < AT86RF231_TX_POWER_MIN) {
        dbm = AT86RF231_TX_POWER_MIN;
    }
    if (dbm < AT86RF231_TX_POWER_MAX) {
        dbm = AT86RF231_TX_POWER_MAX;
    }
    radio_tx_power = dbm;
    uint8_t pow = DBM_TO_TX_POW[dbm - AT86RF231_TX_POWER_MIN];
    at86rf231_reg_write(AT86RF231_REG__PHY_TX_PWR,
                        pow & AT86RF231_PHY_TX_PWR_MASK__TX_PWR);
    return radio_tx_power;
}

int at86rf231_get_tx_power(void)
{
    return radio_tx_power;
}

void at86rf231_reset_sfd_count(void)
{
    sfd_count = 0;
}

unsigned long at86rf231_get_sfd_count(void)
{
    return sfd_count;
}

int at86rf231_set_cca_threshold(int pow)
{
    if (pow < -91) {
        pow = -91;
    }
    if (pow > -61) {
        pow = -61;
    }
    radio_cca_threshold = pow;
    uint8_t val = (pow - AT86RF231_RSSI_BASE_VAL) >> 1;
    at86rf231_reg_write(AT86RF231_REG__CCA_THRES,
                        val & AT86RF231_CCA_THRES_MASK__CCA_ED_THRES);
    return radio_cca_threshold;
}

int at86rf231_get_cca_threshold(void)
{
    return radio_cca_threshold;
}


void at86rf231_gpio_spi_interrupts_init(void)
{
    /* SPI init */
    spi_acquire(AT86RF231_SPI);
    spi_init_master(AT86RF231_SPI, SPI_CONF_FIRST_RISING, SPI_SPEED);
    spi_release(AT86RF231_SPI);
    /* IRQ0 */
    gpio_init_int(AT86RF231_INT, GPIO_NOPULL, GPIO_RISING,
                  (gpio_cb_t)at86rf231_irq, NULL);
    /* CS */
    gpio_init_out(AT86RF231_CS, GPIO_NOPULL);
    /* SLEEP */
    gpio_init_out(AT86RF231_SLEEP, GPIO_NOPULL);
    /* RESET */
    gpio_init_out(AT86RF231_RESET, GPIO_NOPULL);
}

void at86rf231_reset(void)
{
    /* force reset */
    gpio_clear(AT86RF231_RESET);
    at86rf231_pll_locked = false;

    /* put pins to default values */
    gpio_set(AT86RF231_CS);
    gpio_clear(AT86RF231_SLEEP);

    /* disable transceiver-related interrupts */
    gpio_irq_disable(AT86RF231_INT);

    /* additional waiting (~60us) to comply to min rst pulse width */
    hwtimer_spin(2);
    gpio_set(AT86RF231_RESET);
    /* wait for SPI interface to be ready */
    hwtimer_wait(HWTIMER_TICKS(POWER_ON_DELAY_uS));

    /* interrupts configuration: we want to know about SFDs, TRX ends,
       CCAs, and frame buffer errors */
    at86rf231_reg_write(AT86RF231_REG__IRQ_MASK,
                        (AT86RF231_IRQ_STATUS_MASK__TRX_UR
                       | AT86RF231_IRQ_STATUS_MASK__CCA_ED_DONE
                       | AT86RF231_IRQ_STATUS_MASK__TRX_END
                       | AT86RF231_IRQ_STATUS_MASK__RX_START
                       | AT86RF231_IRQ_STATUS_MASK__PLL_UNLOCK
                       | AT86RF231_IRQ_STATUS_MASK__PLL_LOCK));
    /* Allows masked interrupts' "virtual status"
       to be read from IRQ_STATUS register */
    uint8_t reg = at86rf231_reg_read(AT86RF231_REG__TRX_CTRL_1);
    reg |= AT86RF231_TRX_CTRL_1_MASK__IRQ_MASK_MODE;
    at86rf231_reg_write(AT86RF231_REG__TRX_CTRL_1, reg);
    /* Read IRQ status register to clear it */
    at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);

    /* Send a FORCE TRX OFF command if transceiver still in P_ON state */
/*    netdev_802154_tx_status_t res =
           at86rf231_change_mode(AT86RF231_TRX_STATE__FORCE_TRX_OFF);
    if (res != NETDEV_802154_TX_STATUS_OK) {
        core_panic(0x0231,
                   "at86rf231 (RESET) : ERROR : could not enter TRX_OFF mode");
    }*/
    expected_mode = AT86RF231_TRX_STATUS__TRX_OFF;

    /* Start PLL to initialise CSMA seed by RNG */
    netdev_802154_tx_status_t res =
           at86rf231_change_mode(AT86RF231_TRX_STATE__PLL_ON);
    if (res != NETDEV_802154_TX_STATUS_OK) {
        core_panic(0x0231,
                   "at86rf231 (RESET) : ERROR : could not enter PLL_ON mode");
    }

    /* transceiver is now in an active state */
    at86rf231_active = true;

    /* read RNG values into CSMA_SEED_0 */
    for (int i=0; i<7; i+=2) {
        uint8_t tmp = at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA);
        tmp = ((tmp & 0x60) >> 5);
        at86rf231_reg_write(AT86RF231_REG__CSMA_SEED_0, tmp << i);
    }
    /* read CSMA_SEED_1 and write back with RNG value */
    uint8_t tmp = at86rf231_reg_read(AT86RF231_REG__CSMA_SEED_1);
    tmp |= ((at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA) & 0x60) >> 5);
    at86rf231_reg_write(AT86RF231_REG__CSMA_SEED_1, tmp);
    DEBUG("AT86RF231 transceiver is up and running.\n");

    /* change into reception mode */
    at86rf231_switch_to_rx();
}

int at86rf231_get_option(netdev_t *dev, netdev_opt_t opt, void *value,
                         size_t *value_len)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see at86rf231_init) remove when adapter for transceiver exists */
    if (dev != &at86rf231_netdev) {
        return -ENODEV;
    }

    switch (opt) {
    case NETDEV_OPT_CHANNEL:
        if (*value_len < sizeof(unsigned int)) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(unsigned int)) {
            *value_len = sizeof(unsigned int);
        }
        *((unsigned int *)value) = at86rf231_get_channel();
        break;

    case NETDEV_OPT_ADDRESS:
        if (*value_len < sizeof(uint16_t)) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(uint16_t)) {
            *value_len = sizeof(uint16_t);
        }
        *((uint16_t *)value) = at86rf231_get_address();
        break;

    case NETDEV_OPT_NID:
        if (*value_len < sizeof(uint16_t)) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(uint16_t)) {
            *value_len = sizeof(uint16_t);
        }
        *((uint16_t *)value) = at86rf231_get_pan();
        break;

    case NETDEV_OPT_ADDRESS_LONG:
        if (*value_len < sizeof(uint64_t)) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(uint64_t)) {
            *value_len = sizeof(uint64_t);
        }
        *((uint64_t *)value) = at86rf231_get_address_long();
        break;

    case NETDEV_OPT_MAX_PACKET_SIZE:
        if (*value_len == 0) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(uint8_t)) {
            *value_len = sizeof(uint8_t);
        }
        *((uint8_t *)value) = AT86RF231_MAX_PKT_LENGTH;
        break;

    case NETDEV_OPT_PROTO:
        if (*value_len < sizeof(netdev_proto_t)) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(netdev_proto_t)) {
            *value_len = sizeof(netdev_proto_t);
        }
        *((netdev_type_t *)value) = NETDEV_PROTO_802154;
        break;

    case NETDEV_OPT_SRC_LEN:
        if (*value_len < sizeof(size_t)) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(size_t)) {
            *value_len = sizeof(size_t);
        }
        *((size_t *)value) = _default_src_addr_len;

    case NETDEV_OPT_TX_POWER:
        if (*value_len < sizeof(int)) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(int)) {
            *value_len = sizeof(int);
        }
        *((int *)value) = at86rf231_get_tx_power();

    case NETDEV_OPT_CAN_MONITOR:
        if (*value_len == 0) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(uint8_t)) {
            *value_len = sizeof(uint8_t);
        }
        *((uint8_t *)value) = 1;   /* true : AT86RF23x can go promiscuous */
        break;

    case NETDEV_OPT_CCA_THRESHOLD:
        if (*value_len < sizeof(int)) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(int)) {
            *value_len = sizeof(int);
        }
        *((int *)value) = at86rf231_get_cca_threshold();

    case NETDEV_OPT_SFD_COUNT:
        if (*value_len < sizeof(unsigned long)) {
            return -EOVERFLOW;
        }
        if (*value_len > sizeof(unsigned long)) {
            *value_len = sizeof(unsigned long);
        }
        *((int *)value) = at86rf231_get_sfd_count();

    default:
        return -ENOTSUP;
    }

    return 0;
}

static int _type_pun_up_unsigned(void *value_out, size_t desired_len,
                                 void *value_in, size_t given_len)
{
    if (given_len > desired_len) {
        return -EOVERFLOW;
    }

    /* XXX this is ugly, but bear with me */
    switch (given_len) {
        case 8:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (*((uint64_t *)value_in));
                    return 0;
                default:
                    return -EINVAL;
            }

        case 4:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint32_t *)value_in));
                    return 0;
                case 4:
                    *((uint32_t *)value_out) = (*((uint32_t *)value_in));
                    return 0;
                default:
                    return -EINVAL;
            }

        case 2:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint16_t *)value_in));
                    return 0;
                case 4:
                    *((uint32_t *)value_out) = (uint32_t)(*((uint16_t *)value_in));
                    return 0;
                case 2:
                    *((uint16_t *)value_out) = (*((uint16_t *)value_in));
                    return 0;
                default:
                    return -EINVAL;
            }

        case 1:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint8_t *)value_in));
                    return 0;
                case 4:
                    *((uint32_t *)value_out) = (uint32_t)(*((uint8_t *)value_in));
                    return 0;
                case 2:
                    *((uint16_t *)value_out) = (uint16_t)(*((uint8_t *)value_in));
                    return 0;
                case 1:
                    *((uint8_t *)value_out) = (*((uint8_t *)value_in));
                    return 0;
                default:
                    return -EINVAL;
            }

        default:
            return -EINVAL;
    }
}

int at86rf231_set_option(netdev_t *dev, netdev_opt_t opt, void *value,
                         size_t value_len)
{
    uint8_t set_value[sizeof(uint64_t)];
    int res = 0;

    /* XXX: first check only for backwards compatibility with transceiver
     *      (see at86rf231_init) remove when adapter for transceiver exists */
    if (dev != &at86rf231_netdev) {
        return -ENODEV;
    }

    switch (opt) {
    case NETDEV_OPT_CHANNEL:
        if ((res = _type_pun_up_unsigned(set_value, sizeof(unsigned int),
                                         value, value_len)) == 0) {
            unsigned int *v = (unsigned int *)set_value;
            if (*v > 26) {
                return -EINVAL;
            }
            at86rf231_set_channel(*v);
        }
        break;

    case NETDEV_OPT_ADDRESS:
        if ((res = _type_pun_up_unsigned(set_value, sizeof(uint16_t),
                                         value, value_len)) == 0) {
            uint16_t *v = (uint16_t *)set_value;
            if (*v == 0xffff) {
                /* Do not allow setting to broadcast */
                return -EINVAL;
            }
            at86rf231_set_address(*v);
        }
        break;

    case NETDEV_OPT_NID:
        if ((res = _type_pun_up_unsigned(set_value, sizeof(uint16_t),
                                         value, value_len)) == 0) {
            uint16_t *v = (uint16_t *)set_value;
            if (*v == 0xffff) {
                /* Do not allow setting to broadcast */
                return -EINVAL;
            }
            at86rf231_set_pan(*v);
        }
        break;

    case NETDEV_OPT_ADDRESS_LONG:
        if ((res = _type_pun_up_unsigned(set_value, sizeof(uint64_t),
                                         value, value_len)) == 0) {
            uint64_t *v = (uint64_t *)set_value;
            /* TODO: error checking? */
            at86rf231_set_address_long(*v);
        }
        break;

    case NETDEV_OPT_SRC_LEN:
        if ((res = _type_pun_up_unsigned(set_value, sizeof(size_t),
                                         value, value_len)) == 0) {
            size_t *v = (size_t *)set_value;
            if (*v != 2 && *v != 8) {
                return -EINVAL;
            }
            _default_src_addr_len = *v;
        }
        break;

    case NETDEV_OPT_TX_POWER:
        if ((res = _type_pun_up_unsigned(set_value, sizeof(int),
                                         value, value_len)) == 0) {
            int *v = (int *)set_value;
            at86rf231_set_tx_power(*v);
        }
        break;

    case NETDEV_OPT_CCA_THRESHOLD:
        if ((res = _type_pun_up_unsigned(set_value, sizeof(int),
                                         value, value_len)) == 0) {
            int *v = (int *)set_value;
            at86rf231_set_cca_threshold(*v);
        }
        break;

    case NETDEV_OPT_SFD_COUNT:
        /* we don't care the given value... */
        at86rf231_reset_sfd_count();
        break;

    default:
        return -ENOTSUP;
    }

    return res;
}

int at86rf231_get_state(netdev_t *dev, netdev_state_t *state)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see at86rf231_init) remove when adapter for transceiver exists */
    if (dev != &at86rf231_netdev) {
        return -ENODEV;
    }

    if (!at86rf231_is_on()) {
        *state = NETDEV_STATE_POWER_OFF;
        return 0;
    }
    else if (at86rf231_get_monitor()) {
        *state = NETDEV_STATE_PROMISCUOUS_MODE;
        return 0;
    }
    else {
        *state = NETDEV_STATE_RX_MODE;
        return 0;
    }

    return -ENOTSUP;
}

int at86rf231_set_state(netdev_t *dev, netdev_state_t state)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see at86rf231_init) remove when adapter for transceiver exists */
    if (dev != &at86rf231_netdev) {
        return -ENODEV;
    }

    if (state != NETDEV_STATE_PROMISCUOUS_MODE && at86rf231_get_monitor()) {
        at86rf231_set_monitor(0);
    }

    switch (state) {
    case NETDEV_STATE_POWER_OFF:
        if (at86rf231_is_on()) {
            at86rf231_off();
        }
        break;

    case NETDEV_STATE_RX_MODE:
        if (!at86rf231_is_on()) {
            at86rf231_on();
        }
        at86rf231_set_monitor(0);
        at86rf231_switch_to_rx();
        break;

    case NETDEV_STATE_PROMISCUOUS_MODE:
        if (!at86rf231_is_on()) {
            at86rf231_on();
        }
        at86rf231_set_monitor(1);
        at86rf231_switch_to_rx();
        break;

    default:
        return -ENOTSUP;
    }

    return 0;
}

int at86rf231_channel_is_clear(netdev_t *dev)
{
    (void)dev;

    ASSERT_EXPECTED_RADIO_MODE;
    at86rf231_cca_done = false;
    /* Ask for a CCA measure */
    uint8_t reg = at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA);
    reg |= AT86RF231_PHY_CC_CCA_MASK__CCA_REQUEST;
    at86rf231_reg_write(AT86RF231_REG__PHY_CC_CCA, reg);

    /* wait for CCA measurement to be finished */
    hwtimer_wait(HWTIMER_TICKS(CCA_MEASURE_DELAY_uS));

    int countdown = _MAX_RETRIES;
    do {
        hwtimer_spin(1);
        if (--countdown <= 0) {
            at86rf231_print_status();
            printf("*** at86rf231 : ERROR : could not measure CCA\n");
#ifdef RESET_TRANSCEIVER_ON_ERROR
            printf("Resetting transceiver\n");
            at86rf231_reset();
#endif
            return false;
        }
        reg = at86rf231_reg_read(AT86RF231_REG__TRX_STATUS);
    } while (!(reg & AT86RF231_TRX_STATUS_MASK__CCA_DONE)
          && !(at86rf231_cca_done));

    /* return CCA status */
    return (reg & AT86RF231_TRX_STATUS_MASK__CCA_STATUS);
}

void at86rf231_event(netdev_t *dev, uint32_t event_type)
{
    (void)dev;
    (void)event_type;
}

const netdev_802154_driver_t at86rf231_driver = {
    .init = at86rf231_initialize,
    .send_data = netdev_802154_send_data,
    .add_receive_data_callback = at86rf231_add_data_recv_callback,
    .rem_receive_data_callback = at86rf231_rem_data_recv_callback,
    .get_option = at86rf231_get_option,
    .set_option = at86rf231_set_option,
    .get_state = at86rf231_get_state,
    .set_state = at86rf231_set_state,
    .event = at86rf231_event,
    .load_tx = at86rf231_load_tx_buf,
    .transmit = at86rf231_transmit_tx_buf,
    .send = netdev_802154_send,
    .add_receive_raw_callback = at86rf231_add_raw_recv_callback,
    .rem_receive_raw_callback = at86rf231_rem_raw_recv_callback,
    .channel_is_clear = at86rf231_channel_is_clear,
};

netdev_t at86rf231_netdev = {
    NETDEV_TYPE_802154,
    (netdev_driver_t *) &at86rf231_driver,
    NULL
};

