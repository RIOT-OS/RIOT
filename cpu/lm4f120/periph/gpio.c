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
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Marc Poulhiès <dkm@kataplop.net>
 *
 * @}
 */


#include <stdio.h>

#include "cpu.h"
#include "cpu_conf.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief     Extract the pin number of the given pin
 */
static inline uint8_t _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

/**
 * @brief     Extract the port number of the given pin
 */
static inline uint8_t _port_num(gpio_t pin)
{
    return (pin >> 4);
}

static uint32_t _sysctl_port_base[] = {
  SYSCTL_PERIPH_GPIOA,
  SYSCTL_PERIPH_GPIOB,
  SYSCTL_PERIPH_GPIOC,
  SYSCTL_PERIPH_GPIOD,
  SYSCTL_PERIPH_GPIOE,
  SYSCTL_PERIPH_GPIOF,
};

static uint32_t _port_base[] = {
  GPIO_PORTA_BASE,
  GPIO_PORTB_BASE,
  GPIO_PORTC_BASE,
  GPIO_PORTD_BASE,
  GPIO_PORTE_BASE,
  GPIO_PORTF_BASE,
};

static uint32_t _int_assign[] = {
  INT_GPIOA,
  INT_GPIOB,
  INT_GPIOC,
  INT_GPIOD,
  INT_GPIOE,
  INT_GPIOF,
};

#define NUM_OF_PORT 6
#define NUM_OF_PINS 8

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_state_t;

static gpio_state_t gpio_config[NUM_OF_PORT][NUM_OF_PINS] = {{{0}}};

static inline uint16_t _port_addr(gpio_t pin)
{
    uint8_t port_num = _port_num(pin);
    uint32_t port_addr = _port_base[port_num];

    return port_addr;
}

int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pullup)
{
  uint8_t port_num = _port_num(pin);
  uint32_t port_addr = _port_base[port_num];
  uint8_t pin_num = _pin_num(pin);
  uint32_t sysctl_port_base = _sysctl_port_base[port_num];
  unsigned long pin_bit = 1 << pin_num;

  DEBUG("Init GPIO: port %c, %d\n", 'A' + port_num, pin_num);
  DEBUG("Sysctl %" PRIx32 "\n", sysctl_port_base);

  ROM_SysCtlPeripheralEnable(sysctl_port_base);

  HWREG(port_addr+GPIO_LOCK_R_OFF) = GPIO_LOCK_KEY;
  HWREG(port_addr+GPIO_CR_R_OFF) |=  pin_bit;
  HWREG(port_addr+GPIO_DEN_R_OFF) |= pin_bit;
  HWREG(port_addr+GPIO_LOCK_R_OFF) = 0;

  /* GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY; */
  /* GPIO_PORTD_CR_R &= ~(1<<pin_num);		 */

  unsigned long pad_config;
  switch(pullup){
  case GPIO_PULLDOWN:
    pad_config = GPIO_PIN_TYPE_STD_WPD;
    break;
  case GPIO_PULLUP:
    pad_config = GPIO_PIN_TYPE_STD_WPU;
    break;
  case GPIO_NOPULL:
  default:
    pad_config = GPIO_PIN_TYPE_STD;
    break;
  }

  ROM_GPIOPadConfigSet(port_addr, pin_bit,
		       GPIO_STRENGTH_2MA, pad_config);

  switch(dir){
  case GPIO_DIR_IN:
    ROM_GPIODirModeSet(port_addr, pin_bit, GPIO_DIR_MODE_IN);

    /* ROM_GPIOPinTypeGPIOInput(port_addr, 1<<pin_num); */
    //      ROM_GPIODirModeSet(port_addr, 1<<pin_num, GPIO_DIR_MODE_IN);
      break;
  case GPIO_DIR_OUT:
    ROM_GPIODirModeSet(port_addr, pin_bit, GPIO_DIR_MODE_OUT);
    //    ROM_GPIOPinTypeGPIOOutput(port_addr, 1<<pin_num);
      break;
  }
  //  _LOCK_PORT;
  //  _UNLOCK_PORT;
  //  GPIO_PORTD_LOCK_R = 0;
  return 0;
}

static void _isr_gpio(uint32_t port_num){
  uint32_t port_addr = _port_base[port_num];
  uint32_t isr = ROM_GPIOPinIntStatus(port_addr, true);
  uint8_t i;

  ROM_GPIOPinIntClear(port_addr, isr);

  for (i=0; i<8; i++, isr>>=1) {
    if ((isr & 0x1) == 0)
      continue;

    if (gpio_config[port_num][i].cb)
      gpio_config[port_num][i].cb(gpio_config[port_num][i].arg);
  }
}

void isr_gpio_porta(void){
  _isr_gpio(0);
}
void isr_gpio_portb(void){
  _isr_gpio(1);
}
void isr_gpio_portc(void){
  _isr_gpio(2);
}
void isr_gpio_portd(void){
  _isr_gpio(3);
}
void isr_gpio_porte(void){
  _isr_gpio(4);
}
void isr_gpio_portf(void){
  _isr_gpio(5);
}

static void lm4f120_gpio_int_clear(uint32_t icr_reg_addr, uint32_t pin_mask){
    HWREG(icr_reg_addr) = pin_mask;
}

int gpio_init_int(gpio_t pin, gpio_pp_t pullup, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
  uint8_t port_num = _port_num(pin);
  uint32_t port_addr = _port_base[port_num];
  uint32_t icr_reg_addr = port_addr + GPIO_ICR_R_OFF;
  uint8_t pin_num = _pin_num(pin);
  uint8_t pin_bit = 1<<pin_num;
  uint32_t lm4fMode = 0;
  unsigned int int_num = _int_assign[port_num];

  gpio_config[port_num][pin_num].cb = cb;
  gpio_config[port_num][pin_num].arg = arg;

  DEBUG("init int pin:%d, int num %d, port addr %" PRIx32 "\n",
	pin_num, int_num, port_addr);

  switch(flank) {
  /* case LOW: */
  /* 		lm4fMode = GPIO_LOW_LEVEL; */
  /* 		break; */
  case GPIO_BOTH:
    lm4fMode = GPIO_BOTH_EDGES;
    break;
  case GPIO_RISING:
    lm4fMode = GPIO_RISING_EDGE;
    break;
  case GPIO_FALLING:
    lm4fMode = GPIO_FALLING_EDGE;
    break;
  }

  unsigned long pull_val = GPIO_PIN_TYPE_STD;

  switch(pullup){
  case GPIO_PULLDOWN:
    pull_val = GPIO_PIN_TYPE_STD_WPD;
    break;
  case GPIO_PULLUP:
    pull_val = GPIO_PIN_TYPE_STD_WPU;
    break;
  case GPIO_NOPULL:
    pull_val = GPIO_PIN_TYPE_STD;
    break;
  }

  ROM_GPIODirModeSet(port_addr, 1<<pin_num, GPIO_DIR_MODE_IN);
  ROM_GPIOPadConfigSet(port_addr, 1<<pin_num,
		       GPIO_STRENGTH_2MA, pull_val);

  ROM_IntMasterDisable();

  lm4f120_gpio_int_clear(icr_reg_addr, pin_bit);
  ROM_GPIOIntTypeSet(port_addr, pin_bit, lm4fMode);

  HWREG(port_addr+GPIO_LOCK_R_OFF) = GPIO_LOCK_KEY;
  HWREG(port_addr+GPIO_CR_R_OFF) |=  pin_bit;
  HWREG(port_addr+GPIO_DEN_R_OFF) |= pin_bit;
  HWREG(port_addr+GPIO_LOCK_R_OFF) = 0;

  gpio_irq_enable(pin);
  ROM_IntEnable(int_num);

  ROM_IntMasterEnable();

  return 0;
}

void gpio_irq_enable(gpio_t pin)
{
  uint8_t port_num = _port_num(pin);
  uint32_t port_addr = _port_base[port_num];

  uint32_t im_reg_addr =  port_addr + GPIO_IM_R_OFF;
  uint8_t pin_num = _pin_num(pin);
  uint8_t pin_bit = 1<<pin_num;
  HWREG(im_reg_addr) |= pin_bit;
}

void gpio_irq_disable(gpio_t pin)
{
  uint8_t port_num = _port_num(pin);
  uint32_t port_addr = _port_base[port_num];

  uint32_t im_reg_addr =  port_addr + GPIO_IM_R_OFF;
  uint8_t pin_num = _pin_num(pin);
  uint8_t pin_bit = 1<<pin_num;

  HWREG(im_reg_addr) &= ~(pin_bit);
}


int gpio_read(gpio_t pin)
{
  uint8_t port_num = _port_num(pin);
  uint32_t port_addr = _port_base[port_num];
  uint8_t pin_num = _pin_num(pin);

  if(ROM_GPIOPinRead(port_addr, 1<<pin_num)){
    return 1;
  }
  return 0;

}

void gpio_set(gpio_t pin)
{
  uint8_t port_num = _port_num(pin);
  uint32_t port_addr = _port_base[port_num];
  uint8_t pin_num = _pin_num(pin);
  DEBUG("Setting bit %d of port %c\n", pin_num, 'A' + port_num);
  DEBUG("Port addr %" PRIx32 ", vs %x\n", port_addr,  GPIO_PORTF_BASE);
  ROM_GPIOPinWrite(port_addr, 1<<pin_num, 1<<pin_num);
}

void gpio_clear(gpio_t pin)
{
  uint8_t port_num = _port_num(pin);
  uint32_t port_addr = _port_base[port_num];
  uint8_t pin_num = _pin_num(pin);

  ROM_GPIOPinWrite(port_addr, 1<<pin_num, 0);
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    }
    else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}
