/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_cc26x0_cc13x0_definitions
 * @{
 *
 * @file
 * @brief           CC26x0/CC13x0 AUX register definitions
 */


#include <stdbool.h>

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AUX_AIODIO registers
 */
typedef struct {
    reg32_t GPIODOUT; /**< gpio data out */
    reg32_t IOMODE; /**< input output mode */
    reg32_t GPIODIN; /**< gpio data in */
    reg32_t GPIODOUTSET; /**< gpio data out set */
    reg32_t GPIODOUTCLR; /**< gpio data out clear */
    reg32_t GPIODOUTTGL; /**< gpio data out toggle */
    reg32_t GPIODIE; /**< gpio data input enable */
} aux_aiodio_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AUX_AIODIO0_BASE      0x400C1000 /**< AUX_AIODIO0 base address */
#define AUX_AIODIO1_BASE      0x400C2000 /**< AUX_AIODIO1 base address */
/** @} */

#define AUX_AIODIO0          ((aux_aiodio_regs_t *) (AUX_AIODIO0_BASE)) /**< AUX_AIODIO0 register bank */
#define AUX_AIODIO1          ((aux_aiodio_regs_t *) (AUX_AIODIO1_BASE)) /**< AUX_AIODIO1 register bank */

/**
 * @brief AUX_TDC registers
 */
typedef struct {
    reg32_t CTL; /**< control */
    reg32_t STAT; /**< status */
    reg32_t RESULT; /**< result */
    reg32_t SATCFG; /**< saturaion configuration */
    reg32_t TRIGSRC; /**< trigger source */
    reg32_t TRIGCNT; /**< trigger counter */
    reg32_t TRIGCNTLOAD; /**< trigger counter load */
    reg32_t TRIGCNTCFG; /**< trigger counter config */
    reg32_t PRECTL; /**< prescaler control */
    reg32_t PRECNT; /**< prescaler counter */
} aux_tdc_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AUX_TDC_BASE      0x400C4000 /**< AUX_TDC base address */
/** @} */

#define AUX_TDC           ((aux_tdc_regs_t *) (AUX_TDC_BASE)) /**< AUX_TDC register bank */

/**
 * @brief AUX_EVCTL registers
 */
typedef struct {
    reg32_t VECCFG0; /**< vector config 0 */
    reg32_t VECCFG1; /**< vector config 1 */
    reg32_t SCEWEVSEL; /**< sensor controller engine wait event selection */
    reg32_t EVTOAONFLAGS; /**< events to AON domain flags */
    reg32_t EVTOAONPOL; /**< events to AON domain polarity */
    reg32_t DMACTL; /**< direct memory access control */
    reg32_t SWEVSET; /**< software event set */
    reg32_t EVSTAT0; /**< event status 0 */
    reg32_t EVSTAT1; /**< event status 1 */
    reg32_t EVTOMCUPOL; /**< event to MCU domain polarity */
    reg32_t EVTOMCUFLAGS; /**< event to MCU domain flags */
    reg32_t COMBEVTOMCUMASK; /**< combined event to MCU domain mask */
    reg32_t __reserved; /**< meh */
    reg32_t VECFLAGS; /**< vector flags */
    reg32_t EVTOMCUFLAGSCLR; /**< events to MCU domain flags clear */
    reg32_t EVTOAONFLAGSCLR; /**< events to AON domain clear */
    reg32_t VECFLAGSCLR; /**< vector flags clear */
} aux_evtcl_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AUX_EVCTL_BASE      0x400C5000 /**< AUX_EVCTL base address */
/** @} */

#define AUX_EVCTL           ((aux_evctl_regs_t *) (AUX_EVCTL_BASE)) /**< AUX_EVCTL register bank */

/**
 * AUX_WUC registers
 */
typedef struct {
    reg32_t MODCLKEN0; /**< module clock enable */
    reg32_t PWROFFREQ; /**< power off request */
    reg32_t PWRDWNREQ; /**< power down request */
    reg32_t PWRDWNACK; /**<  power down acknowledgement */
    reg32_t CLKLFREQ; /**< low frequency clock request */
    reg32_t CLKLFACK; /**< low frequency clock acknowledgement */
    reg32_t __reserved1[4]; /**< meh */
    reg32_t WUEVFLAGS; /**< wake-up event flags */
    reg32_t WUEVCLR; /**< wake-up event clear */
    reg32_t ADCCLKCTL; /**< ADC clock control */
    reg32_t TDCCLKCTL; /**< TDC clock control */
    reg32_t REFCLKCTL; /**< reference clock control */
    reg32_t RTCSUBSECINC0; /**< real time counter sub second increment 0 */
    reg32_t RTCSUBSECINC1; /**< real time counter sub second increment 1 */
    reg32_t RTCSUBSECINCCTL; /**< real time counter sub second increment control */
    reg32_t MCUBUSCTL; /**< MCU bus control */
    reg32_t MCUBUSSTAT; /**< MCU bus status */
    reg32_t AONCTLSTAT; /**< AON domain control status */
    reg32_t AUXIOLATCH; /**< AUX input output latch */
    reg32_t MODCLKEN1; /**< module clock enable 1 */
} aux_wuc_regs_t;

/**
 * @brief   AUX_WUC register values
 * @{
 */
#define MODCLKEN0_SMPH_EN           0x00000001  /* enable clock for AUX_SMPH */
#define MODCLKEN0_AIODIO0_EN        0x00000002  /* enable clock for AUX_AIODIO0 */
#define MODCLKEN0_AIODIO1_EN        0x00000004  /* enable clock for AUX_AIODIO1 */
#define MODCLKEN0_TIMER_EN          0x00000008  /* enable clock for AUX_TIMER */
#define MODCLKEN0_ANAIF_EN          0x00000010  /* enable clock for AUX_ANAIF */
#define MODCLKEN0_TDC_EN            0x00000020  /* enable clock for AUX_TDC */
#define MODCLKEN0_AUX_DDI0_OSC_EN   0x00000040  /* enable clock for AUX_DDI0_OSC */
#define MODCLKEN0_AUX_ADI4_EN       0x00000080  /* enable clock for AUX_ADI4 */
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AUX_WUC_BASE      0x400C6000 /**< AUX_WUC base address */
/** @} */

#define AUX_WUC           ((aux_wuc_regs_t *) (AUX_WUC_BASE)) /**< AUX_WUC register bank */

/**
 * @brief AUX_TIMER registers
 */
typedef struct {
    reg32_t T0CFG; /**< timer 0 config */
    reg32_t T1CFG; /**< timer 1 config */
    reg32_t T0CTL; /**< timer 0 control */
    reg32_t T0TARGET; /**< timer 0 target */
    reg32_t T1TARGET; /**< timer 1 target */
    reg32_t T1CTL; /**< timer 1 control */
} aux_timer_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AUX_TIMER_BASE      0x400C7000 /**< AUX_WUC base address */
/** @} */

#define AUX_TIMER           ((aux_timer_regs_t *) (AUX_TIMER_BASE)) /**< AUX_TIMER register bank */

/**
 * AUX_SMPH registers
 */
typedef struct {
    reg32_t SMPH0; /**< semaphore 0 */
    reg32_t SMPH1; /**< semaphore 1 */
    reg32_t SMPH2; /**< semaphore 2 */
    reg32_t SMPH3; /**< semaphore 3 */
    reg32_t SMPH4; /**< semaphore 4 */
    reg32_t SMPH5; /**< semaphore 5 */
    reg32_t SMPH6; /**< semaphore 6 */
    reg32_t SMPH7; /**< semaphore 7 */
    reg32_t AUTOTAKE; /**< sticky request for single semaphore */
} aux_smph_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AUX_SMPH_BASE               0x400C8000 /**< AUX_WUC base address */
/** @} */

#define AUX_SMPH ((aux_smph_regs_t *) (AUX_SMPH_BASE)) /**< AUX_SMPH register bank */

/**
 * AUX_ANAIF registers
 */
typedef struct {
    reg32_t __reserved[4]; /**< meh */
    reg32_t ADCCTL; /**< ADC control */
    reg32_t ADCFIFOSTAT; /**< ADC fifo status */
    reg32_t ADCFIFO; /**< ADC fifo */
    reg32_t ADCTRIG; /**< ADC trigger */
    reg32_t ISRCCTL; /**< current source control */
} aux_anaif_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define AUX_ANAIF_BASE      0x400C9000 /**< AUX_WUC base address */
/** @} */

#define AUX_ANAIF           ((aux_anaif_regs_t *) (AUX_ANAIF_BASE)) /**< AUX_ANAIF register bank */

/**
 * ADI_4_AUX registers
 */
typedef struct {
    reg8_t MUX0; /**< multiplexer 0 */
    reg8_t MUX1; /**< multiplexer 1 */
    reg8_t MUX2; /**< multiplexer 2 */
    reg8_t MUX3; /**< multiplexer 3 */
    reg8_t ISRC; /**< current source */
    reg8_t COMP; /**< comparator */
    reg8_t MUX4; /**< multiplexer 4 */
    reg8_t ADC0; /**< ADC control 0 */
    reg8_t ADC1; /**< ADC control 1 */
    reg8_t ADCREF0; /**< ADC reference 0 */
    reg8_t ADCREF1; /**< ADC reference 1 */
} adi_4_aux_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define ADI_4_AUX_BASE      0x400CB000 /**< AUX_WUC base address */
/** @} */

#define ADI_4_AUX           ((adi_4_aux_regs_t *) (ADI_4_AUX_BASE)) /**< ADI_4_AUX register bank */

#define ADDI_SEM AUX_SMPH->SMPH0 /**< the semamphore used for ADDI */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
