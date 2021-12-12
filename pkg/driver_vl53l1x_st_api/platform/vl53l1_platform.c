/*
 * Copyright (c) 2017, STMicroelectronics - All Rights Reserved
 *
 * This file is part of VL53L1 Core and is dual licensed,
 * either 'STMicroelectronics
 * Proprietary license'
 * or 'BSD 3-clause "New" or "Revised" License' , at your option.
 *
 *******************************************************************************
 *
 * 'STMicroelectronics Proprietary license'
 *
 *******************************************************************************
 *
 * License terms: STMicroelectronics Proprietary in accordance with licensing
 * terms at www.st.com/sla0081
 *
 * STMicroelectronics confidential
 * Reproduction and Communication of this document is strictly prohibited unless
 * specifically authorized in writing by STMicroelectronics.
 *
 *
 *******************************************************************************
 *
 * Alternatively, VL53L1 Core may be distributed under the terms of
 * 'BSD 3-clause "New" or "Revised" License', in which case the following
 * provisions apply instead of the ones mentioned above :
 *
 *******************************************************************************
 *
 * License terms: BSD 3-clause "New" or "Revised" License.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 *******************************************************************************
 */

/**
 * Platform dependent I2C implementation
 *
 * Copyright (C) 2021 Gunar Schorcht
 */


#include "vl53l1_platform.h"
#include "vl53l1_api.h"

#include <string.h>

#include "periph/i2c.h"
#include "xtimer.h"

#include "vl53l1x.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define VL53L1_I2C_DEV      (((vl53l1x_t*)Dev)->params.i2c_dev)
#define VL53L1_I2C_ADDR     (Dev->I2cDevAddr)
#define VL5321_ERROR_I2C    ((VL53L1_ERROR_PLATFORM_SPECIFIC_START) - 1)

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[vl53l1x] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#define DEBUG_DEV(f, Dev, ...) \
        DEBUG("[vl53l1x] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, VL53L1_I2C_DEV, VL53L1_I2C_ADDR, ## __VA_ARGS__);

#define DEBUG_NODEV(f, ...) \
        DEBUG("[vl53l1x] %s: " f "\n", \
              __func__, ## __VA_ARGS__);

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond);
#define DEBUG_DEV(f, Dev, ...)
#define DEBUG_NODEV(f, ...)

#endif /* ENABLE_DEBUG */

#define VL53L1_BUFSIZ 32

static uint8_t _buffer[VL53L1_BUFSIZ];

VL53L1_Error VL53L1_WriteMulti(VL53L1_DEV Dev,
                               uint16_t index, uint8_t *pdata, uint32_t count)
{
    ASSERT_PARAM(Dev != NULL);
    ASSERT_PARAM(pdata != NULL);
    DEBUG_DEV("index=%04x pdata=%p count=%u", Dev, index, pdata, count);

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
            return VL5321_ERROR_I2C;
        }
        count -= cnt;
        pdata += cnt;
        index += cnt;
    }

    /* release the I2C device */
    i2c_release(VL53L1_I2C_DEV);

    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_ReadMulti(VL53L1_DEV Dev,
                              uint16_t index, uint8_t *pdata, uint32_t count)
{
    ASSERT_PARAM(Dev != NULL);
    ASSERT_PARAM(pdata != NULL);
    DEBUG_DEV("index=%04x pdata=%p count=%u", Dev, index, pdata, count);

    /* acquire the I2C device */
    i2c_acquire(VL53L1_I2C_DEV);

    /* read blockwise with a maximum size of VL53L1_BUFSIZ */
    while (count) {

        uint8_t bytes[2] = { (index >> 8) & 0xff, index & 0xff };

        /* write the register index for the block */
        if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0) {
            i2c_release(VL53L1_I2C_DEV);
            return VL5321_ERROR_I2C;
        }

        uint8_t cnt = (count < VL53L1_BUFSIZ) ? count : VL53L1_BUFSIZ;

        if (i2c_read_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, _buffer, cnt, 0) != 0) {
            i2c_release(VL53L1_I2C_DEV);
            return VL5321_ERROR_I2C;
        }

        memcpy(pdata, _buffer, cnt);
        count -= cnt;
        pdata += cnt;
        index += cnt;
    }

    /* release the I2C device */
    i2c_release(VL53L1_I2C_DEV);

    return VL53L1_ERROR_NONE;
}

VL53L1_Error VL53L1_WrByte(VL53L1_DEV Dev, uint16_t index, uint8_t data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    ASSERT_PARAM(Dev != NULL);
    DEBUG_DEV("index=%04x data=%02x", Dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[3] = { (index >> 8) & 0xff, index & 0xff, data };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 3, 0) != 0) {
        Status = VL5321_ERROR_I2C;
    }

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

VL53L1_Error VL53L1_WrWord(VL53L1_DEV Dev, uint16_t index, uint16_t data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    ASSERT_PARAM(Dev != NULL);
    DEBUG_DEV("index=%04x data=%04x", Dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[4] = { (index >> 8) & 0xff, index & 0xff,
                         (data >> 8) & 0xff, data & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 4, 0) != 0) {
        Status = VL5321_ERROR_I2C;
    }

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

VL53L1_Error VL53L1_WrDWord(VL53L1_DEV Dev, uint16_t index, uint32_t data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    ASSERT_PARAM(Dev != NULL);
    DEBUG_DEV("index=%04x data=%08x", Dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[6] = { (index >> 8) & 0xff, index & 0xff,
                         (data >> 24) & 0xff, (data >> 16) & 0xff,
                         (data >> 8) & 0xff, data & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 6, 0) != 0) {
        Status = VL5321_ERROR_I2C;
    }

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

VL53L1_Error VL53L1_UpdateByte(VL53L1_DEV Dev,
                               uint16_t index, uint8_t AndData, uint8_t OrData)
{
    ASSERT_PARAM(Dev != NULL);
    DEBUG_DEV("index=%04x and=%02x or=%02x", Dev, index, AndData, OrData);

    uint8_t data;
    VL53L1_Error status = VL53L1_RdByte(Dev, index, &data);
    if (status != VL53L1_ERROR_NONE) {
        return status;
    }
    data &= AndData;
    data |= OrData;

    return VL53L1_WrByte(Dev, index, data);
}

VL53L1_Error VL53L1_RdByte(VL53L1_DEV Dev, uint16_t index, uint8_t *data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    ASSERT_PARAM(Dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("index=%04x data=%p", Dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[2] = { (index >> 8) & 0xff, index & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0 ||
        i2c_read_byte(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, data, 0) != 0) {
        Status = VL5321_ERROR_I2C;
    }

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

VL53L1_Error VL53L1_RdWord(VL53L1_DEV Dev, uint16_t index, uint16_t *data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    ASSERT_PARAM(Dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("index=%04x data=%p", Dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[2] = { (index >> 8) & 0xff, index & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0 ||
        i2c_read_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0) {
        Status = VL5321_ERROR_I2C;
    }
    *data = (bytes[0] << 8) + (bytes[1]);

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

VL53L1_Error VL53L1_RdDWord(VL53L1_DEV Dev, uint16_t index, uint32_t *data)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;

    ASSERT_PARAM(Dev != NULL);
    ASSERT_PARAM(data != NULL);
    DEBUG_DEV("index=%04x data=%p", Dev, index, data);

    i2c_acquire(VL53L1_I2C_DEV);

    uint8_t bytes[4] = { (index >> 8) & 0xff, index & 0xff };

    if (i2c_write_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 2, 0) != 0 ||
        i2c_read_bytes(VL53L1_I2C_DEV, VL53L1_I2C_ADDR, bytes, 4, 0) != 0) {
        Status = VL5321_ERROR_I2C;
    }
    *data = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + (bytes[3]);

    i2c_release(VL53L1_I2C_DEV);

    return Status;
}

VL53L1_Error VL53L1_GetTickCount(uint32_t *ptick_count_ms)
{
    VL53L1_Error status  = VL53L1_ERROR_NONE;

    ASSERT_PARAM(ptick_count_ms != NULL);
    DEBUG_NODEV("ptick_count_ms=%p", ptick_count_ms);

    uint64_t time = xtimer_now_usec64();
    *ptick_count_ms = time / (US_PER_MS);

    return status;
}

VL53L1_Error VL53L1_GetTimerFrequency(int32_t *ptimer_freq_hz)
{
    VL53L1_Error status  = VL53L1_ERROR_NONE;

    ASSERT_PARAM(ptimer_freq_hz != NULL);
    DEBUG_NODEV("ptimer_freq_hz=%p", ptimer_freq_hz);

    *ptimer_freq_hz = (US_PER_MS * MS_PER_SEC);

    return status;
}

VL53L1_Error VL53L1_WaitMs(VL53L1_Dev_t *pdev, int32_t wait_ms){
    VL53L1_Error status  = VL53L1_ERROR_NONE;

    ASSERT_PARAM(pdev != NULL);
    DEBUG_NODEV("wait_ms=%d", wait_ms);

    xtimer_usleep (wait_ms * US_PER_MS);

    return status;
}

VL53L1_Error VL53L1_WaitUs(VL53L1_Dev_t *pdev, int32_t wait_us)
{
    VL53L1_Error status  = VL53L1_ERROR_NONE;

    ASSERT_PARAM(pdev != NULL);
    DEBUG_NODEV("wait_us=%d", wait_us);

    xtimer_usleep (wait_us);

    return status;
}

VL53L1_Error VL53L1_WaitValueMaskEx(VL53L1_Dev_t *pdev,
                                    uint32_t      timeout_ms,
                                    uint16_t      index,
                                    uint8_t       value,
                                    uint8_t       mask,
                                    uint32_t      poll_delay_ms)
{
    VL53L1_Error status  = VL53L1_ERROR_NONE;

    ASSERT_PARAM(pdev != NULL);
    DEBUG_NODEV("index=%04x value=%02x mask=%02x timeout_ms=%u poll_delay_ms=%u",
                index, value, mask, timeout_ms, poll_delay_ms);

    uint8_t data;
    while (timeout_ms > 0) {
        if ((status = VL53L1_RdByte(pdev, index, &data)) != VL53L1_ERROR_NONE) {
            return status;
        }
        if ((data & mask) == value) {
            return VL53L1_ERROR_NONE;
        }
        xtimer_usleep(poll_delay_ms * US_PER_MS);
        timeout_ms -= (poll_delay_ms < timeout_ms) ? poll_delay_ms : timeout_ms;
    }

    return status;
}
