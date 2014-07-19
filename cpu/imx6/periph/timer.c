/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        timer.c
 * @brief       Low-level timer driver implementation for the i.MX6 CPU
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Leon George <l.george@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "imx6_irq.h"

#include "periph/timer.h"
#include "periph_conf.h"

/* imx6sdl reference manual: */
/* epit p. 1314 */
/* gpt  p. 1602 */
#include "ccm_pll.h"
#include "interrupt.h"
#include "soc_memory_map.h"


typedef struct {
	void (*cb)(int);
} timer_conf_t;

/**
 * @brief Timer state memory
 */
timer_conf_t config[TIMER_NUMOF];
/**
 * @brieft active GPT channes. (1 << 0) is the first channel, (1 << 1) the second and (1 << 2) the third
 */
uint8_t active_gpt_channels = 0;

/* TODO maybe these can be used */
/*! @brief Supported output modes */
enum _gpt_compare_modes
{
	OUTPUT_CMP_DISABLE = 0,     /*!< output disconnected from pad*/
	OUTPUT_CMP_TOGGLE = 1,      /*!< output toggle mode*/
	OUTPUT_CMP_CLEAR = 2,       /*!< output set low mode*/
	OUTPUT_CMP_SET = 3,         /*!< output set high mode*/
	OUTPUT_CMP_LOWPULSE = 4     /*!< output set high mode*/
};

#if TIMER_0_EN
__attribute__((naked)) void isr_tc0(void) ;
#endif
#if TIMER_1_EN
__attribute__((naked)) void isr_tc1(void) ;
#endif
#if TIMER_2_EN
__attribute__((naked)) void isr_tc2(void) ;
#endif
/**
 * @brief Setup the given timer
 *
 * The RIoTboard has 3 timers. One GPT with three channels. Two EPITs with one channel each.
 *
 *           ----------
 *           |        |-> IRQ-compareA
 *   ------->| GPT    |-> IRQ-compareB
 *   |       |        |-> IRQ-compareC
 *   |       ----------
 *   |
 *   |       ----------
 *   |       |        |
 *   ------->| EPIT1  |-> IRQ-compareA
 *   |       |        |
 *   |       ----------
 *   |
 *   |
 *   |       ----------
 *   |       |        |
 *   ------->| EPIT1  |-> IRQ-compareA
 *           |        |
 *           ----------
 *
 * Maybe I should ask Hauke for help with the ASCII-art
 * All timers use the TIMER_CLKSRC with TIMER_PRESCALE.
 */
int timer_init(tim_t dev, unsigned int ticks_per_us, void (*callback)(int)) {
	signed char epit_dev = 0, is_gpt = 0;
	switch (dev) {
#if TIMER_0_EN
		case TIMER_0:
#if TIMER_0_ISEPIT
			epit_dev = TIMER_0_EPITDEV;
#else
			is_gpt = 1;
#endif
			break;
#endif
#if TIMER_1_EN
		case TIMER_1:
#if TIMER_1_ISEPIT
			epit_dev = TIMER_1_EPITDEV;
#else
			is_gpt = 1;
#endif
			break;
#endif
#if TIMER_2_EN
		case TIMER_2:
#if TIMER_2_ISEPIT
			epit_dev = TIMER_2_EPITDEV;
#else
			is_gpt = 1;
#endif
			break;
#endif

		case TIMER_UNDEFINED:
		default:
			return -1;
	}

	if (epit_dev != 0) {
		uint32_t control_reg_tmp = 0;

		/* enable the source clocks to the EPIT port */
		clock_gating_config(REGS_EPIT_BASE(epit_dev), CLOCK_ON);

		/* start with a known state by disabling and reseting the module */
		HW_EPIT_CR_WR(epit_dev, BM_EPIT_CR_SWR);

		/* set the reference source clock for the counter */
		control_reg_tmp |= BF_EPIT_CR_CLKSRC(TIMER_CLKSRC);

		/* set the counter clock TIMER_PRESCALE value - 0 to 4095 */
		control_reg_tmp |= BF_EPIT_CR_PRESCALAR(TIMER_PRESCALE-1);

		/* set behavior for low power mode */
		control_reg_tmp |= BM_EPIT_CR_WAITEN; /*!< Timer is enabled in wait mode.*/
		control_reg_tmp |= BM_EPIT_CR_STOPEN; /*!< Timer is enabled in stop mode.*/

		/*
		 * make the counter start from a known value when enabled, this is loaded from
		 * EPITLR register if RLD=FREE_RUNNING=1 or 0xFFFFFFFF if RLD=reload_mode=0
		 */
		control_reg_tmp |= BM_EPIT_CR_IOVW | BM_EPIT_CR_ENMOD;

		/* set the reload mode (we don't do this ) */
		/*control_reg_tmp |= BM_EPIT_CR_RLD;*/

		/*
		 * initialize the load register especially if RLD==SET_AND_FORGET=1
		 * and if the value is different from 0 which is the lowest counter value
		 */
		/* HW_EPIT_LR_WR(epit_dev, 0);*/

		/* wait for the reset to complete */
		while ((HW_EPIT_CR(epit_dev).B.SWR) != 0) ;

		/* finally write the control register */
		HW_EPIT_CR_WR(epit_dev, control_reg_tmp);
	} else if (is_gpt) {
		uint32_t control_reg_tmp = 0;

		/* enable the source clocks to the GPT port */
		clock_gating_config(GPT_BASE_ADDR, CLOCK_ON);

		/* start with a known state by disabling and reseting the module */
		HW_GPT_CR_WR(BM_GPT_CR_SWR);

		/* set the reference source clock for the counter */
		if (TIMER_CLKSRC == CLKSRC_CKIL)
		{
			/* CKIL source is 0x4 for GPT but 0x3 for EPIT*/
			control_reg_tmp |= BF_GPT_CR_CLKSRC(TIMER_CLKSRC + 1);
		} else {
			control_reg_tmp |= BF_GPT_CR_CLKSRC(TIMER_CLKSRC);
		}

		/* wait for the reset to complete */
		while (HW_GPT_CR.B.SWR != 0) ;

		/* the prescaler can be changed at any time, and
		 * this affects the output clock immediately*/
		HW_GPT_PR_WR(BF_GPT_PR_PRESCALER(TIMER_PRESCALE - 1));

		/* set the counter mode */
		control_reg_tmp |= BF_GPT_CR_FRR(1);

		/* set behavior for low power mode */
		control_reg_tmp |= BM_GPT_CR_WAITEN;
		control_reg_tmp |= BM_GPT_CR_STOPEN;

		/* specify from where the counter starts to count when enabled
		 * this code makes it start from 0 */
		control_reg_tmp |= BM_GPT_CR_ENMOD;

		/* finally write the control register */
		HW_GPT_CR_WR(control_reg_tmp);
	}

	config[dev].cb = callback;

	timer_irq_enable(dev);

	timer_start(dev);

	return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout) {
	return timer_set_absolute(dev, channel, timer_read(dev) + timeout);
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
	bool is_gpt = false;
	/* get timer base register address */
	switch (dev) {
#if TIMER_0_EN
		case TIMER_0:
#if TIMER_0_ISEPIT
			HW_EPIT_CMPR_WR(TIMER_0_EPITDEV, TIMER_0_MAX_VALUE - value);
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_1_EN
		case TIMER_1:
#if TIMER_1_ISEPIT
			HW_EPIT_CMPR_WR(TIMER_1_EPITDEV, TIMER_1_MAX_VALUE - value);
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_2_EN
		case TIMER_2:
#if TIMER_2_ISEPIT
			HW_EPIT_CMPR_WR(TIMER_2_EPITDEV, TIMER_2_MAX_VALUE - value);
#else
			is_gpt = true;
#endif
			break;
#endif
		case TIMER_UNDEFINED:
		default:
			return -1;
	}

	if (is_gpt) {
		switch (channel)
		{
			case 0:
				BW_GPT_CR_OM1(OUTPUT_CMP_SET);
				HW_GPT_OCR1_WR(value);
				break;

			case 1:
				BW_GPT_CR_OM2(OUTPUT_CMP_SET);
				HW_GPT_OCR2_WR(value);
				break;

			case 2:
				BW_GPT_CR_OM3(OUTPUT_CMP_SET);
				HW_GPT_OCR3_WR(value);
				break;
		}

		/* Set the correspoing flags for raising interrupts */
		active_gpt_channels |= (1 << channel);
		/* enable the counter TODO */
		timer_start(dev);
	}

	return 1;
}

int timer_clear(tim_t dev, int channel)
{
	bool is_gpt = false;
	switch (dev) {
#if TIMER_0_EN
		case TIMER_0:
#if TIMER_0_ISEPIT
			HW_EPIT_SR_SET(TIMER_0_EPITDEV, BM_EPIT_SR_OCIF);
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_1_EN
		case TIMER_1:
#if TIMER_1_ISEPIT
			HW_EPIT_SR_SET(TIMER_1_EPITDEV, BM_EPIT_SR_OCIF);
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_2_EN
		case TIMER_2:
#if TIMER_2_ISEPIT
			HW_EPIT_SR_SET(TIMER_2_EPITDEV, BM_EPIT_SR_OCIF);
#else
			is_gpt = true;
#endif
			break;
#endif
		case TIMER_UNDEFINED:
		default:
			return -1;
	}

	if (is_gpt) {
		active_gpt_channels &= (~(1 << channel));

		HW_GPT_SR_WR(HW_GPT_SR_RD() & ~(1 << channel));


		if (active_gpt_channels == 0) {
			timer_start(dev);
		}
		else {
			timer_stop(dev);
		}
	}

	return 1;
}

unsigned int timer_read(tim_t dev)
{
	switch (dev) {
		/* EPIT is counting downwards; GPT upwards */
#if TIMER_0_EN
		case TIMER_0:
#if TIMER_0_ISEPIT
			return TIMER_0_MAX_VALUE - HW_EPIT_CNR_RD(TIMER_0_EPITDEV);
#else
			return HW_GPT_CNT_RD();
#endif
			break;
#endif
#if TIMER_1_EN
		case TIMER_1:
#if TIMER_1_ISEPIT
			return TIMER_1_MAX_VALUE - HW_EPIT_CNR_RD(TIMER_1_EPITDEV);
#else
			return HW_GPT_CNT_RD();
#endif
			break;
#endif
#if TIMER_2_EN
		case TIMER_2:
#if TIMER_2_ISEPIT
			return TIMER_2_MAX_VALUE - HW_EPIT_CNR_RD(TIMER_2_EPITDEV);
#else
			return HW_GPT_CNT_RD();
#endif
			break;
#endif
		case TIMER_UNDEFINED:
		default:
			return 0;
	}
}

void timer_stop(tim_t dev)
{
	signed char epit_dev = 0, is_gpt = false;
	switch (dev) {
#if TIMER_0_EN
		case TIMER_0:
#if TIMER_0_ISEPIT
			epit_dev = TIMER_0_EPITDEV;
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_1_EN
		case TIMER_1:
#if TIMER_1_ISEPIT
			epit_dev = TIMER_1_EPITDEV;
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_2_EN
		case TIMER_2:
#if TIMER_2_ISEPIT
			epit_dev = TIMER_2_EPITDEV;
#else
			is_gpt = true;
#endif
			break;
#endif
		case TIMER_UNDEFINED:
		default:
			break;
	}

	if (epit_dev != 0) {
		/* temporary workaround for the discovered issue when disabling the
		 * counter during end of count/reload/set compare flag ??.
		 * Set to the max value so that it ensures that the counter couldn't
		 * reach 0 when it is disabled.
		 */
		HW_EPIT_LR_WR(epit_dev, 0xFFFFFFFF);

		/* disable the counter */
		HW_EPIT_CR_CLR(epit_dev, BM_EPIT_CR_EN);

		/* ensure to leave the counter in a proper state*/
		/* by disabling the output compare interrupt*/
		HW_EPIT_CR_CLR(epit_dev, BM_EPIT_CR_OCIEN);

		/* and clearing possible remaining compare event */
		HW_EPIT_SR_SET(epit_dev, BM_EPIT_SR_OCIF);
	} else if (is_gpt) {
		/* disable the counter */
		HW_GPT_CR.B.EN = 0;

		/* ensure to leave the counter in a proper state by disabling the interrupt sources*/
		HW_GPT_IR_WR(0);

		/* and by clearing possible remaining events */
		HW_GPT_SR_WR((1 << 0) | (1 << 1) | (1 << 2));
	}
}

void timer_start(tim_t dev)
{
	signed char epit_dev = 0, is_gpt = false;
	switch (dev) {
#if TIMER_0_EN
		case TIMER_0:
#if TIMER_0_ISEPIT
			epit_dev = TIMER_0_EPITDEV;
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_1_EN
		case TIMER_1:
#if TIMER_1_ISEPIT
			epit_dev = TIMER_1_EPITDEV;
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_2_EN
		case TIMER_2:
#if TIMER_2_ISEPIT
			epit_dev = TIMER_2_EPITDEV;
#else
			is_gpt = true;
#endif
			break;
#endif
		case TIMER_UNDEFINED:
		default:
			break;
	}

	timer_stop(dev);

	if (epit_dev != 0) {
		/* ensure to start the counter in a proper state
		 * by clearing possible remaining compare event*/
		HW_EPIT_SR_SET(epit_dev, BM_EPIT_SR_OCIF);

		/* Raise an interrupt when the output compare event occurs*/
		HW_EPIT_CR_SET(epit_dev, BM_EPIT_CR_OCIEN);

		/* finally, enable the counter */
		HW_EPIT_CR_SET(epit_dev, BM_EPIT_CR_EN);
	} else if (is_gpt) {

		/* enable the interrupts or clear the register for polling */
		HW_GPT_IR_WR(active_gpt_channels & 0x3F);

		/* finally, enable the counter */
		HW_GPT_CR.B.EN = 1;
	}
}

#define EPIT_IRQS(x) ( (x) == HW_EPIT1 ? IMX_INT_EPIT1 : (x) == HW_EPIT2 ? IMX_INT_EPIT2 : 0xFFFFFFFF)
typedef void (*isr_t)(void);

void timer_irq_enable(tim_t dev)
{
	signed char epit_dev = 0, is_gpt = false;
	isr_t isr;
	switch (dev) {
#if TIMER_0_EN
		case TIMER_0:
#if TIMER_0_ISEPIT
			epit_dev = TIMER_0_EPITDEV;
#else
			is_gpt = true;
#endif
			isr = isr_tc0;
			break;
#endif
#if TIMER_1_EN
		case TIMER_1:
#if TIMER_1_ISEPIT
			epit_dev = TIMER_1_EPITDEV;
#else
			is_gpt = true;
#endif
			isr = isr_tc1;
			break;
#endif
#if TIMER_2_EN
		case TIMER_2:
#if TIMER_2_ISEPIT
			epit_dev = TIMER_2_EPITDEV;
#else
			is_gpt = true;
#endif
			isr = isr_tc2;
			break;
#endif
		case TIMER_UNDEFINED:
			break;
	}

	if (epit_dev != 0) {
		uint32_t irq_id = EPIT_IRQS(epit_dev);

		register_interrupt_routine(irq_id, isr);

		enable_interrupt(irq_id, CPU_0, 0);
	} else if (is_gpt) {
		/* register the IRQ sub-routine */
		register_interrupt_routine(IMX_INT_GPT, isr);

		/* enable the IRQ */
		enable_interrupt(IMX_INT_GPT, CPU_0, 0);
	}
}

void timer_irq_disable(tim_t dev)
{
	switch (dev) {
#if TIMER_0_EN
		case TIMER_0:
#if TIMER_0_ISEPIT
			disable_interrupt(EPIT_IRQS(TIMER_0_EPITDEV), CPU_0);
#else
			/* disable the IRQ */
			disable_interrupt(IMX_INT_GPT, CPU_0);
#endif
			break;
#endif
#if TIMER_1_EN
		case TIMER_1:
#if TIMER_1_ISEPIT
			disable_interrupt(EPIT_IRQS(TIMER_1_EPITDEV), CPU_0);
#else
			/* disable the IRQ */
			disable_interrupt(IMX_INT_GPT, CPU_0);
#endif
			break;
#endif
#if TIMER_2_EN
		case TIMER_2:
#if TIMER_2_ISEPIT
			disable_interrupt(EPIT_IRQS(TIMER_2_EPITDEV), CPU_0);
#else
			/* disable the IRQ */
			disable_interrupt(IMX_INT_GPT, CPU_0);
#endif
			break;
#endif
		case TIMER_UNDEFINED:
			break;
	}
}

void timer_reset(tim_t dev)
{
	signed char epit_dev = 0, is_gpt = 0;
	switch (dev) {
#if TIMER_0_EN
		case TIMER_0:
#if TIMER_0_ISEPIT
			epit_dev = TIMER_0_EPITDEV;
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_1_EN
		case TIMER_1:
#if TIMER_1_ISEPIT
			epit_dev = TIMER_1_EPITDEV;
#else
			is_gpt = true;
#endif
			break;
#endif
#if TIMER_2_EN
		case TIMER_2:
#if TIMER_2_ISEPIT
			epit_dev = TIMER_2_EPITDEV;
#else
			is_gpt = true;
#endif
			break;
#endif
		default:
		case TIMER_UNDEFINED:
			break;
	}

	if (epit_dev) {
		uint32_t control_reg_tmp = 0;

		/* start with a known state by disabling and reseting the module */
		HW_EPIT_CR_WR(epit_dev, BM_EPIT_CR_SWR);

		/* set the reference source clock for the counter */
		control_reg_tmp |= BF_EPIT_CR_CLKSRC(TIMER_CLKSRC);

		/* set the counter clock TIMER_PRESCALE value - 0 to 4095 */
		control_reg_tmp |= BF_EPIT_CR_PRESCALAR(TIMER_PRESCALE-1);

		/* set behavior for low power mode */
		control_reg_tmp |= BM_EPIT_CR_WAITEN; /*!< Timer is enabled in wait mode.*/
		control_reg_tmp |= BM_EPIT_CR_STOPEN; /*!< Timer is enabled in stop mode.*/

		/*
		 * make the counter start from a known value when enabled, this is loaded from
		 * EPITLR register if RLD=FREE_RUNNING=1 or 0xFFFFFFFF if RLD=reload_mode=0
		 */
		control_reg_tmp |= BM_EPIT_CR_IOVW | BM_EPIT_CR_ENMOD;

		/* set the reload mode (we don't do this ) */
		/*control_reg_tmp |= BM_EPIT_CR_RLD;*/

		/*
		 * initialize the load register especially if RLD==SET_AND_FORGET=1
		 * and if the value is different from 0 which is the lowest counter value
		 */
		/* HW_EPIT_LR_WR(epit_dev, 0);*/

		/* wait for the reset to complete */
		while ((HW_EPIT_CR(epit_dev).B.SWR) != 0) ;

		/* finally write the control register */
		HW_EPIT_CR_WR(epit_dev, control_reg_tmp);
	} else if (is_gpt) {
		/* TODO GPT_CNT cant be written to */
	}
}

#if TIMER_0_EN
	__attribute__((naked))
void isr_tc0(void)
{
	enterISR();
#if TIMER_0_ISEPIT
	if (HW_EPIT_SR_RD(TIMER_0_EPITDEV) & BM_EPIT_SR_OCIF)
	{
		/* clear the interrupt-flag */
		timer_clear(TIMER_0, 0);
		config[TIMER_0].cb(0);
	}
#else
	uint32_t status_register = HW_GPT_SR_RD();

	if (status_register & (1 << 0)) {
		/* clear the interrupt-flag */
		timer_clear(TIMER_0, 0);
		config[TIMER_0].cb(0);
	}

	if (status_register & (1 << 1)) {
		/* clear the interrupt-flag */
		timer_clear(TIMER_0, 1);
		config[TIMER_0].cb(1);
	}

	if (status_register & (1 << 2)) {
		/* clear the interrupt-flag */
		timer_clear(TIMER_0, 2);
		config[TIMER_0].cb(2);
	}
#endif
	leaveISR();
}
#endif /* TIMER_0_EN */

#if TIMER_1_EN
	__attribute__((naked))
void isr_tc1(void)
{
	enterISR();
#if TIMER_1_ISEPIT
	if (HW_EPIT_SR_RD(TIMER_1_EPITDEV) & BM_EPIT_SR_OCIF)
	{
		/* clear the interrupt-flag */
		timer_clear(TIMER_1, 0);
		config[TIMER_1].cb(0);
	}
#else
	uint32_t status_register = HW_GPT_SR_RD();

	if (status_register & (1 << 0)) {
		/* clear the interrupt-flag */
		timer_clear(TIMER_1, 0);
		config[TIMER_1].cb(0);
	}

	if (status_register & (1 << 1)) {
		/* clear the interrupt-flag */
		timer_clear(TIMER_1, 1);
		config[TIMER_1].cb(1);
	}

	if (status_register & (1 << 2)) {
		/* clear the interrupt-flag */
		timer_clear(TIMER_1, 2);
		config[TIMER_1].cb(2);
	}
#endif
	leaveISR();
}
#endif /* TIMER_1_EN */

#if TIMER_2_EN
	__attribute__((naked))
void isr_tc2(void)
{
	enterISR();
#if TIMER_2_ISEPIT
	if (HW_EPIT_SR_RD(TIMER_2_EPITDEV) & BM_EPIT_SR_OCIF)
	{
		/* clear the interrupt-flag */
		timer_clear(TIMER_2, 0);
		config[TIMER_2].cb(0);
	}
#else
	uint32_t status_register = HW_GPT_SR_RD();

	if (status_register & (1 << 0)) {
		/* clear the interrupt-flag */
		timer_clear(TIMER_2, 0);
		config[TIMER_2].cb(0);
	}

	if (status_register & (1 << 1)) {
		/* clear the interrupt-flag */
		timer_clear(TIMER_2, 1);
		config[TIMER_2].cb(1);
	}

	if (status_register & (1 << 2)) {
		/* clear the interrupt-flag */
		timer_clear(TIMER_2, 2);
		config[TIMER_2].cb(2);
	}
#endif
	leaveISR();
}
#endif /* TIMER_2_EN */
