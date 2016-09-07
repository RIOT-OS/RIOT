/*
* Copyright (C) 2016 University of California, Berkeley
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
 */

/**
 * @defgroup    drivers_mma7660 MMA7660 Accelerometer
 * @ingroup     drivers_sensors
 * @brief       Driver for the Freescale MMA7660 3-Axis accelerometer.
 *              This driver only implements basic functionality.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the MMA7660 accelerometer driver.
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 */

#ifndef MMA7660_H
#define MMA7660_H

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  i2c_t i2c;
  uint8_t addr;
} mma7660_t;

#define MODE_ACTIVE_SHIFT    0
#define MODE_AUTOWAKE_SHIFT  3
#define MODE_AUTOSLEEP_SHIFT 4
#define MODE_PRESCALE_SHIFT  5
#define MODE_INTERRUPT_DEFAULT 0x40 //Active low, push-pull

#define MMA7660_INTSOURCE_NONE        0x00
#define MMA7660_INTSOURCE_FB          0x01
#define MMA7660_INTSOURCE_UDLR        0x02
#define MMA7660_INTSOURCE_TAP         0x04
#define MMA7660_INTSOURCE_AUTOSLEEP   0x08
#define MMA7660_INTSOURCE_MEASURE     0x10
#define MMA7660_INTSOURCE_SHX         0x20
#define MMA7660_INTSOURCE_SHY         0x40
#define MMA7660_INTSOURCE_SHZ         0x80

#define MMA7660_SR_AMPD   0x00
#define MMA7660_SR_AM64   0x01
#define MMA7660_SR_AM32   0x02
#define MMA7660_SR_AM16   0x03
#define MMA7660_SR_AM8    0x04
#define MMA7660_SR_AM4    0x05
#define MMA7660_SR_AM2    0x06
#define MMA7660_SR_AM1    0x07
#define MMA7660_SR_AW32   0x00
#define MMA7660_SR_AW16   0x08
#define MMA7660_SR_AW8    0x10
#define MMA7660_SR_AW1    0x18
#define MMA7660_PDET_X 0x20
#define MMA7660_PDET_Y 0x40
#define MMA7660_PDET_Z 0x80

#define MMA7660_ADDR    0x4C

int mma7660_set_mode(mma7660_t *dev, uint8_t active,
  uint8_t autowake, uint8_t autosleep, uint8_t prescale);
int mma7660_read_tilt(mma7660_t *dev, uint8_t *res);
int mma7660_write_sleep_count(mma7660_t *dev, uint8_t sleep);
int mma7660_config_interrupts(mma7660_t *dev, uint8_t isource_flags);
int mma7660_config_samplerate(mma7660_t *dev, uint8_t amsr, uint8_t awsr, uint8_t filt);
int mma7660_config_pdet(mma7660_t *dev, uint8_t pdth, uint8_t enabled_axes);
int mma7660_config_pd(mma7660_t *dev, uint8_t pd) ;
int mma7660_init(mma7660_t *dev, i2c_t i2c, uint8_t address);
int mma7660_read(mma7660_t *dev, int8_t *x, int8_t *y, int8_t *z);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
