//------------------------------------------------------------------------------
//
//  Copyright (C) 2017
//
//  Module      : IRQ
//  File        : s5p4418_clkpwr.h
//  Description : S5P4418 CLOCK POWER
//  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)
//  History     :
//------------------------------------------------------------------------------

#ifndef _S5P4418_CLKPWR_H_
#define _S5P4418_CLKPWR_H_

//------------------------------------------------------------------------------
//  includes
//------------------------------------------------------------------------------
#include <stdint.h>
#include "types.h"
#include "s5p4418_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif


//------------------------------------------------------------------------------
/// @brief  CLK PWR register set structure
//------------------------------------------------------------------------------

typedef struct
{
    volatile uint32_t CLKMODEREG0;              //< 0x000 : Clock Mode Register 0
    volatile uint32_t __Reserved0;              //< 0x004
    volatile uint32_t PLLSETREG[4];             //< 0x008 ~ 0x014 : PLL Setting Register
    volatile uint32_t __Reserved1[2];           //< 0x018 ~ 0x01C
    volatile uint32_t DVOREG[5];                //< 0x020 ~ 0x030 : Divider Setting Register
    volatile uint32_t __Reserved2[5];           //< 0x034 ~ 0x044
    volatile uint32_t PLLSETREG_SSCG[6];        //< 0x048 ~ 0x05C
    volatile uint32_t __reserved3[8];           //< 0x060 ~ 0x07C
    volatile uint32_t __Reserved4[(512-128)/4]; //< padding (0x200-..)/4
    volatile uint32_t GPIOWAKEUPRISEENB;        //< 0x200 : GPIO Rising Edge Detect Enable Register
    volatile uint32_t GPIOWAKEUPFALLENB;        //< 0x204 : GPIO Falling Edge Detect Enable Register
    volatile uint32_t GPIORSTENB;               //< 0x208 : GPIO Reset Enable Register
    volatile uint32_t GPIOWAKEUPENB;            //< 0x20C : GPIO Wakeup Source Enable
    volatile uint32_t GPIOINTENB;               //< 0x210 : Interrupt Enable Register
    volatile uint32_t GPIOINTPEND;              //< 0x214 : Interrupt Pend Register
    volatile uint32_t RESETSTATUS;              //< 0x218 : Reset Status Register
    volatile uint32_t INTENABLE;                //< 0x21C : Interrupt Enable Register
    volatile uint32_t INTPEND;                  //< 0x220 : Interrupt Pend Register
    volatile uint32_t PWRCONT;                  //< 0x224 : Power Control Register
    volatile uint32_t PWRMODE;                  //< 0x228 : Power Mode Register
    volatile uint32_t __Reserved5;              //< 0x22C : Reserved Region
    volatile uint32_t SCRATCH[3];               //< 0x230 ~ 0x238: Scratch Register
    volatile uint32_t SYSRSTCONFIG;             //< 0x23C : System Reset Configuration Register.
    volatile uint8_t  __Reserved6[0x100-0x80];  //< 0x80 ~ 0xFC  : Reserved Region
    volatile uint32_t PADSTRENGTHGPIO[5][2];    //< 0x100, 0x104 : GPIOA Pad Strength Register
                                                //< 0x108, 0x10C : GPIOB Pad Strength Register
                                                //< 0x110, 0x114 : GPIOC Pad Strength Register
                                                //< 0x118, 0x11C : GPIOD Pad Strength Register
                                                //< 0x120, 0x124 : GPIOE Pad Strength Register
    volatile uint32_t __Reserved7[2];           //< 0x128 ~ 0x12C: Reserved Region
    volatile uint32_t PADSTRENGTHBUS;           //< 0x130        : Bus Pad Strength Register
}clkpwr_typedef;

/* CLKPWR module base address */
#define CLKPWR  ((clkpwr_typedef *) PHY_BASEADDR_CLKPWR_MODULE)


#ifdef __cplusplus
}
#endif

#endif // _S5P4418_UART_H_
