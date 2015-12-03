/*
 * Copyright (C) 2015 Marc Poulhiès
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Marc Poulhiès <dkm@kataplop.net>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "log.h"
#include "cpu.h"
#include "board.h"
#include "periph/pwm.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* ignore file in case no PWM devices are defined */
#if PWM_NUMOF

typedef struct pwm_chan_s {
  const unsigned long timer_sysctl;
  const unsigned long timer_base;
  const unsigned long gpio_sysctl;
  const unsigned long gpio_port;
  const uint8_t gpio_pin_num;
  const unsigned long timer_cfg;
  const unsigned long timer_side;
  const unsigned long ccp;
} pwm_dev_t;

typedef struct pwm_settings_s {
  unsigned long ticks;
  unsigned long resolution;
} pwm_settings_t;


static pwm_settings_t pwm_settings[PWM_NUMOF];

static const pwm_dev_t pwm_devs[PWM_NUMOF] = {
  { 				/* 0 */
    .timer_sysctl = SYSCTL_PERIPH_TIMER0,
    .timer_base = TIMER0_BASE,
    .gpio_sysctl = SYSCTL_PERIPH_GPIOB,
    .gpio_port = GPIO_PORTB_BASE,
    .gpio_pin_num = GPIO_PIN_3,
    .timer_side = TIMER_A,
    .timer_cfg = TIMER_CFG_A_PWM,
    .ccp = GPIO_PB6_T0CCP0,
  },
  {				/* 1 */
    .timer_sysctl = SYSCTL_PERIPH_TIMER0,
    .timer_base = TIMER0_BASE,
    .gpio_sysctl = SYSCTL_PERIPH_GPIOB,
    .gpio_port = GPIO_PORTB_BASE,
    .gpio_pin_num = GPIO_PIN_7,
    .timer_side = TIMER_B,
    .timer_cfg = TIMER_CFG_B_PWM,
    .ccp = GPIO_PB7_T0CCP1,
  },
  {				/* 2 */
    .timer_sysctl = SYSCTL_PERIPH_TIMER1,
    .timer_base = TIMER1_BASE,
    .gpio_sysctl = SYSCTL_PERIPH_GPIOB,
    .gpio_port = GPIO_PORTB_BASE,
    .gpio_pin_num = GPIO_PIN_4,
    .timer_cfg = TIMER_CFG_A_PWM,
    .timer_side = TIMER_A,
    .ccp = GPIO_PB4_T1CCP0,
  },
  {				/* 3 */
    .timer_sysctl = SYSCTL_PERIPH_TIMER1,
    .timer_base = TIMER1_BASE,
    .gpio_sysctl = SYSCTL_PERIPH_GPIOB,
    .gpio_port = GPIO_PORTB_BASE,
    .gpio_pin_num = GPIO_PIN_5,
    .timer_cfg = TIMER_CFG_B_PWM,
    .timer_side = TIMER_B,
    .ccp = GPIO_PB5_T1CCP1,
  },
  
  /* Other PWM that can be used, but current PWM interface only allows for 4 devices */
  /* {				/\* 4 *\/ */
  /*   .timer_sysctl = SYSCTL_PERIPH_TIMER2, */
  /*   .timer_base = TIMER2_BASE, */
  /*   .gpio_sysctl = SYSCTL_PERIPH_GPIOB, */
  /*   .gpio_port = GPIO_PORTB_BASE, */
  /*   .gpio_pin_num = GPIO_PIN_0, */
  /*   .timer_cfg = TIMER_CFG_A_PWM, */
  /*   .timer_side = TIMER_A, */
  /*   .ccp = GPIO_PB0_T2CCP0, */
  /* }, */
  /* {				/\* 5 *\/ */
  /*   .timer_sysctl = SYSCTL_PERIPH_TIMER1, */
  /*   .timer_base = TIMER1_BASE, */
  /*   .gpio_sysctl = SYSCTL_PERIPH_GPIOB, */
  /*   .gpio_port = GPIO_PORTB_BASE, */
  /*   .gpio_pin_num = GPIO_PIN_1, */
  /*   .timer_cfg = TIMER_CFG_B_PWM, */
  /*   .timer_side = TIMER_B, */
  /*   .ccp = GPIO_PB1_T2CCP1, */
  /* }, */
  /* {				/\* 6 *\/ */
  /*   .timer_sysctl = SYSCTL_PERIPH_TIMER2, */
  /*   .timer_base = TIMER2_BASE, */
  /*   .gpio_sysctl = SYSCTL_PERIPH_GPIOB, */
  /*   .gpio_port = GPIO_PORTB_BASE, */
  /*   .gpio_pin_num = GPIO_PIN_2, */
  /*   .timer_cfg = TIMER_CFG_A_PWM, */
  /*   .timer_side = TIMER_A, */
  /*   .ccp = GPIO_PB2_T3CCP0, */
  /* }, */
  /* {				/\* 7 *\/ */
  /*   .timer_sysctl = SYSCTL_PERIPH_TIMER2, */
  /*   .timer_base = TIMER2_BASE, */
  /*   .gpio_sysctl = SYSCTL_PERIPH_GPIOB, */
  /*   .gpio_port = GPIO_PORTB_BASE, */
  /*   .gpio_pin_num = GPIO_PIN_3, */
  /*   .timer_cfg = TIMER_CFG_B_PWM, */
  /*   .timer_side = TIMER_B, */
  /*   .ccp = GPIO_PB3_T3CCP1, */
  /* } */
};

int pwm_init(pwm_t dev, pwm_mode_t mode,
	     unsigned int frequency, unsigned int resolution)
{
    if (dev >= PWM_NUMOF) {
        return -1;
    }

    pwm_poweron(dev);
    
    const unsigned long timer_pwm_cfg = pwm_devs[dev].timer_cfg;
    const unsigned long timer_base = pwm_devs[dev].timer_base;
    const unsigned long gpio_pin = pwm_devs[dev].gpio_pin_num;
    const unsigned long ccp = pwm_devs[dev].ccp;
    const unsigned long timer_ab = pwm_devs[dev].timer_side;
    const unsigned long gpio_sysctl = pwm_devs[dev].gpio_sysctl;
    const unsigned long gpio_port_base = pwm_devs[dev].gpio_port;

    ROM_SysCtlPeripheralEnable(gpio_sysctl);
   
    ROM_GPIOPinConfigure(ccp);
    ROM_GPIOPinTypePWM(gpio_port_base, gpio_pin);

    ROM_TimerDisable(timer_base, timer_ab);
    ROM_TimerConfigure(timer_base, TIMER_CFG_SPLIT_PAIR | timer_pwm_cfg);

    const unsigned long clock = ROM_SysCtlClockGet();

    const unsigned long ticks = clock / frequency;
    const unsigned long real_freq = frequency;

    pwm_settings[dev].resolution = resolution;
    pwm_settings[dev].ticks = ticks;

    unsigned long ticks_high = ticks >> 16;
    unsigned long ticks_low  = ticks & 0xFFFF;

    if (ticks_high & ~0xFF) {
      DEBUG("Resolution too high\n");
      return -1;
    }

    DEBUG("Prescaler/Load set at 0x%lx/0x%lx\n", ticks_high, ticks_low);

    ROM_TimerPrescaleSet(timer_base, timer_ab, ticks_high);
    ROM_TimerLoadSet(timer_base, timer_ab, ticks_low);

    DEBUG("Setting ticks at %lu (reqf: %u, freq cpu: %lu)\n", ticks, frequency, clock);
    DEBUG("Real freq: %lu\n", real_freq);

    ROM_TimerPrescaleMatchSet(timer_base, timer_ab, 0);
    ROM_TimerMatchSet(timer_base, timer_ab, 0);

    /* set PWM mode */
    unsigned int level;
    switch (mode) {
        case PWM_LEFT:
	  level = 0;
	  break;
        case PWM_RIGHT:
	  level = 1;
	  break;
        case PWM_CENTER:
        default:
            return -1;
    }
    ROM_TimerControlLevel(timer_base, timer_ab, level);

    pwm_start(dev);

    return real_freq;
}


static void dump_pwm(pwm_t dev, int channel){
#if DEBUG_ENABLE
    if (channel >= PWM_MAX_CHANNELS) {
        return;
    }

    const unsigned long timer_base = pwm_devs[dev].timer_base;
    const unsigned long timer_ab = pwm_devs[dev].timer_side;
    
    const unsigned long ticks_low  = ROM_TimerLoadGet(timer_base, timer_ab);
    const unsigned long ticks_high = ROM_TimerPrescaleGet(timer_base, timer_ab);
    const unsigned long long ticks = (ticks_high<<16) | ticks_low;
    
    const unsigned long duty_ticks_low  = ROM_TimerMatchGet(timer_base, timer_ab);
    const unsigned long duty_ticks_high = ROM_TimerPrescaleMatchGet(timer_base, timer_ab);
    const unsigned long long duty = (duty_ticks_high<<16) | duty_ticks_low;

    DEBUG("MATCH Prescaler/Load set at 0x%lx/0x%lx\n", duty_ticks_high, duty_ticks_low);
    DEBUG("LOAD Prescaler/Load set at 0x%lx/0x%lx\n", ticks_high, ticks_low);
    DEBUG("LOAD duty cycle : %lu%%\n",
	  (unsigned long) ((unsigned long long)(ticks-duty) * 100ULL / ticks));
#endif
}


int pwm_set(pwm_t dev, int channel, unsigned int value)
{
    if (channel >= PWM_MAX_CHANNELS) {
        return -1;
    }

    const unsigned long timer_base = pwm_devs[dev].timer_base;
    const unsigned long timer_ab = pwm_devs[dev].timer_side;

    const unsigned long scaled_value = ((unsigned long long)pwm_settings[dev].ticks * (pwm_settings[dev].resolution - value)) / (unsigned long long)pwm_settings[dev].resolution;

    DEBUG("Set, scaled (for %u) %lu\n", value, scaled_value);
    const unsigned long ticks_high = (scaled_value & 0xFFFF0000) >> 16;
    const unsigned long ticks_low  =  scaled_value & 0x0000FFFF;

    dump_pwm(dev, channel);

    ROM_TimerPrescaleMatchSet(timer_base, timer_ab, ticks_high);
    ROM_TimerMatchSet(timer_base, timer_ab, ticks_low);

    dump_pwm(dev, channel);
    return 0;
}

void pwm_start(pwm_t dev)
{
    const unsigned long timer_base = pwm_devs[dev].timer_base;
    const unsigned long timer_ab = pwm_devs[dev].timer_side;

    ROM_TimerEnable(timer_base, timer_ab);
}

void pwm_stop(pwm_t dev)
{
    const unsigned long timer_base = pwm_devs[dev].timer_base;
    const unsigned long timer_ab = pwm_devs[dev].timer_side;

    ROM_TimerDisable(timer_base, timer_ab);
}

void pwm_poweron(pwm_t dev)
{
    const unsigned long timer_sysctl = pwm_devs[dev].timer_sysctl;
    ROM_SysCtlPeripheralEnable(timer_sysctl);
}

void pwm_poweroff(pwm_t dev)
{
    const unsigned long timer_sysctl = pwm_devs[dev].timer_sysctl;
    ROM_SysCtlPeripheralDisable(timer_sysctl);
}

#endif /* PWM_NUMOF */
