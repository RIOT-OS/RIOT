/*
 * Copyright(C) 2017 Imagination Technologies Limited and/or its
 *              affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

 /**
  * @ingroup     cpu_mips_pic32_common
  * @ingroup     drivers_periph_gpio
  * @{
  *
  * @file
  * @brief       Low-level GPIO driver implementation
  *
  * @}
  */

#include "cpu.h"
#include "eic.h"
#include "bitarithm.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define LATx(P)         ((P)[0x30/0x4])
#define LATxCLR(P)      ((P)[0x34/0x4])
#define LATxSET(P)      ((P)[0x38/0x4])
#define LATxINV(P)      ((P)[0x3C/0x4])
#define PORTx(P)        ((P)[0x20/0x4])
#define CNPUxCLR(P)     ((P)[0x54/0x4])
#define CNPUxSET(P)     ((P)[0x58/0x4])
#define CNPDxCLR(P)     ((P)[0x64/0x4])
#define CNPDxSET(P)     ((P)[0x68/0x4])
#define ODCxCLR(P)      ((P)[0x44/0x4])
#define ODCxSET(P)      ((P)[0x48/0x4])
#define ANSELxCLR(P)    ((P)[0x04/0x4])
#define TRISxCLR(P)     ((P)[0x14/0x4])
#define TRISxSET(P)     ((P)[0x18/0x4])
#define CNENxCLR(P)     ((P)[0x84/0x4])
#define CNENxSET(P)     ((P)[0x88/0x4])
#define CNNExCLR(P)     ((P)[0xA4/0x4])
#define CNNExSET(P)     ((P)[0xA8/0x4])
#define CNSTATx(P)      ((P)[0x90/0x4])
#define CNCONxCLR(P)    ((P)[0x74/0x4])
#define CNCONxSET(P)    ((P)[0x78/0x4])
#define CNFx(P)         ((P)[0xB0/0x4])

/**
 * @brief   Extract the port base address from the given pin identifier
 */
static inline volatile unsigned int * _port(gpio_t pin)
{
    return (volatile unsigned int *)(pin & ~(0xff));
}

/**
 * @brief   Extract the port number form the given identifier
 *
 * The port number is extracted by looking at bits 8, 9, 10, 11 of the base
 * register addresses.
 */
static inline int _port_num(gpio_t pin)
{
    return ((pin >> 8) & 0x0f);
}

/**
 * @brief   Extract the pin number from the last 4 bit of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief   The PIC32 family has 7 I/O ports and 16 I/O per port
 */
#define PORT_NUMOF          (7U)
#define GPIO_NUMOF          (16U)

static gpio_flank_t isr_flank[PORT_NUMOF][GPIO_NUMOF];
static gpio_isr_ctx_t isr_ctx[PORT_NUMOF][GPIO_NUMOF];

#if defined(CPU_FAM_PIC32MX)
static void isr_handler(uint32_t port_addr)
{
    uint32_t cnstat = CNSTATx(_port(port_addr));
    uint32_t port = PORTx(_port(port_addr));

    cnstat &= (1 << GPIO_NUMOF) - 1;
    uint8_t pin = 0;
    while (cnstat) {
        cnstat = bitarithm_test_and_clear(cnstat, &pin);

        if (isr_flank[_port_num(port_addr)][pin] == GPIO_BOTH
        || (isr_flank[_port_num(port_addr)][pin] == GPIO_RISING && (port & (1U << pin)))
        || (isr_flank[_port_num(port_addr)][pin] == GPIO_FALLING && !(port & (1U << pin))))
            isr_ctx[_port_num(port_addr)][pin].cb(isr_ctx[_port_num(port_addr)][pin].arg);
    }
}

static void cn_isr(void)
{
#ifdef _PORTA_BASE_ADDRESS
    isr_handler(_PORTA_BASE_ADDRESS);
#endif
#ifdef _PORTB_BASE_ADDRESS
    isr_handler(_PORTB_BASE_ADDRESS);
#endif
#ifdef _PORTC_BASE_ADDRESS
    isr_handler(_PORTC_BASE_ADDRESS);
#endif
#ifdef _PORTD_BASE_ADDRESS
    isr_handler(_PORTD_BASE_ADDRESS);
#endif
#ifdef _PORTE_BASE_ADDRESS
    isr_handler(_PORTE_BASE_ADDRESS);
#endif
#ifdef _PORTF_BASE_ADDRESS
    isr_handler(_PORTF_BASE_ADDRESS);
#endif
#ifdef _PORTG_BASE_ADDRESS
    isr_handler(_PORTG_BASE_ADDRESS);
#endif

    mips32r2_isr_end();
}
#elif defined(CPU_FAM_PIC32MZ)
static void isr_handler(uint32_t port_addr)
{
    uint8_t pin = 0;
    unsigned state = CNFx(_port(port_addr));
    CNFx(_port(port_addr)) = 0;
    while (state) {
        state = bitarithm_test_and_clear(state, &pin);
        isr_ctx[_port_num(port_addr)][pin].cb(isr_ctx[_port_num(port_addr)][pin].arg);
    }
}

static void cn_porta_isr(void)
{
    isr_handler(_PORTA_BASE_ADDRESS);
    mips32r2_isr_end();
}

static void cn_portb_isr(void)
{
    isr_handler(_PORTB_BASE_ADDRESS);
    mips32r2_isr_end();
}

static void cn_portc_isr(void)
{
    isr_handler(_PORTC_BASE_ADDRESS);
    mips32r2_isr_end();
}

static void cn_portd_isr(void)
{
    isr_handler(_PORTD_BASE_ADDRESS);
    mips32r2_isr_end();
}

static void cn_porte_isr(void)
{
    isr_handler(_PORTE_BASE_ADDRESS);
    mips32r2_isr_end();
}

static void cn_portf_isr(void)
{
    isr_handler(_PORTF_BASE_ADDRESS);
    mips32r2_isr_end();
}

static void cn_portg_isr(void)
{
    isr_handler(_PORTG_BASE_ADDRESS);
    mips32r2_isr_end();
}
#endif
#endif /* MODULE_PERIPH_GPIO_IRQ */

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    volatile unsigned int * port = _port(pin);
    int pin_num = _pin_num(pin);
    uint8_t output = 0, pu = 0, pd = 0, od = 0;

    switch (mode) {
        case GPIO_IN:
            break;
        case GPIO_IN_PD:
            pd = 1;
            break;
        case GPIO_IN_PU:
            pu = 1;
            break;
        case GPIO_OD_PU:
            pu = 1; /* fall-through */
        case GPIO_OD:
            od = 1; /* fall-through */
        case GPIO_OUT:
            output = 1;
            break;
    }

    ANSELxCLR(port) = 1U << pin_num;       /* Configure GPIO as digital */

    if (pu)
        CNPUxSET(port) = 1U << pin_num;
    else
        CNPUxCLR(port) = 1U << pin_num;

    if (pd)
        CNPDxSET(port) = 1U << pin_num;
    else
        CNPDxCLR(port) = 1U << pin_num;

    if (od)
        ODCxSET(port) = 1U << pin_num;
    else
        ODCxCLR(port) = 1U << pin_num;

    if (output)
        TRISxCLR(port) = 1U << pin_num;
    else
        TRISxSET(port) = 1U << pin_num;

    return 0;
}

int gpio_read(gpio_t pin)
{
    return PORTx(_port(pin)) & (1U << _pin_num(pin));
}

void gpio_set(gpio_t pin)
{
    LATxSET(_port(pin)) = 1U << _pin_num(pin);
}

void gpio_clear(gpio_t pin)
{
    LATxCLR(_port(pin)) = 1U << _pin_num(pin);
}

void gpio_toggle(gpio_t pin)
{
    LATxINV(_port(pin)) = 1U << _pin_num(pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value)
        gpio_set(pin);
    else
        gpio_clear(pin);
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    int pin_num = _pin_num(pin);
    int port_num = _port_num(pin);

    /* set callback */
    isr_ctx[port_num][pin_num].cb = cb;
    isr_ctx[port_num][pin_num].arg = arg;
    isr_flank[port_num][pin_num] = flank;

    /* initialize pin as input */
    gpio_init(pin, mode);

#if defined(CPU_FAM_PIC32MX)
    set_external_isr_cb(_CHANGE_NOTICE_VECTOR, cn_isr);
    eic_configure_priority(_CHANGE_NOTICE_VECTOR, 1, 0);
    eic_enable(_CHANGE_NOTICE_A_IRQ + port_num);
#elif defined(CPU_FAM_PIC32MZ)
    switch (port_num) {
    case PORT_A: set_external_isr_cb(_CHANGE_NOTICE_A_VECTOR, cn_porta_isr); break;
    case PORT_B: set_external_isr_cb(_CHANGE_NOTICE_B_VECTOR, cn_portb_isr); break;
    case PORT_C: set_external_isr_cb(_CHANGE_NOTICE_C_VECTOR, cn_portc_isr); break;
    case PORT_D: set_external_isr_cb(_CHANGE_NOTICE_D_VECTOR, cn_portd_isr); break;
    case PORT_E: set_external_isr_cb(_CHANGE_NOTICE_E_VECTOR, cn_porte_isr); break;
    case PORT_F: set_external_isr_cb(_CHANGE_NOTICE_F_VECTOR, cn_portf_isr); break;
    case PORT_G: set_external_isr_cb(_CHANGE_NOTICE_G_VECTOR, cn_portg_isr); break;
    }
    eic_configure_priority(_CHANGE_NOTICE_A_VECTOR + port_num, 1, 0);
    eic_enable(_CHANGE_NOTICE_A_VECTOR + port_num);
    CNCONxSET(_port(pin)) = _CNCONB_EDGEDETECT_MASK;
#endif
    CNCONxSET(_port(pin)) = _CNCONB_ON_MASK;
    gpio_irq_enable(pin);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
#if defined(CPU_FAM_PIC32MX)
    CNENxSET(_port(pin)) = 1U << _pin_num(pin);
#elif defined(CPU_FAM_PIC32MZ)
    switch (isr_flank[_port_num(pin)][_pin_num(pin)]) {
    case GPIO_RISING:
        CNENxSET(_port(pin)) = 1U << _pin_num(pin);
        CNNExCLR(_port(pin)) = 1U << _pin_num(pin);
        break;
    case GPIO_FALLING:
        CNENxCLR(_port(pin)) = 1U << _pin_num(pin);
        CNNExSET(_port(pin)) = 1U << _pin_num(pin);
        break;
    case GPIO_BOTH:
        CNENxSET(_port(pin)) = 1U << _pin_num(pin);
        CNNExSET(_port(pin)) = 1U << _pin_num(pin);
        break;
    }
#endif
}

void gpio_irq_disable(gpio_t pin)
{
    CNENxCLR(_port(pin)) = 1U << _pin_num(pin);
#if defined(CPU_FAM_PIC32Mz)
    CNNExCLR(_port(pin)) = 1U << _pin_num(pin);
#endif
}

#endif /* MODULE_PERIPH_GPIO_IRQ */
