/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC13x2 IEEE 802.15.4 netdev driver
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include <errno.h>
#include <string.h>

#include "cc26x2_cc13x2_rf_internal.h"
#include "cc26x2_cc13x2_rfc.h"
#include "vendor/rf_patch_cpe_prop.h"
#include "rf_conf.h"

#include "net/ieee802154.h"

#include "byteorder.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define PREAMBLE_LEN        (4U)    /**< Preamble length in bytes */

#define RX_BANDWIDTH_310KHZ (0x59)  /**< RX bandwidth */
#define RX_BANDWIDTH_190KHZ (0x56)  /**< RX bandwidth */

/**
 * @brief   Transmit buffer length.
 *
 * Format is:
 *
 * ```
 * +-----+------+
 * | PHR | PSDU |
 * +-----+------+
 * ```
 */
#define TX_BUF_LEN         ALIGN_TO_4(IEEE802154_PHY_MR_FSK_PHR_LEN + \
                                      IEEE802154G_FRAME_LEN_MAX)

/**
 * @brief   Receive buffer length.
 *
 * This is the length of an individual receive buffer.
 *
 * Format is:
 *
 * ```
 * +------------------+------+--------+
 * | rfc_data_entry_t | PSDU | Status |
 * +------------------+------+--------+
 * ```
 */
#define RX_BUF_LEN         ALIGN_TO_4(sizeof(rfc_data_entry_t) + \
                                      IEEE802154G_FRAME_LEN_MAX +  \
                                      CC26X2_CC13X2_RF_RX_STATUS_LEN)

static uint8_t _tx_buf[TX_BUF_LEN]  __attribute__ ((aligned(4))); /**< Transmit buffer */
static rfc_data_queue_t _rx_queue __attribute__ ((aligned(4)));  /**< Receive data queue */
/** Receive buffer configured as a circular buffer, e.g.: the last one points
 * to the first, each buffer contains @ref rfc_data_entry_t which is used
 * by the RF Core to mark the buffers that have available data when a packet
 * is received, each buffer represents a different packet. */
static uint8_t _rx_buf[CONFIG_CC26x2_CC13X2_RF_RX_BUF_NUMOF][RX_BUF_LEN] __attribute__ ((aligned(4)));

static rfc_ratmr_t _rat_offset;     /**< Radio timer offset */

static int16_t _tx_pwr;             /**< Transmit power used */
static uint16_t _channel;           /**< Current channel */

static uint8_t _short_addr[IEEE802154_SHORT_ADDRESS_LEN];
static uint8_t _long_addr[IEEE802154_LONG_ADDRESS_LEN];
static uint16_t _pan_id;

enum {
    STATE_RFC_POWER_ON,
    STATE_RFC_SETUP,
    STATE_FINISHED,
} _poweron_state;

/* override_prop_common.xml
 * DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x7 (DITHER_EN=0 and IPEAK=7).
 */
#define OVERRIDE_DCDCCTL5_REG           ((uint32_t)0x00F788D3)
/* override_prop_common_sub1g.xml. Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1.
 * Bits [0:16, 24, 30] are don't care..
 */
#define OVERRIDE_RF_FSCA_0              ((uint32_t)0x4001405D)
/* Set RF_FSCA.ANADIV.DIV_SEL_BIAS = 1. Bits [0:16, 24, 30] are don't care.. */
#define OVERRIDE_RF_FSCA_1              ((uint32_t)0x08141131)
/* override_tc106.xml. Tx: Configure PA ramp time, PACTL2.RC=0x3 (in ADI0, set
 * PACTL2[4:3]=0x3) */
#define OVERRIDE_PA_RAMP_TIME \
    RFC_ADI_2HALFREG_OVERRIDE(0, 16, 0x8, 0x8, 17, 0x1, 0x1)
/* Rx: Set AGC reference level to 0x1A (default: 0x2E) */
#define OVERRIDE_AGC_REFERENCE_LEVEL    RFC_HW_REG_OVERRIDE(0x609C, 0x001A)
/* Rx: Set RSSI offset to adjust reported RSSI by -1 dB (default: -2),
 * trimmed for external bias and differential configuration
 */
#define OVERRIDE_RSSI_OFFSET            ((uint32_t)0x000188A3)
/* Rx: Set anti-aliasing filter bandwidth to 0xD (in ADI0, set
 * IFAMPCTL3[7:4]=0xD) */
#define OVERRIDE_ANTI_ALIAS_BW          RFC_ADI_HALFREG_OVERRIDE(0,61,0xF,0xD)
/* Tx: Set wait time before turning off ramp to 0x1A (default: 0x1F) */
#define OVERRIDE_RAMP_OFF_WAIT_TIME     RFC_HW_REG_OVERRIDE(0x6028,0x001A)
/* TX power override
 * Tx: Set PA trim to max to maximize its output power
 * (in ADI0, set PACTL0=0xF8) */
#define OVERRIDE_PACTL                  RFC_ADI_REG_OVERRIDE(0,12,0xF8)

/**
 * @brief   RF propietary mode radio register overrides.
 *
 * These overrides are the defaults provided by Texas Instruments
 * using the SmartRF software for Sub-GHz. It's all the same on
 * the Sub-GHz family. However some custom boards may need
 * specific trims, if using external PAs for example, to adjust
 * the RSSI offset, PA, etc.
 *
 * This can be defined in the rf_conf.h file if custom overrides
 * are needed.
 */
#ifndef CC26X2_CC13X2_RF_PROP_OVERRIDE
#define CC26X2_CC13X2_RF_PROP_OVERRIDE \
    { \
        OVERRIDE_DCDCCTL5_REG, \
        OVERRIDE_RF_FSCA_0, \
        OVERRIDE_RF_FSCA_1, \
        OVERRIDE_PA_RAMP_TIME, \
        OVERRIDE_AGC_REFERENCE_LEVEL, \
        OVERRIDE_RSSI_OFFSET, \
        OVERRIDE_ANTI_ALIAS_BW, \
        OVERRIDE_RAMP_OFF_WAIT_TIME, \
        OVERRIDE_PACTL, \
        RFC_END_OVERRIDE, \
    }
#endif

/** Overrides for CMD_PROP_RADIO_DIV_SETUP. */
static uint32_t rf_prop_overrides[] = CC26X2_CC13X2_RF_PROP_OVERRIDE;

/**
 * CMD_SYNC_START_RAT
 *
 * This commands starts the Radio Timer, needed to run time-dependant radio
 * operations, and to chain commands. We only use the parameter `rat0` which
 * is the previous RAT offset when it was stopped using CMD_SYNC_STOP_RAT,
 * if no previous offset is present, 0 is fine.
 */
rfc_cmd_sync_start_rat_t rf_cmd_sync_start_rat =
{
    .command_no = RFC_CMD_SYNC_START_RAT,
    .status = RFC_IDLE,
    .next_op = NULL, /* set by us */
    .start_time = 0,
    .start_trigger = {
        .type = RFC_TRIG_NOW,
        .ena_cmd = 0,
        .trigger_no = 0,
        .past_trig = 0,
    },
    .condition = {
        .rule = RFC_COND_NEVER,
        .skip_no = 0,
    },
    .__dummy0 = 0,
    .rat0 = 0, /* set by us */
};

/**
 * CMD_PROP_RADIO_DIV_SETUP
 *
 * This is the "setup" command for the radio, sets the necessary parameters for
 * TX/RX and some configuration of the modulation, whitening, packet format,
 * symbol rate, preamble configuration, receiver bandwidth, intermediate
 * frequency, center frequency.
 */
rfc_cmd_prop_radio_div_setup_t rf_cmd_prop_radio_div_setup =
{
    .command_no = RFC_CMD_PROP_RADIO_DIV_SETUP,
    .status = RFC_IDLE,
    .next_op = NULL, /* set by us */
    .start_time = 0,
    .start_trigger = {
        .type = RFC_TRIG_NOW,
        .ena_cmd = 0,
        .trigger_no = 0,
        .past_trig = 0
    },
    .condition = {
        .rule = RFC_COND_NEVER,
        .skip_no = 0
    },
    .modulation = {
        .mod_type = RFC_MOD_TYPE_GFSK,
        .deviation = 0x64,
        .deviation_step_sz = RFC_DEV_STEP_SZ_250_HZ
    },
    .symbol_rate = {
        .prescale = 15,
        .rate_word = 0x8000,
        .decim_mode = RFC_DECIM_MODE_AUTO
    },
    .rx_bw = RX_BANDWIDTH_190KHZ,
    .pream_conf = {
        .pream_bytes = PREAMBLE_LEN,
        .pream_mode = RFC_PREAM_MODE_0_FIRST,
    },
    .format_conf = {
        .sw_bits = IEEE802154_PHY_MR_FSK_2FSK_SFD_LEN * 8,
        .bit_reversal = 0,
        .msb_first = 1,
        .fec_mode = 0,
        .whiten_mode = RFC_WHITEN_MODE_IEEE802154G_CRC
    },
    .config = {
        .front_end_mode = 0,
        .bias_mode = RFC_BIAS_MODE_EXTERNAL,
        .analog_cfg_mode = 0,
        .no_fs_powerup = 0
    },
    .tx_power = 0x04C0,
    .reg_override = rf_prop_overrides,
    .center_freq = 0x0363,
    .int_freq = 0x8000,
    .lo_divider = 0x05
};

/**
 * CMD_FS
 *
 * The Frequency Sinthesizer command, sets the channel frequency.
 */
rfc_cmd_fs_t rf_cmd_fs =
{
    .command_no = RFC_CMD_FS,
    .status = RFC_IDLE,
    .next_op = NULL, /* set by us */
    .start_time = 0,
    .start_trigger = {
        .type = RFC_TRIG_NOW,
        .ena_cmd = 0,
        .trigger_no = 0,
        .past_trig = 0
    },
    .condition = {
        .rule = RFC_COND_NEVER,
        .skip_no = 0
    },
    .frequency = 0, /* set by us */
    .fract_freq = 0, /* set by us */
    .synth_conf = {
        .txmode = 0,
        .ref_freq = 0
    },
    .__dummy0 = 0,
    .__dummy1 = 0,
    .__dummy2 = 0,
    .__dummy3 = 0
};

/**
 * CMD_PROP_TX_ADV
 *
 * Advanced proprietary transmission. Despite the "proprietary" name, it's
 * compatible with IEEE 802.15.4g. Fields set at runtime are: pkt and pkt_len.
 */
rfc_cmd_prop_tx_adv_t rf_cmd_prop_tx_adv =
{
    .command_no = RFC_CMD_PROP_TX_ADV,
    .status = RFC_IDLE,
    .next_op = NULL, /* set by us */
    .start_time = 0,
    .start_trigger = {
        .type = RFC_TRIG_NOW,
        .ena_cmd = 0,
        .trigger_no = 0,
        .past_trig = 0
    },
    .condition = {
        .rule = RFC_COND_NEVER,
        .skip_no = 0
    },
    .pkt_conf = {
        .fs_off = 0,
        .use_crc = 1,
        .crc_inc_sw = 0,
        .crc_inc_hdr = 0
    },
    .num_hdr_bits = IEEE802154_PHY_MR_FSK_PHR_LEN * 8,
    .pkt_len = 0, /*< set by us */
    .start_conf = {
        .ext_tx_trig = 0,
        .input_mode = 0,
        .source = 0
    },
    .pre_trigger = {
        .type = RFC_TRIG_NOW,
        .ena_cmd = 0,
        .trigger_no = 0,
        .past_trig = 0
    },
    .pre_time = 0,
    .sync_word = IEEE802154_PHY_MR_FSK_2FSK_UNCODED_SFD_0,
    .pkt = 0 /* set by us */
};

/**
 * CMD_PROP_RX_ADV
 *
 * Advanced proprietary RX. Sets the radio into RX mode, it keeps putting
 * the RX queue. This command does not finish unless it's aborted/stopped.
 */
rfc_cmd_prop_rx_adv_t rf_cmd_prop_rx_adv =
{
    .command_no = RFC_CMD_PROP_RX_ADV,
    .status = RFC_IDLE,
    .next_op = NULL, /* set by us */
    .start_time = 0,
    .start_trigger = {
        .type = RFC_TRIG_NOW,
        .ena_cmd = 0,
        .trigger_no = 0,
        .past_trig = 0
    },
    .condition = {
        .rule = RFC_COND_NEVER,
        .skip_no = 0
    },
    .pkt_conf = {
        .fs_off = 0,
        .repeat_ok = 1,
        .repeat_nok = 1,
        .use_crc = 1,
        .crc_inc_sw = 0,
        .crc_inc_hdr = 0,
        .end_type = 0,
        .filter_op = 1
    },
    .rx_conf = {
        .auto_flush_ignored = 1,
        .auto_flush_crc_err = 1,
        .include_hdr = 0,
        .include_crc = 0,
        .append_rssi = 1,
        .append_timestamp = 0,
        .append_status = 0
    },
    .sync_word0 = IEEE802154_PHY_MR_FSK_2FSK_UNCODED_SFD_0,
    .sync_word1 = 0 /*IEEE802154_PHY_MR_FSK_2FSK_UNCODED_SFD_1*/,
    .max_pkt_len = IEEE802154G_FRAME_LEN_MAX,
    .hdr_conf = {
        .num_hdr_bits = IEEE802154_PHY_MR_FSK_PHR_LEN * 8,
        .len_pos = 0,
        .num_len_bits = 11
    },
    .addr_conf = {
        .addr_type = 0,
        .addr_size = 0,
        .addr_pos = 0,
        .num_addr = 0
    },
    .len_offset = -4,
    .end_trigger = {
        .type = RFC_TRIG_NEVER,
        .ena_cmd = 0,
        .trigger_no = 0,
        .past_trig = 0
    },
    .end_time = 0,
    .addr = 0, /* unused */
    .queue = 0, /* set by us */
    .output = 0 /* set by us */
};

/** Carrier sense command, used to perform CCA */
rfc_cmd_prop_cs_t rf_cmd_prop_cs =
{
    .command_no = RFC_CMD_PROP_CS,
    .status = RFC_IDLE,
    .next_op = NULL,
    .start_time = 0,
    .start_trigger = {
        .type = RFC_TRIG_NOW,
        .ena_cmd = 0,
        .trigger_no = 0,
        .past_trig = 0
    },
    .condition = {
        .rule = RFC_COND_NEVER,
        .skip_no = 0
    },
    .fs_conf = {
        /* we don't want to turn off the Frequency Synthesizer after a
         * carrier sense command */
        .off_idle = 0,
        .off_busy = 0,
    },
    .__dummy0 = 0,
    .conf = {
        .ena_rssi = 1, /* use RSSI as the measurement parameter */
        .ena_corr = 0,
        .operation = 0,
        /* stop either on Busy or Idle */
        .busy_op = 1,
        .idle_op = 1,
        .timeout_res = 0,
    },
    .rssi_thr = -80, /* SHOULD be set by us, through hal or netdev */
    .num_rssi_idle = 1, /* consecutive RSSI measurements to mark channel as
                           Idle */
    .num_rssi_busy = 1, /* consecutive RSSI measurements to mark channel as
                           Busy */
    .corr_period = 0,
    .corr_config = {
        .num_corr_inv = 0,
        .num_corr_busy = 0,
    },
    /* end trigger is not used, as Carrier Sense will stop
     * on an Idle/Busy channel. */
    .end_trigger = {
        .type = RFC_TRIG_NEVER,
        .ena_cmd = 0,
        .trigger_no = 0,
        .past_trig = 0
    },
    .end_time = 0,
};

/** Command to clear the RX queue */
rfc_cmd_clear_rx_t rf_cmd_clear_rx =
{
    .command_no = RFC_CMD_CLEAR_RX,
    .__dummy0 = 0,
    .queue = NULL /* set by us */
};

/** Set TX power */
rfc_cmd_set_tx_power_t rf_cmd_set_tx_power =
{
    .command_no = RFC_CMD_SET_TX_POWER,
    .tx_power = 0,
};

void cc26x2_cc13x2_rf_internal_init(void)
{
    /* Initialize data entries as general, with a length field of 2 bytes
     * (uint16_t) and the correct RX buffer size */
    for (unsigned i = 0; i < CONFIG_CC26x2_CC13X2_RF_RX_BUF_NUMOF; i++) {
        /* verify that the compiler put our RX buffers on aligned memory */
        assert(((uintptr_t)_rx_buf[i] & 0x3) == 0);

        /* the initial bytes of an RX buffer is the rfc_data_entry_t structure */
        rfc_data_entry_t *entry = (rfc_data_entry_t *)_rx_buf[i];

        entry->status = RFC_DATA_ENTRY_PENDING;
        entry->config.type = RFC_DATA_ENTRY_TYPE_GEN;
        entry->config.lensz = sizeof(uint16_t);
        entry->length = RX_BUF_LEN - sizeof(rfc_data_entry_t);

        if ((i + 1) == CONFIG_CC26x2_CC13X2_RF_RX_BUF_NUMOF) {
            /* Point to the first entry if this is the last */
            entry->next_entry = _rx_buf[0];
        }
        else {
            entry->next_entry = _rx_buf[i + 1];
        }
    }

    /* Put a pointer to the receive buffers on the received queue, last entry is
     * null, this to make a circular buffer */
    _rx_queue.curr_entry = _rx_buf[0];
    _rx_queue.last_entry = NULL;

    rf_cmd_prop_rx_adv.queue = &_rx_queue;
    rf_cmd_clear_rx.queue = &_rx_queue;

    _channel = CC26X2_CC13X2_RF_CHAN_MIN;
    uint64_t default_freq = cc26x2_cc13x2_rf_chan_freq(_channel);
    cc26x2_cc13x2_rf_freq_convert(default_freq,
                                  &rf_cmd_fs.frequency,
                                  &rf_cmd_fs.fract_freq);

    cc26x2_cc13x2_rf_pa_t *pa_val = &cc26x2_cc13x2_rf_patable[0];
    _tx_pwr = pa_val->dbm;

    rf_cmd_prop_radio_div_setup.condition.rule = RFC_COND_ALWAYS;
    rf_cmd_prop_radio_div_setup.condition.skip_no = 0;
    rf_cmd_prop_radio_div_setup.next_op = (rfc_op_t *)&rf_cmd_fs;
    rf_cmd_prop_radio_div_setup.tx_power = pa_val->val;

    rf_cmd_sync_start_rat.next_op = &rf_cmd_prop_radio_div_setup;
    rf_cmd_sync_start_rat.condition.rule = RFC_COND_ALWAYS;
    rf_cmd_sync_start_rat.rat0 = _rat_offset;

    memset(_short_addr, 0, IEEE802154_SHORT_ADDRESS_LEN);
    memset(_long_addr, 0, IEEE802154_LONG_ADDRESS_LEN);
    memset(&_pan_id, 0, sizeof(uint16_t));
}

void cc26x2_cc13x2_rf_request_on(void)
{
    cc26x2_cc13x2_rfc_request_on();
    _poweron_state = STATE_RFC_POWER_ON;
}

int cc26x2_cc13x2_rf_confirm_on(void)
{
    uint32_t cmdsta;
    (void)cmdsta;

    switch (_poweron_state) {
        case STATE_RFC_POWER_ON:
            if (cc26x2_cc13x2_rfc_confirm_on() != 0) {
                return -EAGAIN;
            }
            cc26x2_cc13x2_rfc_finish_on(rf_patch_cpe_prop);
            cmdsta = cc26x2_cc13x2_rfc_request_execute((uintptr_t)&rf_cmd_sync_start_rat);
            assert((cmdsta & 0xFF) == RFC_CMDSTA_DONE);
            _poweron_state = STATE_RFC_SETUP;
            return -EAGAIN;

        case STATE_RFC_SETUP:
            if (cc26x2_cc13x2_rfc_confirm_execute() != 0) {
                return -EAGAIN;
            }
            _poweron_state = STATE_FINISHED;
            return 0;

        case STATE_FINISHED:
            break;

        default:
            assert(0);
    }

    return 0;
}

int cc26x2_cc13x2_rf_request_cca(void)
{
    uint32_t cmdsta;

    rf_cmd_prop_cs.status = RFC_IDLE;
    cmdsta = cc26x2_cc13x2_rfc_request_execute((uintptr_t)&rf_cmd_prop_cs);
    if ((cmdsta & 0xFF) != RFC_CMDSTA_DONE) {
        DEBUG("[cc26x2_cc13x2_rf]: CMD_PROP_CS failed with cmdsta=%08lx",
              cmdsta);
        return -EIO;
    }

    return 0;
}

bool cc26x2_cc13x2_rf_rx_is_on(void)
{
    return rf_cmd_prop_rx_adv.status == RFC_ACTIVE;
}

int cc26x2_cc13x2_rf_rx_start(void)
{
    uint32_t cmdsta;

    DEBUG_PUTS("rx");

    /* if netdev or IEEE 802.15.4g HAL don't clear up the buffers by calling
     * recv/indication_rx the buffer might fill up, this is the case for
     * netdev as when receiving packets too fast gnrc_netif might not handle
     * them as fast as we want, so in either case if these packets aren't handled
     * just drop them.
     *
     * TODO: what about re-generating the ISR event and return -EBUSY here to
     * avoid the loss of these packets? */
    if (rf_cmd_prop_rx_adv.status == RFC_PROP_ERROR_RXBUF) {
        DEBUG("[cc26x2_cc13x2_rf]: clearing RX queue\n");
        cmdsta = cc26x2_cc13x2_rfc_request_execute((uintptr_t)&rf_cmd_clear_rx);
        assert((cmdsta & 0xFF) == RFC_CMDSTA_DONE);
        rf_cmd_prop_rx_adv.status = RFC_IDLE;
    }

    rf_cmd_prop_rx_adv.queue = &_rx_queue;
    cmdsta = cc26x2_cc13x2_rfc_request_execute((uintptr_t)&rf_cmd_prop_rx_adv);
    if ((cmdsta & 0xFF) == RFC_CMDSTA_SCHEDULINGERROR) {
        DEBUG("[cc26x2_cc13x2_rf]: scheduling error");
        cc26x2_cc13x2_rfc_abort_cmd();

        /* this time should work */
        cmdsta = cc26x2_cc13x2_rfc_request_execute((uintptr_t)&rf_cmd_prop_rx_adv);
    }

    if ((cmdsta & 0xFF) != RFC_CMDSTA_DONE) {
        rf_cmd_prop_rx_adv.status = RFC_IDLE;
        return -EIO;
    }

    return 0;
}

void cc26x2_cc13x2_rf_rx_stop(void)
{
    DEBUG_PUTS("stop");
    assert(cc26x2_cc13x2_rf_rx_is_on());
    cc26x2_cc13x2_rfc_abort_cmd();
    rf_cmd_prop_rx_adv.status = RFC_IDLE;
}

rfc_data_entry_general_t *cc26x2_cc13x2_rf_recv(void)
{
    return (rfc_data_entry_general_t *)cc26x2_cc13x2_rfc_queue_recv(&_rx_queue);
}

int cc26x2_cc13x2_rf_write(const iolist_t *iolist)
{
    DEBUG_PUTS("write");
    const size_t max_psdu_len = TX_BUF_LEN - IEEE802154_PHY_MR_FSK_PHR_LEN;
    const size_t fcs_len = sizeof(uint16_t);
    size_t total_len;
    size_t psdu_len = 0;

    /* PHY header size reserved (two bytes), after the PHR the
     * PSDU starts */
    uint8_t *buf_pos = _tx_buf + IEEE802154_PHY_MR_FSK_PHR_LEN;
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        psdu_len += iol->iol_len;
        if (psdu_len > max_psdu_len) {
            return -EOVERFLOW;
        }

        memcpy(buf_pos, iol->iol_base, iol->iol_len);
        buf_pos += iol->iol_len;
    }

    /* Length in MR-FSK PHR. Includes the PSDU + FCS */
    total_len = psdu_len + fcs_len;
    if (total_len > IEEE802154G_FRAME_LEN_MAX) {
        return -EOVERFLOW;
    }

    /* Prepare the MR-FSK PHR
     * - No mode switching.
     * - Total length: PSDU + FCS length
     * - FCS type (CRC-16)
     *
     * Total length = len (payload) + CRC length
     *
     * The Radio will flip the bits around, so _tx_buf[0] must have the
     * length LSBs (PHR[15:8] and _tx_buf[1] will have PHR[7:0]
     */
    _tx_buf[0] = (((total_len) >> 0) & 0xFF);
    _tx_buf[1] = (((total_len) >> 8) & 0xFF) + 0x08 + 0x10;

    /* The length of the buffer used to transmit is the PHR + PSDU,
     * it does not include the FCS size as the radio writes it on
     * it's own depending on the configured FCS, which is CRC-16 in
     * our case */
    rf_cmd_prop_tx_adv.pkt_len = IEEE802154_PHY_MR_FSK_PHR_LEN + psdu_len;
    rf_cmd_prop_tx_adv.pkt = _tx_buf;
    rf_cmd_prop_tx_adv.status = RFC_IDLE;

    return 0;
}

int cc26x2_cc13x2_rf_request_transmit(void)
{
    DEBUG_PUTS("tx");
    uint32_t cmdsta;

    cmdsta = cc26x2_cc13x2_rfc_request_execute((uintptr_t)&rf_cmd_prop_tx_adv);
    switch (cmdsta & 0xFF) {
        case RFC_CMDSTA_DONE:
            break;
        /* previous command, if any, has not been finished */
        case RFC_CMDSTA_SCHEDULINGERROR:
            return -EBUSY;
        default:
            DEBUG("[cc26x2_cc13x2_rf]: couldn't send TX command, cmdsta=%08lx\n",
                  cmdsta);
            return -EIO;
    }

    return 0;
}

size_t cc26x2_cc13x2_rf_tx_psdu_len(void)
{
    return rf_cmd_prop_tx_adv.pkt_len - IEEE802154_PHY_MR_FSK_PHR_LEN;
}

void cc26x2_cc13x2_rf_set_cca_threshold(int8_t threshold)
{
    (void)threshold;
    rf_cmd_prop_cs.rssi_thr = threshold;
}

int cc26x2_cc13x2_rf_change_chan(uint16_t channel)
{
    const uint16_t min_chan = CC26X2_CC13X2_RF_CHAN_MIN;
    const uint16_t max_chan = CC26X2_CC13X2_RF_CHAN_MAX;
    uint32_t cmdsta;

    if (!(channel >= min_chan &&
          channel <= max_chan)) {
        DEBUG("[cc26x2_cc13x2_rf]: invalid channel value=%u\n", channel);
        return -EINVAL;
    }

    uint64_t new_freq  = cc26x2_cc13x2_rf_chan_freq(channel);
    cc26x2_cc13x2_rf_freq_convert(new_freq,
                                  &rf_cmd_fs.frequency,
                                  &rf_cmd_fs.fract_freq);

    RFC_DBELL->RFCPEIFG = ~CPE_IRQ_LAST_COMMAND_DONE;
    cmdsta = cc26x2_cc13x2_rfc_request_execute((uintptr_t)&rf_cmd_fs);
    if ((cmdsta & 0xFF) != RFC_CMDSTA_DONE) {
        DEBUG("[cc26x2_cc13x2_rf]: couldn't change channel, cmdsta=%08lx",
              cmdsta);
        return -EIO;
    }
    while (cc26x2_cc13x2_rfc_confirm_execute() == -EAGAIN) {}

    assert(rf_cmd_fs.status = RFC_DONE_OK);

    _channel = channel;

    return 0;
}

int cc26x2_cc13x2_rf_set_tx_pwr(int16_t pow)
{
    uint32_t cmdsta;
    cc26x2_cc13x2_rf_pa_t *match = &cc26x2_cc13x2_rf_patable[0];

    for (unsigned i = 0; i < CC26X2_CC13X2_PA_TABLE_NUMOF; i++) {
        if (cc26x2_cc13x2_rf_patable[i].dbm >= pow) {
            match = &cc26x2_cc13x2_rf_patable[i];
        }
        else {
            break;
        }
    }

    DEBUG("[cc26x2_cc13x2_rf]: found matching TX power %"PRIi16" dBm (for %"PRIi16" dBm)\n",
          match->dbm, pow);

    _tx_pwr = match->dbm;
    rf_cmd_set_tx_power.tx_power = match->val;

    /* execute the command, it is immediate */
    cmdsta = cc26x2_cc13x2_rfc_request_execute((uintptr_t)&rf_cmd_set_tx_power);
    if ((cmdsta & 0xFF) != RFC_CMDSTA_DONE) {
        DEBUG("[cc26x2_cc13x2_rf]: couldn't set TX power to %"PRIi16" dBm",
              match->dbm);
        return -EIO;
    }

    return 0;
}

int16_t cc26x2_cc13x2_rf_get_tx_pwr(void)
{
    return _tx_pwr;
}

eui64_t cc26x2_cc13x2_rf_get_hwaddr(void)
{
    eui64_t ret;
    size_t i;
    uint8_t *hwaddr;
    assert(IEEE802154_LONG_ADDRESS_LEN == sizeof(uint64_t));

    /* The IEEE MAC address can be stored in two places. We check the Customer
     * Configuration to see if it has a valid HW address. */
    hwaddr = (uint8_t *)&CCFG->IEEE_MAC_0;
    for (i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++) {
        if (hwaddr[i] != 0xFF) {
            break;
        }
    }

    if (i == IEEE802154_LONG_ADDRESS_LEN) {
        /* The CCFG address was all 0xFF (empty), switch to the fcfg which
         * always has a valid HW address from factory */
        hwaddr = (uint8_t *)&FCFG->MAC_15_4_0;
    }

    /* The IEEE MAC address is stored in network byte order. */
    memcpy(ret.uint8, hwaddr, IEEE802154_LONG_ADDRESS_LEN);
    return ret;
}

void cc26x2_cc13x2_rf_set_short_addr(network_uint16_t short_addr)
{
    memcpy(_short_addr, short_addr.u8, IEEE802154_SHORT_ADDRESS_LEN);
}

void cc26x2_cc13x2_rf_set_long_addr(eui64_t long_addr)
{
    memcpy(_long_addr, long_addr.uint8, IEEE802154_LONG_ADDRESS_LEN);
}

void cc26x2_cc13x2_rf_set_pan_id(uint16_t pan_id)
{
    _pan_id = pan_id;
}

bool cc26x2_cc13x2_rf_l2_filter(uint8_t *mhr)
{
    uint8_t dst_addr[IEEE802154_LONG_ADDRESS_LEN];
    le_uint16_t dst_pan;
    uint8_t pan_bcast[] = IEEE802154_PANID_BCAST;

    int addr_len = ieee802154_get_dst(mhr, dst_addr, &dst_pan);
    if (addr_len < 0) {
        return false;
    }

    /* filter PAN ID */
    /* Will only work on little endian platform (all cc26xx/cc13xx ;-) ) */
    if ((memcmp(pan_bcast, dst_pan.u8, 2) != 0) &&
        (memcmp(&_pan_id, dst_pan.u8, 2) != 0)) {
        return false;
    }

    /* check destination address */
    if (((addr_len == IEEE802154_SHORT_ADDRESS_LEN) &&
          (memcmp(_short_addr, dst_addr, addr_len) == 0 ||
           memcmp(ieee802154_addr_bcast, dst_addr, addr_len) == 0)) ||
        ((addr_len == IEEE802154_LONG_ADDRESS_LEN) &&
          (memcmp(_long_addr, dst_addr, addr_len) == 0))) {
        return true;
    }

    return false;
}
