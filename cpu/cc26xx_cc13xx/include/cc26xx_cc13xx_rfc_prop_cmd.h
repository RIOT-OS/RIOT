/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx RF Core proprietary PHY commands definitions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_RFC_PROP_CMD_H
#define CC26XX_CC13XX_RFC_PROP_CMD_H

#include <stdint.h>

#include "cc26xx_cc13xx_rfc_common_cmd.h"
#include "cc26xx_cc13xx_rfc_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RFC_MOD_TYPE_FSK            (0) /**< FSK modulation type */
#define RFC_MOD_TYPE_GFSK           (1) /**< GFSK modulation type */
#define RFC_MOD_TYPE_OOK            (2) /**< OOK modulation type */

#define RFC_DEV_STEP_SZ_250_HZ      (0) /**< 250 Hz deviation step size */
#define RFC_DEV_STEP_SZ_1000_HZ     (1) /**< 1000 Hz deviation step size */
#define RFC_DEV_STEP_SZ_15_625_HZ   (2) /**< 15.625 Hz deviation step size */
#define RFC_DEV_STEP_SZ_62_5_HZ     (3) /**< 62.5 Hz deviation step size */

#define RFC_DECIM_MODE_AUTO         (0) /**< Automatic PDIF decimation */
#define RFC_DECIM_MODE_FORCE_0      (1) /**< Force PDIF decimation to 0 */
#define RFC_DECIM_MODE_FORCE_1      (2) /**< Force PDIF decimation to 1 */
#define RFC_DECIM_MODE_FORCE_2      (5) /**< Force PDIF decimation to 2 */


#define RFC_PREAM_MODE_0_FIRST      (0) /**< Send 0 as the first preamble bit */
#define RFC_PREAM_MODE_1_FIRST      (1) /**< Send 1 as the first preamble bit */
#define RFC_PREAM_MODE_SAME_BIT_SW  (2) /**< Send same first bit in preamble and
                                             sync word */
#define RFC_PREAM_MODE_DIFF_BIT_SW  (3) /**< Send different first bit in
                                             preamble and sync word */

#define RFC_WHITEN_MODE_NONE                 (0) /**< No whitening */
#define RFC_WHITEN_MODE_CC1101               (1) /**< CC1101/CC2500 compatible
                                                      whitening */
#define RFC_WHITEN_MODE_PN9                  (2) /**< PN9 whitening without byte
                                                      reversal */
#define RFC_WHITEN_MODE_IEEE802154G_NONE     (4) /**< No whitener, 32-bit IEEE
                                                      802.15.4g compatible
                                                      CRC */
#define RFC_WHITEN_MODE_IEEE802154G_CRC32    (5) /**< IEEE 802.15.4g compatible
                                                      whitener and 32-bit CRC */
#define RFC_WHITEN_MODE_IEEE802154G_ONLY_CRC (6) /**< No whitener, dynamically
                                                      IEEE 802.15.4g compatible
                                                      16-bit or 32-bit */
#define RFC_WHITEN_MODE_IEEE802154G_CRC      (7) /**< Dynamically IEEE 802.15.4g
                                                      compatible whitener and
                                                      16-bit or 32-bit CRC */

#define RFC_BIAS_MODE_INTERNAL  (0) /**< Internal bias */
#define RFC_BIAS_MODE_EXTERNAL  (1) /**< External bias */

/**
 * @brief   CMD_PROP_RADIO_DIV_SETUP
 * @{
 */
/**
 * @brief   Modulation parameters
 */
typedef struct {
    /**
     * 0h = FSK
     * 1h = GFSK
     * 2h = OOK
     * Others: Reserved
     */
    uint16_t mod_type:3;
#if defined(CPU_VARIANT_X2)
    /**
     * Deviation, specified in number of steps, with step size given by
     * deviation_step_sz.
     */
    uint16_t deviation:11;
    /**
     * Deviation step size
     * 0: 250 Hz
     * 1: 1000 Hz
     * 2: 15.625 Hz
     * 3: 62.5 Hz
     */
    uint16_t deviation_step_sz:2;
#elif defined(CPU_VARIANT_X0)
    /**
     * Deviation, specified in 250 Hz steps.
     */
    uint16_t deviation:13;
#endif
} rfc_modulation_t;

/**
 * @brief   CMD_PROP_RADIO_DIV_SETUP command ID
 */
#define RFC_CMD_PROP_RADIO_DIV_SETUP (0x3807)
/**
 * @brief   Proprietary Mode Radio Setup Command for All Frequency Bands
 */
typedef struct {
    uint16_t command_no; /**< The command ID number */
    uint16_t status; /**< An integer telling the status of the command. */
    void *next_op; /**< Pointer to the next operation to run */
    rfc_ratmr_t start_time; /**< Absolute or relative start time */
    rfc_trigger_t start_trigger; /**< Identification of the trigger that
                                      starts the operation */
    rfc_cond_t condition; /**< Condition for running next command */
    rfc_modulation_t modulation; /**< Modulation parameters */
    struct {
        uint32_t prescale:8; /**< Prescaler value */
        uint32_t rate_word:21; /**< Rate word */
        /**
         * 0h = Use automatic PDIF decimation
         * 1h = Force PDIF decimation to 0
         * 3h = Force PDIF decimation to 1
         * 5h = Force PDIF decimation to 2
         * Others: Reserved
         */
        uint32_t decim_mode:3;
    } symbol_rate; /**< Symbol rate setting */
    uint8_t rx_bw; /**< Receiver bandwidth */
    struct {
        /**
         * 0  = 1 preamble bit
         * 1--16 = Number of preamble bytes
         * 18, 20, ..., 30 = Number of preamble bytes
         * 31 = 4 preamble bits
         * 32 = 32 preamble bytes
         * Others: Reserved
         */
        uint8_t pream_bytes:6;
        /**
         * 0h = Send 0 as the first preamble bit
         * 1h = Send 1 as the first preamble bit
         * 2h = Send same first bit in preamble and sync word
         * 3h = Send different first bit in preamble and sync word
         */
        uint8_t pream_mode:2;
    } pream_conf; /**< Preamble configuration */
    struct {
        uint16_t sw_bits:6; /**< Number of sync word bits (8--32) */
        /**
         * 0h = Use positive deviation for 1
         * 1h = Use positive deviation for 0
         */
        uint16_t bit_reversal:1;
        /**
         * 0h = Least significant bit transmitted first
         * 1h = Most significant bit transmitted first
         */
        uint16_t msb_first:1;
        /**
         * Select coding
         *
         * 0h = Uncoded binary modulation
         * 10h = Manchester coded binary modulation
         * Others: Reserved
         */
        uint16_t fec_mode:4;
        uint16_t __reserved:1; /**< Reserved */
        /**
         * 0h = No whitening
         * 1h = CC1101/CC2500 compatible whitening
         * 2h = PN9 whitening without byte reversal
         * 3h = Reserved
         * 4h = No whitener, 32-bit IEEE 802.15.4g compatible CRC
         * 5h = IEEE 802.15.4g compatible whitener and 32-bit CRC
         * 6h = No whitener, dynamically IEEE 802.15.4g compatible 16-bit or
         * 32-bit CRC
         * 7h = Dynamically IEEE 802.15.4g compatible whitener and 16-bit or
         * 32-bit CRC
         */
        uint16_t whiten_mode:3;
    } format_conf; /**< Format configuration */
    struct {
        /**
         * 0h = Differential mode
         * 1h = Single-ended mode RFP
         * 2h = Single-ended mode RFN
         * 5h = Single-ended mode RFP with external frontend control on RF pins
         * (RFN and RXTX)
         * 6h = Single-ended mode RFN with external frontend control on RF pins
         * (RFP and RXTX)
         * Others: Reserved
         */
        uint16_t front_end_mode:3;
        /**
         * 0h = Internal bias.
         * 1h = External bias.
         */
        uint16_t bias_mode:1;
        /**
         * 0x00: Write analog configuration. Required first time after boot and
         * when changing frequency band or front-end configuration.
         * 0x2D: Keep analog configuration. May be used after standby or when
         * changing mode with the same frequency band and front-end
         * configuration.
         * Others: Reserved
         */
        uint16_t analog_cfg_mode:6;
        /**
         * 0h = Power up frequency synth
         * 1h = Do not power up frequency synth
         */
        uint16_t no_fs_powerup:1;
    } config; /**< Configuration options */
    uint16_t tx_power; /**< Transmit power */
    /**
     * Pointer to a list of hardware and configuration registers to override. If
     * NULL, no override is used.
     */
    uint32_t *reg_override;
    /**
     * Center frequency of the frequency band used, in MHz; used for calculating
     * some internal TX and RX parameters. For a single channel RF system, this
     * should be set equal to the RF frequency used. For a multi channel RF
     * system (e.g. frequency hopping spread spectrum), this should be set equal
     * to the center frequency of the frequency band used.
     */
    uint16_t center_freq;
    /**
     * Intermediate frequency to use for RX, in MHz on 4.12 signed format. TX
     * will use same intermediate frequency if supported, otherwise 0.
     *
     * 8000h = Use default.
     */
    int16_t int_freq;
    /**
     * LO frequency divider setting to use.
     *
     * Supported values: 0, 2, 4, 5, 6, 10, 12, 15, and 30
     */
    uint8_t lo_divider;
} rfc_cmd_prop_radio_div_setup_t;
/** @} */

/**
 * @brief   CMD_PROP_TX_ADV
 * @{
 */
#define RFC_CMD_PROP_TX_ADV (0x3803) /**< CMD_PROP_TX_ADV command ID */
/**
 * @brief   Proprietary Mode Advanced Transmit Command
 */
typedef struct {
    uint16_t command_no; /**< The command ID number */
    uint16_t status; /**< An integer telling the status of the command. */
    void *next_op; /**< Pointer to the next operation to run */
    rfc_ratmr_t start_time; /**< Absolute or relative start time */
    rfc_trigger_t start_trigger; /**< Identification of the trigger that
                                      starts the operation */
    rfc_cond_t condition; /**< Condition for running next command */
    struct {
        /**
         * 0h = Keep frequency synth on after command
         * 1h = Turn frequency synth off after command
         */
        uint8_t fs_off:1;
        uint8_t __reserved:2; /**< Reserved */
        /**
         * 0h = Do not append CRC
         * 1h = Append CRC
         */
        uint8_t use_crc:1;
        /**
         * 0h = Do not include sync word in CRC calculation
         * 1h = Include sync word in CRC calculation
         */
        uint8_t crc_inc_sw:1;
        /**
         * 0h = Do not include header in CRC calculation
         * 1h = Include header in CRC calculation
         */
        uint8_t crc_inc_hdr:1;
    } pkt_conf; /**< Packet configuration */
    uint8_t num_hdr_bits; /**< Number of bits in header (0--32) */
    uint16_t pkt_len; /**< Packet length. 0h = Unlimited */
    struct {
        /**
         * 0h = Start packet on a fixed time from the command start trigger
         * 1h = Start packet on an external trigger (input event to RAT)
         */
        uint8_t ext_tx_trig:1;
        /**
         * Input mode if external trigger is used for TX start.
         *
         * 0h = Rising edge
         * 1h = Falling edge
         * 2h = Both edges
         * 3h = Reserved
         */
        uint8_t input_mode:2;
        /**
         * RAT input event number used for capture if external trigger is used
         * for TX start
         */
        uint8_t source:5;
    } start_conf; /**< TX start configuration */
    /**
     * Trigger for transition from preamble to sync word
     */
    rfc_trigger_t pre_trigger;
    /**
     * Time used together with pre_trigger for transition from preamble to sync
     * word. If pre_trigger.type is set to "now", one preamble as configured in
     * the setup will be sent. Otherwise, the preamble will be repeated until
     * this trigger is observed.
     */
    rfc_ratmr_t pre_time;
    uint32_t sync_word; /**< Sync word to transmit */
    uint8_t *pkt; /**< Pointer to packet, or TX queue for unlimited length */
} rfc_cmd_prop_tx_adv_t;
/** @} */

/**
 * @brief   CMD_PROP_RX_ADV
 * @{
 */
#define RFC_CMD_PROP_RX_ADV (0x3804) /**< CMD_PROP_RX_ADV command ID */
/**
 * @brief   Proprietary Mode Advanced Receive Command
 */
typedef struct {
    uint16_t command_no; /**< The command ID number */
    uint16_t status; /**< An integer telling the status of the command. */
    void *next_op; /**< Pointer to the next operation to run */
    rfc_ratmr_t start_time; /**< Absolute or relative start time */
    rfc_trigger_t start_trigger; /**< Identification of the trigger that
                                         starts the operation */
    rfc_cond_t condition; /**< Condition for running next command */
    struct {
        /**
         * 0h = Keep frequency synth on after command
         * 1h = Turn frequency synth off after command
         */
        uint8_t fs_off:1;
        /**
         * 0h = End operation after receiving a packet correctly
         * 1h = Go back to sync search after receiving a packet correctly
         */
        uint8_t repeat_ok:1;
        /**
         * 0h = End operation after receiving a packet with CRC error
         * 1h = Go back to sync search after receiving a packet with CRC error
         */
        uint8_t repeat_nok:1;
        /**
         * 0h = Do not check CRC
         * 1h = Check CRC
         */
        uint8_t use_crc:1;
        /**
         * 0h = Do not include sync word in CRC calculation
         * 1h = Include sync word in CRC calculation
         */
        uint8_t crc_inc_sw:1;
        /**
         * 0h = Do not include header in CRC calculation
         * 1h = Include header in CRC calculation
         */
        uint8_t crc_inc_hdr:1;
        /**
         * 0h = Packet is received to the end if end trigger happens after sync
         * is obtained
         * 1h = Packet reception is stopped if end trigger happens
         */
        uint8_t end_type:1;
        /**
         * 0h = Stop receiver and restart sync search on address mismatch
         * 1h = Receive packet and mark it as ignored on address mismatch
         */
        uint8_t filter_op:1;
    } pkt_conf; /**< Packet configuration */
    struct {
        /**
         * If 1, automatically discard ignored packets from RX queue
         */
        uint8_t auto_flush_ignored:1;
        /**
         * If 1, automatically discard packets with CRC error from RX queue
         */
        uint8_t auto_flush_crc_err:1;
        uint8_t __reserved:1; /**< Reserved */
        /**
         * If 1, include the received header or length byte in the stored
         * packet; otherwise discard it
         */
        uint8_t include_hdr:1;
        /**
         * If 1, include the received CRC field in the stored packet; otherwise
         * discard it
         */
        uint8_t include_crc:1;
        /**
         * If 1, append an RSSI byte to the packet in the RX queue
         */
        uint8_t append_rssi:1;
        /**
         * If 1, append a timestamp to the packet in the RX queue
         */
        uint8_t append_timestamp:1;
        /**
         * If 1, append a status byte to the packet in the RX queue
         */
        uint8_t append_status:1;
    } rx_conf; /**< RX configuration */
    uint32_t sync_word0; /**< Sync word to listen for */
    uint32_t sync_word1; /**< Alternative sync word if non-zero */
    /**
     * Packet length for fixed length, maximum packet length for variable
     * length.
     *
     * 0: Unlimited or unknown length
     */
    uint16_t max_pkt_len;
    struct {
        uint16_t num_hdr_bits:6; /**< Number of bits in header (0--32) */
        uint16_t len_pos:5; /**< Position of length field in header (0--31) */
        uint16_t num_len_bits:5; /**< Number of bits in length field (0--16) */
    } hdr_conf; /**< Packet header configuration */
    struct {
        /**
         * 0: Address after header
         * 1: Address in header
         */
        uint16_t addr_type:1;
        /**
         * If addr_type == 0: Address size in bytes
         * If addr_type == 1: Address size in bits
         */
        uint16_t addr_size:5;
        /**
         * If addr_type == 1: Bit position of address in header
         * If addr_type == 0: Non-zero to extend address with sync word
         * identifier
         */
        uint16_t addr_pos:5;
        uint16_t num_addr:5; /**< Number of addresses in address list */
    } addr_conf; /**< Addresses configuration */
    int8_t len_offset; /**< Signed value to add to length field */
    rfc_trigger_t end_trigger; /**< Trigger for ending the operation */
    /**
     * Time used together with endTrigger for ending the operation
     */
    rfc_ratmr_t end_time;
    uint8_t *addr; /**< Pointer to address list */
    rfc_data_queue_t *queue; /**< Pointer to receive queue */
    uint8_t *output; /**< Pointer to output structure */
} rfc_cmd_prop_rx_adv_t;
/** @} */

//! @}

/**
 * @brief   CMD_PROP_CS
 * @{
 */
#define RFC_CMD_PROP_CS (0x3805)
/**
 * @brief   Carrier Sense Command
 */
typedef struct {
    uint16_t command_no; /**< The command ID number */
    uint16_t status; /**< An integer telling the status of the command. */
    void *next_op; /**< Pointer to the next operation to run */
    rfc_ratmr_t start_time; /**< Absolute or relative start time */
    rfc_trigger_t start_trigger; /**< Identification of the trigger that
                                      starts the operation */
    rfc_cond_t condition; /**< Condition for running next command */
    struct {
        /**
         * 0h = Keep synth running if command ends with channel Idle.
         * 1h = Turn off synth if command ends with channel Idle.
         */
        uint8_t off_idle:1;
        /**
         * 0h = Keep synth running if command ends with channel Busy.
         * 1h = Turn off synth if command ends with channel Busy.
         */
        uint8_t off_busy:1;
    } fs_conf; /**< Synthesizer configuration */
    uint8_t __dummy0; /**< Padding */
    struct {
       /**
        * If 1, enable RSSI as a criterion
        */
        uint8_t ena_rssi:1;
        /**
         * If 1, enable correlation as a criterion
         */
        uint8_t ena_corr:1;
        /**
         * 0h = Busy if either RSSI or correlation indicates Busy.
         * 1h = Busy if both RSSI and correlation indicates Busy.
         */
        uint8_t operation:1;
        /**
         * 0h = Continue carrier sense on channel Busy.
         * 1h = End carrier sense on channel Busy.
         *
         * For an RX command, the receiver will continue when carrier sense
         * ends, but it will then not end if channel goes Idle.
         */
        uint8_t busy_op:1;
        /**
         * 0h = Continue on channel Idle.
         * 1h = End on channel Idle
         */
        uint8_t idle_op:1;
        /**
         * 0h = Timeout with channel state Invalid treated as Busy.
         * 1h = Timeout with channel state Invalid treated as Idle.
         */
        uint8_t timeout_res:1;
    } conf; /**< Carrier sense configuration command */
    int8_t rssi_thr; /**< RSSI threshold */
    uint8_t num_rssi_idle; /**< Number of consecutive RSSI measurements below
                                the threshold needed before the channel is
                                declared Idle */
    uint8_t num_rssi_busy; /**< Number of consecutive RSSI measurements above the
                                threshold needed before the channel is declared
                                Busy */
    uint16_t corr_period; /**< Number of RAT ticks for a correlation
                               observation periods */
    struct {
        uint8_t num_corr_inv:4; /**< Number of subsequent correlation tops with
                                     maximum `corr_period` RAT ticks between
                                     them needed to go from Idle to Invalid */
        uint8_t num_corr_busy:4; /**< Number of subsequent correlation tops with
                                      maximum `corr_period` RAT ticks between
                                      them needed to go from Invalid to Busy */
    } corr_config; /**< Correlation observation configuration. */
    rfc_trigger_t end_trigger; /**< Trigger for ending the operation */
    rfc_ratmr_t end_time; /**< Time used together with `end_trigger` */
} rfc_cmd_prop_cs_t;
/** @} */

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif /* CC26XX_CC13XX_RFC_PROP_CMD_H */
/** @} */
