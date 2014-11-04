/*
 * Copyright (C) 2013 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x_legacy
 * @{
 * @file        cc110x.c
 * @brief       Basic functionality of cc110x driver
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */
#include "cc110x_legacy.h"
#include "cc110x-internal.h"

#include "hwtimer.h"
#include "config.h"
#include "cpu.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* some externals */
extern uint8_t pa_table[];              ///< PATABLE with available output powers
extern uint8_t pa_table_index;          ///< Current PATABLE Index

/* global variables */

cc110x_statistic_t cc110x_statistic;

volatile cc110x_flags rflags;                       ///< Radio control flags
volatile uint8_t radio_state = RADIO_UNKNOWN;       ///< Radio state

static radio_address_t radio_address;                     ///< Radio address
static uint8_t radio_channel;                             ///< Radio channel

/* internal function prototypes */
static int rd_set_mode(int mode);
static void reset(void);
static void power_up_reset(void);
static void write_register(uint8_t r, uint8_t value);

/*---------------------------------------------------------------------------*
 *                              Radio Driver API                             *
 *---------------------------------------------------------------------------*/
void cc110x_init(kernel_pid_t tpid)
{
    transceiver_pid = tpid;
    DEBUG("Transceiver PID: %" PRIkernel_pid "\n", transceiver_pid);

    rx_buffer_next = 0;

#ifdef MODULE_CC110X_SPI
    /* Initialize SPI */
    cc110x_spi_init();
#endif

    /* Load driver & reset */
    power_up_reset();

    /* Write configuration to configuration registers */
    cc110x_writeburst_reg(0x00, cc110x_conf, CC1100_CONF_SIZE);

    /* Write PATABLE (power settings) */
    cc110x_write_reg(CC1100_PATABLE, pa_table[pa_table_index]);

    /* Initialize Radio Flags */
    rflags._RSSI         = 0x00;
    rflags.LL_ACK       = 0;
    rflags.CAA          = 0;
    rflags.CRC_STATE    = 0;
    rflags.SEQ          = 0;
    rflags.MAN_WOR      = 0;
    rflags.KT_RES_ERR   = 0;
    rflags.TX           = 0;
    rflags.WOR_RST      = 0;

    /* Set default channel number */
#ifdef MODULE_CONFIG
    cc110x_set_config_channel(sysconfig.radio_channel);
#else
    cc110x_set_channel(CC1100_DEFAULT_CHANNR);
#endif
    DEBUG("CC1100 initialized and set to channel %i\n", radio_channel);

    /* Switch to desired mode (WOR or RX) */
    rd_set_mode(RADIO_MODE_ON);

#ifdef DBG_IGNORE
    cc110x_init_ignore();
#endif
}

void cc110x_disable_interrupts(void)
{
    cc110x_gdo2_disable();
    cc110x_gdo0_disable();
}

void cc110x_gdo0_irq(void)
{
    /* Air was not free -> Clear CCA flag */
    rflags.CAA = false;
    /* Disable carrier sense detection (GDO0 interrupt) */
    cc110x_gdo0_disable();
}

void cc110x_gdo2_irq(void)
{
    cc110x_rx_handler();
}

uint8_t cc110x_get_buffer_pos(void)
{
    return (rx_buffer_next - 1);
}

radio_address_t cc110x_get_address(void)
{
    return radio_address;
}

radio_address_t cc110x_set_address(radio_address_t address)
{
    if ((address < MIN_UID) || (address > MAX_UID)) {
        return 0;
    }

    uint8_t id = (uint8_t) address;

    if (radio_state != RADIO_UNKNOWN) {
        write_register(CC1100_ADDR, id);
    }

    radio_address = id;
    return radio_address;
}

#ifdef MODULE_CONFIG
radio_address_t cc110x_set_config_address(radio_address_t address)
{
    radio_address_t a = cc110x_set_address(address);

    if (a) {
        sysconfig.radio_address = a;
    }

    config_save();
    return a;
}
#endif

void cc110x_set_monitor(uint8_t mode)
{
    if (mode) {
        write_register(CC1100_PKTCTRL1, (0x04));
    }
    else {
        write_register(CC1100_PKTCTRL1, (0x06));
    }
}

void cc110x_setup_rx_mode(void)
{
    /* Stay in RX mode until end of packet */
    cc110x_write_reg(CC1100_MCSM2, 0x07);
    cc110x_switch_to_rx();
}

void cc110x_switch_to_rx(void)
{
    radio_state = RADIO_RX;
    cc110x_strobe(CC1100_SRX);
}

void cc110x_wakeup_from_rx(void)
{
    if (radio_state != RADIO_RX) {
        return;
    }

    DEBUG("CC1100 going to idle\n");
    cc110x_strobe(CC1100_SIDLE);
    radio_state = RADIO_IDLE;
}

char *cc110x_get_marc_state(void)
{
    uint8_t state;

    /* Save old radio state */
    uint8_t old_state = radio_state;

    /* Read content of status register */
    state = cc110x_read_status(CC1100_MARCSTATE) & MARC_STATE;

    /* Make sure in IDLE state.
     * Only goes to IDLE if state was RX/WOR */
    cc110x_wakeup_from_rx();

    /* Have to put radio back to WOR/RX if old radio state
     * was WOR/RX, otherwise no action is necessary */
    if (old_state == RADIO_WOR || old_state == RADIO_RX) {
        cc110x_switch_to_rx();
    }

    switch(state) {
            /* Note: it is not possible to read back the SLEEP or XOFF state numbers
             * because setting CSn low will make the chip enter the IDLE mode from the
             * SLEEP (0) or XOFF (2) states. */
        case 1:
            return "IDLE";

        case 3:
        case 4:
        case 5:
            return "MANCAL";

        case 6:
        case 7:
            return "FS_WAKEUP";

        case 8:
        case 12:
            return "CALIBRATE";

        case 9:
        case 10:
        case 11:
            return "SETTLING";

        case 13:
        case 14:
        case 15:
            return "RX";

        case 16:
            return "TXRX_SETTLING";

        case 17:
            return "RXFIFO_OVERFLOW";

        case 18:
            return "FSTXON";

        case 19:
        case 20:
            return "TX";

        case 21:
            return "RXTX_SETTLING";

        case 22:
            return "TXFIFO_UNDERFLOW";

        default:
            return "UNKNOWN";
    }
}

char *cc110x_state_to_text(uint8_t state)
{
    switch(state) {
        case RADIO_UNKNOWN:
            return "Unknown";

        case RADIO_AIR_FREE_WAITING:
            return "CS";

        case RADIO_WOR:
            return "WOR";

        case RADIO_IDLE:
            return "IDLE";

        case RADIO_SEND_BURST:
            return "TX BURST";

        case RADIO_RX:
            return "RX";

        case RADIO_SEND_ACK:
            return "TX ACK";

        case RADIO_PWD:
            return "PWD";

        default:
            return "unknown";
    }
}

void cc110x_print_config(void)
{
    printf("Current radio state:          %s\r\n", cc110x_state_to_text(radio_state));
    printf("Current MARC state:           %s\r\n", cc110x_get_marc_state());
    printf("Current channel number:       %u\r\n", radio_channel);
}

void cc110x_switch_to_pwd(void)
{
    DEBUG("[cc110x_legacy] switching to powerdown\n");
    cc110x_wakeup_from_rx();
    cc110x_strobe(CC1100_SPWD);
    radio_state = RADIO_PWD;
}

/*---------------------------------------------------------------------------*/
int16_t cc110x_set_channel(uint8_t channr)
{
    uint8_t state = cc110x_read_status(CC1100_MARCSTATE) & MARC_STATE;

    if ((state != 1) && (channr > MAX_CHANNR)) {
        return -1;
    }

    write_register(CC1100_CHANNR, channr * 10);
    radio_channel = channr;
    return radio_channel;
}

#ifdef MODULE_CONFIG
int16_t cc110x_set_config_channel(uint8_t channr)
{
    int16_t c = cc110x_set_channel(channr);

    if (c) {
        sysconfig.radio_channel = c;
    }

    config_save();
    return c;
}
#endif

int16_t cc110x_get_channel(void)
{
    return radio_channel;
}


/*---------------------------------------------------------------------------
 *                          CC1100 reset functionality
 *---------------------------------------------------------------------------*/

static void reset(void)
{
    cc110x_wakeup_from_rx();
#ifdef MODULE_CC110x_SPI
    cc110x_spi_select();
#endif
    cc110x_strobe(CC1100_SRES);
    hwtimer_wait(RTIMER_TICKS(100));
}

static void power_up_reset(void)
{
#ifdef MODULE_CC110x_SPI
    cc110x_spi_unselect();
    cc110x_spi_cs();
    cc110x_spi_unselect();
#endif
    hwtimer_wait(RESET_WAIT_TIME);
    reset();
    radio_state = RADIO_IDLE;
}

static void write_register(uint8_t r, uint8_t value)
{
    /* Save old radio state */
    uint8_t old_state = radio_state;

    /* Wake up from WOR/RX (if in WOR/RX, else no effect) */
    cc110x_wakeup_from_rx();
    cc110x_write_reg(r, value);

    /* Have to put radio back to WOR/RX if old radio state
     * was WOR/RX, otherwise no action is necessary */
    if ((old_state == RADIO_WOR) || (old_state == RADIO_RX)) {
        cc110x_switch_to_rx();
    }
}

static int rd_set_mode(int mode)
{
    int result;

    /* Get current radio mode */
    if ((radio_state == RADIO_UNKNOWN) || (radio_state == RADIO_PWD)) {
        result = RADIO_MODE_OFF;
    }
    else {
        result = RADIO_MODE_ON;
    }

    switch(mode) {
        case RADIO_MODE_ON:
            DEBUG("Enabling rx mode\n");
            cc110x_init_interrupts();           /* Enable interrupts */
            cc110x_setup_rx_mode();             /* Set chip to desired mode */
            break;

        case RADIO_MODE_OFF:
            cc110x_disable_interrupts();        /* Disable interrupts */
            cc110x_switch_to_pwd();                 /* Set chip to power down mode */
            break;

        case RADIO_MODE_GET:

            /* do nothing, just return current mode */
        default:
            /* do nothing */
            break;
    }

    /* Return previous mode */
    return result;
}
