/*
 *  Copyright (c) 2018, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc13x2
 * @{
 *
 * @file
 * @brief           CC13x2 IEEE 802.15.4 netdev driver
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include <stdio.h>
#include <assert.h>

#include "random.h"
#include "iolist.h"
#include "rf_prop_internal.h"
#include "cc13x2_rf_internal.h"
#include "cc26xx_cc13xx_int.h"

#include <driverlib/chipinfo.h>
#include <driverlib/gpio.h>
#include <driverlib/ioc.h>
#include <driverlib/osc.h>
#include <driverlib/prcm.h>
#include <driverlib/rf_common_cmd.h>
#include <driverlib/rf_data_entry.h>
#include <driverlib/rf_ieee_cmd.h>
#include <driverlib/rf_ieee_mailbox.h>
#include <driverlib/rf_prop_mailbox.h>
#include <driverlib/rf_mailbox.h>
#include <driverlib/rfc.h>
#include <inc/hw_ccfg.h>
#include <inc/hw_fcfg1.h>
#include <inc/hw_memmap.h>
#include <inc/hw_prcm.h>
#include <rf_patches/rf_patch_cpe_ieee_802_15_4.h>

#define CC1352_RF_CMD0 0x0607

/* phy state as defined by openthread */
volatile cc13x2_PropPhyState_t _cc13x2_rf_prop_state;

/* set to max transmit power by default */
static output_config_t const *sCurrentOutputPower = &(rgOutputPower[0]);

static uint32_t sPropOverrides[]  =
{
    (uint32_t)0x00F788D3,
    ADI_2HALFREG_OVERRIDE(0,16,0x8,0x8,17,0x1,0x1),
    HW_REG_OVERRIDE(0x609C,0x001A),
    (uint32_t)0x000188A3,
    ADI_HALFREG_OVERRIDE(0,61,0xF,0xD),
    HW_REG_OVERRIDE(0x6028,0x001A),
    (uint32_t)0xFFFFFFFF,
};

/*
 * Number of retry counts left to the currently transmitting frame.
 *
 * Initialized when a frame is passed to be sent over the air, and decremented
 * by the radio ISR every time the transmit command string fails to receive a
 * corresponding ack.
 */
static volatile unsigned int sTransmitRetryCount = 0;

/*
 * Offset of the radio timer from the rtc.
 *
 * Used when we start and stop the RAT on enabling and disabling of the rf
 * core.
 */
static uint32_t sRatOffset = 0;

/*
 * Radio command structures that run on the CM0.
 */
// clang-format off
static volatile __attribute__((aligned(4))) rfc_CMD_SYNC_START_RAT_t       sStartRatCmd;
static volatile __attribute__((aligned(4))) rfc_CMD_PROP_RADIO_DIV_SETUP_t sRadioSetupCmd;
static volatile __attribute__((aligned(4))) rfc_CMD_FS_t sFsCmd;

static volatile __attribute__((aligned(4))) rfc_CMD_FS_POWERDOWN_t         sFsPowerdownCmd;
static volatile __attribute__((aligned(4))) rfc_CMD_SYNC_STOP_RAT_t        sStopRatCmd;

static volatile __attribute__((aligned(4))) rfc_CMD_CLEAR_RX_t             sClearReceiveQueueCmd;

static volatile __attribute__((aligned(4))) rfc_CMD_PROP_RX_ADV_t          sReceiveCmd;
static volatile __attribute__((aligned(4))) rfc_CMD_PROP_TX_ADV_t          sTransmitCmd;
// clang-format on

static volatile uint16_t _channel;

/*
 * Structure containing radio statistics.
 */
static __attribute__((aligned(4))) rfc_propRxOutput_t sRfStats;

/*
 * Two receive buffers entries with room for 1 max IEEE802.15.4 frame in each
 *
 * These will be setup in a circular buffer configuration by /ref sRxDataQueue.
 */
#define RX_BUF_SIZE 144
static __attribute__((aligned(4))) uint8_t sRxBuf0[RX_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t sRxBuf1[RX_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t sRxBuf2[RX_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t sRxBuf3[RX_BUF_SIZE];

#define TX_BUF_SIZE 144
static uint8_t sTxBuf0[TX_BUF_SIZE] __attribute__((aligned(4)));

volatile unsigned sFlags;

/*
 * The RX Data Queue used by @ref sReceiveCmd.
 */
static __attribute__((aligned(4))) dataQueue_t sRxDataQueue = {0};

/*
 * OpenThread data primitives
 */
//static otRadioFrame sTransmitFrame;
static uint_fast8_t sTransmitError;

static volatile bool sTxCmdChainDone = false;

/* network stack ISR handlers */
static void (*_irq_handler)(void*);
static void *_irq_handler_arg;

/**
 * Initialize the RX/TX buffers.
 *
 * Zeros out the receive and transmit buffers and sets up the data structures
 * of the receive queue.
 */
static void rfCoreInitBufs(void)
{
    rfc_dataEntry_t *entry;
    memset(sRxBuf0, 0, sizeof(sRxBuf0));
    memset(sRxBuf1, 0, sizeof(sRxBuf1));
    memset(sRxBuf2, 0, sizeof(sRxBuf2));
    memset(sRxBuf3, 0, sizeof(sRxBuf3));

    entry               = (rfc_dataEntry_t *)sRxBuf0;
    entry->pNextEntry   = sRxBuf1;
    entry->config.lenSz = DATA_ENTRY_LENSZ_BYTE;
    entry->length       = sizeof(sRxBuf0) - sizeof(rfc_dataEntry_t);

    entry               = (rfc_dataEntry_t *)sRxBuf1;
    entry->pNextEntry   = sRxBuf2;
    entry->config.lenSz = DATA_ENTRY_LENSZ_BYTE;
    entry->length       = sizeof(sRxBuf1) - sizeof(rfc_dataEntry_t);

    entry               = (rfc_dataEntry_t *)sRxBuf2;
    entry->pNextEntry   = sRxBuf3;
    entry->config.lenSz = DATA_ENTRY_LENSZ_BYTE;
    entry->length       = sizeof(sRxBuf2) - sizeof(rfc_dataEntry_t);

    entry               = (rfc_dataEntry_t *)sRxBuf3;
    entry->pNextEntry   = sRxBuf0;
    entry->config.lenSz = DATA_ENTRY_LENSZ_BYTE;
    entry->length       = sizeof(sRxBuf3) - sizeof(rfc_dataEntry_t);
}

/**
 * Initialize the RX command structure.
 *
 * Sets the default values for the receive command structure.
 */
static void rfCoreInitReceiveParams(void)
{
    // clang-format off
    static const rfc_CMD_PROP_RX_ADV_t cReceiveCmd =
    {
        .commandNo                  = CMD_PROP_RX_ADV,
        .status                     = IDLE,
        .pNextOp                    = NULL,
        .startTime                  = 0u,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  =
        {
            .rule                   = COND_NEVER,
        },
        .pktConf                    =
        {
            .bFsOff                 = 0,
            .bRepeatOk              = 1,
            .bRepeatNok             = 1,
            .bUseCrc                = 1,
            .bCrcIncSw              = 0,
            .bCrcIncHdr             = 0,
            .endType                = 0,
            .filterOp               = 1,
        },
        .rxConf                     =
        {
            .bAutoFlushIgnored      = 1,
            .bAutoFlushCrcErr       = 1,
            .bIncludeHdr            = 0,
            .bIncludeCrc            = 0,
            .bAppendRssi            = 1,
            .bAppendTimestamp       = 0,
            .bAppendStatus          = 1,
        },
        .syncWord0                  = 0x0055904E,
        .syncWord1                  = 0x00000000,
        .maxPktLen                  = 0,
        .hdrConf                    =
        {
            .numHdrBits             = 0x10,
            .lenPos                 = 0,
            .numLenBits             = 0x0B,
        },
        .addrConf                   =
        {
            .addrType               = 0,
            .addrSize               = 0,
            .addrPos                = 0,
            .numAddr                = 0,
        },
        .lenOffset                  = 0xFC,
        .endTrigger                 =
        {
            .triggerType            = TRIG_NEVER,
            .bEnaCmd                = 0x0,
            .triggerNo              = 0x0,
            .pastTrig               = 0x0,
        },
        .endTime                    = 0x00000000,
        .pAddr                      = 0,
        .pQueue                     = 0,
        .pOutput                    = 0
    };
    // clang-format on
    sReceiveCmd = cReceiveCmd;

    sReceiveCmd.pQueue  = &sRxDataQueue;
    sReceiveCmd.pOutput = (uint8_t *)&sRfStats;
}

/**
 * Sends the direct abort command to the radio core.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t rfCoreExecuteAbortCmd(void)
{
    return (RFCDoorbellSendTo(CMDR_DIR_CMD(CMD_ABORT)) & 0xFF);
}

/**
 * Sends the direct ping command to the radio core.
 *
 * Check that the Radio core is alive and able to respond to commands.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t rfCoreExecutePingCmd(void)
{
    return (RFCDoorbellSendTo(CMDR_DIR_CMD(CMD_PING)) & 0xFF);
}

/**
 * Sends the immediate clear rx queue command to the radio core.
 *
 * Uses the radio core to mark all of the entries in the receive queue as
 * pending. This is used instead of clearing the entries manually to avoid race
 * conditions between the main processor and the radio core.
 *
 * @param [in] aQueue A pointer to the receive queue to be cleared.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t rfCoreClearReceiveQueue(dataQueue_t *aQueue)
{
    /* memset skipped because sClearReceiveQueueCmd has only 2 members and padding */
    sClearReceiveQueueCmd.commandNo = CMD_CLEAR_RX;
    sClearReceiveQueueCmd.pQueue    = aQueue;

    return (RFCDoorbellSendTo((uint32_t)&sClearReceiveQueueCmd) & 0xFF);
}

/**
 * Sends the tx command to the radio core.
 *
 * Sends the packet to the radio core to be sent asynchronously.
 *
 * @note @ref aPsdu *must* be 4 byte aligned and not include the FCS.
 *
 * @param [in] aPsdu A pointer to the data to be sent.
 * @param [in] aLen  The length in bytes of data pointed to by PSDU.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t rfCoreSendTransmitCmd(uint8_t *aPsdu, uint8_t aLen)
{
    // clang-format off
    static const rfc_CMD_PROP_TX_ADV_t cTransmitCmd =
    {
        .commandNo                  = CMD_IEEE_TX,
        .status                     = IDLE,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_NEVER,
        },
        .pNextOp                    = NULL,
        .pktConf                    =
        {
            .bFsOff                 = 0,
            .bUseCrc                = 1,
            .bCrcIncSw              = 0,
            .bCrcIncHdr             = 0,
        },
        .numHdrBits                 = 0x10,
        .startConf                  =
        {
            .bExtTxTrig             = 0,
            .inputMode              = 0,
            .source                 = 0,
        },
        .preTrigger                 =
        {
            .triggerType            = TRIG_REL_START,
            .bEnaCmd                = 0,
            .triggerNo              = 0,
            .pastTrig               = 1,
        },
        .preTime                    = 0x00000000,
        .syncWord                   = 0x0055904E,
    };
    // clang-format on

    /* reset retry count */
    sTransmitRetryCount = 0;

    sTransmitCmd = cTransmitCmd;
    /* no need to look for an ack if the tx operation was stopped */
    sTransmitCmd.pktLen = aLen;
    sTransmitCmd.pPkt = aPsdu;

    if (aPsdu[0] & IEEE802154_ACK_REQUEST)
    {
        /* XXX: ack? */
    }

    return (RFCDoorbellSendTo((uint32_t)&sTransmitCmd) & 0xFF);
}

/**
 * Sends the rx command to the radio core.
 *
 * Sends the pre-built receive command to the radio core. This sets up the
 * radio to receive packets according to the settings in the global rx command.
 *
 * @note This function does not alter any of the parameters of the rx command.
 * It is only concerned with sending the command to the radio core. See @ref
 * rfSetPanId for an example of how the rx settings are set changed.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command completed correctly.
 */
static uint_fast8_t rfCoreSendReceiveCmd(void)
{
    sReceiveCmd.status = IDLE;
    return (RFCDoorbellSendTo((uint32_t)&sReceiveCmd) & 0xFF);
}

static uint_fast8_t rfCoreSendFsCmd(uint16_t frequency, uint16_t fractFreq)
{
    // clang-format off
    static const rfc_CMD_FS_t cFsCmd =
    {
        .commandNo                  = CMD_FS,
        .status                     = IDLE,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_NEVER,
        },
        .frequency                  = 0x0393,
        .fractFreq                  = 0,
        .synthConf                  =
        {
            .bTxMode                = 0,
            .refFreq                = 0,
        },
    };
    // clang-format on
    sFsCmd = cFsCmd;

    sFsCmd.frequency = frequency;
    sFsCmd.fractFreq = fractFreq;

    return (RFCDoorbellSendTo((uint32_t)&sFsCmd) & 0xFF);
}

/**
 * Enables the cpe0 and cpe1 radio interrupts.
 *
 * Enables the @ref IRQ_LAST_COMMAND_DONE and @ref IRQ_LAST_FG_COMMAND_DONE to
 * be handled by the @ref RFCCPE0IntHandler interrupt handler.
 */
static void rfCoreSetupInt(void)
{
    bool interruptsWereDisabled;

    assert(PRCMRfReady());

    interruptsWereDisabled = IntMasterDisable();

    /* Set all interrupt channels to CPE0 channel, error to CPE1 */
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEISL) = IRQ_INTERNAL_ERROR;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) = IRQ_LAST_COMMAND_DONE | IRQ_LAST_FG_COMMAND_DONE;

    IntPendClear(INT_RFC_CPE_0);
    IntPendClear(INT_RFC_CPE_1);
    IntEnable(INT_RFC_CPE_0);
    IntEnable(INT_RFC_CPE_1);

    if (!interruptsWereDisabled)
    {
        IntMasterEnable();
    }
}

/**
 * Disables and clears the cpe0 and cpe1 radio interrupts.
 */
static void rfCoreStopInt(void)
{
    bool interruptsWereDisabled;

    interruptsWereDisabled = IntMasterDisable();

    /* clear and disable interrupts */
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = 0x0;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) = 0x0;

    IntUnregister(INT_RFC_CPE_0);
    IntUnregister(INT_RFC_CPE_1);
    IntPendClear(INT_RFC_CPE_0);
    IntPendClear(INT_RFC_CPE_1);
    IntDisable(INT_RFC_CPE_0);
    IntDisable(INT_RFC_CPE_1);

    if (!interruptsWereDisabled)
    {
        IntMasterEnable();
    }
}

/**
 * Turns on the radio core.
 *
 * Sets up the power and resources for the radio core.
 * - switches the high frequency clock to the xosc crystal
 * - sets the mode for the radio core to IEEE 802.15.4
 * - initializes the rx buffers and command
 * - powers on the radio core power domain
 * - enables the radio core power domain
 * - sets up the interrupts
 * - sends the ping command to the radio core to make sure it is running
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command was received.
 */
static uint_fast8_t rfCorePowerOn(void)
{
    bool interruptsWereDisabled;
    bool oscSourceSwitch = false;

    /* Request the HF XOSC as the source for the HF clock. Needed before we can
     * use the FS. This will only request, it will _not_ perform the switch.
     */
    if (OSCClockSourceGet(OSC_SRC_CLK_HF) != OSC_XOSC_HF)
    {
        /* Request to switch to the crystal to enable radio operation. It takes a
         * while for the XTAL to be ready so instead of performing the actual
         * switch, we do other stuff while the XOSC is getting ready.
         */
        OSCClockSourceSet(OSC_SRC_CLK_HF, OSC_XOSC_HF);
        oscSourceSwitch = true;
    }

    /* Set of RF Core data queue. Circular buffer, no last entry */
    sRxDataQueue.pCurrEntry = sRxBuf0;
    sRxDataQueue.pLastEntry = NULL;

    rfCoreInitBufs();

    /*
     * Trigger a switch to the XOSC, so that we can subsequently use the RF FS
     * This will block until the XOSC is actually ready, but give how we
     * requested it early on, this won't be too long a wait.
     * This should be done before starting the RAT.
     */
    if (oscSourceSwitch)
    {
        /* Block until the high frequency clock source is ready */
        while (!OSCHfSourceReady())
            ;

        /* Switch the HF clock source (cc26xxware executes this from ROM) */
        OSCHfSourceSwitch();
    }

    interruptsWereDisabled = IntMasterDisable();

    /* Enable RF Core power domain */
    PRCMPowerDomainOn(PRCM_DOMAIN_RFCORE);

    while (PRCMPowerDomainStatus(PRCM_DOMAIN_RFCORE) != PRCM_DOMAIN_POWER_ON)
        ;

    PRCMDomainEnable(PRCM_DOMAIN_RFCORE);
    PRCMLoadSet();

    while (!PRCMLoadGet())
        ;

    rfCoreSetupInt();

    if (!interruptsWereDisabled)
    {
        IntMasterEnable();
    }

    /* Let CPE boot */
    RFCClockEnable();

    /* Enable ram clocks for patches */
    RFCDoorbellSendTo(CMDR_DIR_CMD_2BYTE(CC1352_RF_CMD0, RFC_PWR_PWMCLKEN_MDMRAM | RFC_PWR_PWMCLKEN_RFERAM));

    /* Send ping (to verify RFCore is ready and alive) */
    return rfCoreExecutePingCmd();
}

/**
 * Turns off the radio core.
 *
 * Switches off the power and resources for the radio core.
 * - disables the interrupts
 * - disables the radio core power domain
 * - powers off the radio core power domain
 * - switches the high frequency clock to the rcosc to save power
 */
static void rfCorePowerOff(void)
{
    rfCoreStopInt();

    PRCMDomainDisable(PRCM_DOMAIN_RFCORE);
    PRCMLoadSet();

    while (!PRCMLoadGet())
        ;

    PRCMPowerDomainOff(PRCM_DOMAIN_RFCORE);

    while (PRCMPowerDomainStatus(PRCM_DOMAIN_RFCORE) != PRCM_DOMAIN_POWER_OFF)
        ;

    if (OSCClockSourceGet(OSC_SRC_CLK_HF) != OSC_RCOSC_HF)
    {
        /* Request to switch to the RC osc for low power mode. */
        OSCClockSourceSet(OSC_SRC_CLK_HF, OSC_RCOSC_HF);
        /* Switch the HF clock source (cc26xxware executes this from ROM) */
        OSCHfSourceSwitch();
    }
}

/**
 * Applies CPE patche to the radio.
 */
static void rfCoreApplyPatch(void)
{
    rf_patch_cpe_ieee_802_15_4();

    /* disable ram bus clocks */
    RFCDoorbellSendTo(CMDR_DIR_CMD_2BYTE(CC1352_RF_CMD0, 0));
}

/**
 * Sends the setup command string to the radio core.
 *
 * Enables the clock line from the RTC to the RF core RAT. Enables the RAT
 * timer and sets up the radio in IEEE mode.
 *
 * @return The value from the command status register.
 * @retval CMDSTA_Done The command was received.
 */
static uint_fast16_t rfCoreSendEnableCmd(void)
{
    uint8_t       doorbellRet;
    bool          interruptsWereDisabled;
    uint_fast16_t ret;

    // clang-format off
    static const rfc_CMD_SYNC_START_RAT_t cStartRatCmd =
    {
        .commandNo                  = CMD_SYNC_START_RAT,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_STOP_ON_FALSE,
        },
    };
    static const rfc_CMD_PROP_RADIO_DIV_SETUP_t cRadioSetupCmd =
    {
        .commandNo                  = CMD_PROP_RADIO_DIV_SETUP,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_NEVER,
        },
        .modulation                 =
        {
            .modType                = 0x1,
            .deviation              = 0x64,
            .deviationStepSz        = 0x0,
        },
        .symbolRate                 =
        {
            .preScale               = 0xF,
            .rateWord               = 0x8000,
            .decimMode              = 0x0,
        },
        .rxBw                       = 0x52,
        .preamConf                  =
        {
            .nPreamBytes            = 0x7,
            .preamMode              = 0x0,
        },
        .formatConf                 =
        {
            .nSwBits                = 0x18,
            .bBitReversal           = 0x0,
            .bMsbFirst              = 0x1,
            .fecMode                = 0x0,
            .whitenMode             = 0x7,
        },
        .config =
        {
            .frontEndMode           = 0x0,
            .biasMode               = 0x0,
            .analogCfgMode          = 0x0,
            .bNoFsPowerUp           = 0x0,
        },
        .txPower                    = 0xB224,
        .pRegOverride               = 0,
        .centerFreq                 = 0x0393,
        .intFreq                    = 0x8000,
        .loDivider                  = 0x05,
    };
    // clang-format on
    /* turn on the clock line to the radio core */
    HWREGBITW(AON_RTC_BASE + AON_RTC_O_CTL, AON_RTC_CTL_RTC_UPD_EN_BITN) = 1;

    /* initialize the rat start command */
    sStartRatCmd         = cStartRatCmd;
    sStartRatCmd.pNextOp = (rfc_radioOp_t *)&sRadioSetupCmd;
    sStartRatCmd.rat0    = sRatOffset;

    /* initialize radio setup command */
    sRadioSetupCmd = cRadioSetupCmd;
    /* initally set the radio tx power to the max */
    sRadioSetupCmd.txPower             = sCurrentOutputPower->value;
    sRadioSetupCmd.pRegOverride        = sPropOverrides;
    sRadioSetupCmd.config.frontEndMode = 0; /* differential */
    sRadioSetupCmd.config.biasMode     = 0; /* internal bias */
    sRadioSetupCmd.centerFreq          = PROP_MODE_CENTER_FREQ;
    sRadioSetupCmd.loDivider           = PROP_MODE_LO_DIVIDER;

    interruptsWereDisabled = IntMasterDisable();

    //rfCoreApplyPatch();

    doorbellRet = (RFCDoorbellSendTo((uint32_t)&sStartRatCmd) & 0xFF);
    if (CMDSTA_Done != doorbellRet) {
        ret = doorbellRet;
        goto exit;
    }

    /* synchronously wait for the CM0 to stop executing */
    while ((HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_LAST_COMMAND_DONE) == 0x00)
        ;

    ret = sRadioSetupCmd.status;

exit:

    if (!interruptsWereDisabled)
    {
        IntMasterEnable();
    }

    return ret;
}

/**
 * Sends the shutdown command string to the radio core.
 *
 * Powers down the frequency synthesizer and stops the RAT.
 *
 * @note synchronously waits until the command string completes.
 *
 * @return The status of the RAT stop command.
 * @retval DONE_OK The command string executed properly.
 */
static uint_fast16_t rfCoreSendDisableCmd(void)
{
    uint8_t       doorbellRet;
    bool          interruptsWereDisabled;
    uint_fast16_t ret;

    // clang-format off
    static const rfc_CMD_FS_POWERDOWN_t cFsPowerdownCmd =
    {
        .commandNo                  = CMD_FS_POWERDOWN,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_ALWAYS,
        },
    };
    static const rfc_CMD_SYNC_STOP_RAT_t cStopRatCmd =
    {
        .commandNo                  = CMD_SYNC_STOP_RAT,
        .startTrigger               =
        {
            .triggerType            = TRIG_NOW,
        },
        .condition                  = {
            .rule                   = COND_NEVER,
        },
    };
    // clang-format on

    HWREGBITW(AON_RTC_BASE + AON_RTC_O_CTL, AON_RTC_CTL_RTC_UPD_EN_BITN) = 1;

    /* initialize the command to power down the frequency synth */
    sFsPowerdownCmd         = cFsPowerdownCmd;
    sFsPowerdownCmd.pNextOp = (rfc_radioOp_t *)&sStopRatCmd;

    sStopRatCmd = cStopRatCmd;

    interruptsWereDisabled = IntMasterDisable();

    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_LAST_COMMAND_DONE;

    doorbellRet = (RFCDoorbellSendTo((uint32_t)&sFsPowerdownCmd) & 0xFF);
    if (CMDSTA_Done != doorbellRet) {
        ret = doorbellRet;
        goto exit;
    };

    /* synchronously wait for the CM0 to stop */
    while ((HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_LAST_COMMAND_DONE) == 0x00)
        ;

    ret = sStopRatCmd.status;

    if (sStopRatCmd.status == DONE_OK)
    {
        sRatOffset = sStopRatCmd.rat0;
    }

exit:

    if (!interruptsWereDisabled)
    {
        IntMasterEnable();
    }

    return ret;
}

/**
 * Error interrupt handler.
 */
void _isr_rfc_cpe1(void)
{
    /* Clear INTERNAL_ERROR interrupt flag */
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = 0x7FFFFFFF;
}

/**
 * Command done handler.
 */
void _isr_rfc_cpe0(void)
{
    if (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_LAST_COMMAND_DONE)
    {
        HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_LAST_COMMAND_DONE;

        if (_cc13x2_rf_prop_state == cc13x2_stateReceive && sReceiveCmd.status != ACTIVE && sReceiveCmd.status != IEEE_SUSPENDED)
        {
            /* the rx command was probably aborted to change the channel */
            _cc13x2_rf_prop_state = cc13x2_stateSleep;
        }
    }

    if (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_LAST_FG_COMMAND_DONE)
    {
        HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_LAST_FG_COMMAND_DONE;

        if (_cc13x2_rf_prop_state == cc13x2_stateTransmit)
        {
            if (sTransmitCmd.pPkt[0] & IEEE802154_ACK_REQUEST)
            {
                /* XXX: ack? */
            }
            else
            {
                /* The TX command was either stopped or we are not looking for
                 * an ack */
                switch (sTransmitCmd.status)
                {
                case PROP_DONE_OK:
                    sTransmitError = 0;
                    break;

                case PROP_ERROR_NO_SETUP:
                case PROP_ERROR_NO_FS:
                    sTransmitError = -1;
                    break;

                case PROP_ERROR_TXUNF:
                    sTransmitError = -1;
                    break;

                default:
                    sTransmitError = -1;
                    break;
                }

                /* signal polling function we are done transmitting */
                sTxCmdChainDone = true;
            }
        }
    }

    if (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_RX_OK) {
        sFlags |= 1;
        HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_RX_OK;
    }

    if (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) & IRQ_TX_DONE) {
        sFlags |= 2;
        _cc13x2_rf_prop_state = cc13x2_stateReceive;
        HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~IRQ_TX_DONE;
    }

    if (sFlags) {
        if (_irq_handler) {
            _irq_handler(_irq_handler_arg);
        }
    }
}

/**
 * Function documented in platform-cc13x2.h
 */
void cc13x2_rf_prop_init(void)
{
    /* Populate the RX parameters data structure with default values */
    rfCoreInitReceiveParams();

    _cc13x2_rf_prop_state = cc13x2_stateDisabled;
    cc26xx_cc13xx_ints.isr_rfc_cpe0 = _isr_rfc_cpe0;
    cc26xx_cc13xx_ints.isr_rfc_cpe1 = _isr_rfc_cpe1;
}

/**
 * Function documented in platform/radio.h
 */
int_fast8_t cc13x2_rf_prop_enable(void)
{
    int_fast8_t error = -1;

    if (_cc13x2_rf_prop_state == cc13x2_stateSleep)
    {
        error = 0;
    }
    else if (_cc13x2_rf_prop_state == cc13x2_stateDisabled)
    {
        if (rfCorePowerOn() != CMDSTA_Done) {
            error = -1;
            goto exit;
        }

        if (rfCoreSendEnableCmd() != PROP_DONE_OK) {
            error = -1;
            goto exit;
        }

        /* The CC1352R1_LAUNCHXL development board has an RF switch to enable
         * usage of the same antenna between the 2.4G and sub-1G RF paths. This
         * is connected to IOID 30 on the LaunchPad, and a low voltage will set
         * the switch to the 2.4G path.
         */

        /* Enable the peripheral domain for the GPIO module */
        PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
        /* Enable the GPIO module in all power modes, this will not be turned off */
        PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
        PRCMPeripheralSleepEnable(PRCM_PERIPH_GPIO);
        PRCMPeripheralDeepSleepEnable(PRCM_PERIPH_GPIO);
        /* Ensure the PRCM module is synced with these settings */
        PRCMLoadSet();
        /* Enable output on RF switch */
        IOCPinTypeGpioOutput(IOID_30);
        /* Set the RF Switch to the 2.4 GHz path */
        GPIO_writeDio(IOID_30, 0);

        _cc13x2_rf_prop_state = cc13x2_stateSleep;
        error  = 0;
    }

exit:

    if (error == -1)
    {
        rfCorePowerOff();
        _cc13x2_rf_prop_state = cc13x2_stateDisabled;
    }

    return error;
}

int_fast8_t cc13x2_rf_prop_disable(void)
{
    int_fast8_t error = -1;

    if (_cc13x2_rf_prop_state == cc13x2_stateDisabled)
    {
        error = 0;
    }
    else if (_cc13x2_rf_prop_state == cc13x2_stateSleep)
    {
        rfCoreSendDisableCmd();
        /* we don't want to fail if this command string doesn't work, just turn
         * off the whole core
         */
        rfCorePowerOff();
        _cc13x2_rf_prop_state = cc13x2_stateDisabled;
        error  = 0;
    }

    return error;
}

/**
 * Function documented in platform/radio.h
 */
int8_t cc13x2_rf_prop_get_txpower(void)
{
    return sCurrentOutputPower->dbm;
}

int_fast8_t cc13x2_rf_prop_set_txpower(int8_t aPower)
{
    unsigned int           i;
    output_config_t const *powerCfg = &(rgOutputPower[0]);

    for (i = 1; i < OUTPUT_CONFIG_COUNT; i++)
    {
        if (rgOutputPower[i].dbm >= aPower)
        {
            powerCfg = &(rgOutputPower[i]);
        }
        else
        {
            break;
        }
    }

    sCurrentOutputPower = powerCfg;

    return 0;
}

int_fast8_t rfSleep(void)
{
    int_fast8_t error = -1;

    if (_cc13x2_rf_prop_state == cc13x2_stateSleep)
    {
        error = 0;
    }
    else if (_cc13x2_rf_prop_state == cc13x2_stateReceive)
    {
        if (rfCoreExecuteAbortCmd() != CMDSTA_Done)
        {
            error = -1;
        }
        else
        {
            _cc13x2_rf_prop_state = cc13x2_stateSleep;
        }
    }

    return error;
}

int8_t cc13x2_rf_prop_get_rssi(void)
{
    return sRfStats.lastRssi;
}

int_fast8_t cc13x2_rf_prop_rx_start(void)
{
    int_fast8_t error = -1;

    if (_cc13x2_rf_prop_state == cc13x2_stateSleep)
    {
        _cc13x2_rf_prop_state = cc13x2_stateReceive;

        /* initialize the receive command
         * XXX: no memset here because we assume init has been called and we
         *      may have changed some values in the rx command
         */
        if (!(rfCoreSendReceiveCmd() == CMDSTA_Done)) {
            error = -1;
            goto exit;
        }
        error = 0;
    }
    else if (_cc13x2_rf_prop_state == cc13x2_stateReceive)
    {
        if (sReceiveCmd.status == ACTIVE)
        {
            /* we are already receiving */
            _cc13x2_rf_prop_state = cc13x2_stateReceive;
            error  = -1;
        }
        else
        {
            /* we have either not fallen back into our receive command or
             * we are running on the wrong channel. Either way assume the
             * caller correctly called us and abort all running commands.
             */
            if (!(rfCoreExecuteAbortCmd() == CMDSTA_Done)) {
                error = -1;
                goto exit;
            }

            /* any frames in the queue will be for the old channel */
            if (!(rfCoreClearReceiveQueue(&sRxDataQueue) == CMDSTA_Done)) {
                error = -1;
                goto exit;
            }

            if (!(rfCoreSendReceiveCmd() == CMDSTA_Done)) {
                error = -1;
                goto exit;
            }

            _cc13x2_rf_prop_state = cc13x2_stateReceive;
             error  = 0;
        }
    }

exit:
    return error;
}

int_fast8_t cc13x2_rf_prop_rx_stop(void)
{
    int_fast8_t error = 0;

    if (_cc13x2_rf_prop_state == cc13x2_stateReceive)
    {
        if (!(rfCoreExecuteAbortCmd() == CMDSTA_Done)) {
            error = -1;
            goto exit;
        }

        _cc13x2_rf_prop_state = cc13x2_stateSleep;
        error  = 0;
    }

exit:
    return error;
}

uint8_t cc13x2_rf_prop_get_chan(void)
{
    return _channel;
}

void cc13x2_rf_prop_set_chan(uint16_t channel)
{
    if (_channel == channel) {
        return;
    }

    int rx_was_active = (_cc13x2_rf_prop_state == cc13x2_stateReceive);

    if (rx_was_active) {
        cc13x2_rf_prop_rx_stop();
    }

    const uint32_t new_freq = ieee802154_freq(channel);
    const uint16_t freq = (uint16_t)(new_freq / 1000);
    const uint16_t frac = (uint16_t)(((new_freq - (freq * 1000)) * 0x10000) / 1000);

    assert(rfCoreSendFsCmd(freq, frac) == CMDSTA_Done);

    _channel = channel;

    if (rx_was_active) {
        cc13x2_rf_prop_rx_start();
    }
    // TODO: handle cc13x2_rf_prop_state == cc13x2_stateTransmit

    return;
}

/**
 * Function documented in platform/radio.h
 */
void cc13x2_rf_prop_get_ieee_eui64(uint8_t *aIeeeEui64)
{
    uint8_t *    eui64;
    unsigned int i;

    /*
     * The IEEE MAC address can be stored two places. We check the Customer
     * Configuration was not set before defaulting to the Factory
     * Configuration.
     */
    eui64 = (uint8_t *)(CCFG_BASE + CCFG_O_IEEE_MAC_0);

    for (i = 0; i < 8; i++)
    {
        if (eui64[i] != CCFG_UNKNOWN_EUI64)
        {
            break;
        }
    }

    if (i >= 8)
    {
        /* The ccfg address was all 0xFF, switch to the fcfg */
        eui64 = (uint8_t *)(FCFG1_BASE + FCFG1_O_MAC_15_4_0);
    }

    /*
     * The IEEE MAC address is stored in network byte order.  The caller seems
     * to want the address stored in little endian format, which is backwards
     * of the conventions setup by @ref rfSetExtendedAddress.
     * rfSetExtendedAddress assumes that the address being passed to
     * it is in network byte order, so the caller of
     * rfSetExtendedAddress must swap the endianness before calling.
     *
     * It may be easier to have the caller of this function store the IEEE
     * address in network byte order.
     */
    for (i = 0; i < 8; i++)
    {
        aIeeeEui64[i] = eui64[(8 - 1) - i];
    }
}

void cc13x2_rf_prop_irq_set_handler(void(*handler)(void *), void * arg)
{
    _irq_handler = handler;
    _irq_handler_arg = arg;
}

int cc13x2_rf_prop_recv(void *buf, size_t len, netdev_ieee802154_rx_info_t *rx_info)
{
    rfc_ieeeRxCorrCrc_t *   crcCorr;
    rfc_dataEntryGeneral_t *curEntry, *startEntry;
    uint8_t                 rssi;
    (void)rssi;

    startEntry = (rfc_dataEntryGeneral_t *)sRxDataQueue.pCurrEntry;
    curEntry   = startEntry;

    /* loop through receive queue */
    do {
        uint8_t *payload = &(curEntry->data);

        if (curEntry->status == DATA_ENTRY_FINISHED) {
            uint8_t      payload_len;

            /* get the information appended to the end of the frame.
             * This array access looks like it is a fencepost error, but the
             * first byte is the number of bytes that follow.
             */
            payload_len = payload[0];
            crcCorr     = (rfc_ieeeRxCorrCrc_t *)&payload[len];
            rssi        = payload[payload_len - 1];

            if (crcCorr->status.bCrcErr == 0 && (payload_len - 2) < 144) {
                if (!buf) {
                    if (!len) {
                        return payload_len;
                    }
                    if (len == payload_len) {
                        /* drop packet */
                        curEntry->status = DATA_ENTRY_PENDING;
                        return 0;
                    }
                }

                if (payload_len > len) {
                    return -ENOSPC;
                }

                /* substract checksum bytes */
                payload_len -= 2;

                /* copy packet data */
                memcpy(buf, &(payload[1]), payload_len);

                /* save rx info */
                if (rx_info) {
                    rx_info->rssi = rssi;
                    rx_info->lqi = crcCorr->status.corr;
                }

                curEntry->status = DATA_ENTRY_PENDING;

                return payload_len;
            }
            else {
                puts("recv error fcs");
            }
            curEntry->status = DATA_ENTRY_PENDING;
        }

        curEntry = (rfc_dataEntryGeneral_t *)(curEntry->pNextEntry);
    } while (curEntry != startEntry);
    return 0;
}

int cc13x2_rf_prop_recv_avail(void)
{
    rfc_dataEntryGeneral_t *curEntry, *startEntry;

    startEntry = (rfc_dataEntryGeneral_t *)sRxDataQueue.pCurrEntry;
    curEntry   = startEntry;

    /* loop through receive queue */
    do {
        if (curEntry->status == DATA_ENTRY_FINISHED) {
            return 1;
        }

        curEntry = (rfc_dataEntryGeneral_t *)(curEntry->pNextEntry);
    } while (curEntry != startEntry);
    return 0;
}

int cc13x2_rf_prop_send(const iolist_t *iolist)
{
    unsigned error = -EAGAIN;

    if (_cc13x2_rf_prop_state == cc13x2_stateReceive)
    {
        size_t len = 0;
        uint8_t *bufpos = sTxBuf0;

        for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
            len += iol->iol_len;
            if (len > TX_BUF_SIZE) {
//                DEBUG("cc13x2_rf: error: packet too large (%u bytes to be sent)\n",
//                      (unsigned)len);
                error = -EOVERFLOW;
                goto exit;
            }

            memcpy(bufpos, iol->iol_base, iol->iol_len);
            bufpos += iol->iol_len;
        }

        _cc13x2_rf_prop_state = cc13x2_stateTransmit;

        if (!(rfCoreSendTransmitCmd(sTxBuf0, len) == CMDSTA_Done)) {
            error = -1;
            goto exit;
        }

        sTxCmdChainDone = false;

        return len;
    }

exit:
    _cc13x2_rf_prop_state = cc13x2_stateTransmit;

    return error;
}

unsigned cc13x2_rf_prop_irq_is_enabled(unsigned irq)
{
    return (HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) & irq) != 0;
}

void cc13x2_rf_prop_irq_enable(unsigned irq)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = ~irq;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) |= irq;
}

void cc13x2_rf_prop_irq_disable(unsigned irq)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) &= ~irq;
}

void cc13x2_rf_prop_set_pan(uint16_t aPanId)
{
    (void)aPanId;
}

uint16_t cc13x2_rf_prop_get_pan(void)
{
    return 0x0000;
}

/**
 * Function documented in platform/radio.h
 *
 * @note it is entirely possible for this function to fail, but there is no
 * valid way to return that error since the function prototype was changed.
 */
void cc13x2_set_short_addr(uint16_t aAddress)
{
    (void)aAddress;
    /* XXX: todo? */
}

unsigned cc13x2_rf_prop_get_flags(void)
{
    unsigned flags = sFlags;
    sFlags = 0;
    return flags;
}
