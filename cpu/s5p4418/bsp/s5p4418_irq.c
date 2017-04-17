//------------------------------------------------------------------------------
//
//  Copyright (C) 2017
//
//  Module      : IRQ
//  File        : s5p4418_irq.c
//  Description : S5P4418 IRQ Handler
//  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)
//  History     :
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "s5p4418_irq.h"
#include "io.h"
#include "cp15.h"

extern uint32_t nesting_level;

static irq_typedef irq_table[] =
{
    /* VIC0 */
    {
        .name    = "MCUSTOP",     // IRQ No = 00
        .handler = NULL,
    },
    {
        .name     = "DMA0",        // IRQ No = 01
        .handler = NULL,
    },
    {
        .name     = "DMA1",        // IRQ No = 02
        .handler = NULL,
    },
    {
        .name     = "CLKPWR0",     // IRQ No = 03
        .handler = NULL,
    },
    {
        .name     = "CLKPWR1",     // IRQ No = 04
        .handler = NULL,
    },
    {
        .name     = "CLKPWR2",     // IRQ No = 05
        .handler = NULL,
    },
    {
        .name     = "UART1",       // IRQ No = 06
        .handler = NULL,
    },
    {
        .name     = "UART0",       // IRQ No = 07
        .handler = NULL,
    },
    {
        .name     = "UART2",       // IRQ No = 08
        .handler = NULL,
    },
    {
        .name     = "UART3",       // IRQ No = 09
        .handler = NULL,
    },
    {
        .name     = "UART4",       // IRQ No = 10
        .handler = NULL,
    },
    {
        .name     = "RESERVED",    // IRQ No = 11
        .handler = NULL,
    },
    {
        .name     = "SSP0",        // IRQ No = 12
        .handler = NULL,
    },
    {
        .name     = "SSP1",        // IRQ No = 13
        .handler = NULL,
    },
    {
        .name     = "SSP2",        // IRQ No = 14
        .handler = NULL,
    },
    {
        .name     = "I2C0",        // IRQ No = 15
        .handler = NULL,
    },
    {
        .name     = "I2C1",        // IRQ No = 16
        .handler = NULL,
    },
    {
        .name     = "I2C2",        // IRQ No = 17
        .handler = NULL,
    },
    {
        .name     = "DEINTERLACE", // IRQ No = 18
        .handler = NULL,
    },
    {
        .name     = "SCALER",      // IRQ No = 19
        .handler = NULL,
    },
    {
        .name     = "AC97",        // IRQ No = 20
        .handler = NULL,
    },
    {
        .name     = "SPDIFRX",     // IRQ No = 21
        .handler = NULL,
    },
    {
        .name     = "SPDIFTX",     // IRQ No = 22
        .handler = NULL,
    },
    {
        .name     = "TIMER0",      // IRQ No = 23
        .handler = NULL,
    },
    {
        .name     = "TIMER1",      // IRQ No = 24
        .handler = NULL,
    },
    {
        .name     = "TIMER2",      // IRQ No = 25
        .handler = NULL,
    },
    {
        .name     = "TIMER3",      // IRQ No = 26
        .handler = NULL,
    },
    {
        .name     = "PWM0",        // IRQ No = 27
        .handler = NULL,
    },
    {
        .name     = "PWM1",        // IRQ No = 28
        .handler = NULL,
    },
    {
        .name     = "PWM2",        // IRQ No = 29
        .handler = NULL,
    },
    {
        .name     = "PWM3",        // IRQ No = 30
        .handler = NULL,
    },
    {
        .name     = "WDT",         // IRQ No = 31
        .handler = NULL,
    },

    /* VIC1 */
    {
        .name     = "MPEGTSI",     // IRQ No = 32
        .handler = NULL,
    },
    {
        .name     = "DISPLAYTOP0", // IRQ No = 33
        .handler = NULL,
    },
    {
        .name     = "DISPLAYTOP1", // IRQ No = 34
        .handler = NULL,
    },
    {
        .name     = "DISPLAYTOP2", // IRQ No = 35
        .handler = NULL,
    },
    {
        .name     = "DISPLAYTOP3", // IRQ No = 36
        .handler = NULL,
    },
    {
        .name     = "VIP0",        // IRQ No = 37
        .handler = NULL,
    },
    {
        .name     = "VIP1",        // IRQ No = 38
        .handler = NULL,
    },
    {
        .name     = "MIPI",        // IRQ No = 39
        .handler = NULL,
    },
    {
        .name     = "3DGPU",       // IRQ No = 40
        .handler = NULL,
    },
    {
        .name     = "ADC",         // IRQ No = 41
        .handler = NULL,
    },
    {
        .name     = "PPM",         // IRQ No = 42
        .handler = NULL,
    },
    {
        .name     = "SDMMC0",      // IRQ No = 43
        .handler = NULL,
    },
    {
        .name     = "SDMMC1",      // IRQ No = 44
        .handler = NULL,
    },
    {
        .name     = "SDMMC2",      // IRQ No = 45
        .handler = NULL,
    },
    {
        .name     = "CODA9600",    // IRQ No = 46
        .handler = NULL,
    },
    {
        .name     = "CODA9601",    // IRQ No = 47
        .handler = NULL,
    },
    {
        .name     = "GMAC",        // IRQ No = 48
        .handler = NULL,
    },
    {
        .name     = "USB20OTG",    // IRQ No = 49
        .handler = NULL,
    },
    {
        .name     = "USB20HOST",   // IRQ No = 50
        .handler = NULL,
    },
    {
        .name     = "N/A",         // IRQ No = 51
        .handler = NULL,
    },
    {
        .name     = "N/A",         // IRQ No = 52
        .handler = NULL,
    },
    {
        .name     = "GPIOA",       // IRQ No = 53
        .handler = NULL,
    },
    {
        .name     = "GPIOB",       // IRQ No = 54
        .handler = NULL,
    },
    {
        .name     = "GPIOC",       // IRQ No = 55
        .handler = NULL,
    },
    {
        .name     = "GPIOD",       // IRQ No = 56
        .handler = NULL,
    },
    {
        .name     = "GPIOE",       // IRQ No = 57
        .handler = NULL,
    },
    {
        .name     = "CRYPTO",      // IRQ No = 58
        .handler = NULL,
    },
    {
        .name     = "PDM",         // IRQ No = 59
        .handler = NULL,
    },
};

/**
 *  @brief     Initialize interrupt controller
 *  @param[in] None.
 *  @return    None.
 */
void s5p4418_irq_init(void)
{
    int i;

    /* Link VIC to CPU core */
    write32(0xF0000100, 0);

    /* Select irq mode */
    VIC0->INTSELECT = 0x00000000;
    VIC1->INTSELECT = 0x00000000;

    /* Disable all interrupts */
    VIC0->INTENABLE = 0x00000000;
    VIC1->INTENABLE = 0x00000000;

    /* Clear all interrupts */
    VIC0->INTENCLEAR = 0xffffffff;
    VIC1->INTENCLEAR = 0xffffffff;

    /* Clear all irq status */
    VIC0->IRQSTATUS = 0x00000000;
    VIC1->IRQSTATUS = 0x00000000;

    /* Clear all fiq status */
    VIC0->FIQSTATUS = 0x00000000;
    VIC1->FIQSTATUS = 0x00000000;

    /* Clear all software interrupts */
    VIC0->SOFTINTCLEAR = 0xffffffff;
    VIC1->SOFTINTCLEAR = 0xffffffff;

    /* Set vic address to zero */
    VIC0->VICADDRESS = 0x00000000;
    VIC1->VICADDRESS = 0x00000000;

    /* Set priority to lowest */
    for(i = 0; i < 32; i++)
    {
        VIC0->VICVECTPRIORITY[i] = 0xF;
        VIC1->VICVECTPRIORITY[i] = 0xF;
    }

    /* Set ISR vector address */
    for(i = 0; i < 32; i++)
    {
        VIC0->VECTADDR[i] = i;
        VIC1->VECTADDR[i] = i + 32;
    }

    /* Enable vector interrupt controller */
    __vic_enable();

    /* Enable irq */
    __irq_enable();
}


/**
 *  @brief      Register interrupt line
 *  @param[in]  void(*)(void) Interrupt callback function
 *  @param[in]  uint8_t IRQ number
 *  @param[in]  uint8_t IRQ priority
 *  @return     bool
 *              true  Successfully
 *              false Failed
 */
bool s5p4418_irq_register(void(*cbfunc)(void), uint8_t irq, uint8_t priority)
{
    /* Check valid IRQ number */
    if((irq > MAX_IRQ_NUMBER) || (irq == 11) ||
       (irq == 51) || (irq == 52))
    {
        printf("Error! invalid IRQ number\r\n");
        return false;
    }

    /* Check valid callback function */
    if(NULL == cbfunc)
    {
        printf("Error! invalid callback function\r\n");
        return false;
    }

    /* Check valid priority */
    if(priority > 15)
    {
        priority = 15;
    }

    /* Assign interrupt function */
    irq_table[irq].handler = cbfunc;

    /* Set priority */
    if(irq <= 31)
    {
       VIC0->VICVECTPRIORITY[irq] = priority;
    }
    else
    {
       VIC1->VICVECTPRIORITY[irq % 32] = priority;
    }

    /* Enable the corresponding interrupt line */
    if(irq <= 31)
    {
        VIC0->INTENABLE |= (1 << irq);
    }
    else
    {
        /* Enable interrupt */
        VIC1->INTENABLE |= (1 << (irq % 32));
    }

    return true;
}

/**
 *  @brief     Unregister interrupt line
 *  @param[in] uint32_t Unregister IRQ number
 *  @return    None.
 */
void s5p4418_irq_unregister(uint8_t irq)
{
    /* Check valid IRQ number */
    if((irq > MAX_IRQ_NUMBER) || (irq == 11) ||
       (irq == 51) || (irq == 52))
    {
        return;
    }

    /* Disable the corresponding interrupt line */
    if(irq <= 31)
    {
        VIC0->INTENCLEAR |= (1 << irq);
    }
    else
    {
        VIC0->INTENCLEAR |= (1 << (irq % 32));
    }

    /* Disable interrupt function */
    irq_table[irq].handler = NULL;
}

/**
 *  @brief     IRQ handler function
 *  @param[in] None.
 *  @return    None.
 */
void s5p4418_irq_handler(void)
{
    uint32_t vic0_irq_status;
    uint32_t vic1_irq_status;
    uint32_t vic0_swi_status;
    uint32_t vic1_swi_status;
    uint32_t dummy;
    uint8_t  irq;

    /* Dummy read VICADDRESS */
    dummy = VIC0->VICADDRESS;
    dummy = VIC1->VICADDRESS;
    dummy = dummy;

    /* Get IRQ status */
    vic0_irq_status = VIC0->IRQSTATUS;
    vic1_irq_status = VIC1->IRQSTATUS;

    /* Get SWI status */
    vic0_swi_status = VIC0->SOFTINT;
    vic1_swi_status = VIC1->SOFTINT;

    /* Check IRQ source from VIC0 */
    if(vic0_irq_status != 0)
    {
       /* Find irq number */
       irq = __builtin_ctzl(vic0_irq_status);

       /* Execute IRQ callback function */
       if(irq_table[irq].handler != NULL)
       {
            irq_table[irq].handler();
       }

       /* Write VIC ADDRESS register */
       VIC0->VICADDRESS = 0x00000000;
    }

    /* Check IRQ source from VIC1 */
    if(vic1_irq_status != 0)
    {
       /* Find irq number */
       irq = __builtin_ctzl(vic1_irq_status);

       /* Execute IRQ callback function */
       if(irq_table[irq + 32].handler != NULL)
       {
            irq_table[irq + 32].handler();
       }

       /* Write VIC ADDRESS register */
       VIC1->VICADDRESS = 0x00000000;
    }

    /* Check SWI source form VIC0 */
    if (vic0_swi_status != 0)
    {
        VIC0->SOFTINTCLEAR = 0xFFFFFFFF;
    }

    /* Check SWI source form VIC0 */
    if (vic1_swi_status != 0)
    {
        VIC1->SOFTINTCLEAR = 0xFFFFFFFF;
    }
}

/**
 *  @brief     Undefined instruction handler
 *  @param[in] None.
 *  @return    None.
 */
void udefined_instruction_handler(void)
{
     unsigned int lr;

     __asm__ __volatile__
         (
           "mov    %0, lr\n"
           : "=r" (lr)
           :
           : "memory"
         );

    printf("%s lr = %X\r\n", __FUNCTION__, lr);
    while(1);
}

/**
 *  @brief     Software interrupt handler
 *  @param[in] None.
 *  @return    None.
 */
//void swi_handler (void)
//{
//    printf("%s\r\n", __FUNCTION__);
//    while(1);
//}

/**
 *  @brief     Prefect abort handler
 *  @param[in] None.
 *  @return    None.
 */
void prefetch_abort_handler (void)
{
    printf("%s\r\n", __FUNCTION__);
    while(1);
}

/**
 *  @brief     Data abort handler
 *  @param[in] None.
 *  @return    None.
 */
void data_abort_handler (void)
{
     unsigned int lr;

     __asm__ __volatile__
         (
           "mov    %0, lr\n"
           : "=r" (lr)
           :
           : "memory"
         );

    printf("%s lr = %X\r\n", __FUNCTION__, lr);
    while(1);
}

