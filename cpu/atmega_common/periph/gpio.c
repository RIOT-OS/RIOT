/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA
 *               2017 TU Braunschweig, IBR
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation for ATmega family
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * @}
 */


#include <stdio.h>

#include <avr/interrupt.h>

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "board.h"

#define GPIO_BASE_PORT_A        (0x20)
#define GPIO_OFFSET_PORT_H      (0xCB)
#define GPIO_OFFSET_PIN_PORT    (0x02)
#define GPIO_OFFSET_PIN_PIN     (0x03)

/*
 * @brief     Define GPIO interruptions for an specific atmega CPU, by default
 *            2 (for small atmega CPUs)
 */
#if defined(INT2_vect)
#define GPIO_EXT_INT_NUMOF      (3U)
#elif defined(INT3_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT4_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT5_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT6_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT7_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#else
#define GPIO_EXT_INT_NUMOF      (2U)
#endif

#ifdef AVR_USE_PCINT
#if defined(PCINT3_vect)
#define GPIO_PC_INT_NUMOF       (32U)
#elif defined(PCINT2_vect)
#define GPIO_PC_INT_NUMOF       (24U)
#elif defined(PCINT1_vect)
#define GPIO_PC_INT_NUMOF       (16U)
#elif defined(PCINT0_vect)
#define GPIO_PC_INT_NUMOF       (8U)
#endif
#endif

/* holds the callback and argument for regular interrupts  */
static gpio_isr_ctx_t config[GPIO_EXT_INT_NUMOF];
#ifdef AVR_USE_PCINT
/* holds the callback and argument for pin change interrupts */
static gpio_isr_ctx_t pcint[GPIO_PC_INT_NUMOF];
/* stores the configured flank for the respective pcint */
static gpio_flank_t pcint_flank[GPIO_PC_INT_NUMOF];
/* stores the last state of each port */
static uint8_t pcint_state[GPIO_PC_INT_NUMOF / 8];
#endif

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
    return (pin >> 4) & 0x0f;
}

/**
 * @brief     Generate the PORTx address of the give pin.
 */
static inline uint16_t _port_addr(gpio_t pin)
{
    uint8_t port_num = _port_num(pin);
    uint16_t port_addr = port_num * GPIO_OFFSET_PIN_PIN;

    port_addr += GPIO_BASE_PORT_A;
    port_addr += GPIO_OFFSET_PIN_PORT;

#if defined (PORTG)
    if (port_num > PORT_G) {
        port_addr += GPIO_OFFSET_PORT_H;
    }
#endif
    return port_addr;
}

/**
 * @brief     Generate the DDRx address of the given pin
 */
static inline uint16_t _ddr_addr(gpio_t pin)
{
    return (_port_addr(pin) - 0x01);
}

/**
 * @brief     Generate the PINx address of the given pin.
 */
static inline uint16_t _pin_addr(gpio_t pin)
{
    return (_port_addr(pin) - 0x02);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    switch (mode) {
        case GPIO_OUT:
            _SFR_MEM8(_ddr_addr(pin)) |= (1 << _pin_num(pin));
            break;
        case GPIO_IN:
            _SFR_MEM8(_ddr_addr(pin)) &= ~(1 << _pin_num(pin));
            _SFR_MEM8(_port_addr(pin)) &= ~(1 << _pin_num(pin));
            break;
        case GPIO_IN_PU:
            _SFR_MEM8(_port_addr(pin)) |= (1 << _pin_num(pin));
            break;
        default:
            return -1;
    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    uint8_t pin_num = _pin_num(pin);
    uint8_t port_num = _port_num(pin);

    if ((_port_num(pin) == PORT_D && pin_num > 3)
#if defined (PORTE)
        || (_port_num(pin) == PORT_E && pin_num < 4)
        || (_port_num(pin) != PORT_D && _port_num(pin) != PORT_E)
#elif defined(CPU_ATMEGA328P)   /* INT0: PD2, INT1: PD3 */
        || (pin_num < 2) || (_port_num(pin) != PORT_D)
#elif defined(CPU_ATMEGA1284P)  /* INT0: PD2, INT1: PD3, INT2: PB2 */
        || ((pin_num < 2) && (_port_num(pin) == PORT_D))
        || ((pin_num < 2) && (_port_num(pin) == PORT_B))
        || ((pin_num > 2) && (_port_num(pin) == PORT_B))
        || (_port_num(pin) == PORT_C)
        || (_port_num(pin) == PORT_A)
#endif
        || ((mode != GPIO_IN) && (mode != GPIO_IN_PU))) {
        
        /* If pin change interrupts are enabled, enable mask and interrupt */
        #ifdef GPIO_PC_INT_NUMOF
        gpio_init(pin, mode);
        cli();
        switch (_port_num(pin)) {
            case 0:
                PCMSK0 |= (1 << pin_num);
                PCICR |= (1 << PCIE0);
                break;
            case 1:
                PCMSK1 |= (1 << pin_num);
                PCICR |= (1 << PCIE1);
                break;
#ifdef PCIE2
            case 2:
                PCMSK2 |= (1 << pin_num);
                PCICR |= (1 << PCIE2);
                break;
#endif
#ifdef PCIE3
            case 3:
                PCMSK3 |= (1 << pin_num);
                PCICR |= (1 << PCIE3);
                break;
#endif
            default:
                return -1;
                break;
        }
        /* set configuration */
        int pcint_num = _port_num(pin) * 8 + pin_num;
        pcint[pcint_num].cb = cb;
        pcint[pcint_num].arg = arg;
        pcint_flank[pcint_num] = flank;
        /* store current value of the port */
        pcint_state[_port_num(pin)] = (_SFR_MEM8(_pin_addr( GPIO_PIN( _port_num(pin), pin_num ) )));
        /* enable global interrupt flag */
        sei();
        return 0;
        #else /* if no pin change interrupts are used */
        return -1;
        #endif /* GPIO_PC_INT_NUMOF */
    }

    gpio_init(pin, mode);

    /* clear global interrupt flag */
    cli();

#if defined(CPU_ATMEGA328P)
    /* INT pins start at PD2 instead of at PD0 */
    pin_num -= 2;
#endif

    EIMSK |= (1 << pin_num);

    /* configure the flank */
    if (flank > GPIO_RISING) {
        return -1;
    }

    if (pin_num < 4) {
        EICRA |= (flank << (pin_num * 2));
    }
#if defined(EICRB)
    else {
        EICRB |= (flank << (pin_num * 2) % 4);
    }
#endif

    /* set callback */
    config[pin_num].cb = cb;
    config[pin_num].arg = arg;

    /* set global interrupt flag */
    sei();

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
#if defined(CPU_ATMEGA328P)
    /* INT pins start at PD2 instead of at PD0 */
    EIMSK |= (1 << (_pin_num(pin) - 2));
#else
    EIMSK |= (1 << _pin_num(pin));
#endif
}

void gpio_irq_disable(gpio_t pin)
{
#if defined(CPU_ATMEGA328P)
    /* INT pins start at PD2 instead of at PD0 */
    EIMSK &= ~(1 << (_pin_num(pin) - 2));
#else
    EIMSK &= ~(1 << _pin_num(pin));
#endif
}

int gpio_read(gpio_t pin)
{
    return (_SFR_MEM8(_pin_addr(pin)) & (1 << _pin_num(pin)));
}

void gpio_set(gpio_t pin)
{
    _SFR_MEM8(_port_addr(pin)) |= (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _SFR_MEM8(_port_addr(pin)) &= ~(1 << _pin_num(pin));
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

#ifdef GPIO_PC_INT_NUMOF
/* inline function that is used by the PCINT ISR */
static inline void pcint_handler(uint8_t port_num, volatile uint8_t *mask_reg)
{
    uint8_t pin_num = 0;
    /* calculate changed bits */
    uint8_t state = _SFR_MEM8(_pin_addr(GPIO_PIN(port_num, 1)));
    /* get pins that changed */
    uint8_t change = pcint_state[port_num] ^ state;

    /* apply mask to change */
    change &= *mask_reg;
    /* loop through all changed pins with enabled pcint */
    while (change > 0) {
        /* check if this pin is enabled & has changed */
        if (change & 0x1) {
            uint8_t pin_mask = (1 << pin_num);
            gpio_flank_t flank = pcint_flank[ port_num * 8 + pin_num ];
            /* trigger only on correct flank */
            if (flank == GPIO_BOTH || ((state & pin_mask) && flank == GPIO_RISING) || (!(state & pin_mask) && flank == GPIO_FALLING)) {
                /* finally execute callback routine */
                __enter_isr();
                pcint[port_num * 8 + pin_num].cb( pcint[port_num * 8 + pin_num].arg );
                __exit_isr();
            }
        }
        change = change >> 1;
        pin_num++;
    }

    /* store current state */
    pcint_state[port_num] = state;
}

#ifndef AVR_CONTEXT_SWAP_INTERRUPT_VECT_NUM
#error gpio.c requires the definition of AVR_CONTEXT_SWAP_INTERRUPT_VECT_NUM
#endif

/*
 * PCINT0 is always defined, if GPIO_PC_INT_NUMOF is defined
 */
#if PCINT0_vect_num != AVR_CONTEXT_SWAP_INTERRUPT_VECT_NUM
ISR(PCINT0_vect, ISR_BLOCK) {
    pcint_handler(0, &PCMSK0);
}
#endif /* AVR_CONTEXT_SWAP_INTERRUPT_VECT */
#if defined(PCINT1_vect)
#if PCINT1_vect_num != AVR_CONTEXT_SWAP_INTERRUPT_VECT_NUM
ISR(PCINT1_vect, ISR_BLOCK) {
    pcint_handler(1, &PCMSK1);
}
#endif  /* AVR_CONTEXT_SWAP_INTERRUPT_VECT */
#endif  /* PCINT1_vect */
#if defined(PCINT2_vect)
#if PCINT2_vect_num != AVR_CONTEXT_SWAP_INTERRUPT_VECT_NUM
ISR(PCINT2_vect, ISR_BLOCK) {
    pcint_handler(2, &PCMSK2);
}
#endif  /* AVR_CONTEXT_SWAP_INTERRUPT_VECT */
#endif  /* PCINT2_vect */
#if defined(PCINT3_vect)
#if PCINT3_vect_num != AVR_CONTEXT_SWAP_INTERRUPT_VECT_NUM
ISR(PCINT3_vect, ISR_BLOCK) {
    pcint_handler(3, &PCMSK3);
}
#endif  /* AVR_CONTEXT_SWAP_INTERRUPT_VECT */
#endif  /* PCINT3_vect */
#endif  /* GPIO_PC_INT_NUMOF */

static inline void irq_handler(uint8_t pin_num)
{
    __enter_isr();
    config[pin_num].cb(config[pin_num].arg);
    __exit_isr();
}

ISR(INT0_vect, ISR_BLOCK){
    irq_handler(0); /**< predefined interrupt pin */
}

ISR(INT1_vect, ISR_BLOCK){
    irq_handler(1); /**< predefined interrupt pin */
}

#if defined(INT2_vect)
ISR(INT2_vect, ISR_BLOCK){
    irq_handler(2); /**< predefined interrupt pin */
}
#endif

#if defined(INT3_vect)
ISR(INT3_vect, ISR_BLOCK){
    irq_handler(3); /**< predefined interrupt pin */
}
#endif

#if defined(INT4_vect)
ISR(INT4_vect, ISR_BLOCK){
    irq_handler(4); /**< predefined interrupt pin */
}
#endif

#if defined(INT5_vect)
ISR(INT5_vect, ISR_BLOCK){
    irq_handler(5); /**< predefined interrupt pin */
}
#endif

#if defined(INT6_vect)
ISR(INT6_vect, ISR_BLOCK){
    irq_handler(6); /**< predefined interrupt pin */
}
#endif

#if defined(INT7_vect)
ISR(INT7_vect, ISR_BLOCK){
    irq_handler(7); /**< predefined interrupt pin */
}
#endif
