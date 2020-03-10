/*
 * Copyright (c) 2018, Texas Instruments Incorporated - http://www.ti.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <driverlib/rf_mailbox.h>
#include <driverlib/rf_common_cmd.h>
#include <driverlib/rf_prop_cmd.h>
#include <rf_patches/rf_patch_cpe_multi_protocol.h>

#include <ti/drivers/rf/RF.h>

#define CC_ALIGN(n) __attribute__ ((__aligned__(n)))

/**
 * @brief   override_prop_common.xml
 *
 * DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x7 (DITHER_EN=0 and IPEAK=7).
 */
#define DCDC_REGULATOR_OVERRIDE ((uint32_t)0x00F788D3)

/**
 * @rief    override_tc106.xml
 *
 * Tx: Configure PA ramp time, PACTL2.RC=0x3 (in ADI0, set PACTL2[4:3]=0x3)
 */
#define PA_RAMP_TIME_OVERRIDE (ADI_2HALFREG_OVERRIDE(0,16,0x8,0x8,17,0x1,0x1))

/**
 * @brief   Rx: Set AGC reference level to 0x1A (default: 0x2E)
 */
#define AGC_REFERENCE_LEVEL_OVERRIDE (HW_REG_OVERRIDE(0x609C,0x001A))


/**
 * @brief   Rx: Set RSSI offset to adjust reported RSSI by -1 dB (default: -2),
 *          trimmed for external bias and differential configuration
 */
#define RSSI_REPORTED_OFFSET_OVERRIDE ((uint32_t)0x000188A3)

/**
 * @brief   Rx: Set anti-aliasing filter bandwidth to 0xD (in ADI0, set
 *          IFAMPCTL3[7:4]=0xD)
 */
#define ANTI_ALIASING_FILTER_OVERRIDE (ADI_HALFREG_OVERRIDE(0,61,0xF,0xD))

/**
 * @brief   Tx: Set wait time before turning off ramp to 0x1A (default: 0x1F)
 */
#define RAMP_OFF_WAIT_TIME_OVERRIDE (HW_REG_OVERRIDE(0x6028,0x001A))

/**
 * @brief   RF Propietary mode overrides
 */
#define RF_PROP_OVERRIDES { DCDC_REGULATOR_OVERRIDE,       \
                            PA_RAMP_TIME_OVERRIDE,         \
                            AGC_REFERENCE_LEVEL_OVERRIDE,  \
                            RSSI_REPORTED_OFFSET_OVERRIDE, \
                            ANTI_ALIASING_FILTER_OVERRIDE, \
                            RAMP_OFF_WAIT_TIME_OVERRIDE,   \
                            (uint32_t)0xFFFFFFFF, }

/* TI-RTOS RF Mode Object */
RF_Mode rf_prop_mode =
{
    .rfMode = RF_MODE_AUTO,
    .cpePatchFxn = &rf_patch_cpe_multi_protocol,
    .mcePatchFxn = 0,
    .rfePatchFxn = 0,
};

/* Overrides for CMD_PROP_RADIO_DIV_SETUP */
uint32_t rf_prop_overrides[] CC_ALIGN(4) = RF_PROP_OVERRIDES;

/* CMD_PROP_RADIO_DIV_SETUP: Proprietary Mode Radio Setup Command for All
 * Frequency Bands */
rfc_CMD_PROP_RADIO_DIV_SETUP_t rf_cmd_prop_radio_div_setup = {
    .commandNo = CMD_PROP_RADIO_DIV_SETUP,
    .status = IDLE,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger = {
        .triggerType = TRIG_NOW,
        .bEnaCmd = 0x0,
        .triggerNo = 0x0,
        .pastTrig = 0x0,
    },
    .condition = {
        .rule = COND_NEVER,
        .nSkip = 0x0,
    },
    .modulation = {
        .modType = 0x1,
        .deviation = 0x64,
        .deviationStepSz = 0x0,
    },
    .symbolRate = {
        .preScale = 0xF,
        .rateWord = 0x8000,
        .decimMode = 0x0,
    },
    .rxBw = 0x52,
    .preamConf = {
        .nPreamBytes = 0x7,
        .preamMode = 0x0,
    },
    .formatConf = {
        .nSwBits = 0x18,
        .bBitReversal = 0x0,
        .bMsbFirst = 0x1,
        .fecMode = 0x0,
        .whitenMode = 0x7,
    },
    .config = {
        .frontEndMode = 0x0, /* set by driver */
        .biasMode = 0x0, /* set by driver */
        .analogCfgMode = 0x0,
        .bNoFsPowerUp = 0x0,
    },
    .txPower = 0xCC14, /* set by driver */
    .pRegOverride = rf_prop_overrides,
    .centerFreq = 0x0393, /* set by driver */
    .intFreq = 0x8000, /* set by driver */
    .loDivider = 0x05, /* set by driver */
};

/* CMD_FS: Frequency Synthesizer Programming Command */
rfc_CMD_FS_t rf_cmd_prop_fs =
{
    .commandNo = CMD_FS,
    .status = IDLE,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger = {
        .triggerType = TRIG_NOW,
        .bEnaCmd = 0x0,
        .triggerNo = 0x0,
        .pastTrig = 0x0,
    },
    .condition = {
        .rule = COND_NEVER,
        .nSkip = 0x0,
    },
    .frequency = 0x0393, /* set by driver */
    .fractFreq = 0x0000, /* set by driver */
    .synthConf = {
        .bTxMode = 0x0,
        .refFreq = 0x0,
    },
    .__dummy0 = 0x00,
    .__dummy1 = 0x00,
    .__dummy2 = 0x00,
    .__dummy3 = 0x0000,
};

/* CMD_PROP_TX_ADV: Proprietary Mode Advanced Transmit Command */
rfc_CMD_PROP_TX_ADV_t rf_cmd_prop_tx_adv =
{
    .commandNo = CMD_PROP_TX_ADV,
    .status = IDLE,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger = {
        .triggerType = TRIG_NOW,
        .bEnaCmd = 0x0,
        .triggerNo = 0x0,
        .pastTrig = 0x0,
    },
    .condition = {
        .rule = COND_NEVER,
        .nSkip = 0x0,
    },
    .pktConf = {
        .bFsOff = 0x0,
        .bUseCrc = 0x1,
        .bCrcIncSw = 0x0,
        .bCrcIncHdr = 0x0,
    },
    .numHdrBits = 0x10,
    .pktLen = 0x0, /* set by driver */
    .startConf = {
        .bExtTxTrig = 0x0,
        .inputMode = 0x0,
        .source = 0x0,
    },
    .preTrigger = {
        .triggerType = TRIG_REL_START,
        .bEnaCmd = 0x0,
        .triggerNo = 0x0,
        .pastTrig = 0x1,
    },
    .preTime = 0x00000000,
    .syncWord = 0x0055904E,
    .pPkt = 0, /* set by driver */
};

/* CMD_PROP_RX_ADV: Proprietary Mode Advanced Receive Command */
rfc_CMD_PROP_RX_ADV_t rf_cmd_prop_rx_adv =
{
    .commandNo = CMD_PROP_RX_ADV,
    .status = IDLE,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger = {
        .triggerType = TRIG_NOW,
        .bEnaCmd = 0x0,
        .triggerNo = 0x0,
        .pastTrig = 0x0,
    },
    .condition = {
        .rule = COND_NEVER,
        .nSkip = 0x0,
    },
    .pktConf = {
        .bFsOff = 0x0,
        .bRepeatOk = 0x1,
        .bRepeatNok = 0x1,
        .bUseCrc = 0x1,
        .bCrcIncSw = 0x0,
        .bCrcIncHdr = 0x0,
        .endType = 0x0,
        .filterOp = 0x1,
    },
    .rxConf = {
        .bAutoFlushIgnored = 0x1,
        .bAutoFlushCrcErr = 0x1,
        .bIncludeHdr = 0x0,
        .bIncludeCrc = 0x0,
        .bAppendRssi = 0x1,
        .bAppendTimestamp = 0x0,
        .bAppendStatus = 0x1,
    },
    .syncWord0 = 0x0055904E,
    .syncWord1 = 0x00000000,
    .maxPktLen = 0x0, /* set by driver */
    .hdrConf = {
        .numHdrBits = 0x10,
        .lenPos = 0x0,
        .numLenBits = 0x0B,
    },
    .addrConf = {
        .addrType = 0x0,
        .addrSize = 0x0,
        .addrPos = 0x0,
        .numAddr = 0x0,
    },
    .lenOffset = 0xFC,
    .endTrigger = {
        .triggerType = TRIG_NEVER,
        .bEnaCmd = 0x0,
        .triggerNo = 0x0,
        .pastTrig = 0x0,
    },
    .endTime = 0x00000000,
    .pAddr = 0, /* set by driver */
    .pQueue = 0, /* set by driver */
    .pOutput = 0, /* set by driver */
};
