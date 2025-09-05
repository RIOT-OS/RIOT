/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ata8520e
 * @{
 *
 * @file
 * @brief       Device driver for Microchip ATA8520E transceiver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "ztimer.h"
#include "fmt.h"

#include "periph/spi.h"
#include "periph/gpio.h"

#include "board.h"
#include "ata8520e_internals.h"
#include "ata8520e.h"

#define ENABLE_DEBUG 0
/* Warning: to correctly display the debug messages in callbacks,
   add CFLAGS+=-DTHREAD_STACKSIZE_IDLE=THREAD_STACKSIZE_DEFAULT to the build
   command.
*/
#include "debug.h"

#define SPIDEV               (dev->params.spi)
#define CSPIN                (dev->params.cs_pin)
#define INTPIN               (dev->params.int_pin)
#define RESETPIN             (dev->params.reset_pin)
#define POWERPIN             (dev->params.power_pin)
#define SPI_CS_DELAY_MS      (1)
#define TX_TIMEOUT           (8U)             /* 8 s */
#define TX_RX_TIMEOUT        (50U)            /* 50 s */

static void _print_atmel_status(uint8_t status)
{
    DEBUG("[ata8520e] Atmel status: %d\n", status);
    if (status & ATA8520E_ATMEL_PA_MASK) {
        DEBUG("[ata8520e] Atmel: PA ON\n");
    }
    else {
        DEBUG("[ata8520e] Atmel: PA OFF\n");
    }
    if ((status >> 1) & ATA8520E_ATMEL_SYSTEM_READY_MASK) {
        DEBUG("[ata8520e] Atmel: System ready to operate\n");
        return;
    }
    if ((status >> 1) & ATA8520E_ATMEL_FRAME_SENT_MASK) {
        DEBUG("[ata8520e] Atmel: Frame sent\n");
        return;
    }
    switch ((status >> 1) & 0x0F) {
        case ATA8520E_ATMEL_OK:
            DEBUG("[ata8520e] Atmel: System is ready\n");
            break;
        case ATA8520E_ATMEL_COMMAND_ERROR:
            DEBUG("[ata8520e] Atmel: Command error / not supported\n");
            break;
        case ATA8520E_ATMEL_GENERIC_ERROR:
            DEBUG("[ata8520e] Atmel: Generic error\n");
            break;
        case ATA8520E_ATMEL_FREQUENCY_ERROR:
            DEBUG("[ata8520e] Atmel: Frequency error\n");
            break;
        case ATA8520E_ATMEL_USAGE_ERROR:
            DEBUG("[ata8520e] Atmel: Usage error\n");
            break;
        case ATA8520E_ATMEL_OPENING_ERROR:
            DEBUG("[ata8520e] Atmel: Opening error\n");
            break;
        case ATA8520E_ATMEL_CLOSING_ERROR:
            DEBUG("[ata8520e] Atmel: Closing error\n");
            break;
        case ATA8520E_ATMEL_SEND_ERROR:
            DEBUG("[ata8520e] Atmel: Send error\n");
            break;
        default:
            DEBUG("[ata8520e] Atmel: Unknown status code '%02X'\n",
                  (status >> 1) & 0x0F);
            break;
    }
}

static void _print_sigfox_status(uint8_t status)
{
    DEBUG("[ata8520e] Sigfox status: %d\n", status);
    switch (status) {
        case ATA8520E_SIGFOX_NO_ERROR:
            DEBUG("[ata8520e] Sigfox: OK\n");
            break;
        case ATA8520E_SIGFOX_TX_LEN_TOO_LONG:
            DEBUG("[ata8520e] Sigfox: TX data length exceeds 12 bytes\n");
            break;
        case ATA8520E_SIGFOX_RX_TIMEOUT:
            DEBUG("[ata8520e] Sigfox: Timeout for downlink message\n");
            break;
        case ATA8520E_SIGFOX_RX_BIT_TIMEOUT:
            DEBUG("[ata8520e] Sigfox: Timeout for bit downlink\n");
            break;
        case ATA8520E_SIGFOX2_INIT_ERROR:
            DEBUG("[ata8520e] Sigfox2: Initialization error\n");
            break;
        case ATA8520E_SIGFOX2_TX_ERROR:
            DEBUG("[ata8520e] Sigfox2: Error during send\n");
            break;
        case ATA8520E_SIGFOX2_RF_ERROR:
            DEBUG("[ata8520e] Sigfox2: Error in RF frequency\n");
            break;
        case ATA8520E_SIGFOX2_DF_WAIT_ERROR:
            DEBUG("[ata8520e] Sigfox2: Error during wait for data frame\n");
            break;
        default:
            DEBUG("[ata8520e] Sigfox: Internal error '%02X'\n", status);
            break;
    }
}

static void _irq_handler(void *arg)
{
    (void) arg;
    ata8520e_t * dev = (ata8520e_t *)arg;
    DEBUG("[ata8520e] Event received !\n");
    dev->event_received = 1;
    switch (dev->internal_state) {
        case ATA8520E_STATE_IDLE:
            DEBUG("[ata8520e] IDLE state\n");
            break;
        case ATA8520E_STATE_INIT:
            DEBUG("[ata8520e] INIT state\n");
            break;
        case ATA8520E_STATE_TX:
            DEBUG("[ata8520e] TX state\n");
            break;
        case ATA8520E_STATE_RX:
            DEBUG("[ata8520e] RX state\n");
            break;
    }
}

static void _getbus(const ata8520e_t *dev)
{
    spi_acquire(SPIDEV, CSPIN, SPI_MODE_0, dev->params.spi_clk);
}

static void _spi_transfer_byte(const ata8520e_t *dev, bool cont, uint8_t out)
{
    /* Manually triggering CS because of a required delay, see datasheet,
       section 2.1.1, page 10 */
    gpio_clear((gpio_t)CSPIN);
    ztimer_sleep(ZTIMER_MSEC, SPI_CS_DELAY_MS);
    spi_transfer_byte(SPIDEV, SPI_CS_UNDEF, cont, out);
    ztimer_sleep(ZTIMER_MSEC, SPI_CS_DELAY_MS);
    gpio_set((gpio_t)CSPIN);
}

static void _spi_transfer_bytes(const ata8520e_t *dev, bool cont,
                                const void *out, void *in, size_t len)
{
    /* Manually triggering CS because of a required delay, see datasheet,
       section 2.1.1, page 10 */
    gpio_clear((gpio_t)CSPIN);
    ztimer_sleep(ZTIMER_MSEC, SPI_CS_DELAY_MS);
    spi_transfer_bytes(SPIDEV, SPI_CS_UNDEF, cont, out, in, len);
    ztimer_sleep(ZTIMER_MSEC, SPI_CS_DELAY_MS);
    gpio_set((gpio_t)CSPIN);
}

static void _send_command(const ata8520e_t *dev, uint8_t command)
{
    _getbus(dev);
    _spi_transfer_byte(dev, false, command);
    spi_release(SPIDEV);
}

static void _status(const ata8520e_t *dev)
{
    /* clear the event line and check the device status,
       see datasheet, section 2.1.2.10, page 12 */
    _getbus(dev);
    _spi_transfer_byte(dev, true, ATA8520E_GET_STATUS);
    _spi_transfer_byte(dev, true, 0);
    _spi_transfer_byte(dev, true, 0); /* SSM unused */
    uint8_t atmel = spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    uint8_t sigfox = spi_transfer_byte(SPIDEV, CSPIN, true, 0);
    uint8_t sigfox2 = spi_transfer_byte(SPIDEV, CSPIN, false, 0);
    spi_release(SPIDEV);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_atmel_status(atmel);
        _print_sigfox_status(sigfox);
        _print_sigfox_status(sigfox2);
    }
    else {
        (void)atmel;
        (void)sigfox;
        (void)sigfox2;
    }
}

static void _reset(const ata8520e_t *dev)
{
    gpio_set(RESETPIN);
    ztimer_sleep(ZTIMER_MSEC, 10);
    gpio_clear(RESETPIN);
    ztimer_sleep(ZTIMER_MSEC, 10);
    gpio_set(RESETPIN);
}

static void _poweron(const ata8520e_t *dev)
{
    /* power up procedure, see datasheet, section 2.2.1, page 24 */
    gpio_set(POWERPIN);
    _reset(dev);
}

static void _poweroff(const ata8520e_t *dev)
{
    /* power down procedure, see datasheet, section 2.2.2, page 24 */
    _status(dev);
    gpio_clear(POWERPIN);
    _send_command(dev, ATA8520E_OFF_MODE);
}

int ata8520e_init(ata8520e_t *dev, const ata8520e_params_t *params)
{
    /* write config params to device descriptor */
    dev->params = *params;

    /* Initialize pins*/
    if (gpio_init_int(INTPIN, GPIO_IN_PD,
                      GPIO_FALLING, _irq_handler, dev) < 0 ) {
        DEBUG("[ata8520e] ERROR: Interrupt pin not initialized\n");
        return -ATA8520E_ERR_GPIO_INT;
    }
    if (gpio_init(POWERPIN, GPIO_OUT) < 0) {
        DEBUG("[ata8520e] ERROR: Power pin not initialized\n");
        return -ATA8520E_ERR_GPIO_POWER;
    }
    if (gpio_init(RESETPIN, GPIO_OUT) < 0) {
        DEBUG("[ata8520e] ERROR: Reset pin not initialized\n");
        return -ATA8520E_ERR_GPIO_RESET;
    }

    dev->internal_state = ATA8520E_STATE_INIT;
    _poweron(dev);

    /* Initialize SPI bus*/
    if (spi_init_cs(dev->params.spi, CSPIN) < 0) {
        DEBUG("[ata8520e] ERROR: SPI not initialized\n");
        return -ATA8520E_ERR_SPI;
    }

    ztimer_sleep(ZTIMER_MSEC, 100);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        char sigfox_id[SIGFOX_ID_LENGTH + 1];
        ata8520e_read_id(dev, sigfox_id);

        char sigfox_pac[SIGFOX_PAC_LENGTH + 1];
        ata8520e_read_pac(dev, sigfox_pac);

        uint8_t atmel_version[2];
        ata8520e_read_atmel_version(dev, atmel_version);

        char sigfox_version[11];
        ata8520e_read_sigfox_version(dev, sigfox_version);

        DEBUG("[ata8520e] Atmel version: %d.%d\n",
              atmel_version[0], atmel_version[1]);
        DEBUG("[ata8520e] Sigfox version: %s\n", sigfox_version);
        DEBUG("[ata8520e] Sigfox ID: %s\n", sigfox_id);
        DEBUG("[ata8520e] Sigfox PAC: %s\n", sigfox_pac);
    }

    /* clear event line */
    _status(dev);

    dev->internal_state = ATA8520E_STATE_IDLE;

    _poweroff(dev);

    return ATA8520E_OK;
}

void ata8520e_system_reset(const ata8520e_t *dev)
{
    _send_command(dev, ATA8520E_SYSTEM_RESET);
}

void ata8520e_read_atmel_version(const ata8520e_t *dev, uint8_t *version)
{
    _poweron(dev);
    _getbus(dev);
    _spi_transfer_byte(dev, true, ATA8520E_ATMEL_VERSION);
    _spi_transfer_byte(dev, true, 0);
    _spi_transfer_bytes(dev, false, 0, version, 2);
    spi_release(SPIDEV);
    _poweroff(dev);
}

void ata8520e_read_sigfox_version(const ata8520e_t *dev, char *version)
{
    _poweron(dev);
    _getbus(dev);
    _spi_transfer_byte(dev, true, ATA8520E_SIGFOX_VERSION);
    _spi_transfer_byte(dev, true, 0);
    _spi_transfer_bytes(dev, false, 0, version, 11);
    spi_release(SPIDEV);
    _poweroff(dev);
}

void ata8520e_read_pac(const ata8520e_t *dev, char *pac)
{
    _poweron(dev);
    uint8_t pac_bytes[SIGFOX_PAC_LENGTH];
    _getbus(dev);
    _spi_transfer_byte(dev, true, ATA8520E_GET_PAC);
    _spi_transfer_byte(dev, true, 0);
    _spi_transfer_bytes(dev, false, NULL, pac_bytes, SIGFOX_PAC_LENGTH / 2);
    spi_release(SPIDEV);
    fmt_bytes_hex(pac, pac_bytes, SIGFOX_PAC_LENGTH >> 1);
    pac[SIGFOX_PAC_LENGTH] = '\0';
    _poweroff(dev);
}

void ata8520e_read_id(const ata8520e_t *dev, char *id)
{
    _poweron(dev);
    uint8_t id_bytes[SIGFOX_ID_LENGTH];
    _getbus(dev);
    _spi_transfer_byte(dev, true, ATA8520E_GET_ID);
    _spi_transfer_byte(dev, true, 0);
    _spi_transfer_bytes(dev, false, NULL, id_bytes, SIGFOX_ID_LENGTH);
    spi_release(SPIDEV);
    /* create id string (4 hexadecimal values in reversed order) */
    fmt_bytes_hex_reverse(id, id_bytes, SIGFOX_ID_LENGTH >> 1);
    id[SIGFOX_ID_LENGTH] = '\0';
    _poweroff(dev);
}

static void isr_event_timeout(void *arg)
{
    ata8520e_t *dev = (ata8520e_t *)arg;
    mutex_unlock(&(dev->event_lock));
}

static bool _wait_event(ata8520e_t *dev, uint8_t timeout)
{
    dev->event_received = 0;
    ztimer_now_t start_time = ztimer_now(ZTIMER_MSEC);
    ztimer_t event_timer;
    event_timer.callback = isr_event_timeout;
    event_timer.arg = dev;
    ztimer_set(ZTIMER_MSEC, &event_timer, (uint32_t)timeout);

    /* waiting for the event */
    while ((!dev->event_received) &&
            ((int32_t)(start_time + timeout - ztimer_now(ZTIMER_MSEC)) > 0)) {
        mutex_lock(&(dev->event_lock));
    }
    ztimer_remove(ZTIMER_MSEC, &event_timer);

    if (dev->event_received == 0) {
        DEBUG("[ata8520e] event timeout\n");
        return true;
    }

    return false;
}

static void _prepare_send_frame(ata8520e_t *dev, uint8_t *msg, uint8_t msg_len)
{
    _poweron(dev);
    ztimer_sleep(ZTIMER_MSEC, 5);
    _status(dev);

    /* Verify message length */
    if (msg_len > SIGFOX_MAX_TX_LENGTH) {
        DEBUG("[ata8520e] Message exceeds the maximum %d characters length "
              "allowed. It will be truncated.\n", SIGFOX_MAX_TX_LENGTH);
        msg_len = SIGFOX_MAX_TX_LENGTH;
    }

    dev->internal_state = ATA8520E_STATE_TX;

    /* Write message in TX buffer */
    DEBUG("[ata8520e] Writing send frame to RX buffer\n");
    _getbus(dev);
    _spi_transfer_byte(dev, true, ATA8520E_WRITE_TX_BUFFER);
    _spi_transfer_byte(dev, true, msg_len);
    _spi_transfer_bytes(dev, false, msg, NULL, msg_len);
    spi_release(SPIDEV);
}

static int _wait_send(ata8520e_t *dev, uint8_t timeout)
{
    DEBUG("[ata8520e] waiting for TX to complete\n");
    /* Wait 8s maximum for the message to be sent */
    int ret = ATA8520E_OK;
    if (_wait_event(dev, timeout)) {
        DEBUG("[ata8520e] failed to send message\n");
        ret = -ATA8520E_ERR_EVENT_TIMEOUT;
    }
    else {
        DEBUG("[ata8520e] message sent\n");
    }

    /* Clear event line */
    _status(dev);

    /* back to idle state */
    dev->internal_state = ATA8520E_STATE_IDLE;

    return ret;
}

int ata8520e_send_frame(ata8520e_t *dev, uint8_t *msg, uint8_t msg_len)
{
    DEBUG("[ata8520e] Sending frame '%s', length: %d\n", (char*)msg, msg_len);
    _prepare_send_frame(dev, msg, msg_len);

    /* Trigger TX */
    _send_command(dev, ATA8520E_SEND_FRAME);

    int ret = _wait_send(dev, TX_TIMEOUT);

    /* switch off transceiver before returning */
    _poweroff(dev);

    return ret;
}

int ata8520e_send_receive_frame(ata8520e_t *dev, uint8_t *msg, uint8_t msg_len,
                                uint8_t *rx_payload)
{
    DEBUG("[ata8520e] Sending frame '%s', length: %d\n", (char*)msg, msg_len);
    _prepare_send_frame(dev, msg, msg_len);

    /* Trigger TX */
    _send_command(dev, ATA8520E_SEND_RECEIVE_FRAME);

    int ret = _wait_send(dev, TX_RX_TIMEOUT);
    if (ret == -ATA8520E_ERR_EVENT_TIMEOUT) {
        return ret;
    }

    /* Read RX message */
    dev->internal_state = ATA8520E_STATE_RX;
    DEBUG("[ata8520e] Reading RX buffer\n");
    _getbus(dev);
    _spi_transfer_byte(dev, true, ATA8520E_READ_RX_BUFFER);
    _spi_transfer_byte(dev, true, 0);
    _spi_transfer_bytes(dev, false, NULL, rx_payload, SIGFOX_RX_LENGTH);
    spi_release(SPIDEV);

    /* back to idle state */
    dev->internal_state = ATA8520E_STATE_IDLE;

    /* switch off transceiver before returning */
    _poweroff(dev);

    return ret;
}

int ata8520e_send_bit(ata8520e_t *dev, bool bit)
{
    DEBUG("[ata8520e] Sending bit '%d'\n", bit);
    _poweron(dev);
    ztimer_sleep(ZTIMER_MSEC, 5);
    _status(dev);

    dev->internal_state = ATA8520E_STATE_TX;

    /* Sends the bit */
    _getbus(dev);
    _spi_transfer_byte(dev, true, ATA8520E_SEND_BIT);
    if (bit) {
        _spi_transfer_byte(dev, false, 0x01);
    }
    else {
        _spi_transfer_byte(dev, false, 0x00);
    }
    spi_release(SPIDEV);

    int ret = _wait_send(dev, TX_TIMEOUT);

    /* switch off transceiver before returning */
    _poweroff(dev);

    return ret;
}
