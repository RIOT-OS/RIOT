/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Manual test application for DW3000 UWB devices
 *
 * This test can be used to verify the essential operations of a DW3000 (or
 * DW3720) together with a MCU running RIOT. These include sending, receiving
 * and sleeping.
 *
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "fmt.h"
#include "macros/utils.h"
#include "net/ieee802154.h"
#include "shell.h"

#include "dw3000.h"
#include "deca_device_api.h"

/* PAN ID/short address */
#define PAN_ID              0xDECA
#define SHORT_ADDR_DEFAULT  0x0001

static dwt_config_t _config = {
    .chan = 9,
    .txPreambLength = DWT_PLEN_64,
    .rxPAC = DWT_PAC8,
    .txCode = 11,
    .rxCode = 11,
    .sfdType = DWT_SFD_IEEE_4Z,
    .dataRate = DWT_BR_6M8,
    .phrMode = DWT_PHRMODE_STD,
    .phrRate = DWT_PHRRATE_STD,
    .sfdTO = (64 + 1 + 8 - 8),   /* (plen + 1 + SFD length - PAC size) */
    .stsMode = DWT_STS_MODE_OFF, /* DWT_STS_MODE_1 | DWT_STS_MODE_SDC, */
    .stsLength = DWT_STS_LEN_64,
    .pdoaMode = DWT_PDOA_M0,     /* off */
};

#define FRAME_LEN_MAX   127
static uint8_t _rx_buffer[FRAME_LEN_MAX];
static uint8_t _tx_buffer[FRAME_LEN_MAX];

static uint8_t _seq_no = 0;
static uint16_t _self_address = SHORT_ADDR_DEFAULT;
static bool _sleeping = false;

static void _send_tx(uint16_t dest, uint8_t* buf, uint16_t len)
{
    network_uint16_t src = byteorder_htons(_self_address);
    network_uint16_t dst = byteorder_htons(dest);
    le_uint16_t pan      = byteorder_htols(PAN_ID);

    size_t hdr_len = ieee802154_set_frame_hdr(_tx_buffer,
                                              src.u8, 2,
                                              dst.u8, 2,
                                              pan, pan,
                                              IEEE802154_FCF_TYPE_DATA |
                                              IEEE802154_FCF_ACK_REQ |
                                              IEEE802154_FCF_PAN_COMP,
                                              _seq_no++);

    /* Size of payload capped by the remaining size in the buffer*/
    uint16_t payload_len = MIN(len, FRAME_LEN_MAX - hdr_len);
    memcpy(_tx_buffer + hdr_len, buf, payload_len);

    puts("[deca] TX sending (CRC bytes not included):");
    print_bytes_hex(_tx_buffer, hdr_len + payload_len);
    puts("");

    dwt_forcetrxoff();
    dwt_writetxdata(hdr_len + payload_len, _tx_buffer, 0);
    /* This function needs to include the two CRC bytes */
    dwt_writetxfctrl(hdr_len + payload_len + FCS_LEN, 0, 0);
    dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
}

static void _irq_tx_done_cb(const dwt_cb_data_t* dat)
{
    (void) dat;
    /* Either the ACK has been sent or a message was sent.
     * Either way, listen for new data. */
    dwt_rxenable(DWT_START_RX_IMMEDIATE);
    puts("[deca] TX complete");
}

static void _irq_rx_ok_cb(const dwt_cb_data_t* dat)
{
    (void) dat;
    uint16_t frame_len = dwt_getframelength(NULL);
    printf("[deca] RX ok with length %"PRIu16":\n", frame_len);

    if (frame_len <= FRAME_LEN_MAX) {
        dwt_readrxdata(_rx_buffer, frame_len, 0);
        print_bytes_hex(_rx_buffer, frame_len);
        puts("");
    }

    if (!(_rx_buffer[0] & IEEE802154_FCF_ACK_REQ)) {
        /* If no ACK is requested, continue RX immediately. Otherwise wait for
         * the interrupt and the callback in _irq_tx_done_cb() */
        dwt_rxenable(DWT_START_RX_IMMEDIATE);
    }
}

static void _irq_err_cb(const dwt_cb_data_t* dat)
{
    (void) dat;
    printf("[deca] RX error: 0x%"PRIx32"\n", dat->status);
    dwt_rxenable(DWT_START_RX_IMMEDIATE);
}

/* Note 1: These callbacks happen on the driver "isr" thread, but outside of
 *         ISR.
 * Note 2: The dwt_isr() takes care of clearing the status register, so contrary
 *         to the example using polling, no dwt_writesysstatuslo() is needed. */
dwt_callbacks_s _dwt_callbacks = {
    .cbTxDone = _irq_tx_done_cb,
    .cbRxOk = _irq_rx_ok_cb,
    .cbRxErr = _irq_err_cb,
};

static int _dw_send(int argc, char **argv)
{
    if (argc < 3) {
        puts("Usage: dw_send <dest (hex without 0x)> <payload>");
        return 0;
    }

    if (_sleeping) {
        puts("Device is sleeping, commands will likely not arrive.");
    }

    uint32_t dest = scn_u32_hex(argv[1], 4);
    _send_tx(dest, (uint8_t*) argv[2], strlen(argv[2]));

    return 0;
}

static int _dw_addr(int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage: dw_addr <addr (hex without 0x)>");
        return 0;
    }

    if (_sleeping) {
        puts("Device is sleeping, commands will likely not arrive.");
    }

    _self_address = scn_u32_hex(argv[1], 4);
    dwt_setaddress16(_self_address);

    return 0;
}

static int _dw_sleep(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("[deca] Going to deepsleep. Use dw_wakeup to wake up.");
    dwt_configuresleep(DWT_PGFCAL | DWT_GOTORX | DWT_RUNSAR | DWT_CONFIG,
                       DWT_WAKE_CSN | DWT_WAKE_WUP | DWT_SLP_EN);

    dw3000_hw_wakeup_pin_low();
    _sleeping = true;
    dwt_entersleep(DWT_DW_IDLE_RC);

    return 0;
}

static int _dw_wakeup(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("[deca] Waking up...");

    dw3000_hw_wakeup();
    _sleeping = false;

    return 0;
}

SHELL_COMMAND(dw_send, "Send UWB data frame", _dw_send);
SHELL_COMMAND(dw_addr, "Set UWB 16 bit MAC address", _dw_addr);
SHELL_COMMAND(dw_sleep, "Set DW3000 to deepsleep", _dw_sleep);
SHELL_COMMAND(dw_wakeup, "Wake up the DW3000 again", _dw_wakeup);

int main(void)
{
    dw3000_hw_init();
    dw3000_hw_init_interrupt();
    dw3000_hw_reset();

    dwt_probe((struct dwt_probe_s *)&dw3000_probe_interf);
    uint32_t dev_id = dwt_readdevid();
    printf("[deca init] detected device id: %"PRIx32"\n", dev_id);

    /* The API guide says it is recommended to check for idle rc before
     * dwt_initialse(), while libdeca does the other order. */
    while (!dwt_checkidlerc()) {};
    puts("[deca init] DW3xxx reached IDLE_RC");

    if (dwt_initialise(DWT_DW_IDLE) != DWT_SUCCESS) {
        puts("[deca init] Error initializing device");
        return 1;
    }

    /* After idle rc the SPI speed can be increased */
    dw3000_spi_speed_fast();

    if (dwt_configure(&_config) != DWT_SUCCESS) {
        puts("[deca init] Error configuring device to given parameters");
        return 1;
    }
    dwt_setrxtimeout(0);

    dwt_setpanid(PAN_ID);
    dwt_setaddress16(_self_address);

    dwt_configureframefilter(DWT_FF_ENABLE_802_15_4, DWT_FF_DATA_EN |
                                                     DWT_FF_ACK_EN);
    /* The ACK delay here is arbitrarily high so that the sender is ready for
     * receive again. It might be tuned to be lower. */
    dwt_enableautoack(250, 1);

    dwt_setcallbacks(&_dwt_callbacks);
    /* Note: Making the wrong choice of interrupts here can lead to weird
     *  problems, be aware.
     *  For example, using SYS_STATUS_ALL_RX_GOOD (which one might expect to be
     *  a mask for all good frame receptions) leads to RX errors. Why? (likely)
     *  Because interrupts trigger already when *only the preamble* was detected
     *  (looking at RXPRD), which seems to then cause some issues when
     *  interacting with the DW3000 at that time, leading to no RX GOOD, but
     *  instead an RX ERROR. In the end the values also used in libdeca seem
     *  to work well here.
     * Also, the ARFE interrupt (frame filter error) has to be set so to
     *  generate an interrupt when a frame NOT matching the filter is received.
     *  I would have expected that RX continues when a frame with the wrong
     *  destination is received, but subsequent frames to the correct
     *  destination do NOT arrive. This is especially weird since the DW3000
     *  user manual says the benefit is to "only interrupt the host processor
     *  when a frame arrives that passes the frame filtering criteria", but it
     *  seems an interrupt is needed either way. This (unanswered) forum post
     *  reported this too:
     *  https://forum.qorvo.com/t/frame-filtering-rejection-stops-rx-why/17671*/
    dwt_setinterrupt(DWT_INT_RXFCG_BIT_MASK | /* <-- RX success */
                     /* RX errors: */
                     DWT_INT_RXPHE_BIT_MASK |
                     DWT_INT_RXFCE_BIT_MASK |
                     DWT_INT_RXFSL_BIT_MASK |
                     DWT_INT_RXSTO_BIT_MASK |
                     DWT_INT_CIAERR_BIT_MASK |
                     DWT_INT_ARFE_BIT_MASK |
                     /* RX timeouts: */
                     DWT_INT_RXFTO_BIT_MASK |
                     DWT_INT_RXPTO_BIT_MASK |
                     /* TX done: */
                     DWT_INT_TXFRS_BIT_MASK,
                     0, DWT_ENABLE_INT_ONLY);

    puts("[deca init] Device initialized and configured");

    dwt_rxenable(DWT_START_RX_IMMEDIATE);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
