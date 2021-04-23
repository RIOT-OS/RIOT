/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_lis2dh12
 * @{
 *
 * @file
 * @brief       LIS2DH12 accelerometer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Mohr <jan.mohr@ml-pa.com>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "assert.h"
#include "byteorder.h"
#include "mutex.h"
#include "timex.h"

#include "lis2dh12.h"
#include "lis2dh12_internal.h"
#include "lis2dh12_registers.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/* the following block contains the SPI mode specific adaption */
#ifdef MODULE_LIS2DH12_SPI

/* SPI bus speed and mode */
#define BUS_CLK             SPI_CLK_5MHZ
#define BUS_MODE            SPI_MODE_0
#define BUS_OK              SPI_OK
/* shortcuts for SPI bus parameters */
#define BUS                 (dev->p->spi)
#define BUS_CS              (dev->p->cs)
/* flag to set when reading from the device */
#define FLAG_READ           (0x80)
/* flag to enable address auto incrementation on read or write */
#define FLAG_AINC           (0x40)

static int _init_bus(const lis2dh12_t *dev)
{
    /* for SPI, we only need to initialize the chip select pin */
    if (spi_init_cs(BUS, BUS_CS) != SPI_OK) {
        return LIS2DH12_NOBUS;
    }
    return LIS2DH12_OK;
}

static int _acquire(const lis2dh12_t *dev)
{
    return spi_acquire(BUS, BUS_CS, BUS_MODE, BUS_CLK);
}

static void _release(const lis2dh12_t *dev)
{
    spi_release(BUS);
}

static uint8_t _read(const lis2dh12_t *dev, uint8_t reg)
{
    return spi_transfer_reg(BUS, BUS_CS, (FLAG_READ | reg), 0);
}

static void _read_burst(const lis2dh12_t *dev, uint8_t reg, void *data,
                        size_t len) {
    spi_transfer_regs(BUS, BUS_CS, (FLAG_READ | FLAG_AINC | reg), NULL, data,
                      len);
}

static void _write(const lis2dh12_t *dev, uint8_t reg, uint8_t data)
{
    DEBUG("[lis2dh12] write: reg 0x%02x, val 0x%02x\n", (int)reg, (int)data);
    spi_transfer_reg(BUS, BUS_CS, reg, data);
}

/* and now the I2C specific part of the driver */
#else

/* I2C config */
#define BUS_OK              (0)
/* I2C shortcuts */
#define BUS                 (dev->p->i2c)
#define ADDR                (dev->p->addr)
/* flag for enabling address auto-incrementation */
#define FLAG_AINC           (0x80)

static int _init_bus(const lis2dh12_t *dev)
{
    (void) dev;

    /* for I2C, the bus is already set up by auto_init */
    return LIS2DH12_OK;
}

static int _acquire(const lis2dh12_t *dev)
{
    return i2c_acquire(BUS);
}

static void _release(const lis2dh12_t *dev)
{
    i2c_release(BUS);
}

static uint8_t _read(const lis2dh12_t *dev, uint8_t reg)
{
    uint8_t tmp;
    i2c_read_reg(BUS, ADDR, reg, &tmp, 0);
    return tmp;
}

static void _read_burst(const lis2dh12_t *dev, uint8_t reg,
                              void *data, size_t len)
{
    i2c_read_regs(BUS, ADDR, (FLAG_AINC | reg), data, len, 0);
}

static void _write(const lis2dh12_t *dev, uint8_t reg, uint8_t data)
{
    DEBUG("[lis2dh12] write: reg 0x%02x, val 0x%02x\n", (int)reg, (int)data);
    i2c_write_reg(BUS, ADDR, reg, data, 0);
}

#endif  /* MODULE_LIS2DH12_SPI */

int lis2dh12_init(lis2dh12_t *dev, const lis2dh12_params_t *params)
{
    assert(dev && params);

    dev->p = params;
    /* calculate shift amount to convert raw acceleration data */
    dev->comp = 4 - (dev->p->scale >> 4);

    /* initialize the chip select line */
    if (_init_bus(dev) != LIS2DH12_OK) {
        DEBUG("[lis2dh12] error: unable to initialize the bus\n");
        return LIS2DH12_NOBUS;
    }

    /* acquire the bus and verify that our parameters are valid */
    if (_acquire(dev) != BUS_OK) {
        DEBUG("[lis2dh12] error: unable to acquire the bus\n");
        return LIS2DH12_NOBUS;
    }

    /* read the WHO_IM_I register to verify the connections to the device */
    if (_read(dev, REG_WHO_AM_I) != WHO_AM_I_VAL) {
        DEBUG("[lis2dh12] error: invalid value read from WHO_AM_I register\n");
        _release(dev);
        return LIS2DH12_NODEV;
    }

    /* enable all axes, set sampling rate and scale */
    LIS2DH12_CTRL_REG1_t reg1 = {0};

    reg1.bit.ODR = dev->p->rate;
    reg1.bit.Xen = 1;
    reg1.bit.Yen = 1;
    reg1.bit.Zen = 1;

    _write(dev, REG_CTRL_REG4, dev->p->scale);
    _write(dev, REG_CTRL_REG1, reg1.reg);

    _release(dev);

    DEBUG("[lis2dh12] initialization successful\n");
    return LIS2DH12_OK;
}

int lis2dh12_read(const lis2dh12_t *dev, int16_t *data)
{
    assert(dev && data);

    /* allocate 6 byte to save the 6 RAW data registers */
    uint8_t raw[6];

    /* read sampled data from the device */
    _acquire(dev);

    /* first check if valid data is available */
    if ((_read(dev, REG_STATUS_REG) & LIS2DH12_STATUS_REG_ZYXDA) == 0) {
        _release(dev);
        return LIS2DH12_NODATA;
    }

    _read_burst(dev, REG_OUT_X_L, raw, 6);
    _release(dev);

    /* calculate the actual g-values for the x, y, and z dimension */
    for (int i = 0; i < 3; i++) {
        data[i] = (int16_t)((raw[i*2 + 1] << 8) | raw[i*2]) >> dev->comp;
    }

    return LIS2DH12_OK;
}

static const uint16_t mg_per_bit[] = {
    16, /* scale = 2g  */
    32, /* scale = 4g  */
    62, /* scale = 8g  */
    186 /* scale = 16g */
};

static const uint16_t hz_per_dr[] = {
    0,      /* power down */
    1,      /* Hz */
    10,     /* Hz */
    25,     /* Hz */
    50,     /* Hz */
    100,    /* Hz */
    200,    /* Hz */
    400,    /* Hz */
    1620,   /* Hz */
    5376,   /* Hz */
};

void lis2dh12_cfg_threshold_event(const lis2dh12_t *dev,
                                  uint32_t mg, uint32_t us,
                                  uint8_t axis, uint8_t event, uint8_t line)
{
    assert(line == LIS2DH12_INT1 || line == LIS2DH12_INT2);
    assert(event == LIS2DH12_EVENT_1 || event == LIS2DH12_EVENT_2);

    _acquire(dev);

    LIS2DH12_CTRL_REG2_t reg2;
    reg2.reg = _read(dev, REG_CTRL_REG2);
    uint8_t odr   = _read(dev, REG_CTRL_REG1) >> 4;
    uint8_t scale = (_read(dev, REG_CTRL_REG4) >> 4) & 0x3;
    uint8_t int_reg = 0;

    /* read current interrupt configuration */
    if (line == LIS2DH12_INT1) {
        int_reg = _read(dev, REG_CTRL_REG3);
    }
    if (line == LIS2DH12_INT2) {
        int_reg = _read(dev, REG_CTRL_REG6);
    }

    DEBUG("[%u] threshold: %lu mg\n", event, mg);

    /* read reference to set it to current data */
    _read(dev, REG_REFERENCE);

    /* configure interrupt */
    switch (event) {
    case LIS2DH12_EVENT_1:
        /* apply high-pass to interrupt */
        reg2.bit.HP_IA1 = 1;
        int_reg |= LIS2DH12_INT_TYPE_IA1;

        /* clear INT flags */
        _read(dev, REG_INT1_SRC);

        _write(dev, REG_INT1_CFG, axis);
        _write(dev, REG_INT1_THS, mg / mg_per_bit[scale]);
        _write(dev, REG_INT1_DURATION, (us * hz_per_dr[odr]) / US_PER_SEC);
        break;
    case LIS2DH12_EVENT_2:
        /* apply high-pass to interrupt */
        reg2.bit.HP_IA2 = 1;
        int_reg |= LIS2DH12_INT_TYPE_IA2;

        /* clear INT flags */
        _read(dev, REG_INT2_SRC);

        _write(dev, REG_INT2_CFG, axis);
        _write(dev, REG_INT2_THS, mg / mg_per_bit[scale]);
        _write(dev, REG_INT2_DURATION, (us * hz_per_dr[odr]) / US_PER_SEC);
        break;
    }

    /* configure high-pass */
    _write(dev, REG_CTRL_REG2, reg2.reg);

    /* write back configuration */
    if (line == LIS2DH12_INT1) {
        _write(dev, REG_CTRL_REG3, int_reg);
    }
    if (line == LIS2DH12_INT2) {
        _write(dev, REG_CTRL_REG6, int_reg);
    }

    _release(dev);
}

void lis2dh12_cfg_click_event(const lis2dh12_t *dev, uint32_t mg,
                              uint32_t us_limit, uint32_t us_latency, uint32_t us_window,
                              uint8_t click, uint8_t line)
{
    _acquire(dev);

    uint8_t odr   = _read(dev, REG_CTRL_REG1) >> 4;
    uint8_t scale = (_read(dev, REG_CTRL_REG4) >> 4) & 0x3;

    DEBUG("click threshold: %lu mg\n", mg);

    /* read reference to set it to current data */
    _read(dev, REG_REFERENCE);

    /* select click axis & mode */
    _write(dev, REG_CLICK_CFG, click);

    /* enable interrupt latching */
    _write(dev, REG_CLICK_THS, (mg / mg_per_bit[scale]) | LIS2DH12_CLICK_THS_LIR);

    /* set timing parameters */
    _write(dev, REG_TIME_LIMIT,   (us_limit   * hz_per_dr[odr]) / US_PER_SEC);
    _write(dev, REG_TIME_LATENCY, (us_latency * hz_per_dr[odr]) / US_PER_SEC);
    _write(dev, REG_TIME_WINDOW,  (us_window  * hz_per_dr[odr]) / US_PER_SEC);

    /* enable high-pass */
    _write_or(dev, REG_CTRL_REG2, LIS2DH12_CTRL_REG2_HPCLICK);

    /* clear INT flags */
     _read(dev, REG_CLICK_SRC);

    /* configure interrupt */
    if (line == LIS2DH12_INT1) {
        _write_or(dev, REG_CTRL_REG3, LIS2DH12_INT_TYPE_CLICK);
    }
    if (line == LIS2DH12_INT2) {
        _write_or(dev, REG_CTRL_REG6, LIS2DH12_INT_TYPE_CLICK);
    }

    _release(dev);
}

void lis2dh12_cfg_disable_event(const lis2dh12_t *dev, uint8_t event, uint8_t line)
{
    uint8_t reg = 0;

    _release(dev);

    /* read current interrupt configuration */
    if (line == LIS2DH12_INT1) {
        reg = _read(dev, REG_CTRL_REG3);
    }
    if (line == LIS2DH12_INT2) {
        reg = _read(dev, REG_CTRL_REG6);
    }

    /* remove event */
    if (event == LIS2DH12_EVENT_1) {
        reg &= ~LIS2DH12_INT_TYPE_IA1;

        /* clear INT flags */
        _read(dev, REG_INT1_SRC);
    }
    if (event == LIS2DH12_EVENT_2) {
        reg &= ~LIS2DH12_INT_TYPE_IA2;

        /* clear INT flags */
        _read(dev, REG_INT2_SRC);
    }
    if (event == LIS2DH12_EVENT_CLICK) {
        reg &= ~LIS2DH12_INT_TYPE_CLICK;

        /* clear INT flags */
        _read(dev, REG_CLICK_SRC);
    }

    /* write back configuration */
    if (line == LIS2DH12_INT1) {
        _write(dev, REG_CTRL_REG3, reg);
    }
    if (line == LIS2DH12_INT2) {
        _write(dev, REG_CTRL_REG6, reg);
    }

    _release(dev);
}

#ifdef MODULE_LIS2DH12_INT
static void _cb(void *lock)
{
    mutex_unlock(lock);
}

static uint32_t _merge_int_flags(const lis2dh12_t *dev, uint8_t events)
{
    uint32_t int_src = 0;

    /* merge interrupt flags (7 bit per event) into one word */
    if (events & LIS2DH12_INT_TYPE_IA1) {
        int_src |= _read(dev, REG_INT1_SRC);
    }
    if (events & LIS2DH12_INT_TYPE_IA2) {
        int_src |= _read(dev, REG_INT2_SRC) << 8;
    }
    if (events & LIS2DH12_INT_TYPE_CLICK) {
        int_src |= _read(dev, REG_CLICK_SRC) << 16;
    }

    DEBUG("int_src: %lx\n", int_src);

    return int_src;
}

#define LIS2DH12_INT_SRC_ANY ((LIS2DH12_INT_SRC_IA <<  0) | \
                              (LIS2DH12_INT_SRC_IA <<  8) | \
                              (LIS2DH12_INT_SRC_IA << 16))

int lis2dh12_wait_event(const lis2dh12_t *dev, uint8_t line, bool stale_events)
{
    uint32_t int_src;
    uint8_t events = 0;
    mutex_t lock = MUTEX_INIT_LOCKED;
    gpio_t pin = line == LIS2DH12_INT2
                       ? dev->p->int2_pin
                       : dev->p->int1_pin;

    _acquire(dev);

    /* find out which events are configured */
    if (line == LIS2DH12_INT1) {
        events = _read(dev, REG_CTRL_REG3);
    }
    if (line == LIS2DH12_INT2) {
        events = _read(dev, REG_CTRL_REG6);
    }

    /* check for stale interrupt */
    int_src = _merge_int_flags(dev, events);

    _release(dev);

    /* return early if stale interrupt is present */
    if (stale_events && (int_src & LIS2DH12_INT_SRC_ANY)) {
        return int_src;
    }

    /* enable interrupt pin */
    assert(gpio_is_valid(pin));
    if (gpio_init_int(pin, GPIO_IN, GPIO_RISING, _cb, &lock)) {
        return LIS2DH12_NOINT;
    }

    /* wait for interrupt */
    mutex_lock(&lock);
    gpio_irq_disable(pin);

    /* read interrupt source */
    _acquire(dev);
    int_src = _merge_int_flags(dev, events);
    _release(dev);

    return int_src;
}
#endif /* MODULE_LIS2DH12_INT */

int lis2dh12_set_fifo(const lis2dh12_t *dev, const lis2dh12_fifo_t *config) {

    assert(dev && config);

    LIS2DH12_CTRL_REG5_t reg5 = {0};
    LIS2DH12_FIFO_CTRL_REG_t fifo_reg = {0};

    if (config->FIFO_mode != LIS2DH12_FIFO_MODE_BYPASS) {
        reg5.bit.FIFO_EN = 1;
    }
    fifo_reg.bit.TR = config->FIFO_set_INT2;
    fifo_reg.bit.FM = config->FIFO_mode;
    fifo_reg.bit.FTH = config->FIFO_watermark;

    _acquire(dev);
    _write(dev, REG_CTRL_REG5, reg5.reg);
    _write(dev, REG_FIFO_CTRL_REG, fifo_reg.reg);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_restart_fifo(const lis2dh12_t *dev) {

    assert(dev);

    _acquire(dev);
    uint8_t reg5 = _read(dev, REG_CTRL_REG5);
    LIS2DH12_FIFO_CTRL_REG_t fifo_reg = {0};
    fifo_reg.reg = _read(dev, REG_FIFO_CTRL_REG);

    uint8_t fifo_mode_old = fifo_reg.bit.FM;
    fifo_reg.bit.FM = LIS2DH12_FIFO_MODE_BYPASS;

    /* switch to Bypass mode */
    _write(dev, REG_FIFO_CTRL_REG, fifo_reg.reg);

    fifo_reg.bit.FM = fifo_mode_old;

    _write(dev, REG_CTRL_REG5, reg5);
    _write(dev, REG_FIFO_CTRL_REG, fifo_reg.reg);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_read_fifo_src(const lis2dh12_t *dev, LIS2DH12_FIFO_SRC_REG_t *data) {

    assert(dev && data);

    _acquire(dev);
    data->reg = _read(dev, REG_FIFO_SRC_REG);
    _release(dev);

    return LIS2DH12_OK;
}

uint8_t lis2dh12_read_fifo_data(const lis2dh12_t *dev, lis2dh12_fifo_data_t *fifo_data,
                                uint8_t number) {

    assert(dev && fifo_data);
    /* check max FIFO length */
    assert(number <= 32);

    _acquire(dev);
    /* check if number is available */
    LIS2DH12_FIFO_SRC_REG_t src_reg = {0};
    src_reg.reg = _read(dev, REG_FIFO_SRC_REG);

    if (src_reg.bit.FSS <= number) {
        number = src_reg.bit.FSS;
    }

    if (src_reg.bit.EMPTY) {
        return 0;
    }

    /* calculate X, Y and Z values */
    for (uint8_t i = 0; i < number; i++){
        fifo_data[i].X_AXIS = (int16_t)(_read(dev, REG_OUT_X_L) | (_read(dev, REG_OUT_X_H) << 8))
                                >> dev->comp;
        fifo_data[i].Y_AXIS = (int16_t)(_read(dev, REG_OUT_Y_L) | (_read(dev, REG_OUT_Y_H) << 8))
                                >> dev->comp;
        fifo_data[i].Z_AXIS = (int16_t)(_read(dev, REG_OUT_Z_L) | (_read(dev, REG_OUT_Z_H) << 8))
                                >> dev->comp;
    }

    _release(dev);

    return number;
}

int lis2dh12_clear_data(const lis2dh12_t *dev) {

    assert(dev);

    LIS2DH12_CTRL_REG5_t ctrl_reg5 = {0};
    ctrl_reg5.bit.BOOT = 1;

    _acquire(dev);
    _write(dev, REG_CTRL_REG5, ctrl_reg5.reg);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_set_reference(const lis2dh12_t *dev, uint8_t reference) {

    assert(dev);

    _acquire(dev);
    _write(dev, REG_REFERENCE, reference);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_read_reference(const lis2dh12_t *dev, uint8_t *data) {

    assert(dev);

    _acquire(dev);
    *data = _read(dev, REG_REFERENCE);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_set_highpass(const lis2dh12_t *dev, const lis2dh12_highpass_t *config) {

    assert(dev && config);

    LIS2DH12_CTRL_REG2_t data = {0};

    data.bit.HPM = config->Highpass_mode;
    data.bit.HPCF = config->Highpass_freq;
    data.bit.FDS = config->DATA_OUT_enable;
    data.bit.HP_IA1 = config->INT1_enable;
    data.bit.HP_IA2 = config->INT2_enable;
    data.bit.HPCLICK = config->CLICK_enable;

    _acquire(dev);
    _write(dev, REG_CTRL_REG2, data.reg);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_set_click(const lis2dh12_t *dev, const lis2dh12_click_t *config) {
    assert(dev);

    LIS2DH12_CLICK_CFG_t click_CFG = {0};
    if (config->enable_DOUBLE) {
        click_CFG.bit.XD = config->enable_X_CLICK;
        click_CFG.bit.YD = config->enable_Y_CLICK;
        click_CFG.bit.ZD = config->enable_Z_CLICK;
    }
    else {
        click_CFG.bit.XS = config->enable_X_CLICK;
        click_CFG.bit.YS = config->enable_Y_CLICK;
        click_CFG.bit.ZS = config->enable_Z_CLICK;
    }

    LIS2DH12_CLICK_THS_t click_thold  = {0};
    click_thold.bit.LIR_CLICK = config->noINT_latency;
    click_thold.bit.THS = config->CLICK_thold;

    _acquire(dev);
    _write(dev, REG_CLICK_CFG, click_CFG.reg);
    _write(dev, REG_CLICK_THS, click_thold.reg);
    _write(dev, REG_TIME_LIMIT, config->TIME_limit);
    _write(dev, REG_TIME_LATENCY, config->TIME_latency);
    _write(dev, REG_TIME_WINDOW, config->TIME_window);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_read_click_src(const lis2dh12_t *dev, LIS2DH12_CLICK_SRC_t *data) {
    assert(dev && data);

    _acquire(dev);
    data->reg = _read(dev, REG_CLICK_SRC);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_set_resolution(const lis2dh12_t *dev, lis2dh12_resolution_t resolution)
{
    assert(dev);

    LIS2DH12_CTRL_REG1_t reg1;
    LIS2DH12_CTRL_REG4_t reg4;

    _acquire(dev);
    reg1.reg = _read(dev, REG_CTRL_REG1);
    reg4.reg = _read(dev, REG_CTRL_REG4);

    /* set power mode */
    if (resolution == LIS2DH12_POWER_LOW) {
        reg1.bit.LPen = 1;
        reg4.bit.HR = 0;
    }
    else if (resolution == LIS2DH12_POWER_HIGH) {
        reg1.bit.LPen = 0;
        reg4.bit.HR = 1;
    }
    else if (resolution == LIS2DH12_POWER_NORMAL) {
        reg1.bit.LPen = 0;
        reg4.bit.HR = 0;
    }
    else { /* power down mode */
        reg1.bit.ODR = 0;
    }

    _write(dev, REG_CTRL_REG1, reg1.reg);
    _write(dev, REG_CTRL_REG4, reg4.reg);
    _release(dev);

    return LIS2DH12_OK;
}

lis2dh12_resolution_t lis2dh12_get_resolution(const lis2dh12_t *dev)
{
    assert(dev);

    LIS2DH12_CTRL_REG1_t reg1;
    LIS2DH12_CTRL_REG4_t reg4;

    _acquire(dev);
    reg1.reg = _read(dev, REG_CTRL_REG1);
    reg4.reg = _read(dev, REG_CTRL_REG4);
    _release(dev);

    if (!reg1.bit.ODR) {
        return LIS2DH12_POWER_DOWN;
    }
    if (reg1.bit.LPen) {
        return LIS2DH12_POWER_LOW;
    }
    if (reg4.bit.HR) {
        return LIS2DH12_POWER_HIGH;
    }
    return LIS2DH12_POWER_NORMAL;
}
int lis2dh12_set_datarate(const lis2dh12_t *dev, lis2dh12_rate_t rate) {

    assert(dev);
    assert(rate <= 0x9);

    LIS2DH12_CTRL_REG1_t reg1 = {0};

    _acquire(dev);
    reg1.reg = _read(dev, REG_CTRL_REG1);

    reg1.bit.ODR = rate;

    _write(dev, REG_CTRL_REG1, reg1.reg);
    _release(dev);

    return LIS2DH12_OK;
}

int lis2dh12_set_scale(lis2dh12_t *dev, lis2dh12_scale_t scale) {

    assert(dev);
    assert((scale>>4) <= 0x3);

    LIS2DH12_CTRL_REG4_t reg4 = {0};

    _acquire(dev);
    reg4.reg = _read(dev, REG_CTRL_REG4);

    reg4.bit.FS = scale >> 4;

    _write(dev, REG_CTRL_REG4, reg4.reg);
    _release(dev);

    dev->comp = 4 - (scale >> 4);

    return LIS2DH12_OK;
}

int lis2dh12_poweron(const lis2dh12_t *dev)
{
    assert(dev);

    /* set default param values */
    lis2dh12_set_datarate(dev, dev->p->rate);
    lis2dh12_set_resolution(dev, dev->p->resolution);

    return LIS2DH12_OK;
}

int lis2dh12_poweroff(const lis2dh12_t *dev)
{
    assert(dev);

    /* set datarate to zero */
    lis2dh12_set_datarate(dev, 0);

    return LIS2DH12_OK;
}
