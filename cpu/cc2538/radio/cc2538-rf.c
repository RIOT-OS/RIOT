/*
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file        cc2538-rf.c
 * @brief       Low-level radio driver for the CC2538
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "msg.h"
#include "thread.h"

#include "cc2538-rf.h"

#include "byteorder.h"
#include "transceiver.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define OUTPUT_POWER_MIN            -24  /**< Min output power in dBm */
#define OUTPUT_POWER_MAX            + 7  /**< Max output power in dBm */
#define NUM_POWER_LEVELS            ( OUTPUT_POWER_MAX - OUTPUT_POWER_MIN + 1 )

#define CC2538_MIN_FREQ             2394
#define CC2538_MAX_FREQ             2507

#ifndef ENABLE_RF_STATUS_LEDS
#define ENABLE_RF_STATUS_LEDS       1
#endif /* ENABLE_RF_STATUS_LEDS */

/* IEEE 802.15.4 defined constants (2.4 GHz logical channels) */
#define IEEE802154_MIN_FREQ         2405 /**< Min. frequency (2405 MHz) */
#define IEEE802154_MAX_FREQ         2480 /**< Max. frequency (2480 MHz) */

#define IEEE802154_MIN_CHANNEL      11   /**< Min. channel (2405 MHz) */
#define IEEE802154_MAX_CHANNEL      26   /**< Max. channel (2480 MHz) */
#define IEEE802154_CHANNEL_SPACING  5    /**< Channel spacing in MHz  */

#define IEEE802154_CHAN2FREQ(chan)  ( IEEE802154_MIN_FREQ + ((chan) - IEEE802154_MIN_CHANNEL) * IEEE802154_CHANNEL_SPACING )
#define IEEE802154_FREQ2CHAN(freq)  ( IEEE802154_MIN_CHANNEL + ((freq) - IEEE802154_MIN_FREQ) / IEEE802154_CHANNEL_SPACING )

#ifndef CC2538_RF_CHANNEL_DEFAULT
#define CC2538_RF_CHANNEL_DEFAULT   17
#endif /* CC2538_RF_CHANNEL_DEFAULT */

#define ABS_DIFF(x, y)              ( ((x) < (y))? ((y) - (x)) : ((x) - (y)) )
#define BOOLEAN(x)                  ( (x) != 0 )
#define NOT(x)                      ( (x) == 0 )
#define GET_BYTE(buffer, index)     ( (unsigned char*)(buffer) )[index]

#define BIT(n)                      ( 1 << (n) )

#define RFCORE_FLUSH_RECEIVE_FIFO() rfcore_strobe(ISFLUSHRX)

static const uint8_t power_lut[NUM_POWER_LEVELS] = {
      0, /**< -24 dBm */
      7, /**< -23 dBm */
     15, /**< -22 dBm */
     22, /**< -21 dBm */
     29, /**< -20 dBm */
     37, /**< -19 dBm */
     44, /**< -18 dBm */
     51, /**< -17 dBm */
     59, /**< -16 dBm */
     66, /**< -15 dBm */
     77, /**< -14 dBm */
     88, /**< -13 dBm */
     93, /**< -12 dBm */
     98, /**< -11 dBm */
    106, /**< -10 dBm */
    114, /**<  -9 dBm */
    125, /**<  -8 dBm */
    136, /**<  -7 dBm */
    141, /**<  -6 dBm */
    145, /**<  -5 dBm */
    153, /**<  -4 dBm */
    161, /**<  -3 dBm */
    169, /**<  -2 dBm */
    176, /**<  -1 dBm */
    182, /**<   0 dBm */
    197, /**<   1 dBm */
    205, /**<   2 dBm */
    213, /**<   3 dBm */
    225, /**<   4 dBm */
    237, /**<   5 dBm */
    246, /**<   6 dBm */
    255, /**<   7 dBm */
};

static kernel_pid_t local_transceiver_pid = KERNEL_PID_UNDEF;
static bool monitor_mode = false;
static uint_fast8_t power_level_raw = 213; /**< 3 dBm */
static int channel = CC2538_RF_CHANNEL_DEFAULT;

cc2538_packet_t cc2538_packet;

/* pointer to the callback low-level function for packet reception */
static receive_802154_packet_callback_t recv_func = NULL;

/**
 * @brief RFCORE_XREG_RFERRM bits
 */
enum {
    STROBE_ERR = BIT(6),
    TXUNDERF   = BIT(5),
    TXOVERF    = BIT(4),
    RXUNDERF   = BIT(3),
    RXOVERF    = BIT(2),
    RXABO      = BIT(1),
    NLOCK      = BIT(0),
};

/**
 * @brief RFCORE_XREG_FRMCTRL0 bits
 */
enum {
    ENERGY_SCAN      = BIT(4),
    AUTOACK          = BIT(5),
    AUTOCRC          = BIT(6),
    APPEND_DATA_MODE = BIT(7),
};

/**
 * @brief RFCORE_XREG_RFIRQM0 / RFCORE_XREG_RFIRQF0 bits
 */
enum {
    ACT_UNUSED       = BIT(0),
    SFD              = BIT(1), /**< Start of frame event */
    FIFOP            = BIT(2),
    SRC_MATCH_DONE   = BIT(3),
    SRC_MATCH_FOUND  = BIT(4),
    FRAME_ACCEPTED   = BIT(5),
    RXPKTDONE        = BIT(6), /**< End of frame event */
    RXMASKZERO       = BIT(7),
};

/**
 * @brief RFCORE_XREG_RFIRQF1 bits
 */
enum {
    CSP_WAIT    = BIT(5),
    CSP_STOP    = BIT(4),
    CSP_MANINT  = BIT(3),
    RFIDLE      = BIT(2),
    TXDONE      = BIT(1), /**< End of frame event */
    TXACKDONE   = BIT(0),
};

typedef struct {
    cc2538_reg_t *reg_addr;
    uint32_t value;
} init_pair_t;

static const init_pair_t init_table[] = {
    {&SYS_CTRL_RCGCRFC,      0x01                     }, /* Enable the RF Core clock in run mode. */
    {&SYS_CTRL_SCGCRFC,      0x01                     }, /* Enable the RF Core clock in sleep mode. */
    {&SYS_CTRL_DCGCRFC,      0x01                     }, /* Enable the RF Core clock in deep sleep mode. */
    {&RFCORE_XREG_CCACTRL0,  0xf8                     }, /* -81 dB (from User Guide) */
    {&RFCORE_XREG_TXFILTCFG, 0x09                     }, /* TX anti-aliasing filter bandwidth */
    {&RFCORE_XREG_AGCCTRL1,  0x15                     }, /* target value for AGC control loop */
    {&ANA_REGS_IVCTRL,       0x0b                     }, /* Bias currents */
    {&RFCORE_XREG_MDMTEST1,  0x08                     },
    {&RFCORE_XREG_FSCAL1,    0x01                     }, /* Tune frequency calibration / Adjust current in VCO? */
    {&RFCORE_XREG_RXCTRL,    0x3f                     }, /* MCU default */
    {&RFCORE_XREG_MDMCTRL1,  0x14                     }, /* MCU default */
    {&RFCORE_XREG_ADCTEST0,  0x10                     }, /* MCU default */
    {&RFCORE_XREG_ADCTEST1,  0x0e                     }, /* MCU default */
    {&RFCORE_XREG_ADCTEST2,  0x03                     }, /* MCU default */
    {&RFCORE_XREG_CSPT,      0xff                     }, /* Disable the CSPT register compare function (MCU default) */
    {&RFCORE_XREG_MDMCTRL0,  0x85                     }, /* Tune sync word detection by requiring two zero symbols before the sync word (MCU default) */
    {&RFCORE_XREG_FSCTRL,    0x55                     }, /* Adjust current in synthesizer (not MCU default, MCU default is 0x5A) */
    {&RFCORE_XREG_FRMCTRL0,  AUTOCRC | AUTOACK        }, /* Enable automatic CRC calculation and RSSI append */
    {&RFCORE_XREG_FRMCTRL1,  0x00                     }, /* STXON does not affect RXENABLE[6] */
    {&RFCORE_XREG_SRCMATCH,  0x00                     }, /* Disable source address matching and autopend */
    {&RFCORE_XREG_FIFOPCTRL, CC2538_RF_MAX_PACKET_LEN }, /* MAX FIFOP threshold */
    {&RFCORE_XREG_RFIRQM0,   FIFOP | RXPKTDONE        }, /* Acknowledge RF interrupts */
    {&RFCORE_XREG_RFERRM,    STROBE_ERR | TXUNDERF | TXOVERF | RXUNDERF | RXOVERF | NLOCK},
    {NULL, 0},
};

bool RFCORE_ASSERT_failure(const char *expr, const char *func, int line)
{
#if (DEVELHELP || ENABLE_DEBUG)
    DEBUG_PRINT("RFCORE_ASSERT(%s) failed at line %u in %s()!\n", expr, line, func);
    DEBUG_PRINT("  RFCORE_SFR_RFERRF = 0x%02x\n", (unsigned int)RFCORE_SFR_RFERRF);
#endif

    return false;
}

void rfcore_strobe(uint_fast8_t instr)
{
    RFCORE_SFR_RFST = instr;
}

uint_fast8_t rfcore_read_byte(void)
{
    RFCORE_ASSERT(RFCORE_XREG_RXFIFOCNT > 0);
    return RFCORE_SFR_RFDATA;
}

void rfcore_write_byte(uint_fast8_t byte)
{
    RFCORE_ASSERT(RFCORE_XREG_TXFIFOCNT < CC2538_RF_FIFO_SIZE);
    RFCORE_SFR_RFDATA = byte;
}

void rfcore_read_fifo(void *buf, uint_fast8_t len)
{
    uint_fast8_t n;

    RFCORE_ASSERT(len <= RFCORE_XREG_RXFIFOCNT);

    for (n = 0; n < len; n++) {
        GET_BYTE(buf, n) = RFCORE_SFR_RFDATA;
    }
}

void rfcore_write_fifo(const void *buf, uint_fast8_t len)
{
    uint_fast8_t n;

    RFCORE_ASSERT(len <= CC2538_RF_FIFO_SIZE - RFCORE_XREG_TXFIFOCNT);

    for (n = 0; n < len; n++) {
        RFCORE_SFR_RFDATA = GET_BYTE(buf, n);
    }
}

void cc2538_initialize(void)
{
    const init_pair_t *pair;

    for (pair = init_table; pair->reg_addr != NULL; pair++) {
        *pair->reg_addr = pair->value;
    }

    cc2538_set_monitor(monitor_mode);
    cc2538_set_tx_power_raw(power_level_raw);
    cc2538_set_channel(channel);

#if ENABLE_RF_STATUS_LEDS
    /* Select the observable signals (maximum of three) */
    RFCORE_XREG_RFC_OBS_CTRL0 = tx_active;
    RFCORE_XREG_RFC_OBS_CTRL1 = rx_active;
    RFCORE_XREG_RFC_OBS_CTRL2 = ffctrl_fifo;

    /* Select output pins for the three observable signals */
#ifdef BOARD_OPENMOTE
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
#endif /* RIOT_BOARD */

#endif /* ENABLE_RF_STATUS_LEDS */

    if (SYS_CTRL->I_MAP) {
        NVIC_SetPriority(RF_RXTX_ALT_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(RF_RXTX_ALT_IRQn);

        NVIC_SetPriority(RF_ERR_ALT_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(RF_ERR_ALT_IRQn);
    }
    else {
        NVIC_SetPriority(RF_RXTX_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(RF_RXTX_IRQn);

        NVIC_SetPriority(RF_ERR_IRQn, RADIO_IRQ_PRIO);
        NVIC_EnableIRQ(RF_ERR_IRQn);
    }

    /* Flush the receive and transmit FIFOs */
    rfcore_strobe(ISFLUSHTX);

    cc2538_on();
}

void cc2538_init(kernel_pid_t tpid)
{
    local_transceiver_pid = tpid;
    cc2538_initialize();
}

bool cc2538_on(void)
{
    RFCORE_FLUSH_RECEIVE_FIFO();
    rfcore_strobe(ISRXON);
    return true;
}

void cc2538_off(void)
{
    /* Wait for ongoing TX to complete (e.g. this could be an outgoing ACK) */
    RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE == 0);

    RFCORE_FLUSH_RECEIVE_FIFO();

    /* Don't turn off if we are off as this will trigger a Strobe Error */
    if (RFCORE_XREG_RXENABLE != 0) {
        rfcore_strobe(ISRFOFF);
    }
}

bool cc2538_is_on(void)
{
    return RFCORE->XREG_FSMSTAT1bits.RX_ACTIVE || RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE;
}

void cc2538_switch_to_rx(void)
{
    RFCORE_FLUSH_RECEIVE_FIFO();
    rfcore_strobe(ISRXON);
}

void isr_rfcorerxtx(void)
{
    uint_fast8_t irq_status0, irq_status1, len;
    static uint8_t rx_buf[256];
    cc2538_fcs_t fcs;

    irq_status1 = RFCORE_SFR_RFIRQF1;

    if (irq_status1) {
        RFCORE_SFR_RFIRQF1 = 0;
    }

    while ((irq_status0 = RFCORE_SFR_RFIRQF0)) {
        RFCORE_SFR_RFIRQF0 = 0;

        if (irq_status0 & FIFOP) {
            if (RFCORE_XREG_RXFIFOCNT < 1) {
                DEBUG("%s(): FIFOP = 1 but RXFIFOCNT = 0!\n", __FUNCTION__);
            }
            else {
                len = rfcore_read_byte();

                if (len < CC2538_FCS_LEN) {
                    DEBUG("%s(): length %u < CC2538_FCS_LEN!\n", __FUNCTION__, len);
                }
                else if (len > RFCORE_XREG_RXFIFOCNT) {
                    DEBUG("%s(): length %u > RXFIFOCNT!\n", __FUNCTION__, len);
                    RFCORE_FLUSH_RECEIVE_FIFO();
                }
                else {
                    len -= CC2538_FCS_LEN;

                    rfcore_read_fifo(rx_buf, len);
                    rfcore_read_fifo(&fcs, CC2538_FCS_LEN);

                    /* read buffer into ieee802154_frame */
                    ieee802154_frame_read(rx_buf, &cc2538_packet.frame, len + IEEE_802154_FCS_LEN);

                    /* if packet is not an ACK */
                    if (cc2538_packet.frame.fcf.frame_type != IEEE_802154_ACK_FRAME) {
#if ENABLE_DEBUG
                        ieee802154_frame_print_fcf_frame(&cc2538_packet.frame);
#endif

                        if (recv_func != NULL) {
                            recv_func((void *)rx_buf, len, fcs.rssi - CC2538_RSSI_OFFSET, fcs.lqi, fcs.crc);
                        }

                        if (netdev_rcv_data_cb != NULL) {
                            netdev_rcv_data_cb(
                                (netdev_t *)&cc2538_netdev,
                                cc2538_packet.frame.src_addr,
                                sizeof(cc2538_packet.frame.src_addr),
                                cc2538_packet.frame.dest_addr,
                                sizeof(cc2538_packet.frame.dest_addr),
                                (void *)rx_buf,
                                len
                            );
                        }

#ifdef MODULE_TRANSCEIVER
                        cc2538_packet.rssi = fcs.rssi - CC2538_RSSI_OFFSET;
                        cc2538_packet.crc = fcs.crc;
                        cc2538_packet.lqi = fcs.lqi;
                        cc2538_packet.length = len;

                        /* notify transceiver thread if any */
                        if (local_transceiver_pid != KERNEL_PID_UNDEF) {
                            msg_t m;
                            m.type = RCV_PKT_CC2538;
                            m.content.ptr = (void *)rx_buf;
#if DEVELHELP
                            int result = msg_send_int(&m, local_transceiver_pid);

                            if (result < 1) {
                                DEBUG_PRINT(
                                    "%s(): Unable to notify transceiver thread. msg_send_int() result was %d.\n",
                                    __FUNCTION__,
                                    result
                                );
                            }

#else
                            msg_send_int(&m, local_transceiver_pid);
#endif

                        }

#endif
                    } /* end if (cc2538_packet.frame.fcf.frame_type != IEEE_802154_ACK_FRAME) */
                }
            }
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

void isr_rfcoreerr(void)
{
    uint_fast8_t flags;

    /* Latch and clear the interrupt status */
    flags = RFCORE_SFR_RFERRF;
    RFCORE_SFR_RFERRF = 0;

    /* These conditions shouldn't happen if the driver is working properly */
    RFCORE_ASSERT(NOT(flags & RXUNDERF));
    RFCORE_ASSERT(NOT(flags & TXOVERF));
    RFCORE_ASSERT(NOT(flags & TXUNDERF));

    /* Fail in case of miscallaneous unexpected error conditions */
    RFCORE_ASSERT(NOT(flags & STROBE_ERR));

    DEBUG("%s(): RFERRF=0x%02x.\n", __FUNCTION__, flags);

    /* Flush the receive FIFO in case of a receive overflow */
    if (flags & RXOVERF) {
        DEBUG("%s(): RXOVERF detected!\n", __FUNCTION__);
        RFCORE_FLUSH_RECEIVE_FIFO();
    }

    /* Flush the receive FIFO in case of a receive overflow */
    if (flags & NLOCK) {
        /* The frequency synthesizer failed to achieve lock after time-out, or
         * lock is lost during reception. The receiver must be restarted to clear
         * this error situation. */
        DEBUG("%s(): NLOCK detected!\n", __FUNCTION__);
        RFCORE_FLUSH_RECEIVE_FIFO();
    }
}

void cc2538_set_monitor(bool mode)
{
    RFCORE->XREG_FRMFILT0bits.FRAME_FILTER_EN = NOT(mode);
    monitor_mode = mode;
}

bool cc2538_get_monitor(void)
{
    return NOT(RFCORE->XREG_FRMFILT0bits.FRAME_FILTER_EN);
}

void cc2538_set_freq(unsigned int freq)
{
    DEBUG("%s(%u): Setting frequency to ", __FUNCTION__, freq);

    if (freq < CC2538_MIN_FREQ) {
        freq = CC2538_MIN_FREQ;
    }
    else if (freq > CC2538_MAX_FREQ) {
        freq = CC2538_MAX_FREQ;
    }

    DEBUG("%u + %u = %u MHz.\n", CC2538_MIN_FREQ, freq - CC2538_MIN_FREQ, freq);
    RFCORE_XREG_FREQCTRL = freq - CC2538_MIN_FREQ;
}

void cc2538_set_channel(unsigned int chan)
{
    DEBUG("%s(%u);\n", __FUNCTION__, chan);

    cc2538_set_freq(IEEE802154_CHAN2FREQ(chan));
    channel = chan;
}

unsigned int cc2538_get_channel(void)
{
    return IEEE802154_FREQ2CHAN(CC2538_MIN_FREQ + RFCORE_XREG_FREQCTRL);
}

void cc2538_set_address(uint16_t addr)
{
    RFCORE_FFSM_SHORT_ADDR0 = addr;
    RFCORE_FFSM_SHORT_ADDR1 = addr >> 8;
}

void cc2538_set_address_long(uint64_t addr)
{
    RFCORE_FFSM_EXT_ADDR7 = addr >> (7 * 8);
    RFCORE_FFSM_EXT_ADDR6 = addr >> (6 * 8);
    RFCORE_FFSM_EXT_ADDR5 = addr >> (5 * 8);
    RFCORE_FFSM_EXT_ADDR4 = addr >> (4 * 8);
    RFCORE_FFSM_EXT_ADDR3 = addr >> (3 * 8);
    RFCORE_FFSM_EXT_ADDR2 = addr >> (2 * 8);
    RFCORE_FFSM_EXT_ADDR1 = addr >> (1 * 8);
    RFCORE_FFSM_EXT_ADDR0 = addr >> (0 * 8);
}

uint16_t cc2538_get_address(void)
{
    return (RFCORE_FFSM_SHORT_ADDR1 << 8) | RFCORE_FFSM_SHORT_ADDR0;
}

uint64_t cc2538_get_address_long(void)
{
    uint64_t addr = RFCORE_FFSM_EXT_ADDR7;
    addr <<= 8;
    addr |= RFCORE_FFSM_EXT_ADDR6;
    addr <<= 8;
    addr |= RFCORE_FFSM_EXT_ADDR5;
    addr <<= 8;
    addr |= RFCORE_FFSM_EXT_ADDR4;
    addr <<= 8;
    addr |= RFCORE_FFSM_EXT_ADDR3;
    addr <<= 8;
    addr |= RFCORE_FFSM_EXT_ADDR2;
    addr <<= 8;
    addr |= RFCORE_FFSM_EXT_ADDR1;
    addr <<= 8;
    addr |= RFCORE_FFSM_EXT_ADDR0;
    return addr;
}

void cc2538_set_pan(uint16_t pan)
{
    RFCORE_FFSM_PAN_ID0 = pan;
    RFCORE_FFSM_PAN_ID1 = pan >> 8;
}

uint16_t cc2538_get_pan(void)
{
    return (RFCORE_FFSM_PAN_ID1 << 8) | RFCORE_FFSM_PAN_ID0;
}

void cc2538_set_tx_power_raw(uint_fast8_t pow)
{
    RFCORE_XREG_TXPOWER = pow;
    power_level_raw = pow;
}

void cc2538_set_tx_power(int pow)
{
    if (pow < OUTPUT_POWER_MIN) {
        pow = OUTPUT_POWER_MIN;
    }
    else if (pow > OUTPUT_POWER_MAX) {
        pow = OUTPUT_POWER_MAX;
    }

    cc2538_set_tx_power_raw(power_lut[pow - OUTPUT_POWER_MIN]);
}

int cc2538_get_tx_power(void)
{
    int index;
    int best_index = 0;
    int best_delta = INT_MAX;
    int txpower;

    txpower = RFCORE_XREG_TXPOWER & 0xff;

    for (index = 0; index < NUM_POWER_LEVELS; index++) {
        int delta = ABS_DIFF(power_lut[index], txpower);

        if (delta < best_delta) {
            best_delta = delta;
            best_index = index;
        }
    }

    return OUTPUT_POWER_MIN + best_index;
}

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

radio_tx_status_t cc2538_load_tx_buf(ieee802154_packet_kind_t kind,
                                     ieee802154_node_addr_t dest,
                                     bool use_long_addr,
                                     bool wants_ack,
                                     void *buf,
                                     unsigned int len)
{
    mac_header_t hdr;
    uint_fast8_t hdr_size;
    static uint_fast8_t sequence_nr = 0;

    /* FCS : frame version 0, we don't manage security,
       nor batches of packets */
    switch (kind) {
        case PACKET_KIND_BEACON:
        case PACKET_KIND_DATA:
        case PACKET_KIND_ACK:
            hdr.type = kind;
            break;

        default:
            return RADIO_TX_INVALID_PARAM;
    }

    if (wants_ack) {
        hdr.type |= WANTS_ACK;
    }

    if (use_long_addr) {
        hdr.addr_type = ADDR_TYPE_LONG;
        hdr.long_addrs.dest_addr = HTONLL(dest.long_addr);
        hdr.long_addrs.src_addr  = HTONLL(cc2538_get_address_long());
        hdr_size = 3 + sizeof(long_addrs_t);
    }
    else {
        hdr.addr_type = ADDR_TYPE_SHORT;
        uint_fast16_t src_pan = cc2538_get_pan();

        if (dest.pan.id == src_pan) {
            hdr.type |= INTRA_PAN_COMPRESSION;
            hdr.short_addrs_compr.dest_pan  = HTONS(dest.pan.id);
            hdr.short_addrs_compr.dest_addr = HTONS(dest.pan.addr);
            hdr.short_addrs_compr.src_addr  = HTONS(cc2538_get_address());
            hdr_size = 3 + sizeof(short_addrs_compr_t);
        }
        else {
            hdr.short_addrs.dest_pan  = HTONS(dest.pan.id);
            hdr.short_addrs.dest_addr = HTONS(dest.pan.addr);
            hdr.short_addrs.src_pan   = HTONS(src_pan);
            hdr.short_addrs.src_addr  = HTONS(cc2538_get_address());
            hdr_size = 3 + sizeof(short_addrs_t);
        }
    }

    /* total frame size */
    if (CC2538_PACKET_LENGTH_SIZE + hdr_size + len > CC2538_RF_FIFO_SIZE) {
        return RADIO_TX_PACKET_TOO_LONG;
    }

    if (RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE) {
        return RADIO_TX_MEDIUM_BUSY;
    }

    /* sequence number */
    hdr.sequence_nr = sequence_nr++;

    RFCORE_ASSERT(cc2538_is_on());

    rfcore_strobe(ISFLUSHTX);

    /* Send the phy length byte first */
    rfcore_write_byte(hdr_size + len + CC2538_AUTOCRC_LEN);
    rfcore_write_fifo(&hdr, hdr_size);
    rfcore_write_fifo(buf, len);
    rfcore_tx_frame_hook(buf, len);

    /* TODO: Wait for ACK if wants_ack? */

    return RADIO_TX_OK;
}

static uint_fast8_t txon_instr = ISTXON;

radio_tx_status_t cc2538_transmit_tx_buf(void)
{
    bool was_off = false;

    if (!cc2538_is_on()) {
        was_off = true;
        cc2538_on();
        RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT0bits.FSM_FFCTRL_STATE > FSM_STATE_RX_CALIBRATION);
    }

    if (!cc2538_channel_clear()) {
        return RADIO_TX_COLLISION;
    }

    if (RFCORE->XREG_FSMSTAT1bits.SFD) {
        return RADIO_TX_COLLISION;
    }

    /* Start the transmission */
    rfcore_strobe(txon_instr);

    if (was_off) {
        /* Wait for transmission to start */
        RFCORE_WAIT_UNTIL(RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE);

        cc2538_off();
    }

    return RADIO_TX_OK;
}

radio_tx_status_t cc2538_send(ieee802154_packet_kind_t kind,
                              ieee802154_node_addr_t dest,
                              bool use_long_addr,
                              bool wants_ack,
                              void *buf,
                              unsigned int len)
{
    radio_tx_status_t result;

    result = cc2538_load_tx_buf(kind, dest, use_long_addr, wants_ack, buf, len);

    if (result != RADIO_TX_OK) {
        return result;
    }

    result = cc2538_transmit_tx_buf();
    return result;
}

int16_t cc2538_send_lowlevel(const void *buf, int len)
{
    unsigned int packet_len;

    packet_len = len + CC2538_FCS_LEN;

    if (len <= 0) {
        return RADIO_TX_INVALID_PARAM;
    }

    if (packet_len > CC2538_RF_MAX_PACKET_LEN) {
        return RADIO_TX_PACKET_TOO_LONG;
    }

    if (RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE) {
        return RADIO_TX_MEDIUM_BUSY;
    }

    RFCORE_ASSERT(cc2538_is_on());

    rfcore_strobe(ISFLUSHTX);

    /* Send the phy length byte first */
    rfcore_write_byte(packet_len);
    rfcore_write_fifo(buf, len);
    rfcore_tx_frame_hook(buf, len);

    return cc2538_transmit_tx_buf();
}

int16_t cc2538_send_ieee802154_packet(ieee802154_packet_t *packet)
{
    uint8_t header[IEEE_802154_MAX_HDR_LEN];
    uint_fast8_t header_size, total_size;

    /* Generate the 802.15.4 header: */
    header_size = ieee802154_frame_init(&packet->frame, header);

    /* The damage is already done if this fails: */
    RFCORE_ASSERT(header_size < IEEE_802154_MAX_HDR_LEN);

    total_size = header_size + packet->frame.payload_len;
    if (CC2538_PACKET_LENGTH_SIZE + total_size > CC2538_RF_FIFO_SIZE) {
        return RADIO_TX_PACKET_TOO_LONG;
    }

    if (RFCORE->XREG_FSMSTAT1bits.TX_ACTIVE) {
        return RADIO_TX_MEDIUM_BUSY;
    }

    RFCORE_ASSERT(cc2538_is_on());

    rfcore_strobe(ISFLUSHTX);

    /* Send the phy length byte first */
    rfcore_write_byte(total_size + CC2538_FCS_LEN);
    rfcore_write_fifo(header, header_size);
    rfcore_write_fifo(packet->frame.payload, packet->frame.payload_len);

    return cc2538_transmit_tx_buf();
}

void cc2538_set_recv_callback(receive_802154_packet_callback_t recv_cb)
{
    recv_func = recv_cb;
}

#if ENABLE_DEBUG
static void printf_hex(const void *buf, size_t len)
{
    size_t n;

    if (len <= 0) {
        return;
    }

    printf("%02x", ((const uint8_t *)buf)[0]);

    for (n = 1; n < len; n++) {
        printf(":%02x", ((const uint8_t *)buf)[n]);
    }
}
#endif

void rfcore_rx_frame_hook(const void *buf, uint_fast8_t len, const cc2538_fcs_t *fcs)
{
#if ENABLE_DEBUG
    DEBUG_PRINT("%s(): Received ", __FUNCTION__);
    printf_hex(buf, len);
    DEBUG_PRINT(", RSSI=%d, CRC=%u, LQI=%u\n", fcs->rssi - CC2538_RSSI_OFFSET, fcs->crc, fcs->lqi);
#endif /* ENABLE_DEBUG */
}

void rfcore_tx_frame_hook(const void *buf, uint_fast8_t len)
{
#if ENABLE_DEBUG
    DEBUG_PRINT("%s(): Sending ", __FUNCTION__);
    printf_hex(buf, len);
    DEBUG_PRINT("\n");
#endif /* ENABLE_DEBUG */
}

/* CC2538 low-level radio driver definition */
const ieee802154_radio_driver_t cc2538_radio_driver = {
    .init                 = cc2538_initialize,
    .on                   = cc2538_on,
    .off                  = cc2538_off,
    .is_on                = cc2538_is_on,
    .load_tx              = cc2538_load_tx_buf,
    .transmit             = cc2538_transmit_tx_buf,
    .send                 = cc2538_send,
    .set_receive_callback = cc2538_set_recv_callback,
    .switch_to_rx         = cc2538_switch_to_rx,
    .set_channel          = cc2538_set_channel,
    .get_channel          = cc2538_get_channel,
    .set_address          = cc2538_set_address,
    .get_address          = cc2538_get_address,
    .set_long_address     = cc2538_set_address_long,
    .get_long_address     = cc2538_get_address_long,
    .set_pan_id           = cc2538_set_pan,
    .get_pan_id           = cc2538_get_pan,
    .set_tx_power         = cc2538_set_tx_power,
    .get_tx_power         = cc2538_get_tx_power,
    .channel_is_clear     = cc2538_channel_clear,
    .set_promiscuous_mode = cc2538_set_monitor,
    .in_promiscuous_mode  = cc2538_get_monitor,
};
