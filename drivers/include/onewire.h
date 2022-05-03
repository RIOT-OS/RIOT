/*
 * Copyright (C) 2017 Frits Kuipers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ds18 DS18 temperature sensor driver
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver interface for the DS18 temperature sensors
 *
 * This driver provides @ref drivers_saul capabilities.
 * Currently the driver has the following limitations:
 *- Does not allow addressing devices, only supports a single device on the bus.
 *- The 1-Wire bus handling is hardcoded to the driver.
 *- Does not allow configuration of sampling width.
 *
 * @note Due to timing issues present on some boards this drivers features two
 * ways of reading information from the sensor. The optimized uses accurate
 * delays to handle this, while the second way polls the line for changes. If
 * you know that your board can handle ~3us resolution with the xtimer module,
 * then the optimized way is recommended. To used the optimized way add the
 * ds18_optimized module. Also this driver test application has a whitelist of
 * the boards this driver has been tested on and known to work.
 *
 * @{
 *
 * @file
 * @brief       Driver for Maxim Integrated DS1822 and DS18B20 temperature
 *              sensors.
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 */

#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <stddef.h>
#include <stdint.h>

#include "mutex.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name ds18 commands
 * @{
 */
#define DS18_CMD_RSCRATCHPAD        (0xbe)
#define DS18_CMD_WRITESCRATCHPAD    (0x4e)
#define DS18_CMD_COPYSCRATCHPAD     (0x48)
#define DS18_CMD_RECALLE            (0xb8)
#define DS18_CMD_RPWRSPPLY          (0xb4)
#define DS18_CMD_SEARCHROM          (0xf0)

#define ONEWIRE_CMD_READROM            (0x33)
#define ONEWIRE_CMD_MATCHROM           (0x55)
#define ONEWIRE_CMD_SEARCHROM          (0xf0)
#define ONEWIRE_CMD_ALARMSEARCH        (0xec)
#define ONEWIRE_CMD_SKIPROM            (0xcc)
/** @} */

/**
 * @brief Device initialization parameters
 */
typedef struct {
    gpio_t pin;             /**< Pin the sensor is connected to */
    gpio_mode_t out_mode;   /**< Pin output mode */
    gpio_mode_t in_mode;    /**< Pin input mode (usually deduced from output mode) */
} onewire_params_t;

/**
 * @brief   Device descriptor for a ds18 device
 */
typedef struct {
    onewire_params_t params;   /**< Device Parameters */
    mutex_t lock;
} onewire_t;

typedef struct {
    uint8_t type;
    uint8_t serial_number[6];
    uint8_t crc;
} onewire_id_rom_t;

/*static const onewire_params_t onewire_params[] = {*/
/*    {*/
/*        .pin = GPIO_PIN(PORT_F, 13),*/
/*        .out_mode = GPIO_OD_PU,*/
/*    }*/
/*};*/

/**
 * @brief   Initialize a ds18 device
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       ds18 initialization struct
 *
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int onewire_init(onewire_t *dev, const onewire_params_t *params);

int onewire_reset(const onewire_t *dev); //TODO: make static?

void onewire_aquire(onewire_t *dev);
void onewire_release(onewire_t *dev);

int onewire_cmd_skiprom(onewire_t *dev);
int onewire_cmd_readrom(onewire_t *dev, onewire_id_rom_t *rom);

int onewire_read_bit(const onewire_t *dev, uint8_t *bit);
int onewire_read_byte(const onewire_t *dev, uint8_t *byte);
int onewire_read_word(const onewire_t *dev, uint16_t *word);
int onewire_read(const onewire_t *dev, void *buf, size_t size);

void onewire_write_bit(const onewire_t *dev, uint8_t bit);
void onewire_write_byte(const onewire_t *dev, uint8_t byte);
void onewire_write_word(const onewire_t *dev, uint16_t word);
void onewire_write(const onewire_t *dev, const void *buf, size_t size);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ONEWIRE_H */
