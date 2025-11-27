/*
 * Copyright (C) 2020 Antonio Galea <antonio.galea@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ata8510
 * @{
 *
 * @file
 * @brief       Implementation of public functions for ATA8510 radio module
 *
 * @author      Antonio Galea <antonio.galea@gmail.com>
 * @author      Roberto Asquini <bobasquins@gmail.com>
 *
 * @}
 */

#include <string.h>
#include <stdlib.h>
#include "xtimer.h"
#include "thread.h"

#include "periph/spi.h"
#include "periph/gpio.h"

#include "ata8510.h"
#include "ata8510_params.h"
#include "ata8510_internals.h"

#define ENABLE_DEBUG (0)
/*
 * Warning: to correctly display the debug messages in callbacks,
 * add CFLAGS+=-DTHREAD_STACKSIZE_IDLE=THREAD_STACKSIZE_DEFAULT to the build
 * command.
 */
#include "debug.h"

#define SPIDEV               (dev->params.spi)
#define CSPIN                (dev->params.cs_pin)
#define INTPIN               (dev->params.int_pin)
#define RESETPIN             (dev->params.reset_pin)
#define SLEEPPIN             (dev->params.sleep_pin)

static char stack[THREAD_STACKSIZE_DEFAULT];
static void *_recv_thread(void *arg);

static void _irq_handler(void *arg)
{
    ata8510_t *dev = (ata8510_t *)arg;

    switch (dev->internal_state) {
        case ATA8510_MODE_IDLE:
            DEBUG("[ata8510] _irq_handler: event received in mode IDLE\n");
            break;
        case ATA8510_MODE_TX:
            DEBUG("[ata8510] _irq_handler: event received in mode TX\n");
            mutex_unlock(&(dev->tx_lock));
            break;
        case ATA8510_MODE_RX:
            DEBUG("[ata8510] _irq_handler: event received in mode RX\n");
            mutex_unlock(&(dev->rx_lock));
            break;
        case ATA8510_MODE_RX_POLLING:
            DEBUG(
                "[ata8510] _irq_handler: event received in mode RX POLLING\n"
                );
            mutex_unlock(&(dev->rx_lock));
            break;
    }
}

static void _send_command(ata8510_t *dev, const void *out,
                          void *in, size_t len)
{
    if (spi_acquire(SPIDEV, CSPIN, SPI_MODE_0, dev->params.spi_clk) < 0) {
        DEBUG("[ata8510] ERROR: Cannot acquire SPI bus!\n");
    }
    gpio_clear(CSPIN);
    xtimer_usleep(1);
    spi_transfer_bytes(SPIDEV, SPI_CS_UNDEF, true, out, in, len);
    xtimer_usleep(1);
    gpio_set(CSPIN);
    spi_release(SPIDEV);
}

static uint8_t _get_device_signature(ata8510_t *dev)
{
    const uint8_t command[6] = { ATA8510_CMD_GET_VERSION_FLASH, 0, 0, 0, 0, 0 };
    uint8_t data[6] = { 0, 0, 0, 0, 0, 0 };

    _send_command(dev, command, data, sizeof(command));

    DEBUG("[ata8510] _get_device_signature: "
          "%02x:%02x:%02x:%02x:%02x:%02x.\n",
          data[0], data[1], data[2], data[3], data[4], data[5]
          );
    return data[2];
}

int ata8510_init(ata8510_t *dev, const ata8510_params_t *params)
{
    int res;

    /* write config params to device descriptor */
    dev->params = *params;
    dev->receive_cb = NULL;

    /* Initialize pins */
    if (gpio_init_int(INTPIN, GPIO_IN,
                      GPIO_FALLING, _irq_handler, dev) < 0) {
        DEBUG("[ata8510] ERROR: Interrupt pin not initialized\n");
        return -ATA8510_ERR_GPIO_INT;
    }
    if (gpio_init(SLEEPPIN, GPIO_OUT) < 0) {
        DEBUG("[ata8510] ERROR: Power pin not initialized\n");
        return -ATA8510_ERR_GPIO_POWER;
    }
    if (gpio_init(RESETPIN, GPIO_OUT) < 0) {
        DEBUG("[ata8510] ERROR: Reset pin not initialized\n");
        return -ATA8510_ERR_GPIO_RESET;
    }

    ata8510_poweron(dev);

    /* Initialize SPI bus */
    if (spi_init_cs(dev->params.spi, CSPIN) < 0) {
        DEBUG("[ata8510] ERROR: SPI not initialized\n");
        return -ATA8510_ERR_SPI;
    }

    uint8_t sig = _get_device_signature(dev);
    if (sig != ATA8510_PARTNUM) {
        DEBUG("[ata8510] ERROR: signature is not 0x%02x.\n", ATA8510_PARTNUM);
        return -ATA8510_ERR_SIGNATURE;
    }

    /* Clear event line */
    ata8510_get_event_bytes(dev);

    dev->channel = ATA8510_CHANNEL_0;
    dev->service = ATA8510_SERVICE_0;

    /* _recv_thread will block on rx_lock until a frame arrives */
    mutex_lock(&(dev->rx_lock));
    res = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                        THREAD_CREATE_STACKTEST, _recv_thread, (void *)dev,
                        "recv_thread");

    if (res <= KERNEL_PID_UNDEF) {
        puts("Creation of receiver thread failed");
        return -ATA8510_ERR_RECV_THREAD;
    }
    ata8510_set_mode(dev, ATA8510_MODE_IDLE);

    return ATA8510_OK;
}

void ata8510_poweron(ata8510_t *dev)
{
    gpio_clear(RESETPIN);
    gpio_set(RESETPIN);
    gpio_clear(SLEEPPIN);
    xtimer_usleep(ATA8510_POWERON_DELAY);
}

void ata8510_poweroff(ata8510_t *dev)
{
    const uint8_t command[2] = { ATA8510_CMD_OFF_COMMAND, 0 };
    uint8_t data[2] = { 0, 0 };

    ata8510_get_event_bytes(dev);
    gpio_set(SLEEPPIN);
    _send_command(dev, command, data, sizeof(command));

    DEBUG("[ata8510] poweroff\n");
}

void ata8510_system_reset(ata8510_t *dev)
{
    const uint8_t command[2] = { ATA8510_CMD_SYSTEM_RESET, 0 };
    uint8_t data[2] = { 0, 0 };

    _send_command(dev, command, data, sizeof(command));

    DEBUG("[ata8510] ata8510_system_reset\n");
}

void ata8510_get_event_bytes(ata8510_t *dev)
{
    const uint8_t command[4] = { ATA8510_CMD_GET_EVENT_BYTES, 0, 0, 0 };

    _send_command(dev, command, (uint8_t *)&dev->events, sizeof(command));

    DEBUG(
        "SYS_ERR=%d CMD_RDY=%d SYS_RDY=%d SFIFO=%d DFIFO_RX=%d DFIFO_TX=%d "
        "SOTA=%d EOTA=%d CH=%d SER=%d\n",
        dev->events.system.val.sys_err, dev->events.system.val.cmd_rdy,
        dev->events.system.val.sys_rdy, dev->events.system.val.sfifo,
        dev->events.system.val.dfifo_rx, dev->events.system.val.dfifo_tx,
        dev->events.events.val.sota, dev->events.events.val.eota,
        dev->events.config.val.ch, dev->events.config.val.ser
        );
}

ata8510_system_mode_config_opm_t ata8510_set_mode(
    ata8510_t *dev, ata8510_system_mode_config_opm_t opm)
{
    const uint8_t command[3] = { ATA8510_CMD_SET_SYSTEM_MODE, 0, 0 };
    uint8_t data[3] = { 0, 0, 0 };
    ata8510_system_mode_config_t *mode =
        (ata8510_system_mode_config_t *)&command[1];
    ata8510_system_channel_config_t *config =
        (ata8510_system_channel_config_t *)&command[2];
    ata8510_system_mode_config_opm_t previous_mode = dev->internal_state;

    mode->vco_tune = 1;
    mode->opm = opm;

    switch (opm) {
        case ATA8510_MODE_IDLE:
            break;
        case ATA8510_MODE_TX:
            config->ena_path_a = 1;
            config->ch = dev->channel;
            config->ser = dev->service;
            break;
        case ATA8510_MODE_RX:
            config->ena_path_a = 1;
            config->ch = dev->channel;
            config->ser = dev->service;
            break;
        case ATA8510_MODE_RX_POLLING:
            /* service is used as startPollingIndex */
            config->ser = dev->service;
            break;
    }
    dev->internal_state = opm;

    _send_command(dev, command, data, sizeof(command));

    DEBUG("[ata8510] ata8510_set_mode %d\n", opm);

    return previous_mode;
}

static void _write_preamble(ata8510_t *dev)
{
    const uint8_t command[11] =
    { ATA8510_CMD_WRITE_TX_PREAMBLE_FIFO, 9, 0x04, 0x70, 0x8E, 0x0A, 0x55, 0x55,
      0x10, 0x55, 0x56 };
    uint8_t data[11];

    _send_command(dev, command, data, sizeof(command));
}

static uint8_t _read_available_tx_fifo(ata8510_t *dev)
{
    const uint8_t command[3] = { ATA8510_CMD_READ_FILL_LEVEL_TX_FIFO, 0x00, 0x00 };
    uint8_t data[3];

    _send_command(dev, command, data, sizeof(command));
    return ATA8510_DFIFO_TX_LENGTH - data[2];
}

static void _write_tx_fifo(ata8510_t *dev, uint8_t *payload,
                           uint8_t payload_len)
{
    uint8_t command[ATA8510_DFIFO_TX_LENGTH +
                    2] = { ATA8510_CMD_WRITE_TX_FIFO, payload_len };
    uint8_t data[ATA8510_DFIFO_TX_LENGTH + 2];

    memcpy(command + 2, payload, payload_len);
    _send_command(dev, command, data, sizeof(command));
}

void ata8510_send_frame(ata8510_t *dev, uint8_t *payload, uint8_t payload_len)
{
    uint8_t *ptr = payload;
    uint8_t len = payload_len;
    uint8_t n;
    ata8510_events_t *ev = &dev->events;

    uint8_t mode = ata8510_set_mode(dev, ATA8510_MODE_IDLE);

    _write_preamble(dev);
    ata8510_set_mode(dev, ATA8510_MODE_TX);

    while (len > 0) {
        int fifo = _read_available_tx_fifo(dev);
        if (fifo > 0) {
            n = (fifo <= len ? fifo : len);
            _write_tx_fifo(dev, ptr, n);
            DEBUG("[ata8510] send_frame: loaded %d of %d bytes into TX FIFO\n",
                  n, len);
            ptr += n;
            len -= n;
        }
        if (len > 0) {
            while (!ev->system.val.dfifo_tx && !ev->events.val.eota) {
                mutex_lock(&(dev->tx_lock));
                ata8510_get_event_bytes(dev);
            }
        }
    }

    while (!ev->events.val.eota) {
        mutex_lock(&(dev->tx_lock));
        ata8510_get_event_bytes(dev);
    }

    /* restore previous mode */
    ata8510_get_event_bytes(dev);
    ata8510_set_mode(dev, ATA8510_MODE_IDLE);
    ata8510_set_mode(dev, mode);

    DEBUG("[ata8510] send_frame: successfully sent %d bytes\n", payload_len);
}

static uint8_t _read_fill_level_rx_fifo(ata8510_t *dev)
{
    const uint8_t command[3] = { ATA8510_CMD_READ_FILL_LEVEL_RX_FIFO, 0x00, 0x00 };
    uint8_t data[3];

    _send_command(dev, command, data, sizeof(command));
    return data[2];
}

static void _read_rx_fifo(ata8510_t *dev, uint8_t *buffer, uint8_t buffer_len)
{
    const uint8_t command[ATA8510_DFIFO_RX_LENGTH +
                    3] = { ATA8510_CMD_READ_RX_FIFO, buffer_len };
    uint8_t data[ATA8510_DFIFO_RX_LENGTH + 3];

    _send_command(dev, command, data, sizeof(command));
    memcpy(buffer, data + 3, buffer_len);
}

static uint8_t _read_fill_level_rssi_fifo(ata8510_t *dev)
{
    const uint8_t command[3] = { ATA8510_CMD_READ_FILL_LEVEL_RSSI_FIFO, 0x00, 0x00 };
    uint8_t data[3];

    _send_command(dev, command, data, sizeof(command));
    return data[2];
}

static void _read_rssi_fifo(ata8510_t *dev, uint8_t *buffer, uint8_t buffer_len)
{
    const uint8_t command[ATA8510_SFIFO_LENGTH +
                    3] = { ATA8510_CMD_READ_RSSI_FIFO, buffer_len };
    uint8_t data[ATA8510_SFIFO_LENGTH + 3];

    _send_command(dev, command, data, sizeof(command));
    memcpy(buffer, data + 3, buffer_len);
}

static int _recv_frame(ata8510_t *dev, uint8_t *buffer, uint8_t buffer_len,
                       int *dbm)
{
    uint8_t *ptr = buffer;
    uint8_t len = buffer_len;
    uint8_t n, i;
    uint8_t rssi_data[ATA8510_SFIFO_LENGTH];

    while (len > 0) {
        /* check RX FIFO */
        int fifo = _read_fill_level_rx_fifo(dev);
        if (fifo > 0) {
            n = (fifo <= len ? fifo : len);
            _read_rx_fifo(dev, ptr, n);
            DEBUG("[ata8510] _recv_frame: read %d bytes\n", n);
            ptr += n;
            len -= n;
        }
        /* check RSSI FIFO */
        fifo = _read_fill_level_rssi_fifo(dev);
        if (fifo > 0) {
            _read_rssi_fifo(dev, rssi_data, fifo);
            for (i = 0; i < fifo; i++) {
                dev->rssi.data[ (i + dev->rssi.len) %
                                sizeof(dev->rssi.data) ] = rssi_data[i];
            }
            dev->rssi.len += fifo;
            if (dev->rssi.len > sizeof(dev->rssi.data)) {
                dev->rssi.len = sizeof(dev->rssi.data);
            }
        }
        /* if end of transfer, bail out */
        if (dev->events.events.val.eota) {
            break;
        }
        if (len > 0) {
            do {
                mutex_lock(&(dev->rx_lock));
                ata8510_get_event_bytes(dev);
            } while (!dev->events.system.val.dfifo_rx &&
                     !dev->events.events.val.eota &&
                     !dev->events.system.val.sfifo);
        }
    }

    if (!dev->events.events.val.eota) {
        DEBUG(
            "[ata8510] _recv_frame: ERROR - insufficient buffer, "
            "packet truncated\n");
    }

    n = buffer_len - len;
    DEBUG("[ata8510] _recv_frame: received %d bytes\n", n);

    if (dev->rssi.len > 0) {
        int rssi = 0;
        for (i = 0; i < dev->rssi.len; i++) {
            rssi += dev->rssi.data[i];
        }
        rssi /= dev->rssi.len;
        *dbm = (rssi >> 1) - 135;
        DEBUG(
            "[ata8510] _recv_frame: computed RSSI %d out of %d values; "
            "dBm %d\n", rssi, dev->rssi.len, *dbm);
    }
    else {
        *dbm = ATA8150_DBM_UNDEF;
        DEBUG("[ata8510] _recv_frame: no RSSI values available\n");
    }

    return n;
}

static void *_recv_thread(void *arg)
{
    int len;
    uint8_t *buffer;
    int dbm = ATA8150_DBM_UNDEF;
    ata8510_t *dev = (ata8510_t *)arg;

    while (true) {
        mutex_lock(&(dev->rx_lock));
        uint8_t mode = dev->internal_state;
        if ((mode == ATA8510_MODE_RX) || (mode == ATA8510_MODE_RX_POLLING)) {
            ata8510_get_event_bytes(dev);
            if (dev->events.events.val.sota) {
                DEBUG("[ata8510] _recv_thread: awakened by SOTA event\n");
                buffer = malloc(ATA8510_MAX_PACKET_LEN);
                if (buffer == NULL) {
                    DEBUG("[ata8510] _recv_thread: ERROR - cannot allocate "
                          "buffer for incoming packet\n");
                    continue;
                }
                DEBUG("[ata8510] _recv_thread: allocated %d bytes buffer\n",
                      ATA8510_MAX_PACKET_LEN);
                len = _recv_frame(dev, buffer, ATA8510_MAX_PACKET_LEN, &dbm);
                if (dev->receive_cb != NULL) {
                    (dev->receive_cb)(buffer, len, dbm);
                }
                else {
                    DEBUG(
                        "[ata8510] _recv_thread: no callback defined, "
                        "packet discarded\n");
                    free(buffer);
                }
                /* restore previous mode */
                ata8510_get_event_bytes(dev);
                ata8510_set_mode(dev, ATA8510_MODE_IDLE);
                ata8510_set_mode(dev, mode);
            }
        }
    }
    return NULL;
}

static uint8_t _read_sram(ata8510_t *dev, uint16_t addr)
{
    uint8_t addrh = (addr & 0xFF00) >> 8;
    uint8_t addrl = addr & 0x00FF;
    const uint8_t command[6] =
    { ATA8510_CMD_READ_SRAM_REGISTER, 1, addrh, addrl, 0, 0 };
    uint8_t data[6];

    _send_command(dev, command, data, sizeof(command));
    return data[5];
}

static uint8_t _read_eeprom(ata8510_t *dev, uint16_t addr)
{
    uint8_t addrh = (addr & 0xFF00) >> 8;
    uint8_t addrl = addr & 0x00FF;
    const uint8_t command[5] = { ATA8510_CMD_READ_EEPROM, addrh, addrl, 0, 0 };
    uint8_t data[5];

    _send_command(dev, command, data, sizeof(command));
    return data[4];
}

uint8_t ata8510_get_radio_power(ata8510_t *dev)
{
    uint8_t base = _read_eeprom(dev, ATA8510_EEPROM_FEPAC);
    uint8_t damping = _read_sram(dev, ATA8510_SRAM_PA_DAMPING);
    uint8_t power = base > damping ? base - damping : 0;

    DEBUG("[ata8510] get_radio_power: base=%d, damping=%d, power=%d\n",
          base, damping, power);
    return power;
}

static void _write_sram(ata8510_t *dev, uint16_t addr, uint8_t value)
{
    uint8_t addrh = (addr & 0xFF00) >> 8;
    uint8_t addrl = addr & 0x00FF;
    const uint8_t command[5] =
    { ATA8510_CMD_WRITE_SRAM_REGISTER, 1, addrh, addrl, value };
    uint8_t data[5];

    _send_command(dev, command, data, sizeof(command));
}

void ata8510_set_radio_power(ata8510_t *dev, uint8_t power)
{
    uint8_t base = _read_eeprom(dev, ATA8510_EEPROM_FEPAC);
    uint8_t damping = power <= base ? base - power : 0;

    DEBUG("[ata8510] set_radio_power: power=%d, base=%d => damping=%d\n",
          power, base, damping);
    _write_sram(dev, ATA8510_SRAM_PA_DAMPING, damping);
}

void ata8510_get_radio_service_channel(ata8510_t *dev,
                                       ata8510_service_t *service,
                                       ata8510_channel_t *channel)
{
    *service = dev->service;
    *channel = dev->channel;
}

void ata8510_set_radio_service_channel(ata8510_t *dev,
                                       ata8510_service_t service,
                                       ata8510_channel_t channel)
{
    dev->service = service;
    dev->channel = channel;
}
