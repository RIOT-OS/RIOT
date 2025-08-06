/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "vl53l1_platform.h"

#include <errno.h>
#include <string.h>

#include "macros/units.h"
#include "periph/i2c.h"
#include "ztimer.h"

#include "vl53l1x.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define VL53L1_I2C_DEV      (dev >> 8)
#define VL53L1_I2C_ADDR     (dev & 0xff)

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[vl53l1x] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#define DEBUG_DEV(format, dev, ...) \
        DEBUG("[vl53l1x] %s i2c dev=%u addr=%02x: " format "\n", \
              __func__, VL53L1_I2C_DEV, VL53L1_I2C_ADDR, ## __VA_ARGS__);

#define DEBUG_NODEV(format, ...) \
        DEBUG("[vl53l1x] %s: " format "\n", \
              __func__, ## __VA_ARGS__);

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond);
#define DEBUG_DEV(format, dev, ...)
#define DEBUG_NODEV(format, ...)

#endif /* ENABLE_DEBUG */

#define VL53L1_BUFSIZ 32

static uint8_t _buffer[VL53L1_BUFSIZ];

int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    ASSERT_PARAM(pdata != NULL);
    DEBUG_DEV("index=%04x pdata=%p count=%"PRIu32, dev, index, pdata, count);

    i2c_acquire(VL53L1_I2C_DEV);

    /* write blockwise with a maximum size of VL53L1_BUFSIZ-2 */
    while (count) {

        uint8_t cnt = (count < (VL53L1_BUFSIZ - 2)) ? count : VL53L1_BUFSIZ - 2;

        /* fill the first 2 bytes of the buffer with the index */
        _buffer[0] = (index >> 8) & 0xff;
        _buffer[1] = index & 0xff;

        /* fill the buffer with data bytes */
        memcpy(_buffer + 2, pdata, cnt);

        if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, _buffer, cnt + 2, 0) != 0) {
            i2c_release(VL53L1_I2C_DEV);
            return -EIO;
        }
        count -= cnt;
        pdata += cnt;
        index += cnt;
    }

    /* release the I2C device */
    i2c_release(VL53L1_I2C_DEV);

    return 0;
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    ASSERT_PARAM(pdata != NULL);
    DEBUG_DEV("index=%04x pdata=%p count=%"PRIu32, dev, index, pdata, count);

    /* acquire the I2C device */
    i2c_acquire(VL53L1_I2C_DEV);

    /* read blockwise with a maximum size of VL53L1_BUFSIZ */
    while (count) {

        uint8_t bytes[2] = { (index >> 8) & 0xff, index & 0xff };

        /* write the register index for the block */
        if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0) {
            i2c_release(VL53L1_I2C_DEV);
            return -EIO;
        }

        uint8_t cnt = (count < VL53L1_BUFSIZ) ? count : VL53L1_BUFSIZ;

        if (i2c_read_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, _buffer, cnt, 0) != 0) {
            i2c_release(VL53L1_I2C_DEV);
            return -EIO;
        }

        memcpy(pdata, _buffer, cnt);
        count -= cnt;
        pdata += cnt;
        index += cnt;
    }

    /* release the I2C device */
    i2c_release(VL53L1_I2C_DEV);

    return 0;
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data)
{
    int8_t Status = 0;

    DEBUG_DEV("index=%04x data=%02x", dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[3] = { (index >> 8) & 0xff, index & 0xff, data };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 3, 0) != 0) {
        Status = -EIO;
    }

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data)
{
    int8_t Status = 0;

    DEBUG_DEV("index=%04x data=%04x", dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[4] = { (index >> 8) & 0xff, index & 0xff,
                         (data >> 8) & 0xff, data & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 4, 0) != 0) {
        Status = -EIO;
    }

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data)
{
    int8_t Status = 0;

    DEBUG_DEV("index=%04x data=%08"PRIx32, dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[6] = { (index >> 8) & 0xff, index & 0xff,
                         (data >> 24) & 0xff, (data >> 16) & 0xff,
                         (data >> 8) & 0xff, data & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 6, 0) != 0) {
        Status = -EIO;
    }

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data)
{
    int8_t Status = 0;

    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("index=%04x data=%p", dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[2] = { (index >> 8) & 0xff, index & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0 ||
        i2c_read_byte(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, data, 0) != 0) {
        Status = -EIO;
    }

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data)
{
    int8_t Status = 0;

    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("index=%04x data=%p", dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[2] = { (index >> 8) & 0xff, index & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0 ||
        i2c_read_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0) {
        Status = -EIO;
    }
    *data = (bytes[0] << 8) + (bytes[1]);

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data)
{
    int8_t Status = 0;

    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("index=%04x data=%p", dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[4] = { (index >> 8) & 0xff, index & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0 ||
        i2c_read_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 4, 0) != 0) {
        Status = -EIO;
    }
    *data = ((uint32_t)bytes[0] << 24) +
            ((uint32_t)bytes[1] << 16) +
            ((uint32_t)bytes[2] <<  8) + bytes[3];

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms)
{
    int8_t Status = 0;

    (void)dev;
    DEBUG_NODEV("wait_ms=%"PRIi32, wait_ms);

    ztimer_sleep(ZTIMER_MSEC, wait_ms);

    return Status;
}
