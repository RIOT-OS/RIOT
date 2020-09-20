/*
 * Copyright (C) 2016 MUTEX NZ Ltd.
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file
 * @brief       Low-level radio driver for the CC2538
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 * @author      Ian Martin <ian@locicontrols.com>
 * @}
 */

#include "periph_conf.h"

#include "cc2538_rf.h"
#include "cc2538_rf_netdev.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/*
 * @brief MAC timer period
 *
 * The period is set to the CSMA-CA Backoff Period Unit (20 symbols, 320 us).
 * The system clock runs at 32 MHz. Thus, the timeout period is
 * 320us * 32MHz = ~10738 (0x29F2)
 */
#define TIMER_PERIOD_LSB           (0xF2)
#define TIMER_PERIOD_MSB           (0x29)

typedef struct {
    cc2538_reg_t *reg_addr;
    uint32_t value;
} init_pair_t;

static const init_pair_t init_table[] = {
    {&SYS_CTRL_RCGCRFC,      0x01                     },
    {&SYS_CTRL_SCGCRFC,      0x01                     },
    {&SYS_CTRL_DCGCRFC,      0x01                     },
    {&RFCORE_XREG_CCACTRL0,  0xf8                     },
    {&RFCORE_XREG_TXFILTCFG, 0x09                     },
    {&RFCORE_XREG_AGCCTRL1,  0x15                     },
    {&ANA_REGS_IVCTRL,       0x0b                     },
    {&RFCORE_XREG_MDMTEST1,  0x08                     },
    {&RFCORE_XREG_FSCAL1,    0x01                     },
    {&RFCORE_XREG_RXCTRL,    0x3f                     },
    {&RFCORE_XREG_MDMCTRL1,  0x14                     },
    {&RFCORE_XREG_ADCTEST0,  0x10                     },
    {&RFCORE_XREG_ADCTEST1,  0x0e                     },
    {&RFCORE_XREG_ADCTEST2,  0x03                     },
    {&RFCORE_XREG_CSPT,      0xff                     },
    {&RFCORE_XREG_MDMCTRL0,  0x85                     },
    {&RFCORE_XREG_FSCTRL,    0x55                     },
    {&RFCORE_XREG_FRMCTRL0,  AUTOCRC | AUTOACK        },
    {&RFCORE_XREG_FRMCTRL1,  0x00                     },
    {&RFCORE_XREG_SRCMATCH,  0x00                     },
    {&RFCORE_XREG_FIFOPCTRL, CC2538_RF_MAX_DATA_LEN   },
    {&RFCORE_XREG_RFIRQM0,   FIFOP | RXPKTDONE        },
#if IS_USED(MODULE_IEEE802154_RADIO_HAL)
    {&RFCORE_XREG_RFIRQM1,   TXDONE | CSP_STOP        },
#endif
    {&RFCORE_XREG_RFERRM,    STROBE_ERR | TXUNDERF | TXOVERF | RXUNDERF | RXOVERF | NLOCK},
    {NULL, 0},
};

bool cc2538_channel_clear(void)
{
    if (RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE == FSM_STATE_IDLE) {
        bool result;
        cc2538_on();
        RFCORE_WAIT_UNTIL(RFCORE->XREG_RSSISTATbits.RSSI_VALID);
        result = BOOLEAN(RFCORE->XREG_FSMSTAT1bits.CCA);
        cc2538_off();
        return result;
    }
    else {
        RFCORE_WAIT_UNTIL(RFCORE->XREG_RSSISTATbits.RSSI_VALID);
        return BOOLEAN(RFCORE->XREG_FSMSTAT1bits.CCA);
    }
}

void cc2538_init(void)
{
    const init_pair_t *pair;

    for (pair = init_table; pair->reg_addr != NULL; pair++) {
        *pair->reg_addr = pair->value;
    }

    /* Select the observable signals (maximum of three) */
    RFCORE_XREG_RFC_OBS_CTRL0 = tx_active;
    RFCORE_XREG_RFC_OBS_CTRL1 = rx_active;
    RFCORE_XREG_RFC_OBS_CTRL2 = ffctrl_fifo;

    /* Select output pins for the three observable signals */
#ifdef BOARD_OPENMOTE_CC2538
    CCTEST_OBSSEL0 = 0;                        /* PC0 = USB_SEL        */
    CCTEST_OBSSEL1 = 0;                        /* PC1 = N/C            */
    CCTEST_OBSSEL2 = 0;                        /* PC2 = N/C            */
    CCTEST_OBSSEL3 = 0;                        /* PC3 = USER_BUTTON    */
    CCTEST_OBSSEL4 = OBSSEL_EN | rfc_obs_sig0; /* PC4 = RED_LED        */
    CCTEST_OBSSEL5 = 0;                        /* PC5 = ORANGE_LED     */
    CCTEST_OBSSEL6 = OBSSEL_EN | rfc_obs_sig1; /* PC6 = YELLOW_LED     */
    CCTEST_OBSSEL7 = OBSSEL_EN | rfc_obs_sig2; /* PC7 = GREEN_LED      */
#else
    /* Assume BOARD_CC2538DK (or similar). */
    CCTEST_OBSSEL0 = OBSSEL_EN | rfc_obs_sig0; /* PC0 = LED_1 (red)    */
    CCTEST_OBSSEL1 = OBSSEL_EN | rfc_obs_sig1; /* PC1 = LED_2 (yellow) */
    CCTEST_OBSSEL2 = OBSSEL_EN | rfc_obs_sig2; /* PC2 = LED_3 (green)  */
    CCTEST_OBSSEL3 = 0;                        /* PC3 = LED_4 (red)    */
    CCTEST_OBSSEL4 = 0;                        /* PC4 = BTN_L          */
    CCTEST_OBSSEL5 = 0;                        /* PC5 = BTN_R          */
    CCTEST_OBSSEL6 = 0;                        /* PC6 = BTN_UP         */
    CCTEST_OBSSEL7 = 0;                        /* PC7 = BTN_DN         */
#endif /* BOARD_OPENMOTE_CC2538 */

    if (SYS_CTRL->I_MAP) {
        NVIC_SetPriority(RF_RXTX_ALT_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(RF_RXTX_ALT_IRQn);

        NVIC_SetPriority(RF_ERR_ALT_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(RF_ERR_ALT_IRQn);

        NVIC_SetPriority(MAC_TIMER_ALT_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(MAC_TIMER_ALT_IRQn);
    }
    else {
        NVIC_SetPriority(RF_RXTX_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(RF_RXTX_IRQn);

        NVIC_SetPriority(RF_ERR_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(RF_ERR_IRQn);

        NVIC_SetPriority(MACTIMER_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(MACTIMER_IRQn);
    }

    RFCORE_SFR_MTMSEL &= ~CC2538_SFR_MTMSEL_MASK;
    /* Select timer period */
    RFCORE_SFR_MTMSEL |= CC2538_SFR_MTMSEL_TIMER_P;

    /* Fix timer to Backoff period */
    RFCORE_SFR_MTM0 |= TIMER_PERIOD_LSB;
    RFCORE_SFR_MTM1 |= TIMER_PERIOD_MSB;

    RFCORE_SFR_MTMSEL &= ~CC2538_SFR_MTMSEL_MASK;
    RFCORE_SFR_MTCTRL |= CC2538_MCTRL_SYNC_MASK | CC2538_MCTRL_RUN_MASK;

    /* Flush the receive and transmit FIFOs */
    RFCORE_SFR_RFST = ISFLUSHTX;
    RFCORE_SFR_RFST = ISFLUSHRX;
}

bool cc2538_is_on(void)
{
    return RFCORE->XREG_FSMSTAT1bits.RX_ACTIVE || RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE;
}

void cc2538_off(void)
{
    /* Wait for ongoing TX to complete (e.g. this could be an outgoing ACK) */
    RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE == 0);

    /* Flush RX FIFO */
    RFCORE_SFR_RFST = ISFLUSHRX;

    /* Don't turn off if we are off as this will trigger a Strobe Error */
    if (RFCORE_XREG_RXENABLE != 0) {
        RFCORE_SFR_RFST = ISRFOFF;
    }
}

bool cc2538_on(void)
{
    /* Flush RX FIFO */
    RFCORE_SFR_RFST = ISFLUSHRX;

    /* Enable/calibrate RX */
    RFCORE_SFR_RFST = ISRXON;

    return true;
}

void cc2538_setup(cc2538_rf_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &cc2538_rf_driver;

    cc2538_init();

    netdev_register(netdev, NETDEV_CC2538, 0);

    cc2538_set_tx_power(CC2538_RF_POWER_DEFAULT);
    cc2538_set_chan(CC2538_RF_CHANNEL_DEFAULT);

    /* assign default addresses */
    netdev_ieee802154_setup(&dev->netdev);
    cc2538_set_addr_long(dev->netdev.long_addr);
    cc2538_set_addr_short(dev->netdev.short_addr);

    cc2538_on();
}
