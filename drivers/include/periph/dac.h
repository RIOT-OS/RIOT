/*
 * Copyright (C) 2014 Simon Brummer
 *               2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_dac DAC
 * @ingroup     drivers_periph
 * @brief       DAC peripheral driver interface
 *
 * Similar to the ADC driver interface (@ref drivers_periph_adc), the DAC
 * interface uses the concept of lines, corresponds to a tuple of a DAC device
 * and a DAC output channel.
 *
 * The DAC interface expects data to be served as a 16-bit unsigned integer,
 * independent of the actual resolution of the hardware device. It is up to the
 * DAC driver, to scale the given value to the maximal width that can be
 * handled. The device driver should, on the other hand, implement the DAC in a
 * way, that it will use the bit width, that comes closest to 16-bit.
 *
 * This kind of 'auto-scaling' is quite sufficient for this interface, as
 * standard DAC peripherals use a fixed conversion resolution internally anyway,
 * so that any particular bit-width configuration on this driver level would not
 * have much effect...
 *
 * @{
 * @file
 * @brief       DAC peripheral driver interface definition
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_DAC_H
#define PERIPH_DAC_H

#include <stdint.h>
#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define default DAC type identifier
 */
#ifndef HAVE_DAC_T
typedef unsigned int dac_t;
#endif

/**
 * @brief   Return codes used by the DAC driver interface
 */
enum {
    DAC_OK     = 0,
    DAC_NOLINE = -1
};

/**
 * @brief   Default DAC undefined value
 */
#ifndef DAC_UNDEF
#define DAC_UNDEF           (UINT_MAX)
#endif

/**
 * @brief   Default DAC access macro
 */
#ifndef DAC_LINE
#define DAC_LINE(x)         (x)
#endif

/**
 * @brief   Default number of onboard DAC channels
 *
 * It has to be overridden by board definitions.
 */
#ifndef DAC_NUMOF
#define DAC_NUMOF           (0U)
#endif

/**
 * @brief   Set device ID location in dac_t when using extensions
 */
#ifndef DAC_EXT_DEV_LOC
#define DAC_EXT_DEV_LOC     (8*sizeof(dac_t) - 8)
#endif

/**
 * @brief   Line numbers greater than DAC_EXT_THRESH use extensions
 */
#ifndef DAC_EXT_THRESH
#define DAC_EXT_THRESH      ((dac_t)(UINT_MAX) >> 1)
#endif

/**
 * @brief   Convert (device, line) tuple to #dac_t value
 */
#ifndef DAC_EXT_LINE
#define DAC_EXT_LINE(x, y) \
    (dac_t)(~DAC_EXT_THRESH | (x << DAC_EXT_DEV_LOC) | DAC_LINE(y))
#endif

/**
 * @brief   Low-level versions of the DAC functions provided by the CPU
 *
 * These are for cpu dac.c implementation and should not be called directly.
 * @{
 */
int8_t dac_init_cpu(dac_t line);
void dac_set_cpu(dac_t line, uint16_t value);
void dac_poweron_cpu(dac_t line);
void dac_poweroff_cpu(dac_t line);
unsigned int dac_channels_cpu(void);
/** @} */

#if MODULE_EXTEND_DAC || DOXYGEN
/**
 * @brief   Redirecting versions of the DAC functions
 *
 * These are for the extension interface and should not be called directly.
 * @{
 */
int8_t dac_init_redir(dac_t line);
void dac_set_redir(dac_t line, uint16_t value);
void dac_poweron_redir(dac_t line);
void dac_poweroff_redir(dac_t line);
unsigned int dac_channels_redir(dac_t line);
/** @} */
#endif /* MODULE_EXTEND_DAC || DOXYGEN */

/**
 * @brief   Initialize the given DAC line
 *
 * After initialization, the corresponding DAC line is active and its output is
 * set to 0.
 *
 * @param[in] line         DAC line to initialize
 *
 * @return  DAC_OK on success
 * @return  DAC_NOLINE on invalid DAC line
 */
static inline int8_t dac_init(dac_t line)
{
#ifdef MODULE_EXTEND_DAC
    if (line > DAC_EXT_THRESH) {
        return dac_init_redir(line);
    }
#endif
#ifdef MODULE_PERIPH_DAC
    return dac_init_cpu(line);
#endif
    return DAC_NOLINE;
}

/**
 * @brief   Write a value onto DAC Device on a given Channel
 *
 * The value is always given as 16-bit value and is internally scaled to the
 * actual resolution that the DAC unit provides (e.g. 12-bit). So to get the
 * maximum output voltage, this function has to be called with @p value set to
 * 65535 (UINT16_MAX).
 *
 * @param[in] line         DAC line to set
 * @param[in] value        value to set @p line to
 */
static inline void dac_set(dac_t line, uint16_t value)
{
#ifdef MODULE_EXTEND_DAC
    if (line > DAC_EXT_THRESH) {
        dac_set_redir(line, value);
        return;
    }
#endif
#ifdef MODULE_PERIPH_DAC
    dac_set_cpu(line, value);
#endif
}

/**
 * @brief   Enable the given DAC line
 *
 * @param[in] line          DAC line to power on
 */
static inline void dac_poweron(dac_t line)
{
#ifdef MODULE_EXTEND_DAC
    if (line > DAC_EXT_THRESH) {
        dac_poweron_redir(line);
        return;
    }
#endif
#ifdef MODULE_PERIPH_DAC
    dac_poweron_cpu(line);
#endif
}

/**
 * @brief   Disable the given DAC line
 *
 * @param[in] line          DAC line to power off
 */
static inline void dac_poweroff(dac_t line)
{
#ifdef MODULE_EXTEND_DAC
    if (line > DAC_EXT_THRESH) {
        dac_poweroff_redir(line);
        return;
    }
#endif
#ifdef MODULE_PERIPH_DAC
    dac_poweroff_cpu(line);
#endif
}

/**
 * @brief   Returns the number of channels of the device
 *
 * This function returns the number of channels of the device that provides
 * the channel identified by the line. If there is no device or no channel
 * that corresponds to the line, the function returns 0
 *
 * @param[in] line  identifies the device
 * @retval  the number of channels on success or 0 on error
 */
static inline unsigned int dac_channels(dac_t line)
{
#ifdef MODULE_EXTEND_DAC
    if (line > DAC_EXT_THRESH) {
        return dac_channels_redir(line);
    }
#endif
#ifdef MODULE_PERIPH_DAC
    return DAC_NUMOF;
#endif
    return 0;
}


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_DAC_H */
/** @} */
