/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 - 2017 , NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define getSysconClkMux() ((SYSCON->CLK_CTRL & SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) >> SYSCON_CLK_CTRL_SYS_CLK_SEL_SHIFT)

/*******************************************************************************
 * Variables
 ******************************************************************************/
typedef union
{
    struct
    {
        uint8_t crcClkRefCnt;
        uint8_t dmaClkRefCnt;
    } ref_cnt_t;
    uint8_t clkRefCnt[2];
} clock_ref_cnt_t;

/** Clock reference count */
static clock_ref_cnt_t clk_ref_cnt;

/*******************************************************************************
 * Code
 ******************************************************************************/

static void Clk32KConfig(uint8_t choice)
{
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_CLK_32K_SEL_MASK) | SYSCON_CLK_CTRL_CLK_32K_SEL(choice);
}

static void ClkSysConfig(uint8_t choice)
{
    if (choice == 0)
    {
        /* RCO 32MHz ,wait for ready */
        while (!(SYSCON->SYS_MODE_CTRL & SYSCON_SYS_MODE_CTRL_OSC32M_RDY_MASK))
        {
        }
    }

    /* Switch to the clock source */
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) | SYSCON_CLK_CTRL_SYS_CLK_SEL(choice);
}

static void ClkWdtConfig(uint8_t choice)
{
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_CLK_WDT_SEL_MASK) | SYSCON_CLK_CTRL_CLK_WDT_SEL(choice);
}

static void ClkBleConfig(uint8_t choice)
{
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_CLK_BLE_SEL_MASK) | SYSCON_CLK_CTRL_CLK_BLE_SEL(choice);
}

static void ClkXTALConfig(uint8_t choice)
{
    switch (choice)
    {
        /* 16M XTAL */
        case 0:
            SYSCON->CLK_CTRL &= ~SYSCON_CLK_CTRL_CLK_XTAL_SEL_MASK;
            break;
        /* 32M XTAL */
        case 1:
            SYSCON->CLK_CTRL |= SYSCON_CLK_CTRL_CLK_XTAL_SEL_MASK;
            break;
        default:
            break;
    }
    /* wait for ready */
    while (!(SYSCON->SYS_MODE_CTRL & SYSCON_SYS_MODE_CTRL_XTAL_RDY_MASK))
    {
    }
}

void CLOCK_EnableClock(clock_ip_name_t clk)
{
    uint32_t regPrimask = 0U;
    if ((clk == kCLOCK_Crc) || (clk == kCLOCK_Dma))
    {
        regPrimask = DisableGlobalIRQ();
        clk_ref_cnt.clkRefCnt[clk - kCLOCK_Crc] += 1U;
        EnableGlobalIRQ(regPrimask);
    }
    SYSCON->CLK_EN = (1U << clk);
}

void CLOCK_DisableClock(clock_ip_name_t clk)
{
    uint32_t regPrimask = 0U;
    if (((clk == kCLOCK_Crc) || (clk == kCLOCK_Dma)) && (clk_ref_cnt.clkRefCnt[clk - kCLOCK_Crc] > 0))
    {
        regPrimask = DisableGlobalIRQ();
        clk_ref_cnt.clkRefCnt[clk - kCLOCK_Crc] -= 1U;
        EnableGlobalIRQ(regPrimask);
        if (clk_ref_cnt.clkRefCnt[clk - kCLOCK_Crc] > 0)
        {
            return;
        }
    }
    SYSCON->CLK_DIS = (1U << clk);
}

void CLOCK_AttachClk(clock_attach_id_t connection)
{
    uint8_t mux, choice;

    mux = (uint8_t)connection;
    choice = (uint8_t)(((connection & 0xf00) >> 8) - 1);

    switch (mux)
    {
        case CM_32KCLKSEL:
            Clk32KConfig(choice);
            break;
        case CM_SYSCLKSEL:
            ClkSysConfig(choice);
            break;
        case CM_WDTCLKSEL:
            ClkWdtConfig(choice);
            break;
        case CM_BLECLKSEL:
            ClkBleConfig(choice);
            break;
        case CM_XTALCLKSEL:
            ClkXTALConfig(choice);
            break;
        default:
            break;
    }
}

void CLOCK_SetClkDiv(clock_div_name_t div_name, uint32_t divided_by_value)
{
    switch (div_name)
    {
        case kCLOCK_DivXtalClk:
            /* F(XTAL) = F(XTAL) / (divided_by_value + 1), occupy 1 bit, take effect only when k32M_to_XTAL_CLK attached */
            SYSCON->XTAL_CTRL =
                (SYSCON->XTAL_CTRL & ~SYSCON_XTAL_CTRL_XTAL_DIV_MASK) | SYSCON_XTAL_CTRL_XTAL_DIV(divided_by_value);
            break;
        case kCLOCK_DivOsc32mClk:
            /* F(OSC32M) = F(OSC32M) / (divided_by_value + 1), occupy 1 bit */
            SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_CLK_OSC32M_DIV_MASK) |
                               SYSCON_CLK_CTRL_CLK_OSC32M_DIV(divided_by_value);
            break;
        case kCLOCK_DivAhbClk:
            /* F(AHB) = F(SYS) / (divided_by_value  + 1), occupy 13 bits
             * Note: If ble core's clock is enabled by setting SYSCON_CLK_EN_CLK_BLE_EN to 1, ahb clock can only be 8M,
             * 16M or 32M.
             */
            SYSCON->CLK_CTRL =
                (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_AHB_DIV_MASK) | SYSCON_CLK_CTRL_AHB_DIV(divided_by_value);
            break;
        case kCLOCK_DivApbClk:
            /* F(APB) = F(AHB) / (divided_by_value  + 1), occupy 4 bits */
            SYSCON->CLK_CTRL =
                (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_APB_DIV_MASK) | SYSCON_CLK_CTRL_APB_DIV(divided_by_value);
            break;
        case kCLOCK_DivFrg0:
            /* F(Flexcomm0) = F(AHB) / (1 + MULT/DIV), DIV = 0xFF */
            SYSCON->FC_FRG = (SYSCON->FC_FRG & ~SYSCON_FC_FRG_FRG_MULT0_MASK) |
                             SYSCON_FC_FRG_FRG_MULT0(divided_by_value) | SYSCON_FC_FRG_FRG_DIV0_MASK;
            break;
        case kCLOCK_DivFrg1:
            /* F(Flexcomm1) = F(AHB) / (1 + MULT/DIV), DIV = 0xFF */
            SYSCON->FC_FRG = (SYSCON->FC_FRG & ~SYSCON_FC_FRG_FRG_MULT1_MASK) |
                             SYSCON_FC_FRG_FRG_MULT1(divided_by_value) | SYSCON_FC_FRG_FRG_DIV1_MASK;
            break;
        case kCLOCK_DivClkOut:
            /* F(ClkOut) = F(XTAL) / (2 * divided_by_value), occupy 4bits, take effect only when clock out source is
             * XTAL */
            SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_XTAL_OUT_DIV_MASK) |
                               SYSCON_CLK_CTRL_XTAL_OUT_DIV(divided_by_value);
            break;
        default:
            break;
    }
}

static uint32_t CLOCK_GetRco32MFreq(void)
{
    return (SYSCON->CLK_CTRL & SYSCON_CLK_CTRL_CLK_OSC32M_DIV_MASK) ? CLK_OSC_32MHZ / 2 : CLK_OSC_32MHZ;
}

static uint32_t CLOCK_GetXinFreq(void)
{
    return ((SYSCON->CLK_CTRL & SYSCON_CLK_CTRL_CLK_XTAL_SEL_MASK) &&
            (!(SYSCON->XTAL_CTRL & SYSCON_XTAL_CTRL_XTAL_DIV_MASK))) ?
               CLK_XTAL_32MHZ :
               CLK_XTAL_16MHZ;
}

static uint32_t CLOCK_Get32KFreq(void)
{
    return (SYSCON->CLK_CTRL & SYSCON_CLK_CTRL_CLK_32K_SEL_MASK) ? CLK_RCO_32KHZ : CLK_XTAL_32KHZ;
}

static uint32_t CLOCK_GetCoreSysClkFreq(void)
{
    return (getSysconClkMux() == 0) ? CLOCK_GetRco32MFreq() : (getSysconClkMux() == 1) ?
                                      CLOCK_GetXinFreq() :
                                      (getSysconClkMux() == 2) ? CLOCK_Get32KFreq() : 0;
}

static uint32_t CLOCK_GetAhbClkFreq(void)
{
    return CLOCK_GetCoreSysClkFreq() /
           (((SYSCON->CLK_CTRL & SYSCON_CLK_CTRL_AHB_DIV_MASK) >> SYSCON_CLK_CTRL_AHB_DIV_SHIFT) + 1);
}

static uint32_t CLOCK_GetApbClkFreq(void)
{
    return CLOCK_GetAhbClkFreq() /
           (((SYSCON->CLK_CTRL & SYSCON_CLK_CTRL_APB_DIV_MASK) >> SYSCON_CLK_CTRL_APB_DIV_SHIFT) + 1);
}

static uint32_t CLOCK_GetWdtFreq(void)
{
    return (SYSCON->CLK_CTRL & SYSCON_CLK_CTRL_CLK_WDT_SEL_MASK) ? CLOCK_GetApbClkFreq() : CLOCK_Get32KFreq();
}

uint32_t CLOCK_GetFreq(clock_name_t clk)
{
    uint32_t freq;
    switch (clk)
    {
        case kCLOCK_CoreSysClk:
            freq = CLOCK_GetCoreSysClkFreq();
            break;
        case kCLOCK_BusClk:
            freq = CLOCK_GetAhbClkFreq();
            break;
        case kCLOCK_ApbClk:
            freq = CLOCK_GetApbClkFreq();
            break;
        case kCLOCK_WdtClk:
            freq = CLOCK_GetWdtFreq();
            break;
        case kCLOCK_FroHf:
            freq = CLOCK_GetRco32MFreq();
            break;
        case kCLOCK_Xin:
            freq = CLOCK_GetXinFreq();
            break;
        case kCLOCK_32KClk:
            freq = CLOCK_Get32KFreq();
            break;
        default:
            freq = 0;
            break;
    }

    return freq;
}

bool CLOCK_EnableUsbfs0DeviceClock(clock_usb_src_t src, uint32_t freq)
{
    CLOCK_EnableClock(kCLOCK_Usbd0);

    return true;
}

void CLOCK_EnableClkoutSource(uint32_t mask, bool enable)
{
    if (enable)
    {
        SYSCON->CLK_CTRL |= mask;
    }
    else
    {
        SYSCON->CLK_CTRL &= ~mask;
    }
}

void CLOCK_EnableClkoutPin(uint32_t mask, bool enable)
{
    if (enable)
    {
        SYSCON->PIO_WAKEUP_EN1 |= mask;
    }
    else
    {
        SYSCON->PIO_WAKEUP_EN1 &= ~mask;
    }
}

uint32_t CLOCK_GetFRGInputClock(void)
{
    return CLOCK_GetFreq(kCLOCK_BusClk);
}

uint32_t CLOCK_SetFRGClock(clock_div_name_t div_name, uint32_t freq)
{
    uint32_t input = CLOCK_GetFRGInputClock();
    uint32_t mul;

    if ((freq > 32000000) || (freq > input) || (input / freq >= 2))
    {
        /* FRG output frequency should be less than equal to 32MHz */
        return 0;
    }
    else
    {
        mul = ((uint64_t)(input - freq) * 256) / ((uint64_t)freq);
        if (div_name == kCLOCK_DivFrg0)
        {
            SYSCON->FC_FRG = (SYSCON->FC_FRG & ~SYSCON_FC_FRG_FRG_MULT0_MASK) | SYSCON_FC_FRG_FRG_MULT0(mul) |
                             SYSCON_FC_FRG_FRG_DIV0_MASK;
        }
        else if (div_name == kCLOCK_DivFrg1)
        {
            SYSCON->FC_FRG = (SYSCON->FC_FRG & ~SYSCON_FC_FRG_FRG_MULT1_MASK) | SYSCON_FC_FRG_FRG_MULT1(mul) |
                             SYSCON_FC_FRG_FRG_DIV1_MASK;
        }
		else
		{
            /* Add for avoid the misra 2004 rule 14.10 */
		}
        return 1;
    }
}
