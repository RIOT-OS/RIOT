/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Attilio Dona'
 * @}
 */

#include <assert.h>
#include "cpu.h"

#include "inc/hw_hib1p2.h"
#include "inc/hw_hib3p3.h"
#include "inc/hw_common_reg.h"


void cortexm_init(void);

#ifdef TARGET_IS_CC3200
void __attribute__((naked))
UtilsDelay(unsigned long ulCount)
{
    __asm("    subs    r0, #1\n"
          "    bne     UtilsDelay\n"
          "    bx      lr");
}


//*****************************************************************************
//
//! Reads 32-bit value from register at specified address
//!
//! \param ulRegAddr is the address of register to be read.
//!
//! This function reads 32-bit value from the register as specified by
//! \e ulRegAddr.
//!
//! \return Return the value of the register.
//
//*****************************************************************************
unsigned long PRCMHIBRegRead(unsigned long ulRegAddr)
{
  unsigned long ulValue;

  //
  // Read the Reg value
  //
  ulValue = HWREG(ulRegAddr);

  //
  // Wait for 200 uSec
  //
  UtilsDelay((80*200)/3);

  //
  // Return the value
  //
  return ulValue;
}

//*****************************************************************************
//
//! Writes 32-bit value to register at specified address
//!
//! \param ulRegAddr is the address of register to be read.
//! \param ulValue is the 32-bit value to be written.
//!
//! This function writes 32-bit value passed as \e ulValue to the register as
//! specified by \e ulRegAddr
//!
//! \return None
//
//*****************************************************************************
void PRCMHIBRegWrite(unsigned long ulRegAddr, unsigned long ulValue)
{
  //
  // Read the Reg value
  //
  HWREG(ulRegAddr) = ulValue;

  //
  // Wait for 200 uSec
  //
  UtilsDelay((80*200)/3);
}


//*****************************************************************************
//
//! MCU Initialization Routine
//!
//! This function sets mandatory configurations for the MCU
//!
//! \return None
//
//*****************************************************************************
void PRCMCC3200MCUInit()
{

  if( MAP_PRCMSysResetCauseGet() != PRCM_LPDS_EXIT )
  {

    unsigned long ulRegValue;

    //
    // DIG DCDC LPDS ECO Enable
    //
    HWREG(0x4402F064) |= 0x800000;

    //
    // Enable hibernate ECO for PG 1.32 devices only. With this ECO enabled,
    // any hibernate wakeup source will be kept maked until the device enters
    // hibernate completely (analog + digital)
    //
    ulRegValue = PRCMHIBRegRead(HIB3P3_BASE  + HIB3P3_O_MEM_HIB_REG0);
    PRCMHIBRegWrite(HIB3P3_BASE + HIB3P3_O_MEM_HIB_REG0, ulRegValue | (1<<4));

    //
    // Handling the clock switching (for 1.32 only)
    //
    HWREG(0x4402E16C) |= 0x3C;

    //
    // Enable uDMA
    //
    MAP_PRCMPeripheralClkEnable(PRCM_UDMA,PRCM_RUN_MODE_CLK);

    //
    // Reset uDMA
    //
    MAP_PRCMPeripheralReset(PRCM_UDMA);

    //
    // Disable uDMA
    //
    MAP_PRCMPeripheralClkDisable(PRCM_UDMA,PRCM_RUN_MODE_CLK);

    //
    // Enable RTC
    //
    if(MAP_PRCMSysResetCauseGet()== PRCM_POWER_ON)
    {
        PRCMHIBRegWrite(0x4402F804,0x1);
    }

    //
    // SWD mode
    //
    if(((HWREG(0x4402F0C8) & 0xFF) == 0x2))
    {
        HWREG(0x4402E110) = ((HWREG(0x4402E110) & ~0xC0F) | 0x2);
        HWREG(0x4402E114) = ((HWREG(0x4402E110) & ~0xC0F) | 0x2);
    }

    //
    // Override JTAG mux
    //
    HWREG(0x4402E184) |= 0x2;

    //
    // Change UART pins(55,57) mode to PIN_MODE_0 if they are in PIN_MODE_1
    //
    if( (HWREG(0x4402E0A4) & 0xF) == 0x1)
    {
        HWREG(0x4402E0A4) = ((HWREG(0x4402E0A4) & ~0xF));
    }

    if( (HWREG(0x4402E0A8) & 0xF) == 0x1)
    {
        HWREG(0x4402E0A8) = ((HWREG(0x4402E0A8) & ~0xF));
    }

    //
    // DIG DCDC VOUT trim settings based on PROCESS INDICATOR
    //
    if(((HWREG(0x4402DC78) >> 22) & 0xF) == 0xE)
    {
        HWREG(0x4402F0B0) = ((HWREG(0x4402F0B0) & ~(0x00FC0000))|(0x32 << 18));
    }
    else
    {
        HWREG(0x4402F0B0) = ((HWREG(0x4402F0B0) & ~(0x00FC0000))|(0x29 << 18));
    }

    //
    // Enable SOFT RESTART in case of DIG DCDC collapse
    //
    HWREG(0x4402FC74) &= ~(0x10000000);


    //
    // Disable the sleep for ANA DCDC
    //
    HWREG(0x4402F0A8) |= 0x00000004 ;
  }
  else
  {
    unsigned long ulRegVal;

    //
    // I2C Configuration
    //
    ulRegVal = HWREG(COMMON_REG_BASE + COMMON_REG_O_I2C_Properties_Register);
    ulRegVal = (ulRegVal & ~0x3) | 0x1;
    HWREG(COMMON_REG_BASE + COMMON_REG_O_I2C_Properties_Register) = ulRegVal;

    //
    // GPIO configuration
    //
    ulRegVal = HWREG(COMMON_REG_BASE + COMMON_REG_O_GPIO_properties_register);
    ulRegVal = (ulRegVal & ~0x3FF) | 0x155;
    HWREG(COMMON_REG_BASE + COMMON_REG_O_GPIO_properties_register) = ulRegVal;

  }
}
#endif

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void) {

    /* initializes the Cortex-M core */
    cortexm_init();

    PRCMCC3200MCUInit();

    /* 1 priority group */
    MAP_IntPriorityGroupingSet(0);

}
