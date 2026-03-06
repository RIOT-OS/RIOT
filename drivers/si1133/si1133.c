/*
 * SPDX-FileCopyrightText: 2020 iosabi
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_si1133
 * @{
 *
 * @file
 * @brief       Device driver implementation for the SI1133 UV/IR/Ambient light
 *              sensor with I2C interface.
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "timex.h"
#include "ztimer.h"

#include "periph/i2c.h"

#include "si1133.h"
#include "si1133_internals.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief The command execution timeout in blocking mode.
 *
 * When forcing a measurement (SI1133_CMD_FORCE) the command is not done until
 * all the measurements from all the channels are taken. This may take some time
 * depending on the configuration. After the expected sampling time is over we
 * wait up to this command timeout value for the command to be ready.
 */
#define SI1133_COMMAND_TIMEOUT_USEC 10000

/**
 * @brief Return the expected sampling time in us for a FORCE command.
 */
static uint32_t _si1133_force_time_us(si1133_t *dev)
{
    /* Sample time measured in 512 clocks of the 21 MHz, or about 24.4 us. */
    uint32_t sample_time = 0;

    for (uint32_t i = 0; i < dev->num_channels; i++) {
        /* A forced sample consists of (1 << sw_gain) measurements where each
         * one has a total 155 us (about 7 * 24.4 us) of processing time, plus
         * two ADC samples. Each ADC sample requires a t_adstart time (48.8 us
         * or 2 * 24.4 us) plus the time configured by the sample_time_log. */
        sample_time += (2 * (2 + (1u << dev->channel[i].sample_time_log)) + 7)
                       << dev->channel[i].sw_gain;
    }
    /* The resulting sample_time value in microseconds is sample_time * 512 / 21
     * the sample_time is already a 24-bit number so we split the logic to
     * fit in 32-bit arithmetic using the fact that 512 == 24 * 21 + 8. */
    return sample_time * 24 + sample_time * 8 / 21;
}

/**
 * @brief Run a command with no arguments.
 *
 * For commands CMD_PARAM_SET or CMD_PARAM_QUERY use @ref _si1133_set_param
 * and @ref _si1133_get_param respectively instead.
 */
static int _si1133_run_command(si1133_t *dev, uint8_t command)
{
    DEBUG("[si1133] run_command: 0x%.2x, cmd_counter=%d\n",
          (unsigned)command, dev->cmd_counter);
    int ret;
    ret = i2c_write_reg(dev->i2c_dev, dev->address,
                        SI1133_REG_COMMAND, command, 0 /* flags */);
    if (ret) {
        DEBUG("[si1133] write command I2C error: %s\n", strerror(-ret));
        return SI1133_ERR_I2C;
    }

    if (command == SI1133_CMD_FORCE) {
        /* Wait for the expected force acquisition time. */
        ztimer_sleep(ZTIMER_USEC, _si1133_force_time_us(dev));
    }

    if (command == SI1133_CMD_RESET_SW) {
        /* Reset command sets the command counter of 0x0f. */
        dev->cmd_counter = 0x0f;
        /* Reset command puts the device in "Initialization Mode" which requires
         * us to wait until the device is ready. */
        ztimer_sleep(ZTIMER_USEC, SI1133_STARTUP_TIME_MS * US_PER_MS);
    }
    else if (command == SI1133_CMD_RESET_CMD_CTR) {
        /* The reset cmd_counter command, well, resets it to 0. */
        dev->cmd_counter = 0;
    }
    else {
        /* Increment the expected 4-bit command counter value. */
        dev->cmd_counter = (dev->cmd_counter + 1) & SI1133_RESP0_COUNTER_MASK;
    }

    uint8_t new_cmd_ctr;
    ztimer_now_t start_time;
    bool retry = false;
    while (1) {
        ret = i2c_read_reg(dev->i2c_dev, dev->address, SI1133_REG_RESPONSE0,
                           &new_cmd_ctr, 0 /* flags */);
        if (ret) {
            DEBUG("[si1133] read RESPONSE0 I2C error: %s\n", strerror(-ret));
            return SI1133_ERR_I2C;
        }
        if (new_cmd_ctr & SI1133_RESP0_CMD_ERR_MASK) {
            DEBUG("[si1133] Command 0x%.2x returned error %d\n",
                  (unsigned)command, new_cmd_ctr & SI1133_RESP0_COUNTER_MASK);
            /* Error code 2 is "ADC or accumulation overflow", while error code
             * 3 is output buffer overflow which can only occur in BURST mode.
             * However, in FORCE mode after the first overflow we can get an
             * error code 3 if we change the settings in between the overflow
             * and a new FORCE command, probably due to a silicon bug when
             * handling sw_gain overflows since it is not possible to write more
             * than 18 bytes of output in FORCE mode while the output buffer is
             * 26 bytes long. */
            if ((new_cmd_ctr & SI1133_RESP0_COUNTER_MASK) >= 2) {
                return SI1133_ERR_OVERFLOW;
            }
            return SI1133_ERR_LOGIC;
        }
        /* The reset command is done when the RUNNING flag is clear, the other
         * commands are done when the command value is set to the expected
         * one. */
        if ((command == SI1133_CMD_RESET_SW)
            ? !(new_cmd_ctr & SI1133_RESP0_RUNNING_MASK)
            : (dev->cmd_counter ==
               (new_cmd_ctr & SI1133_RESP0_COUNTER_MASK))) {
            break;
        }
        /* The command didn't yet finish in this case so it should be in running
         * state and we need to retry the loop with a timeout. This avoids
         * calling ztimer for commands that are immediate. */
        if (retry) {
            if (ztimer_now(ZTIMER_USEC) - start_time > SI1133_COMMAND_TIMEOUT_USEC) {
                DEBUG("[si1133] Command 0x%.2x timeout.\n", (unsigned)command);
                return SI1133_ERR_LOGIC;
            }
        }
        else {
            retry = true;
            start_time = ztimer_now(ZTIMER_USEC);
        }
    }
    if (retry) {
        DEBUG("[si1133] Command overtime: %" PRIu32 " us.\n",
              ztimer_now(ZTIMER_USEC) - start_time);
    }
    return SI1133_OK;
}

static int _si1133_set_param(si1133_t *dev, uint8_t param, uint8_t value)
{
    int ret;

    ret = i2c_write_reg(dev->i2c_dev, dev->address,
                        SI1133_REG_HOSTIN0, value, 0 /* flags */);
    if (ret) {
        DEBUG("[si1133] write HOSTIN0 I2C error: %s\n", strerror(-ret));
        return SI1133_ERR_I2C;
    }
    ret = _si1133_run_command(dev, param | SI1133_CMD_PARAM_SET);
    if (ret) {
        return ret;
    }

    uint8_t resp1;
    ret = i2c_read_reg(dev->i2c_dev, dev->address,
                       SI1133_REG_RESPONSE1, &resp1, 0);
    if (ret) {
        DEBUG("[si1133] read RESPONSE1 I2C error: %s\n", strerror(-ret));
        return SI1133_ERR_I2C;
    }
    if (resp1 != value) {
        DEBUG("[si1133] Expected to read back value 0x%.2" PRIu8
              " when setting param 0x%.2" PRIu8 " but got 0x%.2" PRIu8 "\n",
              value, param, resp1);
        return SI1133_ERR_LOGIC;
    }
    return SI1133_OK;
}

/**
 * @brief Reset the device.
 */
static int _si1133_reset(si1133_t *dev)
{
    DEBUG("[si1133] reset()\n");
    dev->num_channels = 0;
    for (uint32_t i = 0; i < SI1133_NUM_CHANNELS; i++) {
        /* Initialize the config with invalid values to force the first config
         * call to update it. */
        dev->channel[i].sw_gain = 0xff;
    }
    int ret = _si1133_run_command(dev, SI1133_CMD_RESET_SW);
    if (ret) {
        return ret;
    }

    return _si1133_run_command(dev, SI1133_CMD_RESET_CMD_CTR);
}

/**
 * @brief Configure a single channel with the passed parameters.
 */
static si1133_ret_code_t _si1133_configure_channel(
    si1133_t *dev, uint32_t channel_id, const si1133_channel_t *channel)
{
    if (channel->sample_time_log > 14 || channel->sw_gain > 7) {
        return SI1133_ERR_PARAMS;
    }

    if (!memcmp(channel, &dev->channel[channel_id], sizeof(si1133_channel_t))) {
        /* Avoid the I2C roundtrip if the channel configuration didn't
         * change. */
        return SI1133_OK;
    }

    /* Generate the channel configuration. */
    si1133_channel_params_t config;
#define SI1133_SENS_CASE(x)                                       \
    case SI1133_SENS_ ## x:                                       \
        config.adcconfig =                                        \
            SI1133_ADCMUX_ ## x << SI1133_ADCCONFIG_ADCMUX_SHIFT; \
        break;

    switch (channel->sensor) {
        SI1133_SENS_CASE(SMALL_IR)
        SI1133_SENS_CASE(MEDIUM_IR)
        SI1133_SENS_CASE(LARGE_IR)
        SI1133_SENS_CASE(WHITE)
        SI1133_SENS_CASE(LARGE_WHITE)
        SI1133_SENS_CASE(UV)
        SI1133_SENS_CASE(DEEP_UV)
    default:
        return SI1133_ERR_PARAMS;
    }
    /* Use normal decimation (1) except in the extremes where we must use
     * the other decimation values. */
    const uint8_t hw_gain = channel->sample_time_log == 0
                            ? 0
                            : (channel->sample_time_log <= 11
                               ? channel->sample_time_log - 1
                               : 11);
    /* Select decimation. A value of "0" in this field is the "normal"
     * decimation, which corresponds to 1 in our "decimation" equation.
     * The values in this hardware field are offset by 3. */
    const uint8_t decimation =
        (channel->sample_time_log - hw_gain + 3) & 3;
    config.adcconfig |= decimation << SI1133_ADCCONFIG_DECIM_RATE_SHIFT;
    /* HSIG = 0, SW_GAIN and HW_GAIN as configured. */
    config.adcsens = (hw_gain << SI1133_ADCSENS_HW_GAIN_SHIFT) |
                     (channel->sw_gain << SI1133_ADCSENS_SW_GAIN_SHIFT);
    /* 24-bit output, no output shift, no threshold. */
    config.adcpost = SI1133_ADCPOST_24BIT_OUT_MASK;
    /* No counter, this will only be used in Force mode. */
    config.measconfig = 0;

    DEBUG("[si1133] config: %.2x %.2x %.2x %.2x\n",
          ((uint8_t *)&config)[0], ((uint8_t *)&config)[1],
          ((uint8_t *)&config)[2], ((uint8_t *)&config)[3]);

    for (uint8_t i = 0; i < sizeof(config); i++) {
        uint8_t param = SI1133_PARAM_ADCCONFIG0 + sizeof(config) * channel_id +
                        i;
        si1133_ret_code_t ret =
            _si1133_set_param(dev, param, ((uint8_t *)&config)[i]);
        if (ret) {
            return ret;
        }

    }
    memcpy(&(dev->channel[channel_id]), channel, sizeof(si1133_channel_t));
    return SI1133_OK;
}

static int _si1133_read_values(si1133_t *dev, int32_t *values,
                               uint32_t num_channels)
{
    /* We can read all registers in a single I2C burst. */
    uint8_t data[3 * num_channels];

    /* We only request 24-bit values from the device so they are all 3 byte
     * long. */
    int ret = i2c_read_regs(dev->i2c_dev, dev->address, SI1133_REG_HOSTOUTx,
                            data, 3 * num_channels, 0 /* flags */);

    if (ret) {
        return SI1133_ERR_I2C;
    }
    uint8_t *offset = data;
    for (uint8_t i = 0; i < num_channels; i++) {
        /* Sign-extend the first 8-bit value before shifting. */
        int32_t value = ((int32_t)(int8_t)*(offset++)) << 16;
        value |= *(offset++) << 8u;
        value |= *(offset++);
        values[i] = value;
    }
    return SI1133_OK;
}

si1133_ret_code_t si1133_init(si1133_t *dev, const si1133_params_t *params)
{
    dev->i2c_dev = params->i2c_dev;
    dev->address = params->address;

    /* After leaving "Off Mode" the SI1133 enters an "Initialization Mode" for
     * a period of time in which it can't be reached over I2C. After this time
     * the device will be in Standby Mode. */
    ztimer_sleep(ZTIMER_USEC, SI1133_STARTUP_TIME_MS * US_PER_MS);

    i2c_acquire(params->i2c_dev);

    /* check sensor ID */
    uint8_t partid = 0;
    int ret = i2c_read_reg(params->i2c_dev, params->address, SI1133_REG_PART_ID,
                           &partid, 0);
    if (ret != 0) {
        DEBUG("[si1133] i2c communication error: %s.\n", strerror(-ret));
        i2c_release(params->i2c_dev);
        return SI1133_ERR_I2C;
    }
    if (partid != SI1133_ID) {
        DEBUG("[si1133] Invalid part id: 0x%.2u\n", (unsigned)partid);
        i2c_release(params->i2c_dev);
        return SI1133_ERR_NODEV;
    }

#if ENABLE_DEBUG
    uint8_t rev_id, hw_id;
    i2c_read_reg(params->i2c_dev, params->address, SI1133_REG_REV_ID, &rev_id,
                 0 /* flags */);
    i2c_read_reg(params->i2c_dev, params->address, SI1133_REG_HW_ID, &hw_id,
                 0 /* flags */);
    DEBUG("[si1133] impl code: %u, silicon HW rev: %u, rev: %u.%u\n",
          hw_id & 0x1f, hw_id >> 5, rev_id >> 4, rev_id & 0x0f);
#endif  /* ENABLE_DEBUG */

    /* We don't know the state in which the device is at this point so we need
     * to perform a reset, unfortunately this requires another start-up wait. */
    ret = _si1133_reset(dev);
    if (ret) {
        i2c_release(dev->i2c_dev);
        return ret;
    }

    i2c_release(dev->i2c_dev);
    return SI1133_OK;
}

si1133_ret_code_t si1133_configure_channels(si1133_t *dev,
                                            const si1133_channel_t *channels,
                                            uint32_t num_channels)
{
    DEBUG("[si1133] configure_channels(num=%" PRIu32 ")\n", num_channels);
    if (num_channels > SI1133_NUM_CHANNELS) {
        return SI1133_ERR_PARAMS;
    }

    i2c_acquire(dev->i2c_dev);
    si1133_ret_code_t ret;
    for (uint32_t i = 0; i < num_channels; i++) {
        ret = _si1133_configure_channel(dev, i, channels + i);
        if (ret) {
            dev->num_channels = 0;
            i2c_release(dev->i2c_dev);
            return ret;
        }
    }
    /* CHAN_LIST is a bit mask of channels used. */
    ret = _si1133_set_param(dev, SI1133_PARAM_CHAN_LIST,
                            (1u << num_channels) - 1);
    if (ret) {
        num_channels = 0;
    }
    dev->num_channels = num_channels;
    i2c_release(dev->i2c_dev);
    DEBUG("[si1133] Sample Time %" PRIu32 " us\n", _si1133_force_time_us(dev));
    return ret;
}

si1133_ret_code_t si1133_easy_configure(si1133_t *dev,
                                        si1133_sensor_t sensor_mask,
                                        uint8_t sample_time_log,
                                        uint8_t sw_gain)
{
    DEBUG("[si1133] easy_configure(0x%.2x)\n", (unsigned)sensor_mask);
    i2c_acquire(dev->i2c_dev);

    si1133_ret_code_t ret;
    uint8_t num_channels = 0;
    for (uint8_t mask = sensor_mask; mask;
         num_channels++, mask = mask & (mask - 1)) {
        if (num_channels >= SI1133_NUM_CHANNELS) {
            dev->num_channels = 0;
            i2c_release(dev->i2c_dev);
            return SI1133_ERR_PARAMS;
        }

        si1133_channel_t channel;
        channel.sensor = mask ^ (mask & (mask - 1));
        channel.sample_time_log = sample_time_log;
        channel.sw_gain = sw_gain;
        ret = _si1133_configure_channel(dev, num_channels, &channel);
        if (ret) {
            dev->num_channels = 0;
            i2c_release(dev->i2c_dev);
            return ret;
        }
    }

    /* CHAN_LIST is a bit mask of channels used. */
    ret = _si1133_set_param(dev, SI1133_PARAM_CHAN_LIST,
                            (1u << num_channels) - 1);
    if (ret) {
        num_channels = 0;
    }
    dev->num_channels = num_channels;
    i2c_release(dev->i2c_dev);
    DEBUG("[si1133] Sample Time %" PRIu32 " us\n", _si1133_force_time_us(dev));
    return ret;
}

si1133_ret_code_t si1133_capture_sensors(si1133_t *dev, int32_t *values,
                                         uint32_t num_channels)
{
    if (!dev->num_channels) {
        /* Must be configured before calling capture_sensors. */
        return SI1133_ERR_PARAMS;
    }

    i2c_acquire(dev->i2c_dev);

    si1133_ret_code_t force_ret;
    force_ret = _si1133_run_command(dev, SI1133_CMD_FORCE);
    if (force_ret != SI1133_OK && force_ret != SI1133_ERR_OVERFLOW) {
        i2c_release(dev->i2c_dev);
        DEBUG("[si1133] force read command error: %d\n", force_ret);
        return force_ret;
    }
    si1133_ret_code_t ret = SI1133_OK;
    if (force_ret == SI1133_ERR_OVERFLOW) {
        /* We need to reset the overflow condition with a RESET_CMD_CTR */
        ret = _si1133_run_command(dev, SI1133_CMD_RESET_CMD_CTR);
    }
    if (ret == SI1133_OK) {
        ret = _si1133_read_values(dev, values, num_channels);
    }
    i2c_release(dev->i2c_dev);
    /* If there was an error reading the I2C values then return that error,
     * otherwise we want to return the CMD_FORCE return value because there
     * could be an overflow non-fatal error to report. */
    return ret ? ret : force_ret;
}
