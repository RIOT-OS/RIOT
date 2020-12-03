/*
 * Copyright (C) 2016-2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ds18b20 DS18B20 temperature sensor driver
 * @ingroup     drivers_sensors
 * @brief       Driver for DS18B20 temperature sensors
 *
 * @{
 * @file
 * @brief       DS18B20 driver interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef DS18B20_H
#define DS18B20_H

#ifdef __cplusplus
extern "C" {
#endif

#include "onewire.h"

/**
 * @brief   1-Wire family code identifying DS18B20 sensors (LSB in ROM)
 */
#define DS18B20_FAMILY_CODE         0x28

/**
 * @brief   Return codes used by this driver
 */
enum {
    DS18B20_OK          =  0,       /**< all good */
    DS18B20_NODEV       = -1,       /**< no device detected */
    DS18B20_ERR_CRC     = -2,       /**< checksum error */
};

/**
 * @brief   Available resolution values
 */
typedef enum {
    DS18B20_RES_9BIT =  0,          /**< conversion time ~93.75ms */
    DS18B20_RES_10BIT = 1,          /**< conversion time ~187.5ms */
    DS18B20_RES_11BIT = 2,          /**< conversion time ~375ms */
    DS18B20_RES_12BIT = 3,          /**< conversion time ~750ms */
} ds18b20_res_t;

/**
 * @brief   Static device configuration values
 */
typedef struct {
    onewire_rom_t rom;              /**< Onewire bus address */
    ds18b20_res_t res;              /**< sampling resolution */
} ds18b20_params_t;

/**
 * @brief   DS18B20 device descriptor
 */
typedef struct {
    onewire_t *bus;                 /**< reference to the used Onewire bus */
    onewire_rom_t rom;              /**< Onewire bus address */
    ds18b20_res_t res;              /**< conversion time */
} ds18b20_t;

/**
 * @brief   Initialize a DS18B20 sensor
 *
 * @param[out] dev      device descriptor
 * @param[in] owi       pointer a pre-initialized Onewire bus descriptor
 * @param[in] params    device configuration options
 *
 * @return  DS18B20_OK on success
 * @return  DS18B20_NODEV if device did not respond
 * @return  DS18B20_ERR_CRC on checksum error when transferring data from device
 */
int ds18b20_init(ds18b20_t *dev, onewire_t *owi, const ds18b20_params_t *params);

/**
 * @brief   Test if the given ROM code identifies a DS18B20 device
 *
 * @param[in] rom       ROM code to check
 *
 * @return  DS18B20_OK if given ROM belongs to a DS18B20 device
 * @return  DS18B20_NODEV if ROM does not belong to a DS18B20 device
 */
static inline int ds18b20_id(const onewire_rom_t *rom)
{
    return (rom->u8[0] == DS18B20_FAMILY_CODE) ? DS18B20_OK : DS18B20_NODEV;
}

/**
 * @brief   Get the appropriate conversion time [in us]
 *
 * @param[in] dev       device descriptor
 *
 * @return  appropriate conversion time [in us]
 */
uint32_t ds18b20_get_conv_time_us(const ds18b20_t *dev);

/**
 * @brief   Trigger a new temperature data conversion
 *
 * To get the sampled temperature data one has to wait for the sampling time
 * according to the configured resolution before reading the sampled data.
 *
 * @param[in] dev       device to trigger
 *
 * @return  DS18B20_OK on success
 * @return  DS18B20_NODEV if device did not respond
 */
int ds18b20_trigger(const ds18b20_t *dev);

/**
 * @brief   Trigger a new temperature data conversion for ALL devices connected
 *          to the same bus
 *
 * @param[in] dev       device descriptor, only the Onewire interface is used
 *
 * @return  DS18B20_OK on success
 * @return  DS18B20_NODEV if no device responded
 */
int ds18b20_trigger_all(const ds18b20_t *dev);

/**
 * @brief   Read the sampled temperature data from the given device
 *
 * Before reading the temperature data, a conversion has to be triggered using
 * the ds18b20_trigger() or ds18b20_trigger_all() functions. After triggering
 * the conversion, the conversion time corresponding to the configure resolution
 * has to be awaited before reading the temperature data.
 *
 * @param[in] dev       device descriptor
 * @param[out] temp     sampled temperature data
 *
 * @return  DS18B20_OK on success
 * @return  DS18B20_NODEV if device did not respond
 * @return  DS18B20_ERR_CRC on checksum error when transferring data from device
 */
int ds18b20_read(const ds18b20_t *dev, int16_t *temp);

/**
 * @brief   Trigger a reading, wait, and read the sampled temperature data
 *
 * This is a convenience function which combines calls to ds18b20_trigger() and
 * ds18b20_read() with a wait period of the conversion time fitting the
 * configured resolution.
 *
 * @note    This function will block from 93ms to 750ms, depending on the
 *          configured resolution.
 *
 * @param[in] dev       device to read from
 * @param[out] temp     sampled temperature data
 *
 * @return  DS18B20_OK on success
 * @return  DS18B20_NODEV if device did not respond
 * @return  DS18B20_ERR_CRC on checksum error when transferring data from device
 */
int ds18b20_get_temp(const ds18b20_t *dev, int16_t *temp);

#ifdef __cplusplus
}
#endif

#endif /* DS18B20_H */
/** @} */
