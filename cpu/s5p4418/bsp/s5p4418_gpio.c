//------------------------------------------------------------------------------
//
//  Copyright (C) 2017
//
//  Module      : IRQ
//  File        : s5p4418_gpio.c
//  Description : S5P4418 GPIO
//  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)
//  History     :
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "s5p4418_irq.h"
#include "s5p4418_gpio.h"

static void portA_irq_handler(void);
static void portB_irq_handler(void);
static void portC_irq_handler(void);
static void portD_irq_handler(void);
static void portE_irq_handler(void);

// Define GPIO IRQ priority level
#define GPIO_IRQ_PRIORITY IRQ_PRIORITY_LEVEL_15

// GPIO base address table
const gpio_typedef *port_base_addr_table[5] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

// GPIO IRQ table definition
static gpio_irq_t gpio_irq_table[] =
{
    {
        .port_name           = "GPIOA",
        .irq                 = 53,
        .port_irq_registered = false,
        .port_irq_handler    = &portA_irq_handler,
        .pin_irq_handler     = {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
    },
    {
        .port_name           = "GPIOB",
        .irq                 = 54,
        .port_irq_registered = false,
        .port_irq_handler    = &portB_irq_handler,
        .pin_irq_handler     = {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
    },
    {
        .port_name           = "GPIOC",
        .irq                 = 55,
        .port_irq_registered = false,
        .port_irq_handler    = &portC_irq_handler,
        .pin_irq_handler     = {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
    },
    {
        .port_name           = "GPIOD",
        .irq                 = 56,
        .port_irq_registered = false,
        .port_irq_handler    = &portD_irq_handler,
        .pin_irq_handler     = {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
    },
    {
        .port_name           = "GPIOE",
        .irq                 = 57,
        .port_irq_registered = false,
        .port_irq_handler    = &portE_irq_handler,
        .pin_irq_handler     = {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
    },
};

/**
 *  @brief      PORTA IRQ Handler
 *  @param[in]  None
 *  @return     None.
 */
static void portA_irq_handler(void)
{
    uint8_t gpio_irq_pin;

    /* Find GPIO IRQ pin */
    gpio_irq_pin = __builtin_ctzl(GPIOA->GPIOxDET);

    /* Execute context call back function */
    if (gpio_irq_table[PORTA].pin_irq_handler[gpio_irq_pin] != NULL)
    {
       /* Clear GPIO IRQ flag */
       GPIOA->GPIOxDET |= (1 << gpio_irq_pin);

       /* Allow reentrant IRQ */
       __asm__ __volatile__("cpsie i");

       /* Call gpio pin IRQ handler */
       gpio_irq_table[PORTA].pin_irq_handler[gpio_irq_pin](TO_GPIO(PORTA, gpio_irq_pin));
    }
}

/**
 *  @brief      PORTB IRQ Handler
 *  @param[in]  None
 *  @return     None.
 */
static void portB_irq_handler(void)
{
    uint8_t gpio_irq_pin;

    /* Find GPIO IRQ pin */
    gpio_irq_pin = __builtin_ctzl(GPIOB->GPIOxDET);

    /* Execute context call back function */
    if (gpio_irq_table[PORTB].pin_irq_handler[gpio_irq_pin] != NULL)
    {
       /* Clear GPIO IRQ flag */
       GPIOB->GPIOxDET |= (1 << gpio_irq_pin);

       /* Allow reentrant IRQ */
       __asm__ __volatile__("cpsie i");

       /* Call gpio pin IRQ handler */
       gpio_irq_table[PORTB].pin_irq_handler[gpio_irq_pin](TO_GPIO(PORTB, gpio_irq_pin));
    }
}

/**
 *  @brief      PORTC IRQ Handler
 *  @param[in]  None
 *  @return     None.
 */
static void portC_irq_handler(void)
{
    uint8_t gpio_irq_pin;

    /* Find GPIO IRQ pin */
    gpio_irq_pin = __builtin_ctzl(GPIOC->GPIOxDET);

    /* Execute context call back function */
    if (gpio_irq_table[PORTC].pin_irq_handler[gpio_irq_pin] != NULL)
    {
       /* Clear GPIO IRQ flag */
       GPIOC->GPIOxDET |= (1 << gpio_irq_pin);

       /* Allow reentrant IRQ */
       __asm__ __volatile__("cpsie i");

       /* Call gpio pin IRQ handler */
       gpio_irq_table[PORTC].pin_irq_handler[gpio_irq_pin](TO_GPIO(PORTC, gpio_irq_pin));
    }
}

/**
 *	@brief	    PORTD IRQ Handler
 *  @param[in]	None
 *	@return		None.
 */
static void portD_irq_handler(void)
{
    uint8_t gpio_irq_pin;

    /* Find GPIO IRQ pin */
    gpio_irq_pin = __builtin_ctzl(GPIOD->GPIOxDET);

    /* Execute context call back function */
    if (gpio_irq_table[PORTD].pin_irq_handler[gpio_irq_pin] != NULL)
    {
       /* Clear GPIO IRQ flag */
       GPIOA->GPIOxDET |= (1 << gpio_irq_pin);

       /* Allow reentrant IRQ */
       __asm__ __volatile__("cpsie i");

       /* Call gpio pin IRQ handler */
       gpio_irq_table[PORTD].pin_irq_handler[gpio_irq_pin](TO_GPIO(PORTD, gpio_irq_pin));
    }
}

/**
 *	@brief	    PORTE IRQ Handler
 *  @param[in]	None
 *	@return		None.
 */
static void portE_irq_handler(void)
{
    uint8_t gpio_irq_pin;

    printf("%s\r\n", __FUNCTION__);

    /* Find GPIO IRQ pin */
    gpio_irq_pin = __builtin_ctzl(GPIOE->GPIOxDET);

    /* Execute context call back function */
    if (gpio_irq_table[PORTE].pin_irq_handler[gpio_irq_pin] != NULL)
    {
       /* Clear GPIO IRQ flag */
       GPIOE->GPIOxDET |= (1 << gpio_irq_pin);

       /* Allow reentrant IRQ */
       __asm__ __volatile__("cpsie i");

       /* Call gpio pin IRQ handler */
       gpio_irq_table[PORTE].pin_irq_handler[gpio_irq_pin](TO_GPIO(PORTE, gpio_irq_pin));
    }
}

/**
 *  @brief      Configure GPIO alternative function
 *  @param[in]  uint8_t    GPIO number
 *              gpio_alt_t GPIO alternative function
 *  @return     None.
 */
void s5p4418_gpio_set_alt(uint8_t gpio, gpio_alt_t alt)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get GPIO information */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Parameter check */
    if ((port > PORTE) || (pin > 31) || (alt > GPIO_ALT3))
    {
        return;
    }

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Set alternative mode */
    if(pin < 16)
    {
        PORTx->GPIOxALTFN[0] &= ~(ALT_MASK << (pin*2));
        PORTx->GPIOxALTFN[0] |= (alt << (pin*2));
    }
    else
    {
        PORTx->GPIOxALTFN[1] &= ~(ALT_MASK << ((pin-16)*2));
        PORTx->GPIOxALTFN[1] |=  (alt << ((pin-16)*2));
    }
}

/**
 *  @brief      Configure GPIO direction
 *  @param[in]  uint8_t    GPIO number
 *              gpio_dir_t GPIO direction
 *  @return     None.
 */
void s5p4418_gpio_set_dir(uint8_t gpio, gpio_dir_t dir)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get GPIO information */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Parameter check */
    if ((port > PORTE) || (pin > 31) || (dir > GPIO_DIR_OUT))
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return;
    }

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Set GPIO direction */
    PORTx->GPIOxOUTENB &= ~(1 << pin);
    PORTx->GPIOxOUTENB |=  (dir << pin);
}

/**
 *  @brief      Configure GPIO pull up/down
 *  @param[in]  uint8_t     GPIO number
 *              gpio_pull_t Pull up/down type
 *  @return     None.
 */
void s5p4418_gpio_set_pull(uint8_t gpio, gpio_pull_t pull)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get GPIO information */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Parameter check */
    if ((port > PORTE) || (pin > 31) || (pull > GPIO_PULL_NONE))
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return;
    }

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Pull up/down enable */
    PORTx->GPIOx_PULLENB_DISABLE_DEFAULT |= (1 << pin);

    switch(pull)
    {
        case GPIO_PULL_UP:
            PORTx->GPIOx_PULLSEL |= (1 << pin);
            PORTx->GPIOx_PULLENB |= (1 << pin);
        break;

        case GPIO_PULL_DOWN:
            PORTx->GPIOx_PULLSEL &= ~(1 << pin);
            PORTx->GPIOx_PULLENB |= (1 << pin);
        break;

        case GPIO_PULL_NONE:
            PORTx->GPIOx_PULLENB &= ~(1 << pin);
        break;

        default:
        break;
    }
}

/**
 *  @brief      Configure GPIO drive strength
 *  @param[in]  uint8_t    GPIO number
 *              gpio_drv_t Drive strength
 *  @return     None.
 */
void s5p4418_gpio_set_drv(uint8_t gpio, gpio_drv_t drv)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get GPIO information */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Parameter check */
    if ((port > PORTE) || (pin > 31) || (drv > GPIO_DRV_HIGH))
    {
        printf("%s invalid input parameter\r\n", __FUNCTION__);
        return;
    }

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Set drive strength */
    PORTx->GPIOx_DRV0_DISABLE_DEFAULT |= (1 << pin);
    PORTx->GPIOx_DRV1_DISABLE_DEFAULT |= (1 << pin);

    switch(drv)
    {
        case GPIO_DRV_LOW:
            PORTx->GPIOx_DRV0 &= ~(1 << pin);
            PORTx->GPIOx_DRV1 &= ~(1 << pin);
        break;

        case GPIO_DRV_MEDIUM:
            PORTx->GPIOx_DRV0 &= ~(1 << pin);
            PORTx->GPIOx_DRV1 |=  (1 << pin);
        break;

        case GPIO_DRV_HIGH:
            PORTx->GPIOx_DRV0 |= (1 << pin);
            PORTx->GPIOx_DRV1 |= (1 << pin);
        break;
        default:
        break;
    }
}

/**
 *  @brief      Configure GPIO slew rate
 *  @param[in]  uint8_t     GPIO number
 *              gpio_rate_t Slew rate value
 *  @return     None.
 */
void s5p4418_gpio_set_rate(uint8_t gpio, gpio_rate_t rate)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get GPIO information */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Parameter check */
    if ((port > PORTE) || (pin > 31) || (rate > GPIO_RATE_NORM))
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return;
    }

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Set slew rate */
    PORTx->GPIOx_SLEW_DISABLE_DEFAULT |= (1 << pin);
    PORTx->GPIOx_SLEW &= ~(1 << pin);
    PORTx->GPIOx_SLEW |=  (rate << pin);
}

/**
 *  @brief      Configure GPIO pin
 *  @param[in]  gpio_info_t GPIO property
 *  @return     None.
 */
void s5p4418_gpio_config(gpio_info_t gpio_info)
{
    uint8_t port, pin, alt, dir, pull, drv, rate;

    /* Get GPIO information */
    port = TO_PORT(gpio_info.gpio);
    pin  = TO_PIN(gpio_info.gpio);
    alt  = (uint8_t)gpio_info.alt;
    dir  = (uint8_t)gpio_info.dir;
    pull = (uint8_t)gpio_info.pull;
    drv  = (uint8_t)gpio_info.drv;
    rate = (uint8_t)gpio_info.rate;

    /* Parameter check */
    if ((port > PORTE) || (pin > 31) || (alt > GPIO_ALT3) ||
        (dir > GPIO_DIR_OUT) || (pull > GPIO_PULL_NONE) ||
        (drv > GPIO_DRV_HIGH) || (rate > GPIO_RATE_NORM))
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return;
    }

    /* Set alternative mode */
    s5p4418_gpio_set_alt(gpio_info.gpio, gpio_info.alt);

    /* Set GPIO direction */
    s5p4418_gpio_set_dir(gpio_info.gpio, gpio_info.dir);

    /* Pull up/down enable */
    s5p4418_gpio_set_pull(gpio_info.gpio, gpio_info.pull);

    /* Set drive strength */
    s5p4418_gpio_set_drv(gpio_info.gpio, gpio_info.drv);

    /* Set slew rate */
    s5p4418_gpio_set_rate(gpio_info.gpio, gpio_info.rate);
}


/**
 *  @brief      Set GPIO pin
 *  @param[in]  uint8_t GPIO number
 *              bool    true = high, false = low
 *  @return     None.
 */
void s5p4418_gpio_set(uint8_t gpio, bool val)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get port, pin */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    if (val == true)
    {
        PORTx->GPIOxOUT |= (1 << pin);
    }
    else
    {
        PORTx->GPIOxOUT &= ~(1 << pin);
    }
}

/**
 *  @brief      Get GPIO status
 *  @param[in]  uint8_t GPIO number
 *  @return     bool : true = high, false = low
 */
bool s5p4418_gpio_get(uint8_t gpio)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get port, pin */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Get GPIO pin status */
    if(PORTx->GPIOxPAD & (1 << pin))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 *  @brief      GPIO PORT IRQ register
 *  @param[in]  uint8_t port number (PORTA, PORTB, PORTC, PORTD & PORTE)
 *  @return     bool : true = successfully, false = failed
 */
static bool s5p4418_port_irq_init(uint8_t port, uint8_t priority)
{
    uint8_t irq;
    bool ret = true;

    /* Parameter check */
    if (port > PORTE)
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return false;
    }

    /* Get irq number */
    irq = gpio_irq_table[port].irq;

    /* Register IRQ */
    ret = s5p4418_irq_register(gpio_irq_table[port].port_irq_handler, irq, priority);

    /* Update port irq handler register status */
    if (ret == true)
    {
        gpio_irq_table[port].port_irq_registered = true;
    }
    else
    {
        gpio_irq_table[port].port_irq_registered = false;
    }

    return ret;
}

#if 0
/**
 *  @brief      GPIO PORT IRQ unregister
 *  @param[in]  uint8_t port number (PORTA, PORTB, PORTC, PORTD & PORTE)
 *  @return     bool : true = successfully, false = failed
 */
static void s5p4418_port_irq_deinit(uint8_t port)
{
    uint8_t irq;

    /* Parameter check */
    if (port > PORTE)
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return;
    }

    /* Get irq number */
    irq = gpio_irq_table[port].irq;

    /* Unregister IRQ */
    s5p4418_irq_unregister(irq);

    /* Update port irq handler register status */
    gpio_irq_table[port].port_irq_registered = false;

    return;
}
#endif

/**
 *  @brief      GPIO IRQ register
 *  @param[in]  uint8_t GPIO number
 *  @return     bool : true = successfully, false = failed
 */
bool s5p4418_gpio_irq_register(uint8_t gpio, gpio_evt_t evt, void(*cbfunc)(uint8_t gpio))
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get port, pin */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Parameter check */
    if ((port > PORTE) || (pin > 31) || (evt > GPIO_IRQ_BOTH))
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return false;
    }

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Check pin direction */
    if (PORTx->GPIOxOUTENB & (1<<pin))
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return false;
    }

    /* Register port irq handler */
    if( gpio_irq_table[port].port_irq_registered == false)
    {
        s5p4418_port_irq_init(port, GPIO_IRQ_PRIORITY);
    }

    /* Configure event type */
    if(pin < 16)
    {
        PORTx->GPIOxDETMODE[0] &= ~(EVT_MASK << (pin*2));
        PORTx->GPIOxDETMODE[0] |=  (evt << (pin*2));
    }
    else
    {
        PORTx->GPIOxDETMODE[1] &= ~(EVT_MASK << ((pin-16)*2));
        PORTx->GPIOxDETMODE[1] |=  (evt << ((pin-16)*2));
    }

    /* Clear the interrupt flag */
    PORTx->GPIOxDET |= (1 << pin);

    /* Save callback context */
    gpio_irq_table[port].pin_irq_handler[pin] = cbfunc;

    return true;
}

/**
 *  @brief      GPIO IRQ unregister
 *  @param[in]  uint8_t GPIO number
 *  @return     None
 */
void s5p4418_gpio_irq_unregister(uint8_t gpio)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get port, pin */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Parameter check */
    if ((port > PORTE) || (pin > 31))
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return;
    }

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Disable GPIO IRQ */
    PORTx->GPIOxINTENB &= ~(1 << pin);

    /* Unregister pin IRQ handler */
    gpio_irq_table[port].pin_irq_handler[pin] = NULL;
}


/**
 *  @brief      GPIO IRQ enable
 *  @param[in]  uint8_t GPIO number
 *  @return     None
 */
void s5p4418_gpio_irq_enable(uint8_t gpio)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get port, pin */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Parameter check */
    if ((port > PORTE) || (pin > 31))
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return;
    }

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Enable GPIO IRQ */
    PORTx->GPIOxDETENB |= (1 << pin);
    PORTx->GPIOxINTENB |= (1 << pin);
}

/**
 *  @brief      GPIO IRQ disable
 *  @param[in]  uint8_t GPIO number
 *  @return     None
 */
void s5p4418_gpio_irq_disable(uint8_t gpio)
{
    gpio_typedef *PORTx;
    uint8_t port, pin;

    /* Get port, pin */
    port = TO_PORT(gpio);
    pin  = TO_PIN(gpio);

    /* Parameter check */
    if ((port > PORTE) || (pin > 31))
    {
        printf("%s failed!\r\n", __FUNCTION__);
        return;
    }

    /* Get port base address */
    PORTx = (gpio_typedef *)port_base_addr_table[port];

    /* Disable GPIO IRQ */
    PORTx->GPIOxINTENB &= ~(1 << pin);
    PORTx->GPIOxDETENB &= ~(1 << pin);
}

