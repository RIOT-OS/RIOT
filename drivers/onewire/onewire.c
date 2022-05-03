/*
 * Copyright (C) 2017 Frits Kuipers
 *               2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ds18
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Maxim Integrated DS1822 and
 *              DS18B20 temperature sensors.
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "onewire.h"

#include <errno.h>

#include "byteorder.h"
#include "log.h" // TODO: rm?
#include "periph/gpio.h"
#include "xtimer.h" // TODO: use ztimer?

#define ENABLE_DEBUG 0
#include "debug.h"

// 1-wire signal delays (in useconds)
//TODO: better doc?
#define ONEWIRE_DELAY_RESET         (480U)
#define ONEWIRE_DELAY_PRESENCE      (60U)
#define ONEWIRE_DELAY_SLOT          (60U)
#define ONEWIRE_SAMPLE_TIME         (10U)
#define ONEWIRE_DELAY_RW_PULSE      (1U)
#define ONEWIRE_DELAY_R_RECOVER     (ONEWIRE_DELAY_SLOT - ONEWIRE_SAMPLE_TIME)

static uint8_t _crc8_byte(uint8_t crc, uint8_t data)
{
    crc = crc ^ data;

    for (unsigned i = 0; i < 8; i++) {
        if (crc & 0x01) {
            crc = (crc >> 1) ^ 0x8C;
        }
        else {
            crc >>= 1;
        }
    }

    return crc;
}

static uint8_t _crc8(uint8_t crc, uint8_t *buf, size_t size)
{
    for (unsigned i = 0; i < size; i++) {
        crc = _crc8_byte(crc, buf[i]);
    }

    return crc;
}

static void onewire_pin_low(const onewire_t *dev)
{
    /* Set gpio as output and clear pin */
    gpio_init(dev->params.pin, GPIO_OUT);
    gpio_clear(dev->params.pin);
}

static void onewire_pin_release(const onewire_t *dev)
{
    /* Init pin as input */
    gpio_init(dev->params.pin, dev->params.in_mode);
}

int onewire_reset_bus(const onewire_t *dev)
{
    int res;

    /* Line low and sleep the reset delay */
    onewire_pin_low(dev);
    xtimer_usleep(ONEWIRE_DELAY_RESET);

    /* Release and wait for the presence response */
    onewire_pin_release(dev);
    xtimer_usleep(ONEWIRE_DELAY_PRESENCE);

    /* Check device presence */
    res = gpio_read(dev->params.pin);

    /* Sleep for reset delay */
    xtimer_usleep(ONEWIRE_DELAY_RESET);

    /* If no device pulled the bus low */
    if (res)
    {
        return -EIO;
    }

    return 0;
}

int onewire_read_bit(const onewire_t *dev, uint8_t *bit)
{
    /* Initiate read slot */
    onewire_pin_low(dev);
    onewire_pin_release(dev);

#if defined(MODULE_ONEWIRE_OPTIMIZED)
    xtimer_usleep(ONEWIRE_SAMPLE_TIME);
    *bit = gpio_read(dev->params.pin);
    xtimer_usleep(ONEWIRE_DELAY_R_RECOVER);
    return 0;
#else
    uint32_t start, measurement = 0;

    /* Measure time low of device pin, timeout after slot time*/
    start = xtimer_now_usec();
    while (!gpio_read(dev->params.pin) && measurement < ONEWIRE_DELAY_SLOT) {
        measurement = xtimer_now_usec() - start;
    }

    /* If there was a timeout return error */
    if (measurement >= ONEWIRE_DELAY_SLOT) {
        return -EIO;
    }

    /* When gpio was low for less than the sample time, bit is high*/
    *bit = measurement < ONEWIRE_SAMPLE_TIME;

    /* Wait for slot to end */
    xtimer_usleep(ONEWIRE_DELAY_SLOT - measurement);

    return 0;
#endif
}

void onewire_write_bit(const onewire_t *dev, uint8_t bit)
{
    /* Initiate write slot */
    onewire_pin_low(dev);

    /* Release pin when bit==1 */
    if (bit) {
        onewire_pin_release(dev);
    }

    /* Wait for slot to end */
    xtimer_usleep(ONEWIRE_DELAY_SLOT);
    onewire_pin_release(dev);
    xtimer_usleep(1);
}

int onewire_read_byte(const onewire_t *dev, uint8_t *byte)
{
    uint8_t bit = 0;
    *byte = 0;

    for (int i = 0; i < 8; i++) {

        if (onewire_read_bit(dev, &bit) < 0) {
            return -EIO;
        }

        *byte |= (bit << i);
    }

    return 0;
}

void onewire_write_byte(const onewire_t *dev, uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        onewire_write_bit(dev, byte & (0x01 << i));
    }
}

int onewire_read_word(const onewire_t *dev, uint16_t *word)
{
    int res;
    le_uint16_t data;

    res = onewire_read(dev, &data, 2);

    *word = byteorder_ltohs(data); //TODO?

    return res;
}

void onewire_write_word(const onewire_t *dev, uint16_t word)
{
    le_uint16_t data = byteorder_htols(word); //TODO?

    onewire_write(dev, &data, 2);
}

int onewire_read(const onewire_t *dev, void *buf, size_t size)
{
    int res;
    uint8_t *data = buf;

    for (size_t i = 0; i < size; i++) {
        res = onewire_read_byte(dev, &data[i]);

        if (res < 0) {
            return res;
        }
    }

    return size;
}

void onewire_write(const onewire_t *dev, const void *buf, size_t size)
{
    const uint8_t *data = buf;

    for (size_t i = 0; i < size; i++) {
        onewire_write_byte(dev, data[i]);
    }
}

int onewire_init(onewire_t *dev, const onewire_params_t *params)
{
    int res;

    dev->params = *params;

    mutex_init(&dev->lock);

    /* Deduct the input mode from the output mode. If pull-up resistors are
     * used for output then will be used for input as well. */
    dev->params.in_mode = (dev->params.out_mode == GPIO_OD_PU) ? GPIO_IN_PU : GPIO_IN;

    /* Initialize the device and the pin */
    res = gpio_init(dev->params.pin, dev->params.in_mode) == 0 ? 0 : -EIO;

    return res;
}

void onewire_aquire(onewire_t *dev)
{
    mutex_lock(&dev->lock);
}

void onewire_release(onewire_t *dev)
{
    mutex_unlock(&dev->lock);
}

int onewire_cmd_skiprom(onewire_t *dev)
{
    int res;

    res = onewire_reset_bus(dev);
    if (res < 0)
    {
        return -EIO; //TODO
    }

    onewire_write_byte(dev, ONEWIRE_CMD_SKIPROM);

    return 0;
}

int onewire_cmd_readrom(onewire_t *dev, onewire_id_rom_t *rom)
{
    int res;

    res = onewire_reset_bus(dev);
    if (res < 0)
    {
        return -EIO; //TODO
    }

    onewire_write_byte(dev, ONEWIRE_CMD_READROM);
    onewire_read(dev, rom, sizeof(*rom));

#if ENABLE_DEBUG

    DEBUG("device type: 0x%02x\n", rom->type);

    DEBUG("serial number: 0x");
    for (unsigned i = 0; i < sizeof(rom->serial_number); i++)
    {
        DEBUG("%02x", rom->serial_number[i]);
    }
    DEBUG("\n");

    DEBUG("stored crc: 0x%02x\n", rom->crc);

    uint8_t crc;
    crc = _crc8(0  , (uint8_t*)&rom->type         , sizeof(rom->type         ));
    crc = _crc8(crc, (uint8_t*)&rom->serial_number, sizeof(rom->serial_number));
    DEBUG("calculated crc: 0x%02x\n", crc);

#endif

    if (_crc8(0, (uint8_t*)rom, sizeof(*rom)) != 0)
    {
        return -EIO; //TODO
    }

    return 0;
}
