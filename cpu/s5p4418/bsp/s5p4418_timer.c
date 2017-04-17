//------------------------------------------------------------------------------
//
//  Copyright (C) 2017
//
//  Module      : TIMER
//  File        : s5p4418_timer.c
//  Description : S5P4418 TIMER FUNCTIONS
//  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)
//  History     :
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "s5p4418_timer.h"
#include "s5p4418_irq.h"


/* Bit position lookup tables definition */
static uint8_t clock_mux_pos[] = {0, 4, 8, 12};
static uint8_t irq_ena_pos[] = {0, 1, 2, 3};
static uint8_t start_stop_pos[] = {0, 8, 12, 16};
static uint8_t irq_number[] = {23, 24, 25, 26};

/**
 *  @brief      Initialize interrupt controller
 *  @param[in]  None.
 *  @return     None.
 */
void s5p4418_timer_init(void)
{
    /*
     * PCLK = 200MHz,
     * Prescaler = 1/200
     * => Clk = 1MHz
     */
    TIMER->TCFG0 = 0x0000C8C8; // Prescaler = 1/200 => 1MHz
    TIMER->TCFG1 = 0x00000000;
    TIMER->TCON  = 0x00000000;
    TIMER->TINT_CSTAT = 0x00000000;
}

/**
 *  @brief      Setup timer channel
 *  @param[in]  uint8_t  channel no
 *              uint32_t tick time in micro second
 *  @return     None.
 */
void s5p4418_timer_setup(uint8_t ch, uint32_t t_us, bool irq_on)
{
    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return;
    }

    /* Mux input for TIMER channel = 0x0000 (1/1) */
    TIMER->TCFG1 &= ~(0xF << clock_mux_pos[ch]);

    /* Setup reload buffer, interrupt enable */
    switch (ch)
    {
        case 0:
            TIMER->TCNTB0 = t_us;
        break;

        case 1:
            TIMER->TCNTB1 = t_us;
        break;

        case 2:
            TIMER->TCNTB2 = t_us;
        break;

        case 3:
            TIMER->TCNTB3 = t_us;
        break;

        default:

        break;
    }

    /* Enable interrupt on demand */
    if(irq_on)
    {
        TIMER->TINT_CSTAT |= ((1 << (irq_ena_pos[ch] + 5)) | (1 << irq_ena_pos[ch]));
    }
}

/**
 *  @brief      Initialize interrupt controller
 *  @param[in]  uint8_t channel number
 *              bool    true  : auto reload mode
 *                      false : one shot mode
 *  @return     None.
 */
void s5p4418_timer_start(uint8_t ch, bool auto_reload)
{
    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return;
    }

    /* Counter mode setting */
    if(auto_reload)
    {
        TIMER->TCON |= (1 << (start_stop_pos[ch] + 3));
    }
    else
    {
        TIMER->TCON &= ~(1 << (start_stop_pos[ch] + 3));
    }

    /* Manually update counter buffer */
    TIMER->TCON |= (1 << (start_stop_pos[ch] + 1));
    TIMER->TCON &= ~(1 << (start_stop_pos[ch] + 1));

    /* Start timer */
    TIMER->TCON |= (1 << start_stop_pos[ch]);
}

/**
 *  @brief      Initialize interrupt controller
 *  @param[in]  uint8_t channel number
 *  @return     None.
 */
void s5p4418_timer_stop(uint8_t ch)
{
    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return;
    }

    /* Stop timer */
    TIMER->TCON &= ~(1 << start_stop_pos[ch]);
}


/**
 *  @brief      Get current timer count
 *  @param[in]  uint8_t channel number
 *  @return     None.
 */
uint32_t s5p4418_timer_get_cnt(uint8_t ch)
{
    uint32_t ret = 0;

    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return 0;
    }

    switch (ch)
    {
        case 0:
            ret = TIMER->TCNTO0;
        break;

        case 1:
            ret = TIMER->TCNTO1;
        break;

        case 2:
            ret = TIMER->TCNTO2;
        break;

        case 3:
            ret = TIMER->TCNTO3;
        break;

        default:

        break;
    }

    return ret;
}

/**
 *  @brief      Get current timer count
 *  @param[in]  uint8_t channel number
 *  @return     None.
 */
uint32_t s5p4418_timer_get_tick(uint8_t ch)
{
    uint32_t ret = 0;

    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return 0;
    }

    /* Stop timer */
    TIMER->TCON &= ~(1 << start_stop_pos[ch]);	
    
    switch (ch)
    {
        case 0:
            ret = TIMER->TCNTB0 - TIMER->TCNTO0;
        break;

        case 1:
            ret = TIMER->TCNTB1 - TIMER->TCNTO1;
        break;

        case 2:
            ret = TIMER->TCNTB2 - TIMER->TCNTO2;
        break;

        case 3:
            ret = TIMER->TCNTB3 - TIMER->TCNTO3;
        break;

        default:

        break;
    }

    /* Start timer */
    TIMER->TCON |= (1 << start_stop_pos[ch]);

    return ret;	
}

/**
 *  @brief      Initialize interrupt controller
 *  @param[in]  uint8_t channel number
 *              uint8_t set count
 *  @return     None.
 */
void s5p4418_timer_set_cnt(uint8_t ch, uint32_t cnt)
{
    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return;
    }

    s5p4418_timer_stop(ch);

    switch (ch)
    {
        case 0:
            TIMER->TCNTB0 = cnt;
        break;

        case 1:
            TIMER->TCNTB1 = cnt;
        break;

        case 2:
            TIMER->TCNTB2 = cnt;
        break;

        case 3:
            TIMER->TCNTB3 = cnt;
        break;

        default:

        break;
    }

    /* Manually update counter buffer */
    TIMER->TCON |= (1 << (start_stop_pos[ch] + 1));
    TIMER->TCON &= ~(1 << (start_stop_pos[ch] + 1));

    return;
}


/**
 *  @brief      Get timer IRQ flag
 *  @param[in]  uint8_t timer channel
 *  @return     bool true  : IRQ flag set
 *                   false : IRQ flag clear
 */
bool s5p4418_timer_get_irq_flag(uint8_t ch)
{
    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return false;
    }

    return ((TIMER->TINT_CSTAT & (1 << (irq_ena_pos[ch] + 5))) ? true : false);
}

/**
 *  @brief      Clear timer IRQ flag
 *  @param[in]  uint8_t timer channel
 *  @return     None.
 */
void s5p4418_timer_clear_irq_flag(uint8_t ch)
{
    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return;
    }

    TIMER->TINT_CSTAT |= (1 << (irq_ena_pos[ch] + 5));
}

/**
 *  @brief      Get timer irq number
 *  @param[in]  uint8_t timer channel
 *  @return     uint8_t IRQ number
 */
uint8_t s5p4418_timer_get_irq_number(uint8_t ch)
{
    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return 60; /* Invalid irq number */
    }

    return irq_number[ch];
}

/**
 *  @brief      Register timer irq to VIC
 *  @param[in]  uint8_t timer channel
 *  @return     bool.
 */
bool s5p4418_timer_register_irq(uint8_t ch, void(*cbfunc)(void), uint8_t priority)
{
    uint8_t irq;

    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return false;
    }

    /* Get IRQ number */
    irq = s5p4418_timer_get_irq_number(ch);

    return (s5p4418_irq_register(cbfunc, irq, priority));
}

/**
 *  @brief      Get timer irq number
 *  @param[in]  uint8_t timer channel
 *  @return     none.
 */
void s5p4418_timer_unregister_irq(uint8_t ch)
{
    uint8_t irq;

    /* Check valid timmer channel */
    if (ch > MAX_TIMER_CHANNEL_INDEX)
    {
        printf("%s : Error! invalid timer channel\r\n", __FUNCTION__);
        return;
    }

    /* Get IRQ number */
    irq = s5p4418_timer_get_irq_number(ch);

    /* Unregister VIC */
    s5p4418_irq_unregister(irq);
}


