//------------------------------------------------------------------------------
//
//  Copyright (C) 2017
//
//  Module      : IRQ
//  File        : s5p4418_gpio.h
//  Description : S5P4418 GPIO
//  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)
//  History     :
//------------------------------------------------------------------------------

#ifndef __S5P4418_GPIO_H__
#define __S5P4418_GPIO_H__

#include <stdint.h>
#include <stdbool.h>
#include "s5p4418_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* GPIO port */
#define PORTA (0)
#define PORTB (1)
#define PORTC (2)
#define PORTD (3)
#define PORTE (4)

// Convert (port, pin) to GPIO number
#define TO_GPIO(port, pin) (32*(port)+(pin))

// Convert GPIO number to port
#define TO_PORT(gpio) ((gpio)/32)

// Convert GPIO number to pin
#define TO_PIN(gpio) ((gpio)%32)

// Alternative mask
#define ALT_MASK  (0x3)

// Event type mask
#define EVT_MASK  (0x7)

// GPIO alternative function mode
typedef enum
{
    GPIO_ALT0 = 0,
    GPIO_ALT1 = 1,
    GPIO_ALT2 = 2,
    GPIO_ALT3 = 3,
}gpio_alt_t;

// GPIO direction mode
typedef enum
{
    GPIO_DIR_IN  = 0,
    GPIO_DIR_OUT = 1,
}gpio_dir_t;

// GPIO pull up/down mode
typedef enum
{
    GPIO_PULL_UP   = 0,
    GPIO_PULL_DOWN = 1,
    GPIO_PULL_NONE = 2,
}gpio_pull_t;

// GPIO drive strength
typedef enum
{
    GPIO_DRV_LOW    = 0,
    GPIO_DRV_MEDIUM = 1,
    GPIO_DRV_HIGH   = 2,
}gpio_drv_t;

// GPIO slew rate
typedef enum
{
    GPIO_RATE_FAST = 0,
    GPIO_RATE_NORM = 1,
}gpio_rate_t;


// GPIO IRQ event detect
typedef enum
{
    GPIO_IRQ_LOW     = 0,
    GPIO_IRQ_HIGH    = 1,
    GPIO_IRQ_FALLING = 2,
    GPIO_IRQ_RISING  = 3,
    GPIO_IRQ_BOTH    = 4,
}gpio_evt_t;

// GPIO property
typedef struct
{
    uint8_t     gpio;
    gpio_alt_t  alt;
    gpio_dir_t  dir;
    gpio_pull_t pull;
    gpio_drv_t  drv;
    gpio_rate_t rate;
}gpio_info_t;

// GPIO IRQ type
typedef struct
{
    const char *port_name;
    uint8_t irq;
    bool port_irq_registered;
    void (*port_irq_handler)(void);
    void (*pin_irq_handler[32])(uint8_t gpio);
}gpio_irq_t;

// GPIO register set
typedef struct
{
    volatile uint32_t GPIOxOUT;                      // 0x000    // GPIOx output register
    volatile uint32_t GPIOxOUTENB;                   // 0x004    // GPIOx output enable register
    volatile uint32_t GPIOxDETMODE[2];               // 0x008    // GPIOx event detect mode register
    volatile uint32_t GPIOxINTENB;                   // 0x010    // GPIOx interrupt enable register
    volatile uint32_t GPIOxDET;                      // 0x014    // GPIOx event detect register
    volatile uint32_t GPIOxPAD;                      // 0x018    // GPIOx PAD status register
    volatile uint32_t Reserved1[1];                  // 0x01C    // Reverved
    volatile uint32_t GPIOxALTFN[2];                 // 0x020    // GPIOx alternate function select register
    volatile uint32_t GPIOxDETMODEEX;                // 0x028    // GPIOx event detect mode extended register
    volatile uint32_t Reserved2[4];                  // 0x02C    // Reverved
    volatile uint32_t GPIOxDETENB;                   // 0x03C    // GPIOx detect enable register
    volatile uint32_t GPIOx_SLEW;                    // 0x040    // GPIOx slew register
    volatile uint32_t GPIOx_SLEW_DISABLE_DEFAULT;    // 0x044    // GPIOx slew disable default register
    volatile uint32_t GPIOx_DRV1;                    // 0x048    // GPIOx DRV1 register
    volatile uint32_t GPIOx_DRV1_DISABLE_DEFAULT;    // 0x04C    // GPIOx DRV1 disable default register
    volatile uint32_t GPIOx_DRV0;                    // 0x050    // GPIOx DRV0 register
    volatile uint32_t GPIOx_DRV0_DISABLE_DEFAULT;    // 0x054    // GPIOx DRV0 disable default register
    volatile uint32_t GPIOx_PULLSEL;                 // 0x058    // GPIOx PULLSEL register
    volatile uint32_t GPIOx_PULLSEL_DISABLE_DEFAULT; // 0x05C    // GPIOx PULLSEL disable default register
    volatile uint32_t GPIOx_PULLENB;                 // 0x060    // GPIOx PULLENB register
    volatile uint32_t GPIOx_PULLENB_DISABLE_DEFAULT; // 0x064    // GPIOx PULLENB disable default register
}gpio_typedef;

/* GPIO bank register base definition */
#define GPIOA ((gpio_typedef *) S5P4418_GPIOA_BASE)
#define GPIOB ((gpio_typedef *) S5P4418_GPIOB_BASE)
#define GPIOC ((gpio_typedef *) S5P4418_GPIOC_BASE)
#define GPIOD ((gpio_typedef *) S5P4418_GPIOD_BASE)
#define GPIOE ((gpio_typedef *) S5P4418_GPIOE_BASE)

void s5p4418_gpio_set_alt(uint8_t gpio, gpio_alt_t alt);
void s5p4418_gpio_set_dir(uint8_t gpio, gpio_dir_t dir);
void s5p4418_gpio_set_pull(uint8_t gpio, gpio_pull_t pull);
void s5p4418_gpio_set_drv(uint8_t gpio, gpio_drv_t drv);
void s5p4418_gpio_set_rate(uint8_t gpio, gpio_rate_t rate);
void s5p4418_gpio_config(gpio_info_t gpio_info);
void s5p4418_gpio_set(uint8_t gpio, bool val);
bool s5p4418_gpio_get(uint8_t gpio);
bool s5p4418_gpio_irq_register(uint8_t gpio, gpio_evt_t evt, void(*cbfunc)(uint8_t gpio));
void s5p4418_gpio_irq_unregister(uint8_t gpio);
void s5p4418_gpio_irq_enable(uint8_t gpio);
void s5p4418_gpio_irq_disable(uint8_t gpio);


#ifdef __cplusplus
}
#endif


#endif // __S5P4418_GPIO_H__
