/*
 * cpu.c
 *
 *  Created on: 1 wrz 2015
 *      Author: kkk
 */

/**
 * @ingroup     cpu_efm32wg
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Mateusz Kubaszek <mathir.km.riot@gmail.com>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "periph_conf.h"
#include "emlib/inc/em_chip.h"
#include "emlib/inc/em_cmu.h"

static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    enableIRQ();
    /* chip initialization */
    CHIP_Init();
    SystemInit();
    /* initialize the clock system */
    cpu_clock_init();
}


static void cpu_clock_init(void)
{
    SystemLFXOClockSet((uint32_t)EFM32_LFXO_FREQ);
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
	CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
	CMU_HFRCOBandSet(cmuHFRCOBand_28MHz);
//	/* HF Core clock is connected to external oscillator */
//	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	/* HF Core clock is connected to internal RC clock */
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
	/* RTC, LESENSE, LETIMER0, LCD is connected to external 36kHz oscillator */
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
	/* HFPER clock is divided by 2 - 14MHz */
	CMU_ClockDivSet(cmuClock_HFPER, cmuClkDiv_2);
	/* core, DMA etc. clock */
	CMU_ClockDivSet(cmuClock_CORE, cmuClkDiv_1);
	/* enabling clocks */
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_CORELE, true);
}
