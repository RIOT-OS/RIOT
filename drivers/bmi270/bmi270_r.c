#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "bmi270.h"
#include "bmi2.h"

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "ztimer.h"
#include "bmi270_r.h"

#ifndef BMI270_I2C_ADDR_PRIMARY
// SDO = GND
#define BMI270_I2C_ADDR_PRIMARY  (0x68)
#endif
#ifndef BMI270_I2C_ADDR_SECONDARY
// SDO = VDDIO
#define BMI270_I2C_ADDR_SECONDARY (0x69)
#endif

#define _SOFT_RESET_DELAY_US     (300U)

static inline void _set_pin(gpio_t *ref, gpio_t pin)
{
    *ref = pin;
}

static BMI2_INTF_RETURN_TYPE _bmi2_i2c_read(uint8_t reg_addr, uint8_t *reg_data,
                                           uint32_t len, void *intf_ptr);
static BMI2_INTF_RETURN_TYPE _bmi2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data,
                                            uint32_t len, void *intf_ptr);
static void _bmi2_delay_us(uint32_t us, void *intf_ptr);

static inline int _bmi_status_to_rc(int8_t status)
{
    return (status == BMI2_OK) ? 0 : (int)status;
}

// forward Bosch delay cb to RIOT microsecond timer
static void _bmi2_delay_us(uint32_t us, void *intf_ptr)
{
    (void)intf_ptr;
    if (us > 0U) {
        ztimer_sleep(ZTIMER_USEC, us);
    }
}

// read one register window with retry/backoff on I2C errs
static BMI2_INTF_RETURN_TYPE _bmi2_i2c_read(uint8_t reg_addr,
                                            uint8_t *reg_data,
                                            uint32_t len,
                                            void *intf_ptr)
{
    bmi270_t *dev = (bmi270_t *)intf_ptr;
    if ((dev == NULL) || (reg_data == NULL)) {
        return (BMI2_INTF_RETURN_TYPE)BMI2_E_NULL_PTR;
    }

    for (unsigned attempt = 0; attempt < 6U; attempt++) {
        i2c_acquire(dev->i2c);
        int rc = i2c_read_regs(dev->i2c, dev->addr, reg_addr,
                               reg_data, (size_t)len, 0);
        i2c_release(dev->i2c);

        if (rc == 0) {
            return (BMI2_INTF_RETURN_TYPE)BMI2_INTF_RET_SUCCESS;
        }

        unsigned ms = 2U << ((attempt > 4U) ? 4U : attempt);
        ztimer_sleep(ZTIMER_USEC, ms * 1000U);
    }

    return (BMI2_INTF_RETURN_TYPE)BMI2_E_COM_FAIL;
}

// write one register window with retry/backoff on I2C errs
static BMI2_INTF_RETURN_TYPE _bmi2_i2c_write(uint8_t reg_addr,
                                             const uint8_t *reg_data,
                                             uint32_t len,
                                             void *intf_ptr)
{
    bmi270_t *dev = (bmi270_t *)intf_ptr;
    if ((dev == NULL) || (reg_data == NULL)) {
        return (BMI2_INTF_RETURN_TYPE)BMI2_E_NULL_PTR;
    }

    for (unsigned attempt = 0; attempt < 6U; attempt++) {
        i2c_acquire(dev->i2c);
        int rc = i2c_write_regs(dev->i2c, dev->addr, reg_addr,
                                reg_data, (size_t)len, 0);
        i2c_release(dev->i2c);

        if (rc == 0) {
            return (BMI2_INTF_RETURN_TYPE)BMI2_INTF_RET_SUCCESS;
        }

        unsigned ms = 2U << ((attempt > 4U) ? 4U : attempt);
        ztimer_sleep(ZTIMER_USEC, ms * 1000U);
    }

    return (BMI2_INTF_RETURN_TYPE)BMI2_E_COM_FAIL;
}

// init BMI270 on RIOT I2C bus
int bmi270_init_i2c(bmi270_t *dev, i2c_t i2c, uint8_t addr)
{
    if (dev == NULL) {
        return -EINVAL;
    }

    dev->i2c  = i2c;
    dev->addr = addr;

    _set_pin(&dev->intpin1, GPIO_UNDEF);
    _set_pin(&dev->intpin2, GPIO_UNDEF);

    dev->sensor.intf            = BMI2_I2C_INTF;
    dev->sensor.read            = _bmi2_i2c_read;
    dev->sensor.write           = _bmi2_i2c_write;
    dev->sensor.delay_us        = _bmi2_delay_us;
    dev->sensor.intf_ptr        = dev;
    dev->sensor.read_write_len  = 8U;
    dev->sensor.config_file_ptr = NULL;

    int8_t st = bmi270_init(&dev->sensor);
    if (st != BMI2_OK) {
        return _bmi_status_to_rc(st);
    }

    struct bmi2_sens_config cfg = {0};
    st = bmi270_get_sensor_config(&cfg, 1, &dev->sensor);
    return _bmi_status_to_rc(st);
}

// apply default accel+gyro cfg, enable both sensors
int bmi270_config_default(bmi270_t *dev)
{
    if (dev == NULL) {
        return -EINVAL;
    }

    uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_GYRO };
    struct bmi2_sens_config cfg = {0};
    int8_t st;

    st = bmi2_get_sensor_config(&cfg, 1, &dev->sensor);
    if (st != BMI2_OK) {
        return _bmi_status_to_rc(st);
    }
    cfg.type = BMI2_ACCEL;
    cfg.cfg.acc.odr = BMI2_ACC_ODR_100HZ;
    cfg.cfg.acc.range = BMI2_ACC_RANGE_2G;
    cfg.cfg.acc.bwp = BMI2_ACC_OSR4_AVG1;
    cfg.cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

    st = bmi2_set_sensor_config(&cfg, 1, &dev->sensor);
    if (st != BMI2_OK) {
        return _bmi_status_to_rc(st);
    }

    st = bmi2_get_sensor_config(&cfg, 1, &dev->sensor);
    if (st != BMI2_OK) {
        return _bmi_status_to_rc(st);
    }
    cfg.type = BMI2_GYRO;
    cfg.cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
    cfg.cfg.gyr.range = BMI2_GYR_RANGE_2000;
    cfg.cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;
    cfg.cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;
    cfg.cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

    st = bmi2_set_sensor_config(&cfg, 1, &dev->sensor);
    if (st != BMI2_OK) {
        return _bmi_status_to_rc(st);
    }

    st = bmi2_sensor_enable(sens_list, 2, &dev->sensor);
    return _bmi_status_to_rc(st);
}

// read current accel+gyro data
int bmi270_read(bmi270_t *dev, bmi270_data_t *data)
{
    if ((dev == NULL) || (data == NULL)) {
        return -EINVAL;
    }

    int8_t st = bmi2_get_sensor_data(&data->sensor_data, &dev->sensor);
    return _bmi_status_to_rc(st);
}

// run BMI270 accel + gyro self tests
int bmi270_selftest(bmi270_t *dev)
{
    if (dev == NULL) {
        return -EINVAL;
    }

    int8_t st = bmi2_perform_accel_self_test(&dev->sensor);
    ztimer_sleep(ZTIMER_USEC, _SOFT_RESET_DELAY_US);

    if (st == BMI2_OK) {
        st = bmi2_do_gyro_st(&dev->sensor);
    }

    return _bmi_status_to_rc(st);
}

// cfg one GPIO as BMI270 int pin, apply BMI270 int-pin settings
int bmi270_config_int(bmi270_t *dev, struct bmi2_int_pin_config *intsettings,
                      gpio_t pin)
{
    if ((dev == NULL) || (intsettings == NULL)) {
        return -EINVAL;
    }

    if (dev->intpin1 == GPIO_UNDEF || dev->intpin1 == pin) {
        dev->intpin1 = pin;
    }
    else if (dev->intpin2 == GPIO_UNDEF || dev->intpin2 == pin) {
        dev->intpin2 = pin;
    }
    else {
        return -ENOSPC;
    }

    int8_t st = bmi2_set_int_pin_config(intsettings, &dev->sensor);
    return (st == BMI2_OK) ? 0 : -(int)st;
}

// disable and release any cfg int pins
void bmi270_free_pin(bmi270_t *dev)
{
    if (dev == NULL) {
        return;
    }

    dev->intpin1 = GPIO_UNDEF;
    dev->intpin2 = GPIO_UNDEF;
}
