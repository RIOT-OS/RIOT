/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_esp32
 * @ingroup drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation for ESP32 SDK
 *
 * @note        The hardware implementation seems to be very poor and faulty.
 *              I2C commands in the I2C command pipeline are not executed
 *              sporadically. A number of ACK errors and timeouts caused by
 *              protocol errors are the result. You should use the hardware
 *              implementation only if they can be tolerated.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#if defined(MODULE_ESP_I2C_HW) /* hardware implementation used */

/**
 * PLEASE NOTE:
 *
 * Some parts of the implementation were inspired by the Espressif IoT
 * Development Framework [ESP-IDF](https://github.com/espressif/esp-idf.git)
 * implementation of I2C. These partes are marked with an according copyright
 * notice.
*/

#define ENABLE_DEBUG 0
#include "debug.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "cpu.h"
#include "log.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "thread_flags.h"

#include "esp_common.h"
#include "gpio_arch.h"
#include "driver/periph_ctrl.h"
#include "irq_arch.h"
#include "rom/ets_sys.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"
#include "soc/rtc.h"
#include "soc/soc.h"
#include "syscalls.h"
#include "xtensa/xtensa_api.h"

#if defined(I2C0_SPEED) || defined(I2C1_SPEED)

#undef  I2C_CLK_FREQ
#define I2C_CLK_FREQ   rtc_clk_apb_freq_get() /* APB_CLK is used */

/* operation codes used for commands */
#define I2C_CMD_RSTART 0
#define I2C_CMD_WRITE  1
#define I2C_CMD_READ   2
#define I2C_CMD_STOP   3
#define I2C_CMD_END    4

/* maximum number of data that can be written / read in one transfer */
#define I2C_MAX_DATA   30

#define I2C_FIFO_USED  1

struct i2c_hw_t {
    i2c_dev_t* regs;        /* pointer to register data struct of the I2C device */
    uint8_t mod;            /* peripheral hardware module of the I2C interface */
    uint8_t int_src;        /* peripheral interrupt source used by the I2C device */
    uint8_t signal_scl_in;  /* SCL signal to the controller */
    uint8_t signal_scl_out; /* SCL signal from the controller */
    uint8_t signal_sda_in;  /* SDA signal to the controller */
    uint8_t signal_sda_out; /* SDA signal from the controller */
};

static const struct i2c_hw_t _i2c_hw[] = {
    {
        .regs = &I2C0,
        .mod = PERIPH_I2C0_MODULE,
        .int_src = ETS_I2C_EXT0_INTR_SOURCE,
        .signal_scl_in = I2CEXT0_SCL_IN_IDX,
        .signal_scl_out = I2CEXT0_SCL_OUT_IDX,
        .signal_sda_in = I2CEXT0_SDA_IN_IDX,
        .signal_sda_out = I2CEXT0_SDA_OUT_IDX,
    },
    {
        .regs = &I2C1,
        .mod = PERIPH_I2C1_MODULE,
        .int_src = ETS_I2C_EXT1_INTR_SOURCE,
        .signal_scl_in = I2CEXT1_SCL_IN_IDX,
        .signal_scl_out = I2CEXT1_SCL_OUT_IDX,
        .signal_sda_in = I2CEXT1_SDA_IN_IDX,
        .signal_sda_out = I2CEXT1_SDA_OUT_IDX,
    }
};

struct _i2c_bus_t
{
    i2c_speed_t speed; /* bus speed */
    uint8_t cmd;       /* command index */
    uint8_t data;      /* index in RAM for data */
    mutex_t lock;      /* mutex lock */
    kernel_pid_t pid;  /* PID of thread that triggered a transfer */
    uint32_t results;  /* results of a transfer */
};

static struct _i2c_bus_t _i2c_bus[I2C_NUMOF] = {};

/* forward declaration of internal functions */

static int  _i2c_init_pins (i2c_t dev);
static void _i2c_start_cmd (i2c_t dev);
static void _i2c_stop_cmd  (i2c_t dev);
static void _i2c_end_cmd   (i2c_t dev);
static void _i2c_write_cmd (i2c_t dev, const uint8_t* data, uint8_t len);
static void _i2c_read_cmd  (i2c_t dev, uint8_t* data, uint8_t len, bool last);
static void _i2c_transfer  (i2c_t dev);
static void _i2c_reset_hw  (i2c_t dev);
static void _i2c_clear_bus (i2c_t dev);
static void _i2c_intr_handler (void *arg);
static inline void _i2c_delay (uint32_t delay);

/* implementation of i2c interface */

void i2c_init(i2c_t dev)
{
    CHECK_PARAM (dev < I2C_NUMOF)

    if (i2c_config[dev].speed == I2C_SPEED_FAST_PLUS ||
        i2c_config[dev].speed == I2C_SPEED_HIGH) {
        LOG_TAG_INFO("i2c", "I2C_SPEED_FAST_PLUS and I2C_SPEED_HIGH "
                     "are not supported\n");
        return;
    }

    mutex_init(&_i2c_bus[dev].lock);

    i2c_acquire (dev);

    _i2c_bus[dev].cmd = 0;
    _i2c_bus[dev].data = 0;
    _i2c_bus[dev].speed = i2c_config[dev].speed;

    DEBUG ("%s scl=%d sda=%d speed=%d\n", __func__,
           i2c_config[dev].scl, i2c_config[dev].sda, _i2c_bus[dev].speed);

    /* enable (power on) the according I2C module */
    periph_module_enable(_i2c_hw[dev].mod);

    /* initialize pins */
    if (_i2c_init_pins(dev) != 0) {
        return;
    }

    /* set master mode */
    _i2c_hw[dev].regs->ctr.ms_mode = 1;

    /* set bit order to MSB first */
    _i2c_hw[dev].regs->ctr.tx_lsb_first = 0;
    _i2c_hw[dev].regs->ctr.rx_lsb_first = 0;

    /* determine the half period of clock in APB clock cycles */
    uint32_t half_period = 0;

    switch (_i2c_bus[dev].speed) {
        case I2C_SPEED_LOW:
            /* 10 kbps (period 100 us) */
            half_period = (I2C_CLK_FREQ / 10000) >> 1;
            break;

        case I2C_SPEED_NORMAL:
            /* 100 kbps (period 10 us) */
            half_period = (I2C_CLK_FREQ / 100000) >> 1;
            half_period = half_period * 95 / 100; /* correction factor */
            break;

        case I2C_SPEED_FAST:
            /* 400 kbps (period 2.5 us) */
            half_period = (I2C_CLK_FREQ / 400000) >> 1;
            half_period = half_period * 82 / 100; /* correction factor */
            break;

        case I2C_SPEED_FAST_PLUS:
            /* 1 Mbps (period 1 us) not working */
            half_period = (I2C_CLK_FREQ / 1000000) >> 1;
            break;

        case I2C_SPEED_HIGH:
            /* 3.4 Mbps (period 0.3 us) not working */
            half_period = (I2C_CLK_FREQ / 3400000) >> 1;
            break;

        default:
            LOG_TAG_ERROR("i2c", "Invalid speed value in %s\n", __func__);
            return;
    }

    /* set an timeout which is at least 16 times of half cycle */
    _i2c_hw[dev].regs->timeout.tout = half_period << 4;

    /* timing for SCL (low and high time in APB clock cycles) */
    _i2c_hw[dev].regs->scl_low_period.period = half_period;
    _i2c_hw[dev].regs->scl_high_period.period = half_period;

    /* timing for SDA (sample time after rising edge and hold time after falling edge) */
    _i2c_hw[dev].regs->sda_sample.time = half_period >> 1;
    _i2c_hw[dev].regs->sda_hold.time = half_period >> 1;

    /* timing for START condition (START hold and repeated START setup time) */
    _i2c_hw[dev].regs->scl_start_hold.time = half_period >> 1;
    _i2c_hw[dev].regs->scl_rstart_setup.time = half_period >> 1;

    /* timing for STOP condition (STOP hold and STOP setup time) */
    _i2c_hw[dev].regs->scl_stop_hold.time = half_period >> 1;
    _i2c_hw[dev].regs->scl_stop_setup.time = half_period >> 1;

    /* configure open drain outputs */
    _i2c_hw[dev].regs->ctr.scl_force_out = 1;
    _i2c_hw[dev].regs->ctr.sda_force_out = 1;

    /* sample data during high level */
    _i2c_hw[dev].regs->ctr.sample_scl_level = 0;

    /* enable non FIFO access and disable slave FIFO address offset */
    #if I2C_FIFO_USED
    _i2c_hw[dev].regs->fifo_conf.nonfifo_en = 0;
    #else
    _i2c_hw[dev].regs->fifo_conf.nonfifo_en = 1;
    _i2c_hw[dev].regs->fifo_conf.nonfifo_rx_thres = 0;
    _i2c_hw[dev].regs->fifo_conf.nonfifo_tx_thres = 0;
    _i2c_hw[dev].regs->fifo_conf.rx_fifo_full_thrhd = 0;
    _i2c_hw[dev].regs->fifo_conf.tx_fifo_empty_thrhd = 0;

    #endif
    _i2c_hw[dev].regs->fifo_conf.fifo_addr_cfg_en = 0;

    /* route all I2C interrupt sources to same the CPU interrupt */
    intr_matrix_set(PRO_CPU_NUM, _i2c_hw[dev].int_src, CPU_INUM_I2C);

    /* set the interrupt handler and enable the interrupt */
    xt_set_interrupt_handler(CPU_INUM_I2C, _i2c_intr_handler, NULL);
    xt_ints_on(BIT(CPU_INUM_I2C));

    i2c_release (dev);

    return;
}

int i2c_acquire(i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)

    mutex_lock(&_i2c_bus[dev].lock);
    _i2c_reset_hw(dev);
    return 0;
}

void i2c_release(i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    assert(dev < I2C_NUMOF);

    _i2c_reset_hw (dev);
    mutex_unlock(&_i2c_bus[dev].lock);
}

/*
 * This macro checks the result of a read transfer. In case of an error,
 * the hardware is reset and returned with a corresponding error code.
 *
 * @note:
 * In a read transfer, an ACK is only expected for the address field. Thus,
 * an ACK error can only happen for the address field. Therefore, we always
 * return -ENXIO in case of an ACK error.
 */
#define _i2c_return_on_error_read(dev) \
    if (_i2c_bus[dev].results & I2C_ARBITRATION_LOST_INT_ENA) { \
        LOG_TAG_DEBUG("i2c", "arbitration lost dev=%u\n", dev); \
        _i2c_reset_hw (dev); \
        __asm__ volatile ("isync"); \
        return -EAGAIN; \
    } \
    else if (_i2c_bus[dev].results & I2C_ACK_ERR_INT_ENA) { \
        LOG_TAG_DEBUG("i2c", "ack error dev=%u\n", dev); \
        _i2c_reset_hw (dev); \
        __asm__ volatile ("isync"); \
        return -ENXIO; \
    } \
    else if (_i2c_bus[dev].results & I2C_TIME_OUT_INT_ENA) { \
        LOG_TAG_DEBUG("i2c", "bus timeout dev=%u\n", dev); \
        _i2c_reset_hw (dev); \
        __asm__ volatile ("isync"); \
        return -ETIMEDOUT; \
    }

/*
 * This macro checks the result of a write transfer. In case of an error,
 * the hardware is reset and returned with a corresponding error code.
 *
 * @note:
 * In a write transfer, an ACK error can happen for the address field
 * as well as for data. If the FIFO still contains all data bytes,
 * (i.e. _i2c_hw[dev].regs->status_reg.tx_fifo_cnt >= len), the ACK error
 * happened in address field and we have to returen -ENXIO. Otherwise, the
 * ACK error happened in data field and we have to return -EIO.
 */
#define _i2c_return_on_error_write(dev) \
    if (_i2c_bus[dev].results & I2C_ARBITRATION_LOST_INT_ENA) { \
        LOG_TAG_DEBUG("i2c", "arbitration lost dev=%u\n", dev); \
        _i2c_reset_hw (dev); \
        __asm__ volatile ("isync"); \
        return -EAGAIN; \
    } \
    else if (_i2c_bus[dev].results & I2C_ACK_ERR_INT_ENA) { \
        LOG_TAG_DEBUG("i2c", "ack error dev=%u\n", dev); \
        _i2c_reset_hw (dev); \
        __asm__ volatile ("isync"); \
        if (_i2c_hw[dev].regs->status_reg.tx_fifo_cnt >= len) { \
            return -ENXIO; \
        } \
        else { \
            return -EIO; \
        } \
    } \
    else if (_i2c_bus[dev].results & I2C_TIME_OUT_INT_ENA) { \
        LOG_TAG_DEBUG("i2c", "bus timeout dev=%u\n", dev); \
        _i2c_reset_hw (dev); \
        __asm__ volatile ("isync"); \
        return -ETIMEDOUT; \
    }

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t len, uint8_t flags)
{
    DEBUG ("%s dev=%u addr=%02x data=%p len=%d flags=%01x\n",
           __func__, dev, addr, data, len, flags);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -EINVAL);
    CHECK_PARAM_RET (len > 0, -EINVAL);
    CHECK_PARAM_RET (data != NULL, -EINVAL);

    /*  if I2C_NOSTART is not set, START condition and ADDR is used */
    if (!(flags & I2C_NOSTART)) {

        /* send START condition */
        _i2c_start_cmd (dev);

        /* address handling */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr10[2];
            addr10[0] = 0xf0 | (addr & 0x0300) >> 7 | I2C_READ;
            addr10[1] = addr & 0xff;
            /* send ADDR with read flag */
            _i2c_write_cmd (dev, addr10, 2);
        }
        else {
            /* send ADDR with read flag */
            uint8_t addr7 = (addr << 1 | I2C_READ);
            _i2c_write_cmd (dev, &addr7, 1);
        }
    }

    /* read data bytes in blocks of I2C_MAX_DATA bytes */

    uint32_t off = 0;

    /* if len > I2C_MAX_DATA read blocks I2C_MAX_DATA bytes at a time */
    while (len > I2C_MAX_DATA) {

        /* read one block of data bytes command */
        _i2c_read_cmd (dev, data, I2C_MAX_DATA, false);
        _i2c_end_cmd (dev);
        _i2c_transfer (dev);
        _i2c_return_on_error_read (dev);

        /* if transfer was successful, fetch the data from I2C RAM */
        for (unsigned i = 0; i < I2C_MAX_DATA; i++) {
            #if I2C_FIFO_USED
            ((uint8_t*)data)[i + off] = _i2c_hw[dev].regs->fifo_data.data;
            #else
            ((uint8_t*)data)[i + off] = _i2c_hw[dev].regs->ram_data[i];
            #endif
        }

        len -= I2C_MAX_DATA;
        off += I2C_MAX_DATA;
    }

    /* read remaining data bytes command with a final NAK */
    _i2c_read_cmd (dev, data, len, true);

    /* if I2C_NOSTOP flag is not set, send STOP condition is used */
    if (!(flags & I2C_NOSTOP)) {
        /* send STOP condition */
        _i2c_stop_cmd (dev);
    }
    else {
        /* otherwise place end command in pipeline */
        _i2c_end_cmd (dev);
    }

    /* finish operation by executing the command pipeline */
    _i2c_transfer (dev);
    _i2c_return_on_error_read (dev);

    /* if transfer was successful, fetch data from I2C RAM */
    for (unsigned i = 0; i < len; i++) {
        #if I2C_FIFO_USED
        ((uint8_t*)data)[i + off] = _i2c_hw[dev].regs->fifo_data.data;
        #else
        ((uint8_t*)data)[i + off] = _i2c_hw[dev].regs->ram_data[i];
        #endif
    }

    /* return 0 on success */
    return 0;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len, uint8_t flags)
{
    DEBUG ("%s dev=%u addr=%02x data=%p len=%d flags=%01x\n",
           __func__, dev, addr, data, len, flags);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -EINVAL);
    CHECK_PARAM_RET (len > 0, -EINVAL);
    CHECK_PARAM_RET (data != NULL, -EINVAL);

    /*  if I2C_NOSTART is not set, START condition and ADDR is used */
    if (!(flags & I2C_NOSTART)) {

        /* send START condition */
        _i2c_start_cmd (dev);

        /* address handling */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr10[2];
            addr10[0] = 0xf0 | (addr & 0x0300) >> 7;
            addr10[1] = addr & 0xff;
            /* send ADDR without read flag */
            _i2c_write_cmd (dev, addr10, 2);
        }
        else {
            /* send ADDR without read flag */
            uint8_t addr7 = addr << 1;
            _i2c_write_cmd (dev, &addr7, 1);
        }
    }

    /* send data bytes in blocks of I2C_MAX_DATA bytes */

    uint32_t off = 0;

    /* if len > I2C_MAX_DATA write blocks I2C_MAX_DATA bytes at a time */
    while (len > I2C_MAX_DATA) {

        /* send on block of data bytes */
        _i2c_write_cmd (dev, ((uint8_t*)data) + off, I2C_MAX_DATA);
        _i2c_end_cmd (dev);
        _i2c_transfer (dev);
        _i2c_return_on_error_write (dev);

        len -= I2C_MAX_DATA;
        off += I2C_MAX_DATA;
    }

    /* write remaining data bytes command */
    _i2c_write_cmd (dev, ((uint8_t*)data), len);

    /* if I2C_NOSTOP flag is not set, send STOP condition is used */
    if (!(flags & I2C_NOSTOP)) {
        /* send STOP condition */
        _i2c_stop_cmd (dev);
    }
    else {
        /* otherwise place end command in pipeline */
        _i2c_end_cmd (dev);
    }

    /* finish operation by executing the command pipeline */
    _i2c_transfer (dev);
    _i2c_return_on_error_write (dev);

    /* return 0 on success */
    return 0;
}

/* internal functions */

static int _i2c_init_pins(i2c_t dev)
{
    /*
     * reset GPIO usage type if the pins were used already for I2C before to
     * make it possible to reinitialize I2C
     */
    if (gpio_get_pin_usage(i2c_config[dev].scl) == _I2C) {
        gpio_set_pin_usage(i2c_config[dev].scl, _GPIO);
    }
    if (gpio_get_pin_usage(i2c_config[dev].sda) == _I2C) {
        gpio_set_pin_usage(i2c_config[dev].sda, _GPIO);
    }

    /* try to configure SDA and SCL pin as GPIO in open-drain mode with enabled pull-ups */
    if (gpio_init (i2c_config[dev].scl, GPIO_IN_OD_PU) ||
        gpio_init (i2c_config[dev].sda, GPIO_IN_OD_PU)) {
        return -ENODEV;
    }

    /* bring signals to high */
    gpio_set(i2c_config[dev].scl);
    gpio_set(i2c_config[dev].sda);

    /* store the usage type in GPIO table */
    gpio_set_pin_usage(i2c_config[dev].scl, _I2C);
    gpio_set_pin_usage(i2c_config[dev].sda, _I2C);

    /* connect SCL and SDA pins to output signals through the GPIO matrix */
    GPIO.func_out_sel_cfg[i2c_config[dev].scl].func_sel = _i2c_hw[dev].signal_scl_out;
    GPIO.func_out_sel_cfg[i2c_config[dev].sda].func_sel = _i2c_hw[dev].signal_sda_out;

    /* connect SCL and SDA input signals to pins through the GPIO matrix */
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].func_sel = i2c_config[dev].scl;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].func_sel = i2c_config[dev].sda;

    return 0;
}

static void _i2c_start_cmd(i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    /* place START condition command in command queue */
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_RSTART;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

static void _i2c_stop_cmd (i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    /* place STOP condition command in command queue */
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_STOP;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

static void _i2c_end_cmd (i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    /* place END command for continues data transmission in command queue */
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_END;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

static void _i2c_write_cmd (i2c_t dev, const uint8_t* data, uint8_t len)
{
    DEBUG ("%s dev=%u data=%p len=%d\n", __func__, dev, data, len);

    if (_i2c_bus[dev].data + len > I2C_MAX_DATA) {
        LOG_TAG_ERROR("i2c", "Maximum number of bytes (32 bytes) that can be "
                      "sent with on transfer reached\n");
        return;
    }

    /* store the byte in RAM of I2C controller and increment the data counter */
    for (int i = 0; i < len; i++) {
        #if I2C_FIFO_USED
        WRITE_PERI_REG(I2C_DATA_APB_REG(dev), data[i]);
        #else
        _i2c_hw[dev].regs->ram_data[_i2c_bus[dev].data++] = (uint32_t)data[i];
        #endif
    }

    /* place WRITE command for multiple bytes in command queue */
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].byte_num = len;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_en = 1;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_exp = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_WRITE;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

static void _i2c_read_cmd (i2c_t dev, uint8_t* data, uint8_t len, bool last)
{
    DEBUG ("%s dev=%u data=%p len=%d\n", __func__, dev, data, len);

    if (len < 1 || len > I2C_MAX_DATA) {
        /* at least one byte has to be read */
        LOG_TAG_ERROR("i2c", "At least one byte has to be read\n");
        return;
    }

    if (len > 1)
    {
        /* place READ command for len-1 bytes with positive ack in command queue*/
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].byte_num = len-1;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_en = 0;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_exp = 0;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_val = 0;
        _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_READ;

        /* increment the command counter */
        _i2c_bus[dev].cmd++;
    }

    /* place READ command for last byte with negative ack in last segment in command queue*/
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].val = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].byte_num = 1;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_en = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_exp = 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].ack_val = last ? 1 : 0;
    _i2c_hw[dev].regs->command[_i2c_bus[dev].cmd].op_code = I2C_CMD_READ;

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

static inline void _i2c_delay (uint32_t cycles)
{
    /* produces a delay of 0,0625 us per cycle for -O2 compile option */
    /* 1 us = ca. 16 cycles (80 MHz) / 1 us = 32 cycles (160 MHz) */

    if (cycles) {
        __asm__ volatile ("1: _addi.n  %0, %0, -1 \n"
                          "   bnez     %0, 1b     \n" : "=r" (cycles) : "0" (cycles));
    }
}

/* transfer related interrupts handled by the driver */
static const uint32_t transfer_int_mask = I2C_TRANS_COMPLETE_INT_ENA
                                        | I2C_END_DETECT_INT_ENA
                                        | I2C_ACK_ERR_INT_ENA
                                        | I2C_ARBITRATION_LOST_INT_ENA
                                        | I2C_TIME_OUT_INT_ENA;

/* at I2C_SPEED_NORMAL a transfer takes at most 33 byte * 9 clock cycles * 1/100000 s */
#define I2C_TRANSFER_TIMEOUT    3000

#define I2C_THREAD_FLAG BIT     (0)

#include "xtimer.h"

void _i2c_transfer_timeout (void *arg)
{
    i2c_t dev = (i2c_t)arg;

    /* reset the hardware if it I2C got stuck */
    _i2c_reset_hw(dev);

    /* set result to timeout */
    _i2c_bus[dev].results |= I2C_TIME_OUT_INT_ST;

    /* wake up the thread that is waiting for the results */
    thread_flags_set((thread_t*)thread_get(_i2c_bus[dev].pid), I2C_THREAD_FLAG);
}

/* Transfer of commands in I2C controller command pipeline */
static void _i2c_transfer (i2c_t dev)
{
    DEBUG("%s cmd=%d\n", __func__, _i2c_bus[dev].cmd);

    #if FIFO_USED
    /* reset RX FIFO queue */
    _i2c_hw[dev].regs->fifo_conf.rx_fifo_rst = 1;
    _i2c_hw[dev].regs->fifo_conf.rx_fifo_rst = 0;
    #endif

    /* disable and enable all transmission interrupts and clear current status */
    _i2c_hw[dev].regs->int_ena.val &= ~transfer_int_mask;
    _i2c_hw[dev].regs->int_ena.val |= transfer_int_mask;
    _i2c_hw[dev].regs->int_clr.val  = transfer_int_mask;

    /* set a timer for the case the I2C hardware gets stuck */
    xtimer_t i2c_timeout = {};
    i2c_timeout.callback = _i2c_transfer_timeout;
    i2c_timeout.arg = (void*)dev;
    xtimer_set(&i2c_timeout, I2C_TRANSFER_TIMEOUT);

    /* start execution of commands in command pipeline registers */
    _i2c_bus[dev].pid = thread_getpid();
    _i2c_bus[dev].results = 0;
    _i2c_hw[dev].regs->ctr.trans_start = 0;
    _i2c_hw[dev].regs->ctr.trans_start = 1;

    /* wait for transfer results and remove timeout timer*/
    thread_flags_wait_one(I2C_THREAD_FLAG);
    xtimer_remove(&i2c_timeout);

    /* returned from transmission */
    DEBUG("%s results=%08x\n", __func__, _i2c_bus[dev].results);

    #if FIFO_USED
    /* reset TX FIFO queue */
    _i2c_hw[dev].regs->fifo_conf.tx_fifo_rst = 1;
    _i2c_hw[dev].regs->fifo_conf.tx_fifo_rst = 0;
    #endif

    /* reset command and data index */
    _i2c_bus[dev].cmd = 0;
    _i2c_bus[dev].data = 0;
}

static void IRAM_ATTR _i2c_intr_handler (void *arg)
{
    /* to satisfy the compiler */
    (void)arg;

    irq_isr_enter ();

    /* all I2C peripheral interrupt sources are routed to the same interrupt,
       so we have to use the status register to distinguish interruptees */
    for (unsigned dev = 0; dev < I2C_NUMOF; dev++) {
        /* test for transfer related interrupts */
        if (_i2c_hw[dev].regs->int_status.val & transfer_int_mask) {
            /* set transfer result */
            _i2c_bus[dev].results |= _i2c_hw[dev].regs->int_status.val;
            /* disable all interrupts and clear them and left them disabled */
            _i2c_hw[dev].regs->int_ena.val &= ~transfer_int_mask;
            _i2c_hw[dev].regs->int_clr.val  = transfer_int_mask;
            /* wake up the thread that is waiting for the results */
            thread_flags_set((thread_t*)thread_get(_i2c_bus[dev].pid), I2C_THREAD_FLAG);
        }
        else if (_i2c_hw[dev].regs->int_status.val) {
            /* if there are any other interrupts, clear them */
            _i2c_hw[dev].regs->int_clr.val = ~0x0U;
        }
    }

    irq_isr_exit ();
}

#if 1 /* TODO */
/* Some slave devices will die by accident and keep the SDA in low level,
 * in this case, master should send several clock to make the slave release
 * the bus.
 */
static void _i2c_clear_bus(i2c_t dev)
{
    /* reset the usage type in GPIO table */
    gpio_set_pin_usage(i2c_config[dev].scl, _GPIO);
    gpio_set_pin_usage(i2c_config[dev].sda, _GPIO);

    /* configure SDA and SCL pin as GPIO in open-drain mode temporarily */
    gpio_init (i2c_config[dev].scl, GPIO_IN_OD_PU);
    gpio_init (i2c_config[dev].sda, GPIO_IN_OD_PU);

    /* master send some clock pulses to make the slave release the bus */
    gpio_set (i2c_config[dev].scl);
    gpio_set (i2c_config[dev].sda);
    gpio_clear (i2c_config[dev].sda);
    for (int i = 0; i < 20; i++) {
        gpio_toggle(i2c_config[dev].scl);
    }
    gpio_set(i2c_config[dev].sda);

    /* store the usage type in GPIO table */
    gpio_set_pin_usage(i2c_config[dev].scl, _I2C);
    gpio_set_pin_usage(i2c_config[dev].sda, _I2C);

    /* connect SCL and SDA pins to output signals through the GPIO matrix */
    GPIO.func_out_sel_cfg[i2c_config[dev].scl].func_sel = _i2c_hw[dev].signal_scl_out;
    GPIO.func_out_sel_cfg[i2c_config[dev].sda].func_sel = _i2c_hw[dev].signal_sda_out;

    /* connect SCL and SDA input signals to pins through the GPIO matrix */
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_scl_in].func_sel = i2c_config[dev].scl;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[_i2c_hw[dev].signal_sda_in].func_sel = i2c_config[dev].sda;

    return;
}
#endif

/*
 * PLEASE NOTE: Following function is from the ESP-IDF and is licensed
 * under the Apache License, Version 2.0 (the "License").
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
 */
static void _i2c_reset_hw (i2c_t dev)
{
    /* save current configuration */
    uint32_t ctr              = _i2c_hw[dev].regs->ctr.val;
    uint32_t fifo_conf        = _i2c_hw[dev].regs->fifo_conf.val;
    uint32_t scl_low_period   = _i2c_hw[dev].regs->scl_low_period.val;
    uint32_t scl_high_period  = _i2c_hw[dev].regs->scl_high_period.val;
    uint32_t scl_start_hold   = _i2c_hw[dev].regs->scl_start_hold.val;
    uint32_t scl_rstart_setup = _i2c_hw[dev].regs->scl_rstart_setup.val;
    uint32_t scl_stop_hold    = _i2c_hw[dev].regs->scl_stop_hold.val;
    uint32_t scl_stop_setup   = _i2c_hw[dev].regs->scl_stop_setup.val;
    uint32_t sda_hold         = _i2c_hw[dev].regs->sda_hold.val;
    uint32_t sda_sample       = _i2c_hw[dev].regs->sda_sample.val;
    uint32_t timeout          = _i2c_hw[dev].regs->timeout.val;
    uint32_t scl_filter_cfg   = _i2c_hw[dev].regs->scl_filter_cfg.val;
    uint32_t sda_filter_cfg   = _i2c_hw[dev].regs->sda_filter_cfg.val;

    /* reset hardware mpdule */
    periph_module_disable(_i2c_hw[dev].mod);
    _i2c_clear_bus(dev);
    periph_module_enable(_i2c_hw[dev].mod);

    /* restore configuration */
    _i2c_hw[dev].regs->int_ena.val          = 0;
    _i2c_hw[dev].regs->ctr.val              = ctr & (~I2C_TRANS_START_M);
    _i2c_hw[dev].regs->fifo_conf.val        = fifo_conf;
    _i2c_hw[dev].regs->scl_low_period.val   = scl_low_period;
    _i2c_hw[dev].regs->scl_high_period.val  = scl_high_period;
    _i2c_hw[dev].regs->scl_start_hold.val   = scl_start_hold;
    _i2c_hw[dev].regs->scl_rstart_setup.val = scl_rstart_setup;
    _i2c_hw[dev].regs->scl_stop_hold.val    = scl_stop_hold;
    _i2c_hw[dev].regs->scl_stop_setup.val   = scl_stop_setup;
    _i2c_hw[dev].regs->sda_hold.val         = sda_hold;
    _i2c_hw[dev].regs->sda_sample.val       = sda_sample;
    _i2c_hw[dev].regs->timeout.val          = timeout;
    _i2c_hw[dev].regs->scl_filter_cfg.val   = scl_filter_cfg;
    _i2c_hw[dev].regs->sda_filter_cfg.val   = sda_filter_cfg;

    /* disable and clear all interrupt sources */
    _i2c_hw[dev].regs->int_ena.val = 0;
    _i2c_hw[dev].regs->int_clr.val = ~0x0U;

    return;
}

void i2c_print_config(void)
{
    for (unsigned dev = 0; dev < I2C_NUMOF; dev++) {
        printf("\tI2C_DEV(%u)\tscl=%d sda=%d\n",
               dev, i2c_config[dev].scl, i2c_config[dev].sda);
    }
}

#else /* defined(I2C0_SPEED) || defined(I2C1_SPEED) */

void i2c_print_config(void)
{
    LOG_TAG_INFO("i2c", "no I2C devices\n");
}

#endif /* defined(I2C0_SPEED) || defined(I2C1_SPEED) */

#endif /* MODULE_ESP_I2C_HW */
