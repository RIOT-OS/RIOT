/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA

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
 *
 * @}
 */

#include <stdio.h>

#include <avr/interrupt.h>

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define GPIO_BASE_PORT_A        (0x20)
#define GPIO_OFFSET_PORT_H      (0xCB)
#define GPIO_OFFSET_PIN_PORT    (0x02)
#define GPIO_OFFSET_PIN_PIN     (0x03)

#ifdef MODULE_PERIPH_GPIO_IRQ
/*
 * @brief     Define GPIO interruptions for an specific atmega CPU, by default
 *            2 (for small atmega CPUs)
 */

#if defined(INT7_vect)
#define GPIO_EXT_INT_NUMOF      (8U)
#elif defined(INT6_vect)
#define GPIO_EXT_INT_NUMOF      (7U)
#elif defined(INT5_vect)
#define GPIO_EXT_INT_NUMOF      (6U)
#elif defined(INT4_vect)
#define GPIO_EXT_INT_NUMOF      (5U)
#elif defined(INT3_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT2_vect)
#define GPIO_EXT_INT_NUMOF      (3U)
#else
#define GPIO_EXT_INT_NUMOF      (2U)
#endif

static gpio_isr_ctx_t config[GPIO_EXT_INT_NUMOF];

/**
 * @brief detects ammount of possible PCINTs
 */
#if defined(AVR_USE_PCINT) && defined(AVR_PCINT_GPIO_LIST)
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

#ifdef GPIO_PC_INT_NUMOF

/**
 * @brief stores the last pcint state of each port
 */
static uint8_t pcint_state[GPIO_PC_INT_NUMOF / 8];

/**
 * @brief stores all cb and args for all defined pcint. 
 */
typedef struct {
    gpio_cb_t cb;           /**< interrupt callback */
    gpio_t pin;             /**< pin which the interrupt is for */
    gpio_flank_t flank;     /**< type of interrupt the flank should be triggered on */
    void *arg;              /**< optional argument */
} gpio_isr_ctx_pcint_t;

static gpio_t pcint_mapping[] = AVR_PCINT_GPIO_LIST; //wäre schöner wenn das nicht die ganze Zeit im Speicher liegt....
static gpio_isr_ctx_pcint_t pcint_config[ sizeof(pcint_mapping) / sizeof(gpio_t) ]; 
static int8_t pcint_lookup[ GPIO_PC_INT_NUMOF ] = { [0 ... (GPIO_PC_INT_NUMOF-1) ] = -1 };
#endif /* GPIO_PC_INT_NUMOF */


#endif /* MODULE_PERIPH_GPIO_IRQ */

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
    uint8_t pin_mask = (1 << _pin_num(pin));
    switch (mode) {
        case GPIO_OUT:
            _SFR_MEM8(_ddr_addr(pin)) |= pin_mask;
            break;
        case GPIO_IN:
            _SFR_MEM8(_ddr_addr(pin)) &= ~pin_mask;
            _SFR_MEM8(_port_addr(pin)) &= ~pin_mask;
            break;
        case GPIO_IN_PU:
            _SFR_MEM8(_ddr_addr(pin)) &= ~pin_mask;
            _SFR_MEM8(_port_addr(pin)) |= pin_mask;
            break;
        default:
            return -1;
    }

    return 0;
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

#ifdef MODULE_PERIPH_GPIO_IRQ
static inline int8_t _int_num(gpio_t pin)
{
    uint8_t num;
    const gpio_t ext_ints[GPIO_EXT_INT_NUMOF] = CPU_ATMEGA_EXT_INTS;

    /* find pin in ext_ints array to get the interrupt number */
    for (num = 0; num < GPIO_EXT_INT_NUMOF; num++) {
        if (pin == ext_ints[num]) {
            return num;
        }
    }

    return -1;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int8_t int_num = _int_num(pin);

    /* mode not supported */
    if ((mode != GPIO_IN) && (mode != GPIO_IN_PU)) {
        return -1;
    }

    /* not a valid interrupt pin. Set as pcint instead if pcints are enabled */
    if (int_num < 0) {
	    /* If pin change interrupts are enabled, enable mask and interrupt */
        #ifdef GPIO_PC_INT_NUMOF
        uint8_t broke = 0;
        uint8_t pin_num = _pin_num(pin);
        uint8_t mapping_size = sizeof(pcint_mapping) / sizeof(gpio_t);
        for(uint8_t i=0 ; i<mapping_size ; i++) {
            if((gpio_t)pin == (gpio_t)pcint_mapping[i]) {
                /// Found GPIO_PIN definition, and position
                pcint_lookup[ _port_num(pin) * 8 + pin_num ] = i;
                /* overwrite old information */
                pcint_config[i].pin     = pin;
                pcint_config[i].flank   = flank;
                pcint_config[i].arg     = arg;
                pcint_config[i].cb      = cb;
                broke = 1; 
                break;
            }
        }
        if(broke != 1)
        {
            return -1;
        }
        gpio_init(pin, mode);
        cli();
        /* set the right register values*/
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
        pcint_state[_port_num(pin)] = (_SFR_MEM8(_pin_addr( GPIO_PIN( _port_num(pin), pin_num ) )));
        sei();
        return 0;
        #endif /* GPIO_PC_INT_NUMOF */
        return -1;
    }

    /* flank not supported */
    if (flank > GPIO_RISING) {
        return -1;
    }

    gpio_init(pin, mode);

    /* clear global interrupt flag */
    cli();

    /* enable interrupt number int_num */
    EIFR |= (1 << int_num);
    EIMSK |= (1 << int_num);

    /* apply flank to interrupt number int_num */
    if (int_num < 4) {
        EICRA &= ~(0x3 << (int_num * 2));
        EICRA |= (flank << (int_num * 2));
    }
    #if defined(EICRB)
    else {
        EICRB &= ~(0x3 << ((int_num % 4) * 2));
        EICRB |= (flank << ((int_num % 4) * 2));
    }
    #endif

    /* set callback */
    config[int_num].cb = cb;
    config[int_num].arg = arg;

    /* set global interrupt flag */
    sei();

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    EIFR |= (1 << _int_num(pin));
    EIMSK |= (1 << _int_num(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    EIMSK &= ~(1 << _int_num(pin));
}

static inline void irq_handler(uint8_t int_num)
{
    __enter_isr();
    config[int_num].cb(config[int_num].arg);
    __exit_isr();
}

#ifdef GPIO_PC_INT_NUMOF
/* inline function that is used by the PCINT ISR */
static inline void pcint_handler(uint8_t port_num, volatile uint8_t *mask_reg)
{
    __enter_isr();
	//Find right item
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
            int8_t c = pcint_lookup[ port_num * 8 + pin_num ];
            if(c >= 0){
                uint8_t pin_mask = (1 << pin_num);
                gpio_flank_t flank = pcint_config[c].flank;
                if (flank == GPIO_BOTH || ((state & pin_mask) && flank == GPIO_RISING) || (!(state & pin_mask) && flank == GPIO_FALLING)) {
                    /* finally execute callback routine */
                    pcint_config[c].cb(pcint_config[c].arg);
                }
            }
			
        }
        change = change >> 1;
        pin_num++;
    }
    /* store current state */
    pcint_state[port_num] = state;
    __exit_isr();
}
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

#endif /* GPIO_PC_INT_NUMOF */

ISR(INT0_vect, ISR_BLOCK)
{
    irq_handler(0); /**< predefined interrupt pin */
}

ISR(INT1_vect, ISR_BLOCK)
{
    irq_handler(1); /**< predefined interrupt pin */
}

#if defined(INT2_vect)
ISR(INT2_vect, ISR_BLOCK)
{
    irq_handler(2); /**< predefined interrupt pin */
}
#endif

#if defined(INT3_vect)
ISR(INT3_vect, ISR_BLOCK)
{
    irq_handler(3); /**< predefined interrupt pin */
}
#endif

#if defined(INT4_vect)
ISR(INT4_vect, ISR_BLOCK)
{
    irq_handler(4); /**< predefined interrupt pin */
}
#endif

#if defined(INT5_vect)
ISR(INT5_vect, ISR_BLOCK)
{
    irq_handler(5); /**< predefined interrupt pin */
}
#endif

#if defined(INT6_vect)
ISR(INT6_vect, ISR_BLOCK)
{
    irq_handler(6); /**< predefined interrupt pin */
}
#endif

#if defined(INT7_vect)
ISR(INT7_vect, ISR_BLOCK)
{
    irq_handler(7); /**< predefined interrupt pin */
}
#endif

#endif /* MODULE_PERIPH_GPIO_IRQ */
