/**
 * TI Chipcon CC110x radio driver
 *
 * Copyright (C) 2009-2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 */

/**
 * @ingroup		dev_cc110x
 * @{
 */

/**
 * @file
 * @internal
 * @brief		TI Chipcon CC110x Radio driver
 *
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author		Heiko Will <hwill@inf.fu-berlin.de>
 * @version     $Revision: 2283 $
 *
 * @note		$Id: cc1100.c 2283 2010-06-15 14:02:27Z hillebra $
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "irq.h"
#include "arch_cc1100.h"
#include "cc1100.h"
#include "cc1100_phy.h"
#include "cc1100_spi.h"
#include "cc1100-internal.h"
#include "cc1100-defaultSettings.h"

#include "hwtimer.h"
#include "bitarithm.h"

/* TODO: cc1100 port timer */
#ifdef FEUERWARE_CPU_LPC2387
//#include "cpu/lpc2387/lpc2387-timer2.h"
#endif

#if defined(FEUERWARE_CPU_MSP430)
#include <msp430x16x.h>
#elif defined(FEUERWARE_CPU_LPC2387)
#include "lpc2387.h"
#endif

#define PACKET_LENGTH				(0x3E)		///< Packet length = 62 Bytes.
#define CC1100_SYNC_WORD_TX_TIME   (90000)		/* loop count (max. timeout ~ 15 ms) to wait for */
												/* sync word to be transmitted (GDO2 from low to high) */

/**
 * @name	Virtual Radio Device methods (see vdevice_radio_methods)
 * @{
 */
static int rd_set_mode(int mode);
/** @} */

static void switch_to_wor(void);

/*---------------------------------------------------------------------------*/
//					Power control data structures
/*---------------------------------------------------------------------------*/

static uint8_t pa_table_index = PATABLE;	///< Current PATABLE Index
static uint8_t pa_table[] = {				///< PATABLE with available output powers
				   0x00,					///< -52 dBm
				   0x03,					///< -30 dBm
				   0x0D,					///< -20 dBm
				   0x1C,					///< -15 dBm
				   0x34,					///< -10 dBm
				   0x57,					///< - 5 dBm
				   0x3F,					///< - 1 dBm
				   0x8E,					///<   0 dBm
				   0x85,					///< + 5 dBm
				   0xCC,					///< + 7 dBm
				   0xC6, 					///< + 9 dBm
				   0xC3  					///< +10 dBm
}; /* If PATABLE is changed in size, adjust MAX_OUTPUT_POWER definition in CC1100 interface!*/

static int8_t pa_table_dBm[] = {			///< Values of the PATABLE in dBm
				   -52,
				   -30,
				   -20,
				   -15,
				   -10,
				    -5,
				    -1,
				     0,
				     5,
				     7,
				     9,
				    10
};

/*---------------------------------------------------------------------------*/
//					Main radio data structures
/*---------------------------------------------------------------------------*/

volatile cc1100_flags rflags;		///< Radio control flags
static uint8_t radio_address;		///< Radio address
static uint8_t radio_channel;		///< Radio channel number

const radio_t radio_cc1100 = {		///< Radio driver API
    "CC1100",
    CC1100_BROADCAST_ADDRESS,
    MAX_OUTPUT_POWER,
    cc1100_get_avg_transmission_duration,
    cc1100_get_address,
    cc1100_set_address,
    cc1100_set_output_power,
    cc1100_set_packet_monitor,
    cc1100_set_packet_handler,
    cc1100_send_csmaca,
    cc1100_print_statistic,
    cc1100_print_config
};

/*---------------------------------------------------------------------------*/
//					 Data structures for mode control
/*---------------------------------------------------------------------------*/

volatile uint8_t radio_mode;						///< Radio mode
volatile uint8_t radio_state = RADIO_UNKNOWN;		///< Radio state

volatile cc1100_mode_callback_t cc1100_go_idle;		///< Function for going IDLE
volatile cc1100_mode_callback_t cc1100_go_receive;	///< Function for going RX
volatile cc1100_mode_callback_t cc1100_go_after_tx;	///< Function to call after TX (burst send)
volatile cc1100_mode_callback_t cc1100_setup_mode;	///< Function to set up selected mode (RX or WOR)

volatile int wor_hwtimer_id = -1;

/*---------------------------------------------------------------------------*/
/* 						Low-level hardware access */
/*---------------------------------------------------------------------------*/

void cc1100_disable_interrupts(void)
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
    cc1100_phy_rx_handler();
}

/*---------------------------------------------------------------------------*/
/* 		High level CC1100 SPI functions for transferring packet out */
//		of RX FIFO (don't call when in WOR mode)
/*---------------------------------------------------------------------------*/

static bool spi_receive_packet_variable(uint8_t *rxBuffer, uint8_t length)
{
    /* Needed here for statistics */
    extern cc1100_statistic_t cc1100_statistic;

    uint8_t status[2];
    uint8_t packetLength = 0;

    /* Any bytes available in RX FIFO? */
    if ((cc1100_spi_read_status(CC1100_RXBYTES) & BYTES_IN_RXFIFO)) {
        /* Read length byte (first byte in RX FIFO) */
        packetLength = cc1100_spi_read_reg(CC1100_RXFIFO);

        /* Read data from RX FIFO and store in rxBuffer */
        if (packetLength <= length) {
            /* Put length byte at first position in RX Buffer */
            rxBuffer[0] = packetLength;

            /* Read the rest of the packet */
            cc1100_spi_readburst_reg(CC1100_RXFIFO, (char *)rxBuffer + 1, packetLength);

            /* Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI) */
            cc1100_spi_readburst_reg(CC1100_RXFIFO, (char *)status, 2);

            /* Store RSSI value of packet */
            rflags.RSSI = status[I_RSSI];

            /* MSB of LQI is the CRC_OK bit */
            rflags.CRC_STATE = (status[I_LQI] & CRC_OK) >> 7;

            if (!rflags.CRC_STATE) {
                cc1100_statistic.packets_in_crc_fail++;
            }

            /* Bit 0-6 of LQI indicates the link quality (LQI) */
            rflags.LQI = status[I_LQI] & LQI_EST;

            return rflags.CRC_STATE;
        }
        else {
            /* RX FIFO get automatically flushed if return value is false */
            return false;
        }
    }
    else {
        /* RX FIFO get automatically flushed if return value is false */
        return false;
    }
}

bool cc1100_spi_receive_packet(uint8_t *rxBuffer, uint8_t length)
{
    uint8_t pkt_len_cfg = cc1100_spi_read_reg(CC1100_PKTCTRL0) & PKT_LENGTH_CONFIG;

    if (pkt_len_cfg == VARIABLE_PKTLEN) {
        return spi_receive_packet_variable(rxBuffer, length);
    }

    /* Fixed packet length not supported. */
    /* RX FIFO get automatically flushed if return value is false */
    return false;
}

/*---------------------------------------------------------------------------*/
/* 							CC1100 mode functionality */
/*---------------------------------------------------------------------------*/

void cc1100_set_idle(void)
{
    if (radio_state == RADIO_WOR) {
        /* Wake up the chip from WOR/sleep */
        cc110x_spi_select();
        hwtimer_wait(RTIMER_TICKS(122));
        cc110x_spi_unselect();
        radio_state = RADIO_IDLE;
        /* XOSC startup + FS calibration (300 + 809 us  ~ 1.38 ms) */
        hwtimer_wait(FS_CAL_TIME);
        return;
    }

    cc1100_spi_strobe(CC1100_SIDLE);
    radio_state = RADIO_IDLE;
}

static void wakeup_from_rx(void)
{
    if (radio_state != RADIO_RX) {
        return;
    }

    cc1100_spi_strobe(CC1100_SIDLE);
    radio_state = RADIO_IDLE;
}

static void switch_to_rx(void)
{
    radio_state = RADIO_RX;
    cc1100_spi_strobe(CC1100_SRX);
}

static void setup_rx_mode(void)
{
    /* Stay in RX mode until end of packet */
    cc1100_spi_write_reg(CC1100_MCSM2, 0x07);
    switch_to_rx();
}

/**
 * Note: It is possible that this code is executed in an ISR!
 */
static void wakeup_from_wor(void)
{
    if (radio_state != RADIO_WOR) {
        return;
    }

    /* Wake up the chip from WOR/sleep */
    cc110x_spi_select();
    hwtimer_wait(RTIMER_TICKS(122));
    cc110x_spi_unselect();
    radio_state = RADIO_IDLE;
    /* XOSC startup + FS calibration (300 + 809 us  ~ 1.38 ms) */
    hwtimer_wait(FS_CAL_TIME);
}

/**
 * Note: This code is executed in the hwtimer ISR!
 */
void switch_to_wor2(void)
{
    //	if (cc110x_get_gdo2()) return;				/* If incoming packet, then don't go to WOR now */
    cc1100_spi_strobe(CC1100_SIDLE);			/* Put CC1100 to IDLE */
    radio_state = RADIO_IDLE;					/* Radio state now IDLE */
    cc1100_spi_write_reg(CC1100_MCSM2,
                         cc1100_wor_config.rx_time_reg);		/* Configure RX_TIME (for use in WOR) */
    cc1100_spi_write_reg(CC1100_MCSM0, 0x18);	/* Turn on FS-Autocal */

    if (rflags.WOR_RST) {
        cc1100_spi_strobe(CC1100_SWORRST);		/* Resets the real time clock */
        rflags.WOR_RST = false;
    }

    cc1100_spi_strobe(CC1100_SWOR);				/* Put radio back to sleep/WOR (must be in IDLE when this is done) */
    radio_state = RADIO_WOR;					/* Radio state now WOR */
}

/**
 * Note: This code is executed in the hwtimer ISR!
 */
static void hwtimer_switch_to_wor2_wrapper(void *ptr)
{
    (void) ptr;
    wor_hwtimer_id = -1;							/* kernel timer handler function called, clear timer id */

    if (rflags.TX) {
        return;    /* Stability: don't allow WOR timers at this point */
    }

    rflags.WOR_RST = true;
    switch_to_wor2();
}

/**
 * Note: This code is executed in the hwtimer ISR!
 */
static void switch_to_wor(void)
{
    /* Any incoming packet? */
    if (cc110x_get_gdo2()) {
        /* Then don't go to WOR now */
        return;
    }

    /* Step 1: Set chip for random interval (1..RX_INTERVAL) to power down mode */
    if (!rflags.MAN_WOR) {
        rflags.MAN_WOR = true;
        radio_state = RADIO_WOR;
        /* Go to power down mode */
        cc1100_spi_strobe(CC1100_SIDLE);
        cc1100_spi_strobe(CC1100_SPWD);

        /* Set timer to do second step of manual WOR */
        int r = (rand() / (double)(RAND_MAX + 1.0)) * (cc1100_wor_config.rx_interval * 100.0) + 20;
        wor_hwtimer_id = hwtimer_set(r, cc1100_hwtimer_go_receive_wrapper, NULL);

        if (wor_hwtimer_id == -1) {
            rflags.KT_RES_ERR = true;
            /* No hwtimer available, go immediately to WOR mode. */
            /* Else never receiving packets again... */
            rflags.MAN_WOR = false;
            switch_to_wor2();
        }
    }
    /* Step 2: Go to RX and then to WOR mode again */
    else {
        rflags.MAN_WOR = false;
        wakeup_from_wor();
        cc1100_spi_strobe(CC1100_SRX);
        hwtimer_wait(IDLE_TO_RX_TIME);
        radio_state = RADIO_RX;
        /* Register timer to go to WOR after RX timeout */
        wor_hwtimer_id = hwtimer_set((cc1100_wor_config.rx_time_ms * 100 + 150),
                                     hwtimer_switch_to_wor2_wrapper, NULL); /* add 1,5 ms secure time */

        if (wor_hwtimer_id == -1) {
            rflags.KT_RES_ERR = true;
        }
    }
}

static void setup_wor_mode(void)
{
    /* Wake up from WOR (if in WOR, else no effect) */
    cc1100_go_idle();

    /* Make sure CC1100 is in IDLE state */
    cc1100_spi_strobe(CC1100_SIDLE);

    /* Enable automatic initial calibration of RCosc. */
    /* Set T_event1 ~ 1.4 ms, enough for XOSC stabilize and FS calibration before RX. */
    /* Enable RC oscillator before starting with WOR (or else it will not wake up). */
    /* Not using AUTO_SYNC function. */
    cc1100_spi_write_reg(CC1100_WORCTRL, cc1100_wor_config.wor_ctrl);

    /* Set Event0 timeout (RX polling interval) */
    cc1100_spi_write_reg(CC1100_WOREVT1, cc1100_wor_config.wor_evt_1);
    cc1100_spi_write_reg(CC1100_WOREVT0, cc1100_wor_config.wor_evt_0);

    /* Set RX time in WOR mode */
    cc1100_spi_write_reg(CC1100_MCSM2, cc1100_wor_config.rx_time_reg);

    /* Enable automatic FS calibration when going from IDLE to RX/TX/FSTXON (in between EVENT0 and EVENT1) */
    cc1100_spi_write_reg(CC1100_MCSM0, 0x18);

    /* Put the radio to SLEEP by starting Wake-on-Radio. */
    cc1100_spi_strobe(CC1100_SWORRST);	/* Resets the real time clock */
    cc1100_spi_strobe(CC1100_SWOR);		/* Starts Wake-on-Radio */
    radio_state = RADIO_WOR;
}

static void switch_to_pwd(void)
{
    cc1100_go_idle();
    cc1100_spi_strobe(CC1100_SPWD);
    radio_state = RADIO_PWD;
}

uint8_t cc1100_get_mode(void)
{
    return radio_mode;
}

static bool cc1100_set_mode0(uint8_t mode, uint16_t opt_mode_data)
{
    int result;

    switch(mode) {
        case CC1100_MODE_WOR:
            /* Calculate WOR settings, store result (new burst count) */
            result = cc1100_phy_calc_wor_settings(opt_mode_data);

            /* If settings can be applied, set new mode and burst count */
            if (result != -1) {
                radio_mode = mode;
                cc1100_go_idle = wakeup_from_wor;
                cc1100_go_receive = switch_to_wor;
                cc1100_go_after_tx = switch_to_wor2;
                cc1100_setup_mode = setup_wor_mode;
                cc1100_burst_count = result;
                cc1100_retransmission_count_uc = TRANSMISSION_RETRIES_WOR_UC;
                cc1100_retransmission_count_bc = TRANSMISSION_RETRIES_WOR_BC;
                return true;
            }

            break;

        case CC1100_MODE_CONSTANT_RX:
            radio_mode = mode;
            cc1100_go_idle = wakeup_from_rx;
            cc1100_go_receive = switch_to_rx;
            cc1100_go_after_tx = switch_to_rx;
            cc1100_setup_mode = setup_rx_mode;
            cc1100_burst_count = 1;
            cc1100_retransmission_count_uc = TRANSMISSION_RETRIES_CRX_UC;
            cc1100_retransmission_count_bc = TRANSMISSION_RETRIES_CRX_BC;
            return true;
    }

    return false;
}

bool cc1100_set_mode(uint8_t mode, uint16_t opt_mode_data)
{
    /* Wake up from WOR/RX (if in WOR/RX, else no effect) */
    cc1100_go_idle();

    /* Make sure CC1100 is in IDLE state */
    cc1100_spi_strobe(CC1100_SIDLE);

    /* Set the new mode */
    bool result = cc1100_set_mode0(mode, opt_mode_data);

    /* If mode change was successful (mode is valid) */
    if (result) {
        /* Setup new mode configuration */
        cc1100_setup_mode();
        /* Reset statistics */
        cc1100_reset_statistic();
        return true;
    }
    else {
        /* Still in old mode, go to receive mode again */
        cc1100_go_receive();
        return false;
    }
}

char *cc1100_mode_to_text(uint8_t mode)
{
    switch(mode) {
        case CC1100_MODE_WOR:
            return "Wake-On-Radio";

        case CC1100_MODE_CONSTANT_RX:
            return "Constant RX";

        default:
            return "unknown";
    }
}

char *cc1100_state_to_text(uint8_t state)
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

void cc1100_hwtimer_go_receive_wrapper(void *ptr)
{
    (void) ptr;

    /* kernel timer handler function called, clear timer id */
    wor_hwtimer_id = -1;

    /* Stability: don't allow WOR timers at this point */
    if (rflags.TX) {
        return;
    }

    if (radio_state == RADIO_PWD) {
        /* Go to RX state, listen for packets as long as WOR_TIMEOUT_2 */
        cc1100_spi_strobe(CC1100_SRX);
        hwtimer_wait(IDLE_TO_RX_TIME);
        radio_state = RADIO_RX;
        /* Set hwtimer to put CC1100 back to WOR after WOR_TIMEOUT_2 */
        wor_hwtimer_id = hwtimer_set(WOR_TIMEOUT_2, cc1100_hwtimer_go_receive_wrapper, NULL);

        if (wor_hwtimer_id == -1) {
            rflags.KT_RES_ERR = true;
            /* No hwtimer available, go immediately to WOR mode. */
            /* Else never receiving packets again... */
            rflags.MAN_WOR = false;
            switch_to_wor2();
        }
    }
    else {
        cc1100_go_receive();
    }
}

/*---------------------------------------------------------------------------*/
/* 							CC1100 reset functionality */
/*---------------------------------------------------------------------------*/

static void reset(void)
{
    cc1100_go_idle();
    cc110x_spi_select();
    cc1100_spi_strobe(CC1100_SRES);
    hwtimer_wait(RTIMER_TICKS(10));
}

static void power_up_reset(void)
{
    cc110x_spi_unselect();
    cc110x_spi_cs();
    cc110x_spi_unselect();
    hwtimer_wait(RESET_WAIT_TIME);
    reset();
    radio_state = RADIO_IDLE;
}

/*---------------------------------------------------------------------------*/
/* 							CC1100 low level send function */
/*---------------------------------------------------------------------------*/

void cc1100_send_raw(uint8_t *tx_buffer, uint8_t size)
{
    volatile uint32_t abort_count;

    /* The number of bytes to be transmitted must be smaller */
    /* or equal to PACKET_LENGTH (62 bytes). So the receiver */
    /* can put the whole packet in its RX-FIFO (with appended */
    /* packet status bytes). */
    if (size > PACKET_LENGTH) {
        return;
    }

    /* Disables RX interrupt etc. */
    cc110x_before_send();

    /* But CC1100 in IDLE mode to flush the FIFO */
    cc1100_spi_strobe(CC1100_SIDLE);
    /* Flush TX FIFO to be sure it is empty */
    cc1100_spi_strobe(CC1100_SFTX);
    /* Write packet into TX FIFO */
    cc1100_spi_writeburst_reg(CC1100_TXFIFO, (char *) tx_buffer, size);
    /* Switch to TX mode */
    abort_count = 0;
    unsigned int cpsr = disableIRQ();
    cc1100_spi_strobe(CC1100_STX);

    /* Wait for GDO2 to be set -> sync word transmitted */
    while (cc110x_get_gdo2() == 0) {
        abort_count++;

        if (abort_count > CC1100_SYNC_WORD_TX_TIME) {
            /* Abort waiting. CC1100 maybe in wrong mode */
            /* e.g. sending preambles for always */
            puts("[CC1100 TX] fatal error\n");
            break;
        }
    }

    restoreIRQ(cpsr);

    /* Wait for GDO2 to be cleared -> end of packet */
    while (cc110x_get_gdo2() != 0);

    /* Experimental - TOF Measurement */
    cc110x_after_send();
}

/*---------------------------------------------------------------------------*/
/* 		Various functions (mode safe - they can be called in any radio mode) */
/*---------------------------------------------------------------------------*/
uint8_t
read_register(uint8_t r)
{
    uint8_t result;

    /* Save old radio state */
    uint8_t old_state = radio_state;

    /* Wake up from WOR/RX (if in WOR/RX, else no effect) */
    cc1100_go_idle();
    result = cc1100_spi_read_reg(r);

    /* Have to put radio back to WOR/RX if old radio state */
    /* was WOR/RX, otherwise no action is necessary */
    if (old_state == RADIO_WOR || old_state == RADIO_RX) {
        cc1100_go_receive();
    }

    return result;
}

static void
write_register(uint8_t r, uint8_t value)
{
    /* Save old radio state */
    uint8_t old_state = radio_state;

    /* Wake up from WOR/RX (if in WOR/RX, else no effect) */
    cc1100_go_idle();
    cc1100_spi_write_reg(r, value);

    /* Have to put radio back to WOR/RX if old radio state */
    /* was WOR/RX, otherwise no action is necessary */
    if (old_state == RADIO_WOR || old_state == RADIO_RX) {
        cc1100_go_receive();
    }
}

char *cc1100_get_output_power(char *buf)
{
    sprintf(buf, "%+i dBm", pa_table_dBm[pa_table_index]);
    return buf;
}

uint8_t cc1100_get_channel(void)
{
    return radio_channel;
}

bool
cc1100_set_channel(uint8_t channr)
{
    if (channr > MAX_CHANNR) {
        return false;
    }

    write_register(CC1100_CHANNR, channr * 10);
    radio_channel = channr;
    return true;
}

bool
cc1100_set_output_power(uint8_t pa_idx)
{
    if (pa_idx >= sizeof(pa_table)) {
        return false;
    }

    write_register(CC1100_PATABLE, pa_table[pa_idx]);
    pa_table_index = pa_idx;
    return true;
}

char *cc1100_get_marc_state(void)
{
    uint8_t state;

    /* Save old radio state */
    uint8_t old_state = radio_state;

    /* Read content of status register */
    state = cc1100_spi_read_status(CC1100_MARCSTATE) & MARC_STATE;

    /* Make sure in IDLE state. */
    /* Only goes to IDLE if state was RX/WOR */
    cc1100_go_idle();

    /* Have to put radio back to WOR/RX if old radio state */
    /* was WOR/RX, otherwise no action is necessary */
    if (old_state == RADIO_WOR || old_state == RADIO_RX) {
        cc1100_go_receive();
    }

    switch(state) {
            /* Note: it is not possible to read back the SLEEP or XOFF state numbers */
            /* because setting CSn low will make the chip enter the IDLE mode from the */
            /* SLEEP (0) or XOFF (2) states. */
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

/*
static int8_t
rssi_2_dbm(uint8_t rssi)
{
	if (rssi >= 128) rssi -= 256;
	rssi /= 2;
	rssi -= 78;
	return rssi;
}*/

/*---------------------------------------------------------------------------*/
/* 								Radio Driver API */
/*---------------------------------------------------------------------------*/
void cc1100_init(void)
{
    /* Initialize SPI */
    cc110x_spi_init();

    /* Set default mode (with default (energy optimized) RX interval) */
    cc1100_set_mode0(CC1100_RADIO_MODE, T_RX_INTERVAL);

    /* Load driver & reset */
    power_up_reset();

    /* Write configuration to configuration registers */
    extern char cc1100_conf[];
    cc1100_spi_writeburst_reg(0x00, cc1100_conf, CC1100_CONF_SIZE);

    /* Write PATABLE (power settings) */
    cc1100_spi_write_reg(CC1100_PATABLE, pa_table[pa_table_index]);

    /* Initialize Radio Flags */
    rflags.RSSI = 0x00;
    rflags.LL_ACK = false;
    rflags.CAA = false;
    rflags.CRC_STATE = false;
    rflags.SEQ = false;
    rflags.MAN_WOR = false;
    rflags.KT_RES_ERR = false;
    rflags.TX = false;
    rflags.WOR_RST = false;

    /* Initialize physical layer */
    cc1100_phy_init();

    /* Set radio address of CC1100 */
    cc1100_set_address(radio_address);

    /* Set default channel number */
    radio_channel = CC1100_DEFAULT_CHANNR;

    /* Switch to desired mode (WOR or RX) */
    rd_set_mode(RADIO_MODE_ON);
}

int cc1100_get_avg_transmission_duration(void)
{
    if (radio_mode == CC1100_MODE_WOR) {
        /* Transmission duration ~ RX interval */
        /* Double value because of MAC delay. */
        return 2 * cc1100_wor_config.rx_interval;
    }
    else {
        /* Transmission duration ~ 32 ms */
        /* Double value because of MAC delay. */
        return 2 * 32;
    }
}

radio_address_t cc1100_get_address(void)
{
    return radio_address;
}

bool cc1100_set_address(radio_address_t address)
{
    if (address < MIN_UID || address > MAX_UID) {
        return false;
    }

    uint8_t id = (uint8_t) address;

    if (radio_state != RADIO_UNKNOWN) {
        write_register(CC1100_ADDR, id);
    }

    radio_address = id;
    return true;
}

static int
rd_set_mode(int mode)
{
    int result;

    /* Get current radio mode */
    if (radio_state == RADIO_UNKNOWN || radio_state == RADIO_PWD) {
        result = RADIO_MODE_OFF;
    }
    else {
        result = RADIO_MODE_ON;
    }

    switch(mode) {
        case RADIO_MODE_ON:
            cc110x_init_interrupts();			/* Enable interrupts */
            cc1100_setup_mode();				/* Set chip to desired mode */
            break;

        case RADIO_MODE_OFF:
            cc1100_disable_interrupts();		/* Disable interrupts */
            switch_to_pwd();					/* Set chip to power down mode */
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

/*---------------------------------------------------------------------------*/
/* 						Carrier sense interface functions */
/*---------------------------------------------------------------------------*/

void cc1100_cs_init(void)
{
    cc1100_go_idle();							/* Wake CC1100 up from Wake-On-Radio mode */

    if (radio_state == RADIO_RX) {			/* If radio in RX mode */
        cc1100_spi_strobe(CC1100_SIDLE);		/* Go back to IDLE for calibration */
    }

    cc1100_spi_write_reg(CC1100_MCSM0, 0x08);	/* Turn off FS-Autocal */
    cc1100_spi_strobe(CC1100_SCAL);				/* Calibrate manually (721 us) */
    hwtimer_wait(MANUAL_FS_CAL_TIME);			/* Wait for calibration to finish before packet burst can start */
    radio_state = RADIO_AIR_FREE_WAITING;		/* Set status "waiting for air free" */
    cc1100_spi_write_reg(CC1100_MCSM2, 0x07);	/* Configure RX_TIME = Until end of packet (no timeout) */
    cc1100_spi_strobe(CC1100_SRX);				/* Switch to RX (88.4 us) (Carrier Sense) */
    hwtimer_wait(CS_READY_TIME);					/* Wait until CC1100 is in RX + carrier sense ready (GDO0 ready for readout -> data rate dependent!!!) */
}

void cc1100_cs_set_enabled(bool enabled)
{
    if (enabled) {
        /* Enable carrier sense detection (GDO0 interrupt) */
        cc110x_gdo0_enable();
    }
    else {
        /* Disable carrier sense detection (GDO0 interrupt) */
        cc110x_gdo0_disable();
    }
}

int cc1100_cs_read(void)
{
    /* GDO0 reflects CS (high: air not free, low: air free) */
    return cc110x_get_gdo0();
}

int cc1100_cs_read_cca(void)
{
    return rflags.CAA;
}

void cc1100_cs_write_cca(const int cca)
{
    rflags.CAA = cca;
}
/*---------------------------------------------------------------------------*/
/** @} */
