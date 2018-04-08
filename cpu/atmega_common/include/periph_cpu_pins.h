/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       CPU specific definitions for internal pin handling
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#define GPIO_BASE_PORT_A        (0x20)
#define GPIO_OFFSET_PORT_H      (0xCB)
#define GPIO_OFFSET_PIN_PORT    (0x02)
#define GPIO_OFFSET_PIN_PIN     (0x03)

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
 * @brief     Extract the PCIEx number of the given pin (for PCICR reg)
 */
static inline uint8_t _pcicr_num(gpio_t pin)
{
    switch (_port_num(pin)) {
#if defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA2560)
        case PORT_B: return PCIE0;
        case PORT_E: return PCIE1;
        case PORT_J: return PCIE1;
        case PORT_K: return PCIE2;
#elif defined(CPU_ATMEGA1284P)
        case PORT_A: return PCIE0;
        case PORT_B: return PCIE1;
        case PORT_C: return PCIE2;
        case PORT_D: return PCIE3;
#elif defined(CPU_ATMEGA256RFR2)
        case PORT_B: return PCIE0;
        case PORT_E: return PCIE1;
#elif defined(CPU_ATMEGA328P)
        case PORT_B: return PCIE0;
        case PORT_C: return PCIE1;
        case PORT_D: return PCIE2;
#endif
    }
}

/**
 * @brief     Generate the PCMSKx address of the given pin
 */
static inline uint16_t _pcmsk_addr(gpio_t pin)
{
    switch (_pcicr_num(pin)) {
        case PCIE0: return PCMSK0;
        case PCIE1: return PCMSK1;
        case PCIE2: return PCMSK2;
#ifdef PCMSK3
        case PCIE3: return PCMSK3;
#endif
    }
}

/**
 * @brief     Extract the PCINTxx number of the given pin (for PCMSKx reg)
 */
static inline uint8_t _pcint_num(gpio_t pin)
{
#if defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA2560)
    if (_port_num(pin) == PORT_J) {
        return _pin_num(pin) + 1;
    } else {
        return _pin_num(pin);
    }
#else
    return _pin_num(pin);
#endif
}
