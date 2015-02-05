/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"
#include "vtimer.h"
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @ingroup     cpu_kinetis_common_spi
 *
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

#if SPI_0_EN
#ifdef SPI_0_PORT
#define SPI_0_SCK_PORT          SPI_0_PORT
#define SPI_0_SOUT_PORT         SPI_0_PORT
#define SPI_0_SIN_PORT          SPI_0_PORT
#define SPI_0_PCS0_PORT         SPI_0_PORT
#endif

#ifdef SPI_0_PORT_CLKEN
#define SPI_0_SCK_PORT_CLKEN    SPI_0_PORT_CLKEN
#define SPI_0_SOUT_PORT_CLKEN   SPI_0_PORT_CLKEN
#define SPI_0_SIN_PORT_CLKEN    SPI_0_PORT_CLKEN
#define SPI_0_PCS0_PORT_CLKEN   SPI_0_PORT_CLKEN
#endif

#ifdef SPI_0_AF
#define SPI_0_SCK_AF            SPI_0_AF
#define SPI_0_SOUT_AF           SPI_0_AF
#define SPI_0_SIN_AF            SPI_0_AF
#define SPI_0_PCS0_AF           SPI_0_AF
#endif

#ifndef SPI_0_TCSC_FREQ
#define SPI_0_TCSC_FREQ    (0)
#endif

#ifndef SPI_0_TASC_FREQ
#define SPI_0_TASC_FREQ    (0)
#endif

#ifndef SPI_0_TDT_FREQ
#define SPI_0_TDT_FREQ     (0)
#endif
#endif /* SPI_0_EN */

#if SPI_1_EN
#ifdef SPI_1_PORT
#define SPI_1_SCK_PORT          SPI_1_PORT
#define SPI_1_SOUT_PORT         SPI_1_PORT
#define SPI_1_SIN_PORT          SPI_1_PORT
#define SPI_1_PCS1_PORT         SPI_1_PORT
#endif

#ifdef SPI_1_PORT_CLKEN
#define SPI_1_SCK_PORT_CLKEN    SPI_1_PORT_CLKEN
#define SPI_1_SOUT_PORT_CLKEN   SPI_1_PORT_CLKEN
#define SPI_1_SIN_PORT_CLKEN    SPI_1_PORT_CLKEN
#define SPI_1_PCS1_PORT_CLKEN   SPI_1_PORT_CLKEN
#endif

#ifdef SPI_1_AF
#define SPI_1_SCK_AF            SPI_1_AF
#define SPI_1_SOUT_AF           SPI_1_AF
#define SPI_1_SIN_AF            SPI_1_AF
#define SPI_1_PCS1_AF           SPI_1_AF
#endif

#ifndef SPI_1_TCSC_FREQ
#define SPI_1_TCSC_FREQ    (0)
#endif

#ifndef SPI_1_TASC_FREQ
#define SPI_1_TASC_FREQ    (0)
#endif

#ifndef SPI_1_TDT_FREQ
#define SPI_1_TDT_FREQ     (0)
#endif
#endif /* SPI_1_EN */

#if SPI_2_EN
#ifdef SPI_2_PORT
#define SPI_2_SCK_PORT          SPI_2_PORT
#define SPI_2_SOUT_PORT         SPI_2_PORT
#define SPI_2_SIN_PORT          SPI_2_PORT
#define SPI_2_PCS2_PORT         SPI_2_PORT
#endif

#ifdef SPI_2_PORT_CLKEN
#define SPI_2_SCK_PORT_CLKEN    SPI_2_PORT_CLKEN
#define SPI_2_SOUT_PORT_CLKEN   SPI_2_PORT_CLKEN
#define SPI_2_SIN_PORT_CLKEN    SPI_2_PORT_CLKEN
#define SPI_2_PCS2_PORT_CLKEN   SPI_2_PORT_CLKEN
#endif

#ifdef SPI_2_AF
#define SPI_2_SCK_AF            SPI_2_AF
#define SPI_2_SOUT_AF           SPI_2_AF
#define SPI_2_SIN_AF            SPI_2_AF
#define SPI_2_PCS2_AF           SPI_2_AF
#endif

#ifndef SPI_2_TCSC_FREQ
#define SPI_2_TCSC_FREQ    (0)
#endif

#ifndef SPI_2_TASC_FREQ
#define SPI_2_TASC_FREQ    (0)
#endif

#ifndef SPI_2_TDT_FREQ
#define SPI_2_TDT_FREQ     (0)
#endif
#endif /* SPI_2_EN */

#if SPI_3_EN
#ifdef SPI_3_PORT
#define SPI_3_SCK_PORT          SPI_3_PORT
#define SPI_3_SOUT_PORT         SPI_3_PORT
#define SPI_3_SIN_PORT          SPI_3_PORT
#define SPI_3_PCS3_PORT         SPI_3_PORT
#endif

#ifdef SPI_3_PORT_CLKEN
#define SPI_3_SCK_PORT_CLKEN    SPI_3_PORT_CLKEN
#define SPI_3_SOUT_PORT_CLKEN   SPI_3_PORT_CLKEN
#define SPI_3_SIN_PORT_CLKEN    SPI_3_PORT_CLKEN
#define SPI_3_PCS3_PORT_CLKEN   SPI_3_PORT_CLKEN
#endif

#ifdef SPI_3_AF
#define SPI_3_SCK_AF            SPI_3_AF
#define SPI_3_SOUT_AF           SPI_3_AF
#define SPI_3_SIN_AF            SPI_3_AF
#define SPI_3_PCS3_AF           SPI_3_AF
#endif

#ifndef SPI_3_TCSC_FREQ
#define SPI_3_TCSC_FREQ    (0)
#endif

#ifndef SPI_3_TASC_FREQ
#define SPI_3_TASC_FREQ    (0)
#endif

#ifndef SPI_3_TDT_FREQ
#define SPI_3_TDT_FREQ     (0)
#endif
#endif /* SPI_3_EN */

#if SPI_4_EN
#ifdef SPI_4_PORT
#define SPI_4_SCK_PORT          SPI_4_PORT
#define SPI_4_SOUT_PORT         SPI_4_PORT
#define SPI_4_SIN_PORT          SPI_4_PORT
#define SPI_4_PCS4_PORT         SPI_4_PORT
#endif

#ifdef SPI_4_PORT_CLKEN
#define SPI_4_SCK_PORT_CLKEN    SPI_4_PORT_CLKEN
#define SPI_4_SOUT_PORT_CLKEN   SPI_4_PORT_CLKEN
#define SPI_4_SIN_PORT_CLKEN    SPI_4_PORT_CLKEN
#define SPI_4_PCS4_PORT_CLKEN   SPI_4_PORT_CLKEN
#endif

#ifdef SPI_4_AF
#define SPI_4_SCK_AF            SPI_4_AF
#define SPI_4_SOUT_AF           SPI_4_AF
#define SPI_4_SIN_AF            SPI_4_AF
#define SPI_4_PCS4_AF           SPI_4_AF
#endif

#ifndef SPI_4_TCSC_FREQ
#define SPI_4_TCSC_FREQ    (0)
#endif

#ifndef SPI_4_TASC_FREQ
#define SPI_4_TASC_FREQ    (0)
#endif

#ifndef SPI_4_TDT_FREQ
#define SPI_4_TDT_FREQ     (0)
#endif
#endif /* SPI_4_EN */

#if SPI_5_EN
#ifdef SPI_5_PORT
#define SPI_5_SCK_PORT          SPI_5_PORT
#define SPI_5_SOUT_PORT         SPI_5_PORT
#define SPI_5_SIN_PORT          SPI_5_PORT
#define SPI_5_PCS5_PORT         SPI_5_PORT
#endif

#ifdef SPI_5_PORT_CLKEN
#define SPI_5_SCK_PORT_CLKEN    SPI_5_PORT_CLKEN
#define SPI_5_SOUT_PORT_CLKEN   SPI_5_PORT_CLKEN
#define SPI_5_SIN_PORT_CLKEN    SPI_5_PORT_CLKEN
#define SPI_5_PCS5_PORT_CLKEN   SPI_5_PORT_CLKEN
#endif

#ifdef SPI_5_AF
#define SPI_5_SCK_AF            SPI_5_AF
#define SPI_5_SOUT_AF           SPI_5_AF
#define SPI_5_SIN_AF            SPI_5_AF
#define SPI_5_PCS5_AF           SPI_5_AF
#endif

#ifndef SPI_5_TCSC_FREQ
#define SPI_5_TCSC_FREQ    (0)
#endif

#ifndef SPI_5_TASC_FREQ
#define SPI_5_TASC_FREQ    (0)
#endif

#ifndef SPI_5_TDT_FREQ
#define SPI_5_TDT_FREQ     (0)
#endif
#endif /* SPI_5_EN */

#if SPI_6_EN
#ifdef SPI_6_PORT
#define SPI_6_SCK_PORT          SPI_6_PORT
#define SPI_6_SOUT_PORT         SPI_6_PORT
#define SPI_6_SIN_PORT          SPI_6_PORT
#define SPI_6_PCS6_PORT         SPI_6_PORT
#endif

#ifdef SPI_6_PORT_CLKEN
#define SPI_6_SCK_PORT_CLKEN    SPI_6_PORT_CLKEN
#define SPI_6_SOUT_PORT_CLKEN   SPI_6_PORT_CLKEN
#define SPI_6_SIN_PORT_CLKEN    SPI_6_PORT_CLKEN
#define SPI_6_PCS6_PORT_CLKEN   SPI_6_PORT_CLKEN
#endif

#ifdef SPI_6_AF
#define SPI_6_SCK_AF            SPI_6_AF
#define SPI_6_SOUT_AF           SPI_6_AF
#define SPI_6_SIN_AF            SPI_6_AF
#define SPI_6_PCS6_AF           SPI_6_AF
#endif

#ifndef SPI_6_TCSC_FREQ
#define SPI_6_TCSC_FREQ    (0)
#endif

#ifndef SPI_6_TASC_FREQ
#define SPI_6_TASC_FREQ    (0)
#endif

#ifndef SPI_6_TDT_FREQ
#define SPI_6_TDT_FREQ     (0)
#endif
#endif /* SPI_6_EN */

#if SPI_7_EN
#ifdef SPI_7_PORT
#define SPI_7_SCK_PORT          SPI_7_PORT
#define SPI_7_SOUT_PORT         SPI_7_PORT
#define SPI_7_SIN_PORT          SPI_7_PORT
#define SPI_7_PCS7_PORT         SPI_7_PORT
#endif

#ifdef SPI_7_PORT_CLKEN
#define SPI_7_SCK_PORT_CLKEN    SPI_7_PORT_CLKEN
#define SPI_7_SOUT_PORT_CLKEN   SPI_7_PORT_CLKEN
#define SPI_7_SIN_PORT_CLKEN    SPI_7_PORT_CLKEN
#define SPI_7_PCS7_PORT_CLKEN   SPI_7_PORT_CLKEN
#endif

#ifdef SPI_7_AF
#define SPI_7_SCK_AF            SPI_7_AF
#define SPI_7_SOUT_AF           SPI_7_AF
#define SPI_7_SIN_AF            SPI_7_AF
#define SPI_7_PCS7_AF           SPI_7_AF
#endif

#ifndef SPI_7_TCSC_FREQ
#define SPI_7_TCSC_FREQ    (0)
#endif

#ifndef SPI_7_TASC_FREQ
#define SPI_7_TASC_FREQ    (0)
#endif

#ifndef SPI_7_TDT_FREQ
#define SPI_7_TDT_FREQ     (0)
#endif
#endif /* SPI_7_EN */

/**
 * @brief Array holding one pre-initialized mutex for each hardware SPI device
 */
/* TODO: Avoid adding duplicate entries with the same index. GCC warns about it
 * but it works. It does look strange in the preprocessed output, however.
 *
 * The warning message is:
 * warning: initialized field overwritten [-Woverride-init]
 * warning: (near initialization for ‘locks[0]’) [-Woverride-init]
 *
 * Example of preprocessed source:
 * static mutex_t locks[] = {
 *     [0] = { 0, { ((void *)0) } },
 *     [1] = { 0, { ((void *)0) } },
 *     [0] = { 0, { ((void *)0) } }, // index [0] is given (the same, again) initial value.
 * };
 */

static mutex_t locks[] =  {
#if SPI_0_EN
    [SPI_0_INDEX] = MUTEX_INIT,
#endif
#if SPI_1_EN
    [SPI_1_INDEX] = MUTEX_INIT,
#endif
#if SPI_2_EN
    [SPI_2_INDEX] = MUTEX_INIT,
#endif
#if SPI_3_EN
    [SPI_3_INDEX] = MUTEX_INIT,
#endif
#if SPI_4_EN
    [SPI_4_INDEX] = MUTEX_INIT,
#endif
#if SPI_5_EN
    [SPI_5_INDEX] = MUTEX_INIT,
#endif
#if SPI_6_EN
    [SPI_6_INDEX] = MUTEX_INIT,
#endif
#if SPI_7_EN
    [SPI_7_INDEX] = MUTEX_INIT,
#endif
};

/**
 * @brief Array of pointers that map RIOT SPI device number to actual hardware
 * module lock.
 *
 * Every RIOT device shares the bus lock with any other bus devices for the same
 * hardware module. This allows us to let two RIOT devices point to the same
 * hardware but using different CTAR registers for timing information.
 */
static mutex_t *locks_map[] = {
#if SPI_0_EN
    [SPI_0] = &locks[SPI_0_INDEX],
#endif
#if SPI_1_EN
    [SPI_1] = &locks[SPI_1_INDEX],
#endif
#if SPI_2_EN
    [SPI_2] = &locks[SPI_2_INDEX],
#endif
#if SPI_3_EN
    [SPI_3] = &locks[SPI_3_INDEX],
#endif
#if SPI_4_EN
    [SPI_4] = &locks[SPI_4_INDEX],
#endif
#if SPI_5_EN
    [SPI_5] = &locks[SPI_5_INDEX],
#endif
#if SPI_6_EN
    [SPI_6] = &locks[SPI_6_INDEX],
#endif
#if SPI_7_EN
    [SPI_7] = &locks[SPI_7_INDEX],
#endif
};

typedef struct {
    char(*cb)(char data);
} spi_state_t;

static inline void irq_handler_transfer(SPI_Type *spi, spi_t dev);

static spi_state_t spi_config[SPI_NUMOF];

#define SPI_IDLE_DATA (0xff)

/**
 * @brief Helper function for finding optimal baud rate scalers.
 *
 * Find the prescaler and scaler settings that will yield a clock frequency
 * as close as possible (but not above) the target frequency, given the module
 * runs at module_clock Hz.
 *
 * Hardware properties (Baud rate configuration):
 * Possible prescalers: 2, 3, 5, 7
 * Possible scalers: 2, 4, 6 (sic!), 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
 *
 * SCK baud rate = (f_BUS/PBR) x [(1+DBR)/BR]
 *
 * where PBR is the prescaler, BR is the scaler, DBR is the Double BaudRate bit.
 *
 * @note We are not using the DBR bit because it may affect the SCK duty cycle.
 *
 * @param module_clock Module clock frequency (e.g. F_BUS)
 * @param target_clock Desired baud rate
 * @param closest_prescaler pointer where to write the optimal prescaler index.
 * @param closest_scaler pointer where to write the optimal scaler index.
 *
 * @return The actual achieved frequency on success
 * @return Less than 0 on error.
 */
static int find_closest_baudrate_scalers(const uint32_t module_clock, const uint32_t target_clock,
        uint8_t *closest_prescaler, uint8_t *closest_scaler)
{
    uint8_t i;
    uint8_t k;
    int freq;
    static const uint8_t num_scalers = 16;
    static const uint8_t num_prescalers = 4;
    static const int br_scalers[16] = {
        2,     4,     6,     8,    16,    32,    64,   128,
        256,   512,  1024,  2048,  4096,  8192, 16384, 32768
    };
    static const int br_prescalers[4] = {2, 3, 5, 7};

    int closest_frequency = -1;

    /* Test all combinations until we arrive close to the target clock */
    for (i = 0; i < num_prescalers; ++i) {
        for (k = 0; k < num_scalers; ++k) {
            freq = module_clock / (br_scalers[k] * br_prescalers[i]);

            if (freq <= target_clock) {
                /* Found closest lower frequency at this prescaler setting,
                 * compare to the best result */
                if (closest_frequency < freq) {
                    closest_frequency = freq;
                    *closest_scaler = k;
                    *closest_prescaler = i;
                }

                break;
            }
        }
    }

    if (closest_frequency < 0) {
        /* Error, no solution found, this line is never reachable with current
         * hardware settings unless a _very_ low target clock is requested.
         * (scaler_max * prescaler_max) = 229376 => target_min@100MHz = 435 Hz*/
        return -1;
    }

    return closest_frequency;
}

/**
 * @brief Helper function for finding optimal delay scalers.
 *
 * Find the prescaler and scaler settings that will yield a delay timing
 * as close as possible (but not shorter than) the target delay, given the
 * module runs at module_clock Hz.
 *
 * Hardware properties (delay configuration):
 * Possible prescalers: 1, 3, 5, 7
 * Possible scalers: 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536
 *
 * delay = (1/f_BUS) x prescaler x scaler
 *
 * Because we want to do this using only integers, the target_freq parameter is
 * the reciprocal of the delay time.
 *
 * @param module_clock Module clock frequency (e.g. F_BUS)
 * @param target_freq Reciprocal (i.e. 1/t [Hz], frequency) of the desired delay time.
 * @param closest_prescaler pointer where to write the optimal prescaler index.
 * @param closest_scaler pointer where to write the optimal scaler index.
 *
 * @return The actual achieved frequency on success
 * @return Less than 0 on error.
 */
static int find_closest_delay_scalers(const uint32_t module_clock, const uint32_t target_freq,
                                      uint8_t *closest_prescaler, uint8_t *closest_scaler)
{
    uint8_t i;
    uint8_t k;
    int freq;
    int prescaler;
    int scaler;
    static const uint8_t num_scalers = 16;
    static const uint8_t num_prescalers = 4;

    int closest_frequency = -1;

    /* Test all combinations until we arrive close to the target clock */
    for (i = 0; i < num_prescalers; ++i) {
        for (k = 0; k < num_scalers; ++k) {
            prescaler = (i * 2) + 1;
            scaler = (1 << (k + 1)); /* 2^(k+1) */
            freq = module_clock / (prescaler * scaler);

            if (freq <= target_freq) {
                /* Found closest lower frequency at this prescaler setting,
                 * compare to the best result */
                if (closest_frequency < freq) {
                    closest_frequency = freq;
                    *closest_scaler = k;
                    *closest_prescaler = i;
                }

                break;
            }
        }
    }

    if (closest_frequency < 0) {
        /* Error, no solution found, this line is never reachable with current
         * hardware settings unless a _very_ low target clock is requested.
         * (scaler_max * prescaler_max) = 458752 */
        return -1;
    }

    return closest_frequency;
}

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    SPI_Type *spi_dev;
    uint8_t br_prescaler = 0xff;
    uint8_t br_scaler = 0xff;
    uint8_t prescaler_tmp = 0xff;
    uint8_t scaler_tmp = 0xff;
    uint32_t ctas = 0;
    uint32_t ctar = 0;
    uint32_t br_desired;
    uint32_t module_clock;
    uint32_t tcsc_freq;
    uint32_t tasc_freq;
    uint32_t tdt_freq;

    switch (speed) {
        case SPI_SPEED_100KHZ:
            br_desired = 100000;
            break;

        case SPI_SPEED_400KHZ:
            br_desired = 400000;
            break;

        case SPI_SPEED_1MHZ:
            br_desired = 1000000;
            break;

        case SPI_SPEED_5MHZ:
            br_desired = 5000000;
            break;

        case SPI_SPEED_10MHZ:
            br_desired = 10000000;
            break;

        default:
            return -2;
    }

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            module_clock = SPI_0_FREQ;
            tcsc_freq = SPI_0_TCSC_FREQ;
            tasc_freq = SPI_0_TASC_FREQ;
            tdt_freq = SPI_0_TDT_FREQ;
            ctas = SPI_0_CTAS;
            /* enable clocks */
            SPI_0_CLKEN();
            SPI_0_SCK_PORT_CLKEN();
            SPI_0_SOUT_PORT_CLKEN();
            SPI_0_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_0_SCK_PORT->PCR[SPI_0_SCK_PIN] = PORT_PCR_MUX(SPI_0_SCK_AF);
            SPI_0_SOUT_PORT->PCR[SPI_0_SOUT_PIN] = PORT_PCR_MUX(SPI_0_SOUT_AF);
            SPI_0_SIN_PORT->PCR[SPI_0_SIN_PIN] = PORT_PCR_MUX(SPI_0_SIN_AF);
            break;
#endif /* SPI_0_EN */

#if SPI_1_EN

        case SPI_1:
            spi_dev = SPI_1_DEV;
            module_clock = SPI_1_FREQ;
            tcsc_freq = SPI_1_TCSC_FREQ;
            tasc_freq = SPI_1_TASC_FREQ;
            tdt_freq = SPI_1_TDT_FREQ;
            ctas = SPI_1_CTAS;
            /* enable clocks */
            SPI_1_CLKEN();
            SPI_1_SCK_PORT_CLKEN();
            SPI_1_SOUT_PORT_CLKEN();
            SPI_1_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_1_SCK_PORT->PCR[SPI_1_SCK_PIN] = PORT_PCR_MUX(SPI_1_SCK_AF);
            SPI_1_SOUT_PORT->PCR[SPI_1_SOUT_PIN] = PORT_PCR_MUX(SPI_1_SOUT_AF);
            SPI_1_SIN_PORT->PCR[SPI_1_SIN_PIN] = PORT_PCR_MUX(SPI_1_SIN_AF);
            break;
#endif /* SPI_1_EN */

#if SPI_2_EN

        case SPI_2:
            spi_dev = SPI_2_DEV;
            module_clock = SPI_2_FREQ;
            tcsc_freq = SPI_2_TCSC_FREQ;
            tasc_freq = SPI_2_TASC_FREQ;
            tdt_freq = SPI_2_TDT_FREQ;
            ctas = SPI_2_CTAS;
            /* enable clocks */
            SPI_2_CLKEN();
            SPI_2_SCK_PORT_CLKEN();
            SPI_2_SOUT_PORT_CLKEN();
            SPI_2_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_2_SCK_PORT->PCR[SPI_2_SCK_PIN] = PORT_PCR_MUX(SPI_2_SCK_AF);
            SPI_2_SOUT_PORT->PCR[SPI_2_SOUT_PIN] = PORT_PCR_MUX(SPI_2_SOUT_AF);
            SPI_2_SIN_PORT->PCR[SPI_2_SIN_PIN] = PORT_PCR_MUX(SPI_2_SIN_AF);
            break;
#endif /* SPI_2_EN */

#if SPI_3_EN

        case SPI_3:
            spi_dev = SPI_3_DEV;
            module_clock = SPI_3_FREQ;
            tcsc_freq = SPI_3_TCSC_FREQ;
            tasc_freq = SPI_3_TASC_FREQ;
            tdt_freq = SPI_3_TDT_FREQ;
            ctas = SPI_3_CTAS;
            /* enable clocks */
            SPI_3_CLKEN();
            SPI_3_SCK_PORT_CLKEN();
            SPI_3_SOUT_PORT_CLKEN();
            SPI_3_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_3_SCK_PORT->PCR[SPI_3_SCK_PIN] = PORT_PCR_MUX(SPI_3_SCK_AF);
            SPI_3_SOUT_PORT->PCR[SPI_3_SOUT_PIN] = PORT_PCR_MUX(SPI_3_SOUT_AF);
            SPI_3_SIN_PORT->PCR[SPI_3_SIN_PIN] = PORT_PCR_MUX(SPI_3_SIN_AF);
            break;
#endif /* SPI_3_EN */

#if SPI_4_EN

        case SPI_4:
            spi_dev = SPI_4_DEV;
            module_clock = SPI_4_FREQ;
            tcsc_freq = SPI_4_TCSC_FREQ;
            tasc_freq = SPI_4_TASC_FREQ;
            tdt_freq = SPI_4_TDT_FREQ;
            ctas = SPI_4_CTAS;
            /* enable clocks */
            SPI_4_CLKEN();
            SPI_4_SCK_PORT_CLKEN();
            SPI_4_SOUT_PORT_CLKEN();
            SPI_4_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_4_SCK_PORT->PCR[SPI_4_SCK_PIN] = PORT_PCR_MUX(SPI_4_SCK_AF);
            SPI_4_SOUT_PORT->PCR[SPI_4_SOUT_PIN] = PORT_PCR_MUX(SPI_4_SOUT_AF);
            SPI_4_SIN_PORT->PCR[SPI_4_SIN_PIN] = PORT_PCR_MUX(SPI_4_SIN_AF);
            break;
#endif /* SPI_4_EN */

#if SPI_5_EN

        case SPI_5:
            spi_dev = SPI_5_DEV;
            module_clock = SPI_5_FREQ;
            tcsc_freq = SPI_5_TCSC_FREQ;
            tasc_freq = SPI_5_TASC_FREQ;
            tdt_freq = SPI_5_TDT_FREQ;
            ctas = SPI_5_CTAS;
            /* enable clocks */
            SPI_5_CLKEN();
            SPI_5_SCK_PORT_CLKEN();
            SPI_5_SOUT_PORT_CLKEN();
            SPI_5_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_5_SCK_PORT->PCR[SPI_5_SCK_PIN] = PORT_PCR_MUX(SPI_5_SCK_AF);
            SPI_5_SOUT_PORT->PCR[SPI_5_SOUT_PIN] = PORT_PCR_MUX(SPI_5_SOUT_AF);
            SPI_5_SIN_PORT->PCR[SPI_5_SIN_PIN] = PORT_PCR_MUX(SPI_5_SIN_AF);
            break;
#endif /* SPI_5_EN */

#if SPI_6_EN

        case SPI_6:
            spi_dev = SPI_6_DEV;
            module_clock = SPI_6_FREQ;
            tcsc_freq = SPI_6_TCSC_FREQ;
            tasc_freq = SPI_6_TASC_FREQ;
            tdt_freq = SPI_6_TDT_FREQ;
            ctas = SPI_6_CTAS;
            /* enable clocks */
            SPI_6_CLKEN();
            SPI_6_SCK_PORT_CLKEN();
            SPI_6_SOUT_PORT_CLKEN();
            SPI_6_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_6_SCK_PORT->PCR[SPI_6_SCK_PIN] = PORT_PCR_MUX(SPI_6_SCK_AF);
            SPI_6_SOUT_PORT->PCR[SPI_6_SOUT_PIN] = PORT_PCR_MUX(SPI_6_SOUT_AF);
            SPI_6_SIN_PORT->PCR[SPI_6_SIN_PIN] = PORT_PCR_MUX(SPI_6_SIN_AF);
            break;
#endif /* SPI_6_EN */

#if SPI_7_EN

        case SPI_7:
            spi_dev = SPI_7_DEV;
            module_clock = SPI_7_FREQ;
            tcsc_freq = SPI_7_TCSC_FREQ;
            tasc_freq = SPI_7_TASC_FREQ;
            tdt_freq = SPI_7_TDT_FREQ;
            ctas = SPI_7_CTAS;
            /* enable clocks */
            SPI_7_CLKEN();
            SPI_7_SCK_PORT_CLKEN();
            SPI_7_SOUT_PORT_CLKEN();
            SPI_7_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_7_SCK_PORT->PCR[SPI_7_SCK_PIN] = PORT_PCR_MUX(SPI_7_SCK_AF);
            SPI_7_SOUT_PORT->PCR[SPI_7_SOUT_PIN] = PORT_PCR_MUX(SPI_7_SOUT_AF);
            SPI_7_SIN_PORT->PCR[SPI_7_SIN_PIN] = PORT_PCR_MUX(SPI_7_SIN_AF);
            break;
#endif /* SPI_7_EN */

        default:
            return -1;
    }

    /* Find baud rate scaler and prescaler settings */
    if (find_closest_baudrate_scalers(module_clock, br_desired,
                                      &br_prescaler, &br_scaler) < 0) {
        /* Desired baud rate is too low to be reachable at current module clock frequency. */
        return -2;
    }

    ctar |= SPI_CTAR_PBR(br_prescaler) | SPI_CTAR_BR(br_scaler);

    /* Find the other delay divisors */
    /* tCSC */
    if (tcsc_freq == 0) {
        /* Default to same as baud rate if set to zero. */
        tcsc_freq = br_desired;
    }

    if (find_closest_delay_scalers(module_clock, tcsc_freq,
                                   &prescaler_tmp, &scaler_tmp) < 0) {
        /* failed to find a solution */
        return -2;
    }

    ctar |= SPI_CTAR_PCSSCK(prescaler_tmp) | SPI_CTAR_CSSCK(scaler_tmp);

    /* tASC */
    if (tasc_freq == 0) {
        /* Default to same as baud rate if set to zero. */
        tasc_freq = br_desired;
    }

    if (find_closest_delay_scalers(module_clock, tasc_freq,
                                   &prescaler_tmp, &scaler_tmp) < 0) {
        /* failed to find a solution */
        return -2;
    }

    ctar |= SPI_CTAR_PASC(prescaler_tmp) | SPI_CTAR_ASC(scaler_tmp);

    /* tDT */
    if (tdt_freq == 0) {
        /* Default to same as baud rate if set to zero. */
        tdt_freq = br_desired;
    }

    if (find_closest_delay_scalers(module_clock, tdt_freq,
                                   &prescaler_tmp, &scaler_tmp) < 0) {
        /* failed to find a solution */
        return -2;
    }

    ctar |= SPI_CTAR_PDT(prescaler_tmp) | SPI_CTAR_DT(scaler_tmp);


    /* Set clock polarity and phase. */
    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            break;

        case SPI_CONF_SECOND_RISING:
            ctar |= SPI_CTAR_CPHA_MASK;
            break;

        case SPI_CONF_FIRST_FALLING:
            ctar |= SPI_CTAR_CPOL_MASK;
            break;

        case SPI_CONF_SECOND_FALLING:
            ctar |= SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK;
            break;

        default:
            return -2;
    }

    /* Update CTAR register with new timing settings, 8-bit frame size. */
    spi_dev->CTAR[ctas] = SPI_CTAR_FMSZ(7) | ctar;

    /* enable SPI */
    spi_dev->MCR = SPI_MCR_MSTR_MASK
                   | SPI_MCR_DOZE_MASK
                   | SPI_MCR_CLR_TXF_MASK
                   | SPI_MCR_CLR_RXF_MASK;

    spi_dev->RSER = (uint32_t)0;

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char(*cb)(char data))
{
    SPI_Type *spi_dev;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            /* enable clocks */
            SPI_0_CLKEN();
            SPI_0_PCS0_PORT_CLKEN();
            SPI_0_SCK_PORT_CLKEN();
            SPI_0_SOUT_PORT_CLKEN();
            SPI_0_SIN_PORT_CLKEN();
            /* Set PORT to AF mode */
            SPI_0_PCS0_PORT->PCR[SPI_0_PCS0_PIN] = PORT_PCR_MUX(SPI_0_PCS0_AF);
            SPI_0_SCK_PORT->PCR[SPI_0_SCK_PIN] = PORT_PCR_MUX(SPI_0_SCK_AF);
            SPI_0_SOUT_PORT->PCR[SPI_0_SOUT_PIN] = PORT_PCR_MUX(SPI_0_SOUT_AF);
            SPI_0_SIN_PORT->PCR[SPI_0_SIN_PIN] = PORT_PCR_MUX(SPI_0_SIN_AF);
            break;
#endif /* SPI_0_EN */

        default:
            return -1;
    }

    /* set frame size, slave mode always uses CTAR0 */
    spi_dev->CTAR[0] = SPI_CTAR_SLAVE_FMSZ(7);

    /* Set clock polarity and phase. */
    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            spi_dev->CTAR[0] &= ~(SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK);
            break;

        case SPI_CONF_SECOND_RISING:
            spi_dev->CTAR[0] &= ~(SPI_CTAR_CPOL_MASK);
            spi_dev->CTAR[0] |= SPI_CTAR_CPHA_MASK;
            break;

        case SPI_CONF_FIRST_FALLING:
            spi_dev->CTAR[0] &= ~(SPI_CTAR_CPHA_MASK);
            spi_dev->CTAR[0] |= SPI_CTAR_CPOL_MASK;
            break;

        case SPI_CONF_SECOND_FALLING:
            spi_dev->CTAR[0] |= SPI_CTAR_CPHA_MASK | SPI_CTAR_CPOL_MASK;
            break;

        default:
            return -2;
    }

    /* enable SPI */
    spi_dev->MCR = SPI_MCR_DOZE_MASK
                   | SPI_MCR_PCSIS(SPI_0_PCS0_ACTIVE_LOW << 0)
                   | SPI_MCR_CLR_TXF_MASK
                   | SPI_MCR_CLR_RXF_MASK;

    spi_dev->RSER = (uint32_t)0;

    return 0;
}

int spi_acquire(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }

    mutex_lock(locks_map[dev]);
    return 0;
}

int spi_release(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }

    mutex_unlock(locks_map[dev]);
    return 0;
}

static inline uint8_t spi_transfer_internal(SPI_Type *spi_dev, uint32_t flags, uint8_t byte_out)
{
    /* Wait until there is space in the TXFIFO */
    while (!(spi_dev->SR & SPI_SR_TFFF_MASK));

    spi_dev->PUSHR = flags | SPI_PUSHR_TXDATA(byte_out);

    /* Wait until we have received a byte */
    while (!(spi_dev->SR & SPI_SR_RXCTR_MASK));

    return (uint8_t)spi_dev->POPR;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    SPI_Type *spi_dev;
    uint8_t byte_in;
    uint32_t flags;

    /* The chip select lines are expected to be controlled via software in RIOT.
     * Don't set PCS bits in flags. */
    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_0_CTAS) | SPI_PUSHR_EOQ_MASK);
            break;
#endif

#if SPI_1_EN

        case SPI_1:
            spi_dev = SPI_1_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_1_CTAS) | SPI_PUSHR_EOQ_MASK);
            break;
#endif

#if SPI_2_EN

        case SPI_2:
            spi_dev = SPI_2_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_2_CTAS) | SPI_PUSHR_EOQ_MASK);
            break;
#endif

#if SPI_3_EN

        case SPI_3:
            spi_dev = SPI_3_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_3_CTAS) | SPI_PUSHR_EOQ_MASK);
            break;
#endif

#if SPI_4_EN

        case SPI_4:
            spi_dev = SPI_4_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_4_CTAS) | SPI_PUSHR_EOQ_MASK);
            break;
#endif

#if SPI_5_EN

        case SPI_5:
            spi_dev = SPI_5_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_5_CTAS) | SPI_PUSHR_EOQ_MASK);
            break;
#endif

#if SPI_6_EN

        case SPI_6:
            spi_dev = SPI_6_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_6_CTAS) | SPI_PUSHR_EOQ_MASK);
            break;
#endif

#if SPI_7_EN

        case SPI_7:
            spi_dev = SPI_7_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_7_CTAS) | SPI_PUSHR_EOQ_MASK);
            break;
#endif

        default:
            return -1;
    }

    byte_in = spi_transfer_internal(spi_dev, flags, out);

    /* Clear End-of-Queue status flag */
    spi_dev->SR = SPI_SR_EOQF_MASK;

    if (in != NULL) {
        *in = (char)byte_in;
    }

    return 1;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    SPI_Type *spi_dev;
    uint8_t byte_in;
    uint8_t byte_out;
    uint32_t flags;
    int i;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_0_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_1_EN

        case SPI_1:
            spi_dev = SPI_1_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_1_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_2_EN

        case SPI_2:
            spi_dev = SPI_2_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_2_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_3_EN

        case SPI_3:
            spi_dev = SPI_3_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_3_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_4_EN

        case SPI_4:
            spi_dev = SPI_4_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_4_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_5_EN

        case SPI_5:
            spi_dev = SPI_5_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_5_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_6_EN

        case SPI_6:
            spi_dev = SPI_6_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_6_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_7_EN

        case SPI_7:
            spi_dev = SPI_7_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_7_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

        default:
            return -1;
    }

    /* Default: send idle data */
    byte_out = (uint8_t)SPI_IDLE_DATA;

    for (i = 0; i < length; i++) {
        if (out != NULL) {
            /* Send given out data */
            byte_out = (uint8_t)out[i];
        }

        if (i >= length - 1) {
            /* Last byte, set End-of-Queue flag, clear Continue flag. */
            flags &= ~(SPI_PUSHR_CONT_MASK);
            flags |= SPI_PUSHR_EOQ_MASK;
        }

        byte_in = spi_transfer_internal(spi_dev, flags, byte_out);

        if (in != NULL) {
            /* Save input byte to buffer */
            in[i] = (char)byte_in;
        }
    }

    /* Clear End-of-Queue status flag */
    spi_dev->SR = SPI_SR_EOQF_MASK;

    return i;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    SPI_Type *spi_dev;
    uint8_t byte_in;
    uint32_t flags;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_0_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_1_EN

        case SPI_1:
            spi_dev = SPI_1_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_1_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_2_EN

        case SPI_2:
            spi_dev = SPI_2_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_2_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_3_EN

        case SPI_3:
            spi_dev = SPI_3_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_3_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_4_EN

        case SPI_4:
            spi_dev = SPI_4_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_4_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_5_EN

        case SPI_5:
            spi_dev = SPI_5_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_5_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_6_EN

        case SPI_6:
            spi_dev = SPI_6_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_6_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_7_EN

        case SPI_7:
            spi_dev = SPI_7_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_7_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

        default:
            return -1;
    }

    /* Transfer the register address first. */
    spi_transfer_internal(spi_dev, flags, reg);

    /* Last byte, set End-of-Queue flag, clear Continue flag. */
    flags &= ~(SPI_PUSHR_CONT_MASK);
    flags |= SPI_PUSHR_EOQ_MASK;

    /* Transfer the value. */
    byte_in = spi_transfer_internal(spi_dev, flags, out);

    if (in != NULL) {
        /* Save input byte to buffer */
        *in = (char)byte_in;
    }

    /* Clear End-of-Queue status flag */
    spi_dev->SR = SPI_SR_EOQF_MASK;

    return 2;
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    SPI_Type *spi_dev;
    uint8_t byte_in;
    uint8_t byte_out;
    uint32_t flags;
    int i;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_dev = SPI_0_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_0_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_1_EN

        case SPI_1:
            spi_dev = SPI_1_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_1_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_2_EN

        case SPI_2:
            spi_dev = SPI_2_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_2_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_3_EN

        case SPI_3:
            spi_dev = SPI_3_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_3_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_4_EN

        case SPI_4:
            spi_dev = SPI_4_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_4_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_5_EN

        case SPI_5:
            spi_dev = SPI_5_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_5_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_6_EN

        case SPI_6:
            spi_dev = SPI_6_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_6_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

#if SPI_7_EN

        case SPI_7:
            spi_dev = SPI_7_DEV;
            flags = (SPI_PUSHR_CTAS(SPI_7_CTAS) | SPI_PUSHR_CONT_MASK);
            break;
#endif

        default:
            return -1;
    }

    byte_out = reg;

    /* Send register address */
    byte_in = spi_transfer_internal(spi_dev, flags, byte_out);

    /* Default: send idle data */
    byte_out = (uint8_t)SPI_IDLE_DATA;

    for (i = 0; i < length; i++) {
        if (out != NULL) {
            /* Send given out data */
            byte_out = (uint8_t)out[i];
        }

        if (i >= length - 1) {
            /* Last byte, set End-of-Queue flag, clear Continue flag. */
            flags &= ~(SPI_PUSHR_CONT_MASK);
            flags |= SPI_PUSHR_EOQ_MASK;
        }

        byte_in = spi_transfer_internal(spi_dev, flags, byte_out);

        if (in != NULL) {
            /* Save input byte to buffer */
            in[i] = (char)byte_in;
        }
    }

    /* Clear End-of-Queue status flag */
    spi_dev->SR = SPI_SR_EOQF_MASK;

    return i;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            SPI_0_DEV->PUSHR = SPI_PUSHR_CTAS(SPI_0_CTAS)
                               | SPI_PUSHR_EOQ_MASK
                               | SPI_PUSHR_TXDATA(reset_val);
            break;
#endif
    }
}

void spi_poweron(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            SPI_0_CLKEN();
            break;
#endif
#if SPI_1_EN

        case SPI_1:
            SPI_1_CLKEN();
            break;
#endif
#if SPI_2_EN

        case SPI_2:
            SPI_2_CLKEN();
            break;
#endif
#if SPI_3_EN

        case SPI_3:
            SPI_3_CLKEN();
            break;
#endif
#if SPI_4_EN

        case SPI_4:
            SPI_4_CLKEN();
            break;
#endif
#if SPI_5_EN

        case SPI_5:
            SPI_5_CLKEN();
            break;
#endif
#if SPI_6_EN

        case SPI_6:
            SPI_6_CLKEN();
            break;
#endif
#if SPI_7_EN

        case SPI_7:
            SPI_7_CLKEN();
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    /* Wait until the last byte has been transmitted before turning off
     * the clock. */
    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            while ((SPI_0_DEV->SR & SPI_SR_TXCTR_MASK) != 0);

            SPI_0_CLKDIS();
            break;
#endif
#if SPI_1_EN

        case SPI_1:
            while ((SPI_1_DEV->SR & SPI_SR_TXCTR_MASK) != 0);

            SPI_1_CLKDIS();
            break;
#endif
#if SPI_2_EN

        case SPI_2:
            while ((SPI_2_DEV->SR & SPI_SR_TXCTR_MASK) != 0);

            SPI_2_CLKDIS();
            break;
#endif
#if SPI_3_EN

        case SPI_3:
            while ((SPI_3_DEV->SR & SPI_SR_TXCTR_MASK) != 0);

            SPI_3_CLKDIS();
            break;
#endif
#if SPI_4_EN

        case SPI_4:
            while ((SPI_4_DEV->SR & SPI_SR_TXCTR_MASK) != 0);

            SPI_4_CLKDIS();
            break;
#endif
#if SPI_5_EN

        case SPI_5:
            while ((SPI_5_DEV->SR & SPI_SR_TXCTR_MASK) != 0);

            SPI_5_CLKDIS();
            break;
#endif
#if SPI_6_EN

        case SPI_6:
            while ((SPI_6_DEV->SR & SPI_SR_TXCTR_MASK) != 0);

            SPI_6_CLKDIS();
            break;
#endif
#if SPI_7_EN

        case SPI_7:
            while ((SPI_7_DEV->SR & SPI_SR_TXCTR_MASK) != 0);

            SPI_7_CLKDIS();
            break;
#endif
    }
}

static inline void irq_handler_transfer(SPI_Type *spi, spi_t dev)
{

    if (spi->SR & SPI_SR_RFDF_MASK) {
        char data;
        data = (char)spi->POPR;
        data = spi_config[dev].cb(data);
        spi->PUSHR = SPI_PUSHR_CTAS(0)
                     | SPI_PUSHR_EOQ_MASK
                     | SPI_PUSHR_TXDATA(data);
    }

    /* see if a thread with higher priority wants to run now */
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#if SPI_0_EN
#ifdef SPI_0_IRQ_HANDLER
void SPI_0_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_0_DEV, SPI_0);
}
#endif
#endif

#if SPI_1_EN
#ifdef SPI_1_IRQ_HANDLER
void SPI_1_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_1_DEV, SPI_1);
}
#endif
#endif

#if SPI_2_EN
#ifdef SPI_2_IRQ_HANDLER
void SPI_2_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_2_DEV, SPI_2);
}
#endif
#endif

#if SPI_3_EN
#ifdef SPI_3_IRQ_HANDLER
void SPI_3_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_3_DEV, SPI_3);
}
#endif
#endif

#if SPI_4_EN
#ifdef SPI_4_IRQ_HANDLER
void SPI_4_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_4_DEV, SPI_4);
}
#endif
#endif

#if SPI_5_EN
#ifdef SPI_5_IRQ_HANDLER
void SPI_5_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_5_DEV, SPI_5);
}
#endif
#endif

#if SPI_6_EN
#ifdef SPI_6_IRQ_HANDLER
void SPI_6_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_6_DEV, SPI_6);
}
#endif
#endif

#if SPI_7_EN
#ifdef SPI_7_IRQ_HANDLER
void SPI_7_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_7_DEV, SPI_7);
}
#endif
#endif

#endif /* SPI_NUMOF */
