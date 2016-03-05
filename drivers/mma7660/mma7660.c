/*
 * Copyright (C) 2016 University of California, Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mma7660
 * @{
 *
 * @file
 * @brief       Driver for the Freescale MMA7660 accelerometer.
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"
#include <mma7660.h>
#include <mma7660_reg.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define I2C_SPEED                  I2C_SPEED_FAST

#define ALERT_MASK 0x40
#define SIGN_BIT 0x20
#define SIGN_EXTEND 0xC0
#define SR_FILT_SHIFT 5
#define PDET_AXIS_MASK 0xE0

int mma7660_set_mode(mma7660_t *dev, uint8_t active,
  uint8_t autowake, uint8_t autosleep, uint8_t prescale) {
    char reg;
    int rv;
    reg = (active << MODE_ACTIVE_SHIFT)       |
          (autowake << MODE_AUTOWAKE_SHIFT)   |
          (autosleep << MODE_AUTOSLEEP_SHIFT) |
          (prescale << MODE_PRESCALE_SHIFT)   |
          MODE_INTERRUPT_DEFAULT;
    i2c_acquire(dev->i2c);
    rv = i2c_write_reg(dev->i2c, dev->addr, MMA7660_MODE, reg);
    i2c_release(dev->i2c);
    if (rv != 1) {
        return -1;
    }
    return 0;
}
int mma7660_read_tilt(mma7660_t *dev, uint8_t *res) {
    int rv;
    i2c_acquire(dev->i2c);
    rv = i2c_read_reg(dev->i2c, dev->addr, MMA7660_TILT, (char*)res);
    i2c_release(dev->i2c);
    if (rv != 1) {
        return -1;
    }
    return 0;
}
int mma7660_write_sleep_count(mma7660_t *dev, uint8_t sleep) {
    int rv;
    i2c_acquire(dev->i2c);
    rv = i2c_write_reg(dev->i2c, dev->addr, MMA7660_SPCNT, sleep);
    i2c_release(dev->i2c);
    if (rv != 1) {
        return -1;
    }
    return 0;
}
int mma7660_config_interrupts(mma7660_t *dev, uint8_t isource_flags) {
    int rv;
    i2c_acquire(dev->i2c);
    rv = i2c_write_reg(dev->i2c, dev->addr, MMA7660_INTSU, isource_flags);
    i2c_release(dev->i2c);
    if (rv != 1) {
        return -1;
    }
    return 0;
}
int mma7660_config_samplerate(mma7660_t *dev, uint8_t amsr, uint8_t awsr, uint8_t filt) {
    int rv;
    char ch = amsr | awsr | (filt << SR_FILT_SHIFT);
    i2c_acquire(dev->i2c);
    rv = i2c_write_reg(dev->i2c, dev->addr, MMA7660_SR, ch);
    i2c_release(dev->i2c);
    if (rv != 1) {
        return -1;
    }
    return 0;
}
int mma7660_config_pdet(mma7660_t *dev, uint8_t pdth, uint8_t enabled_axes) {
    int rv;
    char ch = pdth | ((~enabled_axes) & PDET_AXIS_MASK);
    i2c_acquire(dev->i2c);
    rv = i2c_write_reg(dev->i2c, dev->addr, MMA7660_PDET, ch);
    i2c_release(dev->i2c);
    if (rv != 1) {
        return -1;
    }
    return 0;
}
int mma7660_config_pd(mma7660_t *dev, uint8_t pd) {
    int rv;
    i2c_acquire(dev->i2c);
    rv = i2c_write_reg(dev->i2c, dev->addr, MMA7660_PD, (char)pd);
    i2c_release(dev->i2c);
    if (rv != 1) {
        return -1;
    }
    return 0;
}
int mma7660_init(mma7660_t *dev, i2c_t i2c, uint8_t address) {

    // write device descriptor
    dev->i2c = i2c;
    dev->addr = address;

    i2c_acquire(dev->i2c);
    /* initialize the I2C bus */
    if (i2c_init_master(i2c, I2C_SPEED) < 0) {
        i2c_release(dev->i2c);
        return -1;
    }
    i2c_release(dev->i2c);

    if (mma7660_set_mode(dev, 0, 0, 0, 0)) return -1;
    if (mma7660_config_interrupts(dev, MMA7660_INTSOURCE_NONE)) return -1;

    return 0;
}
int mma7660_read(mma7660_t *dev, int8_t *x, int8_t *y, int8_t *z)
{
  int retries = 6;
  char t;
  i2c_acquire(dev->i2c);
  while (retries > 0) {
    if (i2c_read_reg(dev->i2c, dev->addr, MMA7660_XOUT, &t) != 1) {
      i2c_release(dev->i2c);
      return -1;
    }
    if ((t & ALERT_MASK) == 0)
      break;
    retries--;
  }
  if (t & SIGN_BIT) t |= SIGN_EXTEND;
  *x = (int8_t)t;
  while (retries > 0) {
    if (i2c_read_reg(dev->i2c, dev->addr, MMA7660_YOUT, &t) != 1) {
      i2c_release(dev->i2c);
      return -1;
    }
    if ((t & ALERT_MASK) == 0)
      break;
    retries--;
  }
  if (t & SIGN_BIT) t |= SIGN_EXTEND;
  *y = (int8_t)t;
  while (retries > 0) {
    if (i2c_read_reg(dev->i2c, dev->addr, MMA7660_ZOUT, &t) != 1) {
      i2c_release(dev->i2c);
      return -1;
    }
    if ((t & ALERT_MASK) == 0)
      break;
    retries--;
  }
  if (t & SIGN_BIT) t |= SIGN_EXTEND;
  *z = (int8_t)t;
  i2c_release(dev->i2c);
  if (retries > 0)
    return 0;
  return -2;
}
