//------------------------------------------------------------------------------
//
//  Copyright (C) 2017
//
//  Module      : IRQ
//  File        : s5p4418_rstcon.c
//  Description : S5P4418 IP RESET
//  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)
//  History     :
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <io.h>
#include "s5p4418_chip.h"
#include "s5p4418_rstcon.h"

static void __attribute__ ((noinline)) __udelay(uint32_t loop)
{
    for(; loop > 0; loop--);
}

/**
 *  @brief      Do IP reset
 *  @param[in]  ssp_channel_t SSP channel
 *  @param[in]  uint8_t reset ID
 *  @param[in]  uint8_t 0 : reset, 1: no reset
 *  @return     none
 */
void s5p4418_ip_setrst(uint8_t id, uint8_t reset)
{
    uint32_t val;

    if(id < 32)
    {
        val = RSTCON->IP_RESET_REGISTER0;
        val &= ~(0x1 << (id & 0x1f));
        val |= (reset ? 1 : 0) << (id & 0x1f);
        RSTCON->IP_RESET_REGISTER0 = val;
    }
    else if (id < 64)
    {
        val = RSTCON->IP_RESET_REGISTER1;
        val &= ~(0x1 << (id & 0x1f));
        val |= (reset ? 1 : 0) << (id & 0x1f);
        RSTCON->IP_RESET_REGISTER1 = val;

    }else if (id < 96)
    {
        val = RSTCON->IP_RESET_REGISTER2;
        val &= ~(0x1 << (id & 0x1f));
        val |= (reset ? 1 : 0) << (id & 0x1f);
        RSTCON->IP_RESET_REGISTER2 = val;
    }
    else
    {
        return;
    }
}

/**
 *  @brief      Ge IP reset resistor
 *  @param[in]  uint8_t reset ID
 *  @param[out] uint8_t 0 : reset, 1: no reset
 *  @return     none
 */
uint8_t s5p4418_ip_getrst(uint8_t id)
{
    uint32_t val;

    if(id < 32)
    {
        val = ((RSTCON->IP_RESET_REGISTER0 >> (id & 0x1f)) & 0x1);
    }
    else if (id < 64)
    {
        val = ((RSTCON->IP_RESET_REGISTER1 >> (id & 0x1f)) & 0x1);
    }
    else if (id < 96)
    {
        val = ((RSTCON->IP_RESET_REGISTER2 >> (id & 0x1f)) & 0x1);
    }
    else
    {
        return 1;
    }

    return val;
}

/**
 *  @brief      Do IP reset monitoring
 *  @param[in]  uint8_t reset ID
 *  @param[out] uint8_t 0 : reset, 1: no reset
 *  @return     none
 */

void s5p4418_ip_reset(int id)
{
    if(!s5p4418_ip_getrst(id))
    {
        s5p4418_ip_setrst(id, 0);
        __udelay(10);
        s5p4418_ip_setrst(id, 1);
    }
}

