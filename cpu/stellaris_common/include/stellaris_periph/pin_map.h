//*****************************************************************************
//
// pin_map.h - Mapping of peripherals to pins for all parts.
//
// Copyright (c) 2007-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 9453 of the Stellaris Peripheral Driver Library.
//
//*****************************************************************************

#ifndef __PIN_MAP_H__
#define __PIN_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
// LM4F120H5QR Port/Pin Mapping Definitions
//
//*****************************************************************************
#ifdef PART_LM4F120H5QR

#define GPIO_PA0_U0RX           0x00000001

#define GPIO_PA1_U0TX           0x00000401

#define GPIO_PA2_SSI0CLK        0x00000802

#define GPIO_PA3_SSI0FSS        0x00000C02

#define GPIO_PA4_SSI0RX         0x00001002

#define GPIO_PA5_SSI0TX         0x00001402

#define GPIO_PA6_I2C1SCL        0x00001803

#define GPIO_PA7_I2C1SDA        0x00001C03

#define GPIO_PB0_U1RX           0x00010001
#define GPIO_PB0_T2CCP0         0x00010007

#define GPIO_PB1_U1TX           0x00010401
#define GPIO_PB1_T2CCP1         0x00010407

#define GPIO_PB2_I2C0SCL        0x00010803
#define GPIO_PB2_T3CCP0         0x00010807

#define GPIO_PB3_I2C0SDA        0x00010C03
#define GPIO_PB3_T3CCP1         0x00010C07

#define GPIO_PB4_SSI2CLK        0x00011002
#define GPIO_PB4_CAN0RX         0x00011008
#define GPIO_PB4_T1CCP0         0x00011007

#define GPIO_PB5_SSI2FSS        0x00011402
#define GPIO_PB5_CAN0TX         0x00011408
#define GPIO_PB5_T1CCP1         0x00011407

#define GPIO_PB6_SSI2RX         0x00011802
#define GPIO_PB6_T0CCP0         0x00011807

#define GPIO_PB7_SSI2TX         0x00011C02
#define GPIO_PB7_T0CCP1         0x00011C07

#define GPIO_PC0_TCK            0x00020001
#define GPIO_PC0_SWCLK          0x00020001
#define GPIO_PC0_T4CCP0         0x00020007

#define GPIO_PC1_TMS            0x00020401
#define GPIO_PC1_SWDIO          0x00020401
#define GPIO_PC1_T4CCP1         0x00020407

#define GPIO_PC2_TDI            0x00020801
#define GPIO_PC2_T5CCP0         0x00020807

#define GPIO_PC3_SWO            0x00020C01
#define GPIO_PC3_TDO            0x00020C01
#define GPIO_PC3_T5CCP1         0x00020C07

#define GPIO_PC4_U4RX           0x00021001
#define GPIO_PC4_U1RX           0x00021002
#define GPIO_PC4_WT0CCP0        0x00021007
#define GPIO_PC4_U1RTS          0x00021008

#define GPIO_PC5_U4TX           0x00021401
#define GPIO_PC5_U1TX           0x00021402
#define GPIO_PC5_WT0CCP1        0x00021407
#define GPIO_PC5_U1CTS          0x00021408

#define GPIO_PC6_U3RX           0x00021801
#define GPIO_PC6_WT1CCP0        0x00021807

#define GPIO_PC7_U3TX           0x00021C01
#define GPIO_PC7_WT1CCP1        0x00021C07

#define GPIO_PD0_SSI3CLK        0x00030001
#define GPIO_PD0_SSI1CLK        0x00030002
#define GPIO_PD0_I2C3SCL        0x00030003
#define GPIO_PD0_WT2CCP0        0x00030007

#define GPIO_PD1_SSI3FSS        0x00030401
#define GPIO_PD1_SSI1FSS        0x00030402
#define GPIO_PD1_I2C3SDA        0x00030403
#define GPIO_PD1_WT2CCP1        0x00030407

#define GPIO_PD2_SSI3RX         0x00030801
#define GPIO_PD2_SSI1RX         0x00030802
#define GPIO_PD2_WT3CCP0        0x00030807

#define GPIO_PD3_SSI3TX         0x00030C01
#define GPIO_PD3_SSI1TX         0x00030C02
#define GPIO_PD3_WT3CCP1        0x00030C07

#define GPIO_PD4_U6RX           0x00031001
#define GPIO_PD4_WT4CCP0        0x00031007

#define GPIO_PD5_U6TX           0x00031401
#define GPIO_PD5_WT4CCP1        0x00031407

#define GPIO_PD6_U2RX           0x00031801
#define GPIO_PD6_WT5CCP0        0x00031807

#define GPIO_PD7_U2TX           0x00031C01
#define GPIO_PD7_WT5CCP1        0x00031C07
#define GPIO_PD7_NMI            0x00031C08

#define GPIO_PE0_U7RX           0x00040001

#define GPIO_PE1_U7TX           0x00040401

#define GPIO_PE4_U5RX           0x00041001
#define GPIO_PE4_I2C2SCL        0x00041003
#define GPIO_PE4_CAN0RX         0x00041008

#define GPIO_PE5_U5TX           0x00041401
#define GPIO_PE5_I2C2SDA        0x00041403
#define GPIO_PE5_CAN0TX         0x00041408

#define GPIO_PF0_U1RTS          0x00050001
#define GPIO_PF0_SSI1RX         0x00050002
#define GPIO_PF0_CAN0RX         0x00050003
#define GPIO_PF0_T0CCP0         0x00050007
#define GPIO_PF0_NMI            0x00050008
#define GPIO_PF0_C0O            0x00050009
#define GPIO_PF0_TRD2           0x0005000E

#define GPIO_PF1_U1CTS          0x00050401
#define GPIO_PF1_SSI1TX         0x00050402
#define GPIO_PF1_T0CCP1         0x00050407
#define GPIO_PF1_C1O            0x00050409
#define GPIO_PF1_TRD1           0x0005040E

#define GPIO_PF2_T1CCP0         0x00050807
#define GPIO_PF2_SSI1CLK        0x00050802
#define GPIO_PF2_TRD0           0x0005080E

#define GPIO_PF3_CAN0TX         0x00050C03
#define GPIO_PF3_T1CCP1         0x00050C07
#define GPIO_PF3_SSI1FSS        0x00050C02
#define GPIO_PF3_TRCLK          0x00050C0E

#define GPIO_PF4_T2CCP0         0x00051007

#endif // PART_LM4F120H5QR

//*****************************************************************************
//
// Pin Mapping Functions
//
// This section describes the code that is responsible for handling the
// mapping of peripheral functions to their physical location on the pins of
// a device.
//
//*****************************************************************************

//*****************************************************************************
//
// Definitions to support mapping GPIO Ports and Pins to their function.
//
//*****************************************************************************

//*****************************************************************************
//
// Configures the specified ADC pin to function as an ADC pin.
//
// \param ulName is one of the valid names for the ADC pins.
//
// This function takes on of the valid names for an ADC pin and configures
// the pin for its ADC functionality depending on the part that is defined.
//
// The valid names for the pins are as follows: \b ADC0, \b ADC1, \b ADC2,
// \b ADC3, \b ADC4, \b ADC5, \b ADC6, or \b ADC7.
//
// \sa GPIOPinTypeADC() in order to configure multiple ADC pins at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypeADC(ulName)      GPIOPinTypeADC(ulName##_PORT, ulName##_PIN)

//*****************************************************************************
//
// Configures the specified CAN pin to function as a CAN pin.
//
// \param ulName is one of the valid names for the CAN pins.
//
// This function takes one of the valid names for a CAN pin and configures
// the pin for its CAN functionality depending on the part that is defined.
//
// The valid names for the pins are as follows: \b CAN0RX, \b CAN0TX,
// \b CAN1RX, \b CAN1TX, \b CAN2RX, or \b CAN2TX.
//
// \sa GPIOPinTypeCAN() in order to configure multiple CAN pins at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypeCAN(ulName)      GPIOPinTypeCAN(ulName##_PORT, ulName##_PIN)

//*****************************************************************************
//
// Configures the specified comparator pin to function as a comparator pin.
//
// \param ulName is one of the valid names for the Comparator pins.
//
// This function takes one of the valid names for a comparator pin and
// configures the pin for its comparator functionality depending on the part
// that is defined.
//
// The valid names for the pins are as follows: \b C0_MINUS, \b C0_PLUS,
// \b C1_MINUS, \b C1_PLUS, \b C2_MINUS, or \b C2_PLUS.
//
// \sa GPIOPinTypeComparator() in order to configure multiple comparator pins
// at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypeComparator(ulName)                                    \
                                GPIOPinTypeComparator(ulName##_PORT, \
                                                      ulName##_PIN)

//*****************************************************************************
//
// Configures the specified I2C pin to function as an I2C pin.
//
// \param ulName is one of the valid names for the I2C pins.
//
// This function takes one of the valid names for an I2C pin and configures
// the pin for its I2C functionality depending on the part that is defined.
//
// The valid names for the pins are as follows: \b I2C0SCL, \b I2C0SDA,
// \b I2C1SCL, or \b I2C1SDA.
//
// \sa GPIOPinTypeI2C() in order to configure multiple I2C pins at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypeI2C(ulName)      GPIOPinTypeI2C(ulName##_PORT, ulName##_PIN)

//*****************************************************************************
//
// Configures the specified Ethernet LED to function as an Ethernet LED pin.
//
// \param ulName is one of the valid names for the Ethernet LED pins.
//
// This function takes one of the valid names for an Ethernet LED pin and
// configures the pin for its Ethernet LED functionality depending on the part
// that is defined.
//
// The valid names for the pins are as follows: \b LED0 or \b LED1.
//
// sa GPIOPinTypeEthernetLED() in order to configure multiple Ethernet LED
// pins at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypeEthernetLED(ulName)                                    \
                                GPIOPinTypeEthernetLED(ulName##_PORT, \
                                                       ulName##_PIN)

//*****************************************************************************
//
// Configures the specified PWM pin to function as a PWM pin.
//
// \param ulName is one of the valid names for the PWM pins.
//
// This function takes one of the valid names for a PWM pin and configures
// the pin for its PWM functionality depending on the part that is defined.
//
// The valid names for the pins are as follows: \b PWM0, \b PWM1, \b PWM2,
// \b PWM3, \b PWM4, \b PWM5, or \b FAULT.
//
// \sa GPIOPinTypePWM() in order to configure multiple PWM pins at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypePWM(ulName)      GPIOPinTypePWM(ulName##_PORT, ulName##_PIN)

//*****************************************************************************
//
// Configures the specified QEI pin to function as a QEI pin.
//
// \param ulName is one of the valid names for the QEI pins.
//
// This function takes one of the valid names for a QEI pin and configures
// the pin for its QEI functionality depending on the part that is defined.
//
// The valid names for the pins are as follows: \b PHA0, \b PHB0, \b IDX0,
// \b PHA1, \b PHB1, or \b IDX1.
//
// \sa GPIOPinTypeQEI() in order to configure multiple QEI pins at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypeQEI(ulName)      GPIOPinTypeQEI(ulName##_PORT, ulName##_PIN)

//*****************************************************************************
//
// Configures the specified SSI pin to function as an SSI pin.
//
// \param ulName is one of the valid names for the SSI pins.
//
// This function takes one of the valid names for an SSI pin and configures
// the pin for its SSI functionality depending on the part that is defined.
//
// The valid names for the pins are as follows: \b SSI0CLK, \b SSI0FSS,
// \b SSI0RX, \b SSI0TX, \b SSI1CLK, \b SSI1FSS, \b SSI1RX, or \b SSI1TX.
//
// \sa GPIOPinTypeSSI() in order to configure multiple SSI pins at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypeSSI(ulName)      GPIOPinTypeSSI(ulName##_PORT, ulName##_PIN)

//*****************************************************************************
//
// Configures the specified Timer pin to function as a Timer pin.
//
// \param ulName is one of the valid names for the Timer pins.
//
// This function takes one of the valid names for a Timer pin and configures
// the pin for its Timer functionality depending on the part that is defined.
//
// The valid names for the pins are as follows: \b CCP0, \b CCP1, \b CCP2,
// \b CCP3, \b CCP4, \b CCP5, \b CCP6, or \b CCP7.
//
// \sa GPIOPinTypeTimer() in order to configure multiple CCP pins at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypeTimer(ulName)    GPIOPinTypeTimer(ulName##_PORT, ulName##_PIN)

//*****************************************************************************
//
// Configures the specified UART pin to function as a UART pin.
//
// \param ulName is one of the valid names for the UART pins.
//
// This function takes one of the valid names for a UART pin and configures
// the pin for its UART functionality depending on the part that is defined.
//
// The valid names for the pins are as follows: \b U0RX, \b U0TX, \b U1RX,
// \b U1TX, \b U2RX, or \b U2TX.
//
// \sa GPIOPinTypeUART() in order to configure multiple UART pins at once.
//
// \return None.
//
//*****************************************************************************
#define PinTypeUART(ulName)     GPIOPinTypeUART(ulName##_PORT, ulName##_PIN)

//*****************************************************************************
//
//! Configures the specified USB digital pin to function as a USB pin.
//!
//! \param ulName is one of the valid names for a USB digital pin.
//!
//! This function takes one of the valid names for a USB digital pin and
//! configures the pin for its USB functionality depending on the part that is
//! defined.
//!
//! The valid names for the pins are as follows: \b EPEN or \b PFAULT.
//!
//! \sa GPIOPinTypeUSBDigital() in order to configure multiple USB pins at
//! once.
//!
//! \return None.
//
//*****************************************************************************
#define PinTypeUSBDigital(ulName)                                    \
                                GPIOPinTypeUSBDigital(ulName##_PORT, \
                                                      ulName##_PIN)

//*****************************************************************************
//
//! Enables the peripheral port used by the given pin.
//!
//! \param ulName is one of the valid names for a pin.
//!
//! This function takes one of the valid names for a pin function and
//! enables the peripheral port for that pin depending on the part that is
//! defined.
//!
//! Any valid pin name can be used.
//!
//! \sa SysCtlPeripheralEnable() in order to enable a single port when
//! multiple pins are on the same port.
//!
//! \return None.
//
//*****************************************************************************
#define PeripheralEnable(ulName)                                    \
                                SysCtlPeripheralEnable(ulName##_PERIPH)

#ifdef __cplusplus
}
#endif

#endif // __PIN_MAP_H__
