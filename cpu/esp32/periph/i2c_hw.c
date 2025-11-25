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

/*
 * The I2C peripheral driver uses a mixture of the high level functions and
 * the HAL driver. While complex functions like the configuration of the I2C
 * interface use the high level interface, the HAL interface is used by data
 * transfer functions for better performance and better control.
 */
#include <errno.h>

#include "gpio_arch.h"
#include "irq_arch.h"
#include "log.h"
#include "macros/units.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"
#include "ztimer.h"

#include "esp_attr.h"
#include "driver/i2c.h"
#include "hal/i2c_hal.h"
#include "hal/interrupt_controller_types.h"
#include "hal/interrupt_controller_ll.h"
#include "rom/ets_sys.h"
#include "soc/i2c_reg.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Ensure that the I2Cn_* symbols define I2C_DEV(n) */
#if !defined(I2C0_SPEED) && defined(I2C1_SPEED)
#error "I2C_DEV(1) is used but I2C_DEV(0) is not defined"
#endif

typedef struct {
    i2c_speed_t clk_freq;   /* clock freuency in Hz according to bus speed */
    uint8_t cmd;            /* command index of a transfer*/
    uint8_t cmd_op;         /* main command op code of a transfer */
    uint8_t len;            /* data length of a transfer */
    uint32_t status;        /* results of a transfer */
    mutex_t cmd_lock;       /* command execution locking */
    mutex_t dev_lock;       /* device locking */
} _i2c_bus_t;

static i2c_hal_context_t _i2c_hw[] = {
#if defined(I2C0_SPEED)
    { .dev = I2C_LL_GET_HW(0) },
#endif
#if defined(I2C1_SPEED)
    { .dev = I2C_LL_GET_HW(1) },
#endif
};

_Static_assert(I2C_NUMOF == ARRAY_SIZE(_i2c_hw),
               "Size of bus descriptor table doesn't match I2C_NUMOF");
_Static_assert(I2C_NUMOF <= I2C_NUMOF_MAX,
               "Number of defined I2C devices is greater than the number of supported devices");

static _i2c_bus_t _i2c_bus[I2C_NUMOF] = {
#if defined(I2C0_SPEED)
    {
        .cmd_lock = MUTEX_INIT_LOCKED,
        .dev_lock = MUTEX_INIT,
    },
#endif
#if defined(I2C1_SPEED)
    {
        .cmd_lock = MUTEX_INIT_LOCKED,
        .dev_lock = MUTEX_INIT,
    },
#endif
};

/* functions used from ESP-IDF driver that are not exposed in API */
extern esp_err_t i2c_hw_fsm_reset(i2c_port_t i2c_num);

/* forward declaration of internal functions */
static void _i2c_start_cmd(i2c_t dev);
static void _i2c_stop_cmd(i2c_t dev);
static void _i2c_end_cmd(i2c_t dev);
static void _i2c_write_cmd(i2c_t dev, const uint8_t* data, uint8_t len);
static void _i2c_read_cmd(i2c_t dev, uint8_t len, bool last);
static void _i2c_transfer(i2c_t dev);
static void _i2c_intr_handler(void *arg);
static int _i2c_status_to_errno(i2c_t dev);

void i2c_init(i2c_t dev)
{
    _Static_assert(I2C_NUMOF <= I2C_NUMOF_MAX, "Too many I2C devices defined");
    _Static_assert(I2C_NUMOF == ARRAY_SIZE(_i2c_hw),
                   "Number of hardware descriptors doesn't match the I2C_NUMOF");

    assert(dev < I2C_NUMOF);

    /* According to the Technical Reference Manual, only FAST mode is supported,
     * but FAST PLUS mode seems to work also. */
    assert(i2c_config[dev].speed <= I2C_SPEED_FAST_PLUS);

    /* GPIOs for SCL and SDA signals must not already be used for peripherals */
    if (((gpio_get_pin_usage(i2c_config[dev].scl) != _I2C) &&
         (gpio_get_pin_usage(i2c_config[dev].scl) != _GPIO)) ||
        ((gpio_get_pin_usage(i2c_config[dev].sda) != _I2C) &&
         (gpio_get_pin_usage(i2c_config[dev].sda) != _GPIO))) {
        LOG_TAG_ERROR("i2c", "GPIO%u and/or GPIO%u are used for %s/%s and "
                      "cannot be used as I2C interface\n",
                       i2c_config[dev].scl, i2c_config[dev].sda,
                       gpio_get_pin_usage_str(i2c_config[dev].scl),
                       gpio_get_pin_usage_str(i2c_config[dev].sda));
        assert(0);
    }

    _i2c_bus[dev].cmd = 0;
    _i2c_bus[dev].len = 0;

    i2c_acquire(dev);

    i2c_config_t cfg = {};

    cfg.mode = I2C_MODE_MASTER;
    cfg.sda_io_num = i2c_config[dev].sda;
    cfg.scl_io_num = i2c_config[dev].scl;
    cfg.sda_pullup_en = i2c_config[dev].sda_pullup;
    cfg.scl_pullup_en = i2c_config[dev].scl_pullup;
#if defined(SOC_I2C_SUPPORT_RTC) && !defined(CPU_FAM_ESP32S3)
    cfg.clk_flags = I2C_SCLK_SRC_FLAG_LIGHT_SLEEP;
#endif

    switch (i2c_config[dev].speed) {
        case I2C_SPEED_LOW:
            cfg.master.clk_speed = 10 * KHZ(1);
            break;
        case I2C_SPEED_NORMAL:
            cfg.master.clk_speed = 100 * KHZ(1);
            break;
        case I2C_SPEED_FAST:
            cfg.master.clk_speed = 400 * KHZ(1);
            break;
        case I2C_SPEED_FAST_PLUS:
            cfg.master.clk_speed = 1000 * KHZ(1);
            break;
        case I2C_SPEED_HIGH:
            cfg.master.clk_speed = 3400 * KHZ(1);
            break;
        default:
            LOG_TAG_ERROR("i2c", "Invalid speed value in %s\n", __func__);
            assert(0);
    }

    _i2c_bus[dev].clk_freq = cfg.master.clk_speed;

    /* configures the GPIOs, sets the bus timing and enables the periphery */
    i2c_param_config(dev, &cfg);

#if defined(SOC_I2C_SUPPORT_APB)
    /* If I2C clock is derived from APB clock, the bus timing parameters
     * have to be corrected if the APB clock is less than 80 MHz */
    extern uint32_t rtc_clk_apb_freq_get(void);
    uint32_t apb_clk = rtc_clk_apb_freq_get();

    if (apb_clk < MHZ(80)) {
        i2c_clk_cal_t clk_cfg;
        i2c_ll_cal_bus_clk(apb_clk, cfg.master.clk_speed, &clk_cfg);
        i2c_ll_set_bus_timing(_i2c_hw[dev].dev, &clk_cfg);
    }
#endif

    /* store the usage type in GPIO table */
    gpio_set_pin_usage(i2c_config[dev].scl, _I2C);
    gpio_set_pin_usage(i2c_config[dev].sda, _I2C);

    /* route all I2C interrupt sources to same the CPU interrupt */
    intr_matrix_set(PRO_CPU_NUM, i2c_periph_signal[dev].irq, CPU_INUM_I2C);

    /* set interrupt handler and enable the CPU interrupt */
    intr_cntrl_ll_set_int_handler(CPU_INUM_I2C, _i2c_intr_handler, NULL);
    intr_cntrl_ll_enable_interrupts(BIT(CPU_INUM_I2C));

    i2c_release(dev);
}

void i2c_acquire(i2c_t dev)
{
    DEBUG ("%s\n", __func__);
    assert(dev < I2C_NUMOF);

    mutex_lock(&_i2c_bus[dev].dev_lock);
}

void i2c_release(i2c_t dev)
{
    DEBUG ("%s\n", __func__);
    assert(dev < I2C_NUMOF);

    mutex_unlock(&_i2c_bus[dev].dev_lock);
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t len, uint8_t flags)
{
    DEBUG ("%s dev=%u addr=%02x data=%p len=%d flags=%01x\n",
           __func__, dev, addr, data, len, flags);

    assert(dev < I2C_NUMOF);
    assert(len > 0);
    assert(data != NULL);

    int res;

    _i2c_bus[dev].cmd_op = I2C_LL_CMD_READ;
    _i2c_bus[dev].cmd = 0;

    /* reset TX/RX FIFO queue */
    i2c_hal_txfifo_rst(&_i2c_hw[dev]);
    i2c_hal_rxfifo_rst(&_i2c_hw[dev]);

    /*  if I2C_NOSTART is not set, START condition and ADDR is used */
    if (!(flags & I2C_NOSTART)) {
        /* send START condition */
        _i2c_start_cmd(dev);

        /* address handling */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr10[2];
            addr10[0] = 0xf0 | (addr & 0x0300) >> 7 | I2C_MASTER_READ;
            addr10[1] = addr & 0xff;
            /* send ADDR with read flag */
            _i2c_write_cmd(dev, addr10, 2);
        }
        else {
            /* send ADDR with read flag */
            uint8_t addr7 = (addr << 1 | I2C_READ);
            _i2c_write_cmd(dev, &addr7, 1);
        }
    }

    /* read data bytes in blocks of SOC_I2C_FIFO_LEN bytes */
    uint32_t off = 0;

    /* if len > SOC_I2C_FIFO_LEN read SOC_I2C_FIFO_LEN bytes at a time */
    while (len > SOC_I2C_FIFO_LEN) {
        /* read one block of data bytes command */
        _i2c_bus[dev].len = SOC_I2C_FIFO_LEN;
        _i2c_read_cmd(dev, SOC_I2C_FIFO_LEN, false);
        _i2c_end_cmd(dev);
        _i2c_transfer(dev);

        res = _i2c_status_to_errno(dev);
        if (res) {
            return res;
        }

        /* if transfer was successful, fetch the data from I2C RAM */
        i2c_hal_read_rxfifo(&_i2c_hw[dev], data + off, len);

        /* reset RX FIFO queue */
        i2c_hal_rxfifo_rst(&_i2c_hw[dev]);

        len -= SOC_I2C_FIFO_LEN;
        off += SOC_I2C_FIFO_LEN;
    }

    /* read remaining data bytes command with a final NAK */
    _i2c_bus[dev].len = len;
    _i2c_read_cmd(dev, len, true);

    /* if I2C_NOSTOP flag is not set, send STOP condition is used */
    if (!(flags & I2C_NOSTOP)) {
        /* send STOP condition */
        _i2c_stop_cmd(dev);
    }
    else {
        /* otherwise place end command in pipeline */
        _i2c_end_cmd(dev);
    }

    /* finish operation by executing the command pipeline */
    _i2c_transfer(dev);

    if ((res = _i2c_status_to_errno(dev))) {
        return res;
    }

    /* fetch the data from RX FIFO */
    i2c_hal_read_rxfifo(&_i2c_hw[dev], data + off, len);

    /* return 0 on success */
    return 0;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len, uint8_t flags)
{
    DEBUG ("%s dev=%u addr=%02x data=%p len=%d flags=%01x\n",
           __func__, dev, addr, data, len, flags);

    assert(dev < I2C_NUMOF);
    assert(len > 0);
    assert(data != NULL);

    int res;

    _i2c_bus[dev].cmd_op = I2C_LL_CMD_WRITE;
    _i2c_bus[dev].cmd = 0;

    /* reset TX FIFO queue */
    i2c_hal_txfifo_rst(&_i2c_hw[dev]);

    /*  if I2C_NOSTART is not set, START condition and ADDR is used */
    if (!(flags & I2C_NOSTART)) {

        /* send START condition */
        _i2c_start_cmd(dev);

        /* address handling */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr10[2];
            addr10[0] = 0xf0 | (addr & 0x0300) >> 7 | I2C_MASTER_WRITE;
            addr10[1] = addr & 0xff;
            /* send ADDR without read flag */
            _i2c_write_cmd(dev, addr10, 2);
        }
        else {
            /* send ADDR without read flag */
            uint8_t addr7 = addr << 1;
            _i2c_write_cmd(dev, &addr7, 1);
        }
    }

    /* send data bytes in blocks of SOC_I2C_FIFO_LEN bytes */

    uint32_t off = 0;
    uint32_t tx_fifo_free;

    /* get available TX FIFO space */
    i2c_hal_get_txfifo_cnt(&_i2c_hw[dev], &tx_fifo_free);

    /* if len > SOC_I2C_FIFO_LEN write SOC_I2C_FIFO_LEN bytes at a time */
    while (len > tx_fifo_free) {
        /* send one block of data bytes */
        _i2c_bus[dev].len = tx_fifo_free;
        _i2c_write_cmd(dev, (uint8_t*)data + off, tx_fifo_free);
        _i2c_end_cmd(dev);
        _i2c_transfer(dev);
        res = _i2c_status_to_errno(dev);

        if (res) {
            return res;
        }

        len -= tx_fifo_free;
        off += tx_fifo_free;

        /* reset TX FIFO queue */
        i2c_hal_txfifo_rst(&_i2c_hw[dev]);

        /* update available TX FIFO space */
        i2c_hal_get_txfifo_cnt(&_i2c_hw[dev], &tx_fifo_free);
    }

    /* write remaining data bytes command */
    _i2c_bus[dev].len = len;
    _i2c_write_cmd(dev, (uint8_t*)data + off, len);

    /* if I2C_NOSTOP flag is not set, send STOP condition is used */
    if (!(flags & I2C_NOSTOP)) {
        /* send STOP condition */
        _i2c_stop_cmd(dev);
    }
    else {
        /* otherwise place end command in pipeline */
        _i2c_end_cmd(dev);
    }

    /* finish operation by executing the command pipeline */
    _i2c_transfer(dev);

    return _i2c_status_to_errno(dev);
}

#if defined(CPU_FAM_ESP32)
#define I2C_NACK_INT_ENA_M  I2C_ACK_ERR_INT_ENA_M
#endif

/*
 * @note:
 * It is a known problem that the ESP32x I2C hardware implementation checks
 * the ACK when the next command in the pipeline is executed. That is,
 * for a segmented write operation of type `START ADDR DATA0 STOP` the
 * `I2C_END_DETECT_INT` and the `I2C_NACK_INT` are triggered simultaneously
 * in case of acknowledge errors. In this case it is not clear from the
 * interrupt status whether the acknolegdement error has occurred for the
 * ADDR or the DATA0 byte. The only way to find out is to check the remaining
 * TX FIFO length. If the number of bytes in the TX-FIFO is still higher
 * (for two-byte addresses) or equal (for one-byte addresses) to the length of
 * the data, the acknolegdement error occurred for the ADDR field.
 */
static int _i2c_status_to_errno(i2c_t dev)
{
    if (_i2c_bus[dev].status & I2C_ARBITRATION_LOST_INT_ENA_M) {
        LOG_TAG_DEBUG("i2c", "arbitration lost dev=%u\n", dev);
        return -EAGAIN;
    }

    if (_i2c_bus[dev].status & I2C_NACK_INT_ENA_M) {
        LOG_TAG_DEBUG("i2c", "ack error dev=%u\n", dev);
        if (_i2c_bus[dev].cmd_op == I2C_LL_CMD_WRITE) {
            /*
             * @note: In a write transfer, an ACK error can happen for the
             * address field as well as for data. If the FIFO still contains
             * all data bytes, the ACK error happened in address field and we
             * have to return -ENXIO. Otherwise, the ACK error happened in data
             * field and we have to return -EIO.
             */
            uint32_t cnt;

            i2c_hal_get_txfifo_cnt(&_i2c_hw[dev], &cnt);
            return ((SOC_I2C_FIFO_LEN - cnt) >= _i2c_bus[dev].len) ? -ENXIO : -EIO;
        }
        else {
            /*
             * @note: In a read transfer, an ACK is only expected for the
             * address field. Thus, an ACK error can only happen for the address
             * field. Therefore, we always return -ENXIO in case of an ACK
             * error.
             */
            return -ENXIO;
        }
    }

    if (_i2c_bus[dev].status & I2C_TIME_OUT_INT_ENA_M) {
        LOG_TAG_DEBUG("i2c", "bus timeout dev=%u\n", dev);
        i2c_hw_fsm_reset(dev);
        return -ETIMEDOUT;
    }

    return 0;
}

static void _i2c_start_cmd(i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    /* place START condition command in command queue */
    i2c_hw_cmd_t cmd = { .op_code = I2C_LL_CMD_RESTART };
    i2c_hal_write_cmd_reg(&_i2c_hw[dev], cmd, _i2c_bus[dev].cmd);

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

static void _i2c_stop_cmd(i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    /* place STOP condition command in command queue */
    i2c_hw_cmd_t cmd = { .op_code = I2C_LL_CMD_STOP };
    i2c_hal_write_cmd_reg(&_i2c_hw[dev], cmd, _i2c_bus[dev].cmd);

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

static void _i2c_end_cmd(i2c_t dev)
{
    DEBUG ("%s\n", __func__);

    /* place END command for continues data transmission in command queue */
    i2c_hw_cmd_t cmd = { .op_code = I2C_LL_CMD_END };
    i2c_hal_write_cmd_reg(&_i2c_hw[dev], cmd, _i2c_bus[dev].cmd);

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

static void _i2c_write_cmd(i2c_t dev, const uint8_t* data, uint8_t len)
{
    DEBUG ("%s dev=%u data=%p len=%d\n", __func__, dev, data, len);

    if (len > SOC_I2C_FIFO_LEN) {
        LOG_TAG_ERROR("i2c", "Maximum number of bytes (32 bytes) that can be "
                      "sent with on transfer reached\n");
        return;
    }

    /* store the data in TX FIFO */
    i2c_hal_write_txfifo(&_i2c_hw[dev], (uint8_t *)data, len);

    /* place WRITE command for multiple bytes in command queue */
    i2c_hw_cmd_t cmd = { .op_code = I2C_LL_CMD_WRITE,
                         .byte_num = len,
                         .ack_en = 1,
                         .ack_exp = 0,
                         .ack_val = 0 };
    i2c_hal_write_cmd_reg(&_i2c_hw[dev], cmd, _i2c_bus[dev].cmd);

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

static void _i2c_read_cmd(i2c_t dev, uint8_t len, bool last)
{
    DEBUG ("%s dev=%u len=%d\n", __func__, dev, len);

    if (len < 1 || len > SOC_I2C_FIFO_LEN) {
        /* at least one byte has to be read */
        LOG_TAG_ERROR("i2c", "At least one byte has to be read\n");
        return;
    }

    if (len > 1)
    {
        /* place READ command for len-1 bytes with positive ack in command queue*/
        i2c_hw_cmd_t cmd = { .op_code = I2C_LL_CMD_READ,
                             .byte_num = len-1,
                             .ack_en = 0,
                             .ack_exp = 0,
                             .ack_val = 0 };
        i2c_hal_write_cmd_reg(&_i2c_hw[dev], cmd, _i2c_bus[dev].cmd);

        /* increment the command counter */
        _i2c_bus[dev].cmd++;
    }

    /* place READ command for last byte with negative ack in last segment in command queue*/
    i2c_hw_cmd_t cmd = { .op_code = I2C_LL_CMD_READ,
                         .byte_num = 1,
                         .ack_en = 0,
                         .ack_exp = 0,
                         .ack_val = last ? 1 : 0 };
    i2c_hal_write_cmd_reg(&_i2c_hw[dev], cmd, _i2c_bus[dev].cmd);

    /* increment the command counter */
    _i2c_bus[dev].cmd++;
}

/* a maximum transfer 33 byte * 9 clock cycles * 1/100000 s takes at
 * I2C_SPEED_NORMAL about 450 us, at I2C_SPEED_NORMAL_LOW about 4.5 ms */
#define I2C_TRANSFER_TIMEOUT    640

/* combination of all transfer interrupts */
#define I2C_LL_MASTER_INT   (I2C_LL_MASTER_TX_INT | I2C_LL_MASTER_RX_INT)

void _i2c_transfer_timeout(void *arg)
{
    i2c_t dev = (i2c_t)(uintptr_t)arg;

    /* reset the hardware if I2C got stuck */
    i2c_hw_fsm_reset(dev);

    /* set result to timeout */
    _i2c_bus[dev].status = I2C_TIME_OUT_INT_ENA_M;

    /* wake up the thread that is waiting for the results */
    mutex_unlock(&_i2c_bus[dev].cmd_lock);
}

/* Transfer of commands in I2C controller command pipeline */
static void _i2c_transfer(i2c_t dev)
{
    DEBUG("%s cmd=%d\n", __func__, _i2c_bus[dev].cmd);

    /* disable and enable all transmission interrupts and clear current status */
    i2c_hal_clr_intsts_mask(&_i2c_hw[dev], I2C_LL_MASTER_INT);
    i2c_hal_enable_intr_mask(&_i2c_hw[dev], I2C_LL_MASTER_INT);

    /* set a timer for the case the I2C hardware gets stuck */
#if defined(MODULE_ZTIMER_MSEC)
    uint32_t timeout = ((I2C_TRANSFER_TIMEOUT * KHZ(1)) / _i2c_bus[dev].clk_freq) + 1;
    ztimer_t timer = { .callback = _i2c_transfer_timeout,
                       .arg = (void*)(uintptr_t)dev };
    ztimer_set(ZTIMER_MSEC, &timer, timeout);
#endif

    /* start the execution of commands in command pipeline */
    _i2c_bus[dev].status = 0;

    i2c_hal_update_config(&_i2c_hw[dev]);
    i2c_hal_trans_start(&_i2c_hw[dev]);

    /* wait for transfer results and remove timeout timer*/
    mutex_lock(&_i2c_bus[dev].cmd_lock);

#if defined(MODULE_ZTIMER_MSEC)
    ztimer_remove(ZTIMER_MSEC, &timer);
#endif

    /* returned from transmission */
    DEBUG("%s status=%08"PRIx32"\n", __func__, _i2c_bus[dev].status);

    /* reset command and data index */
    _i2c_bus[dev].cmd = 0;
}

static void IRAM_ATTR _i2c_intr_handler(void *arg)
{
    /* to satisfy the compiler */
    (void)arg;

    irq_isr_enter();

    /* all I2C peripheral interrupt sources are routed to the same interrupt,
       so we have to use the status register to distinguish interruptees */
    for (unsigned dev = 0; dev < I2C_NUMOF; dev++) {
        uint32_t mask = i2c_ll_get_intsts_mask(_i2c_hw[dev].dev);
        /* test for transfer related interrupts */
        if (mask) {
            _i2c_bus[dev].status = mask;
            /* disable all interrupts and clear pending interrupts */
            i2c_hal_clr_intsts_mask(&_i2c_hw[dev], I2C_LL_MASTER_INT);
            i2c_hal_disable_intr_mask(&_i2c_hw[dev], I2C_LL_MASTER_INT);

            /* wake up the thread that is waiting for the results */
            mutex_unlock(&_i2c_bus[dev].cmd_lock);
        }
    }

    irq_isr_exit();
}

void i2c_print_config(void)
{
    if (I2C_NUMOF) {
        for (unsigned dev = 0; dev < I2C_NUMOF; dev++) {
            printf("\tI2C_DEV(%u)\tscl=%d sda=%d\n",
                   dev, i2c_config[dev].scl, i2c_config[dev].sda);
        }
    }
    else {
        LOG_TAG_INFO("i2c", "no I2C devices\n");
    }
}
