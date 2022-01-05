/*
 * Copyright (C) 2020 Michel Rottleuthner
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sps30
 * @brief       Device driver for the Sensirion SPS30 particulate matter sensor
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @file
 */
#define LOG_LEVEL LOG_DEBUG
#include "log.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "checksum/crc8.h"
#include "sps30.h"
#include "byteorder.h"
#include "kernel_defines.h"

/**
 * @name     SPS30 literal definitions
 * @{
 */
#define SPS30_I2C_ADDR       (0x69) /**< Fixed I2C address for the Sensor */
#define SPS30_CRC_POLY       (0x31) /**< Polynomial for the CRC calculation */
#define SPS30_CRC_INIT       (0xFF) /**< Init value for the CRC calculation */
#define SPS30_MESAURE_MODE   (0x03) /**< Fixed mode byte for start cmd */
#define SPS30_DUMMY_BYTE     (0x00) /**< Fixed dummy byte for unused bytes */
#define SPS30_PTR_LEN           (2) /**< Pointer address length in bytes */
/** @} */

/**
* @name      Address pointer values for all SPS30 I2C commands
* @{
*/
typedef enum {
    SPS30_CMD_START_MEASURE   = 0x0010, /**< Start measurement mode */
    SPS30_CMD_STOP_MEASURE    = 0x0104, /**< Stop measurement mode */
    SPS30_CMD_RD_DATA_READY   = 0x0202, /**< Read data-ready flag */
    SPS30_CMD_RD_MEASUREMENTS = 0x0300, /**< Read measured values */
    SPS30_CMD_RW_AUTOCLEAN    = 0x8004, /**< Read/write autoclean interval */
    SPS30_CMD_START_FAN_CLEAN = 0x5607, /**< Start fan cleaning */
    SPS30_CMD_RD_ARTICLE      = 0xD025, /**< Read article code */
    SPS30_CMD_RD_SERIAL       = 0xD033, /**< Read serial number */
    SPS30_CMD_RESET           = 0xD304, /**< Reset */
    SPS30_CMD_SLEEP           = 0x1001, /**< Sleep */
    SPS30_CMD_WAKE_UP         = 0x1103  /**< Wake-up */
} sps30_cmd_t;
/** @} */

/**
 * @brief    Combine payload and CRC into an SDS30 I2C data frame.
 *
 * @details  The CRC for `data` is calculated and written to `crcd_data`
 *           together with the payload itself.
 *           The format of `data` must be just the pure payload:
 *           { data[0], data[1], data[2], ..., data[n]}
 *           The format of `crcd_data` same as it shall be sent to the SPS30.
 *           It consists of payload byte-pairs followed by a single CRC byte:
 *           { data[0], data[1], csum[0],.., data[n-1], data[n], csum[n-1] }
 *
 * @param[in]    data        Source buffer containing just the raw payload
 * @param[in]    len         Size of the 'data' buffer
 * @param[out]   crcd_data   Destination buffer for combined payload and CRCs
 *
 * @pre          sizeof(crcd_data) must be equal to 1.5 * len
 *
 */
static inline void _cpy_add_crc(uint8_t *data, size_t len, uint8_t *crcd_data)
{
    for (size_t elem = 0; elem < len / 2; elem++) {
        int idx = (elem << 1);
        crcd_data[idx + elem] = data[idx];
        crcd_data[idx + elem + 1] = data[idx + 1];
        crcd_data[idx + elem + 2] = crc8(&data[idx], 2, SPS30_CRC_POLY,
                                         SPS30_CRC_INIT);
    }
}

/**
 * @brief    Check the CRC of an SDS30 I2C data frame.
 *
 * @details  The CRC contained in the `crcd_data` is checked and the payload is
 *           copied to `data`.
 *           The format of `data` will be just the pure payload:
 *           { data[0], data[1], data[2], ..., data[n]}
 *           The format of `crcd_data` is same as it is read from the SPS30.
 *           It consists of payload byte-pairs followed by a single CRC byte:
 *           { data[0], data[1], csum[0],.., data[n-1], data[n], csum[n-1] }
 *
 * @pre          sizeof(crcd_data) must be equal to 1.5 * len
 *
 * @param[out]   data        Destination buffer for just the raw payload
 * @param[in]    len         Size of the 'data' buffer
 * @param[in]    crcd_data   Source buffer containing combined payload and CRCs
 *
 * @return       true if all CRCs are valid
 * @return       false if at least one CRC is invalid
 */
static inline bool _cpy_check_crc(uint8_t *data, size_t len, const uint8_t *crcd_data)
{
    for (size_t elem = 0; elem < len / 2; elem++) {
        int idx = (elem << 1);
        data[idx] = crcd_data[idx + elem];
        data[idx + 1] = crcd_data[idx + elem + 1];

        if (crc8(&data[idx], 2, SPS30_CRC_POLY, SPS30_CRC_INIT) !=
            (crcd_data[idx + elem + 2])) {
            return false;
        }
    }
    return true;
}

/**
 * @brief    Communicates with an SPS30 device by reading or writing data.
 *
 * @details  This performs all three data transfer types supported by SPS30.
 *           (1) `Set Pointer`: writes a 16 bit pointer address to the device
 *           (2) `Set Pointer & Read Data`: (1) followed by separate data-read
 *           (3) `Set Pointer & Write Data`: (1) combined with a data-write
 *
 * @param[in]      dev        Pointer to SPS30 device handle
 * @param[in]      ptr_addr   16 bit pointer address used as command
 * @param[in/out]  data       Pre-allocated memory pointing to either the data
 *                            that will be sent to the device or to memory that
 *                            will hold the response. For type (1) transfers
 *                            this parameter will be ignored.
 * @param[in]      len        Length of `data` buffer, set to 0 for type (1)
 * @param[in]      read       set to true for reading or false for writing
 *
 * @return         SPS30_OK           if everything went fine
 * @return         -SPS30_CRC_ERROR   if the CRC check failed
 * @return         -SPS30_I2C_ERROR   if the I2C communication failed
 */
static int _rx_tx_data(const sps30_t *dev, uint16_t ptr_addr,
                       uint8_t *data, size_t len, bool read)
{
    int res = 0;
    unsigned retr = CONFIG_SPS30_ERROR_RETRY;

    i2c_acquire(dev->p.i2c_dev);

    do {
        size_t addr_data_crc_len = SPS30_PTR_LEN + len + len / 2;
        uint8_t frame_data[addr_data_crc_len];
        frame_data[0] = ptr_addr >> 8;
        frame_data[1] = ptr_addr & 0xFF;

        /* Both transfer types, `Set Pointer` and `Set Pointer & Read Data`
           require writing a pointer address to the device in a separate write
           transaction */
        if (len == 0 || read) {
            res = i2c_write_bytes(dev->p.i2c_dev, SPS30_I2C_ADDR,
                                  &frame_data[0], SPS30_PTR_LEN, 0);
        }

        if (res == 0 && read) {
            /* The `Set Pointer & Read Data` transfer type requires a separate
               read transaction to actually read the data */
            res = i2c_read_bytes(dev->p.i2c_dev, SPS30_I2C_ADDR,
                                 &frame_data[SPS30_PTR_LEN],
                                 addr_data_crc_len - SPS30_PTR_LEN, 0);

            if (!_cpy_check_crc(data, len, &frame_data[SPS30_PTR_LEN])) {
                res = -SPS30_CRC_ERROR;
            }
        } else {
            /* For the `Set Pointer & Write Data` transfer type the full frame
               is transmitted as one single chunk */
            _cpy_add_crc(data, len, &frame_data[SPS30_PTR_LEN]);
            res = i2c_write_bytes(dev->p.i2c_dev, SPS30_I2C_ADDR,
                                  &frame_data[0], addr_data_crc_len, 0);
        }
    } while (res != 0 && retr--);

    i2c_release(dev->p.i2c_dev);

    return res == 0 ? SPS30_OK : -SPS30_I2C_ERROR;
}

int sps30_init(sps30_t *dev, const sps30_params_t *params)
{
    assert(dev && params);
    dev->p = *params;
    return sps30_start_measurement(dev);
}

int sps30_start_measurement(const sps30_t *dev)
{
    assert(dev);
    uint8_t data[] = {SPS30_MESAURE_MODE, SPS30_DUMMY_BYTE};
    return _rx_tx_data(dev, SPS30_CMD_START_MEASURE, (uint8_t*)data,
                       sizeof(data), false);
}

int sps30_stop_measurement(const sps30_t *dev)
{
    assert(dev);
    return _rx_tx_data(dev, SPS30_CMD_STOP_MEASURE, NULL, 0, false);
}

bool sps30_data_ready(const sps30_t *dev, int *error)
{
    assert(dev);
    uint8_t data[2];
    int res = _rx_tx_data(dev, SPS30_CMD_RD_DATA_READY, data, sizeof(data),
                          true);
    if (*error) {
        *error = res;
    }
    return (res == SPS30_OK) && data[1];
}

int sps30_read_measurement(const sps30_t *dev, sps30_data_t *data)
{
    /* This compile time check is needed to ensure the below method used for
       endianness conversion will work as expected */
    static_assert(sizeof(sps30_data_t) == (sizeof(float) * 10),
                  "sps30_data_t must be sized 10 floats");
    assert(dev && data);

    /* The target buffer is also used for storing the raw data temporarily */
    int res = _rx_tx_data(dev, SPS30_CMD_RD_MEASUREMENTS, (uint8_t*)data,
                          sizeof(sps30_data_t), true);

    /* The sps30_data_t consists only of floats, so it is safe to treat it as
       an array of 32 bit values for swapping to correct endianness */
    uint32_t *values = (uint32_t*)data;

    /* swap to the endianness of this platform */
    for (unsigned i = 0; i < (sizeof(sps30_data_t) / sizeof(uint32_t)); i++) {
        values[i] = ntohl(values[i]);
    }

    return res;
}

int sps30_read_ac_interval(const sps30_t *dev, uint32_t *seconds) {
    assert(dev);
    int res = _rx_tx_data(dev, SPS30_CMD_RW_AUTOCLEAN, (uint8_t*)seconds,
                          sizeof(uint32_t), true);
    *seconds = ntohl(*seconds);
    return res;
}

int sps30_write_ac_interval(const sps30_t *dev, uint32_t seconds)
{
    assert(dev);
    seconds = htonl(seconds);
    int res = _rx_tx_data(dev, SPS30_CMD_RW_AUTOCLEAN, (uint8_t*)&seconds,
                          sizeof(uint32_t), false);
    return res;
}

int sps30_start_fan_clean(const sps30_t *dev)
{
    assert(dev);
    return _rx_tx_data(dev, SPS30_CMD_START_FAN_CLEAN, NULL, 0, false);
}

int sps30_read_article_code(const sps30_t *dev, char *str, size_t len)
{
    assert(dev && str && (len == SPS30_SER_ART_LEN));
    return _rx_tx_data(dev, SPS30_CMD_RD_ARTICLE, (uint8_t*)str, len, true);
}

int sps30_read_serial_number(const sps30_t *dev, char *str, size_t len)
{
    assert(dev && str && (len == SPS30_SER_ART_LEN));
    return _rx_tx_data(dev, SPS30_CMD_RD_SERIAL, (uint8_t*)str, len, true);
}

int sps30_reset(const sps30_t *dev)
{
    assert(dev);
    return _rx_tx_data(dev, SPS30_CMD_RESET, NULL, 0, false);
}

int sps30_sleep(const sps30_t *dev)
{
    assert(dev);
    sps30_stop_measurement(dev);
    return _rx_tx_data(dev, SPS30_CMD_SLEEP, NULL, 0, false);
}

int sps30_wakeup(const sps30_t *dev)
{
    assert(dev);
    /* Send I2C start stop sequence to re-enable I2C interface on sensor */
    i2c_write_bytes(dev->p.i2c_dev, SPS30_I2C_ADDR, NULL, 0, 0);
    return _rx_tx_data(dev, SPS30_CMD_WAKE_UP, NULL, 0, false);
}
