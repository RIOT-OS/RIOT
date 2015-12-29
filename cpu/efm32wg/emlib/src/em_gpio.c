/***************************************************************************//**
 * @file em_gpio.c
 * @brief General Purpose IO (GPIO) peripheral API
 *   devices.
 * @version 4.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/


#include "em_gpio.h"

#if defined(GPIO_COUNT) && (GPIO_COUNT > 0)
/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup GPIO
 * @brief General Purpose Input/Output (GPIO) API
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Validation of pin typically usable in assert statements. */
#define GPIO_DRIVEMODE_VALID(mode)    ((mode) <= 3)

/** @endcond */


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Sets the pin location of the debug pins (Serial Wire interface).
 *
 * @note
 *   Changing the pins used for debugging uncontrolled, may result in a lockout.
 *
 * @param[in] location
 *   The debug pin location to use (0-3).
 ******************************************************************************/
void GPIO_DbgLocationSet(unsigned int location)
{
#if defined ( _GPIO_ROUTE_SWLOCATION_MASK )
  EFM_ASSERT(location < AFCHANLOC_MAX);

  GPIO->ROUTE = (GPIO->ROUTE & ~_GPIO_ROUTE_SWLOCATION_MASK) |
                (location << _GPIO_ROUTE_SWLOCATION_SHIFT);
#else
  (void)location;
#endif
}


/***************************************************************************//**
 * @brief
 *   Sets the drive mode for a GPIO port.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] mode
 *   Drive mode to use for port.
 ******************************************************************************/
void GPIO_DriveModeSet(GPIO_Port_TypeDef port, GPIO_DriveMode_TypeDef mode)
{
  EFM_ASSERT(GPIO_PORT_VALID(port) && GPIO_DRIVEMODE_VALID(mode));

  GPIO->P[port].CTRL = (GPIO->P[port].CTRL & ~(_GPIO_P_CTRL_DRIVEMODE_MASK))
                       | (mode << _GPIO_P_CTRL_DRIVEMODE_SHIFT);
}


/***************************************************************************//**
 * @brief
 *   Configure GPIO interrupt.
 *
 * @details
 *   If reconfiguring a GPIO interrupt that is already enabled, it is generally
 *   recommended to disable it first, see GPIO_Disable().
 *
 *   The actual GPIO interrupt handler must be in place before enabling the
 *   interrupt.
 *
 *   Notice that any pending interrupt for the selected pin is cleared by this
 *   function.
 *
 * @note
 *   A certain pin number can only be associated with one port. Ie, if GPIO
 *   interrupt 1 is assigned to port A/pin 1, then it is not possibly to use
 *   pin 1 from any other ports for interrupts. Please refer to the reference
 *   manual.
 *
 * @param[in] port
 *   The port to associate with @p pin.
 *
 * @param[in] pin
 *   The GPIO interrupt number (= port pin).
 *
 * @param[in] risingEdge
 *   Set to true if interrupts shall be enabled on rising edge, otherwise false.
 *
 * @param[in] fallingEdge
 *   Set to true if interrupts shall be enabled on falling edge, otherwise false.
 *
 * @param[in] enable
 *   Set to true if interrupt shall be enabled after configuration completed,
 *   false to leave disabled. See GPIO_IntDisable() and GPIO_IntEnable().
 ******************************************************************************/
void GPIO_IntConfig(GPIO_Port_TypeDef port,
                    unsigned int pin,
                    bool risingEdge,
                    bool fallingEdge,
                    bool enable)
{
  uint32_t tmp;

  EFM_ASSERT(GPIO_PORT_VALID(port) && GPIO_PIN_VALID(pin));

  /* There are two registers controlling the interrupt configuration:
   * The EXTIPSELL register controls pins 0-7 and EXTIPSELH controls
   * pins 8-15. */
  if (pin < 8)
  {
    GPIO->EXTIPSELL = (GPIO->EXTIPSELL & ~(0xF << (4 * pin))) |
                      (port << (4 * pin));
  }
  else
  {
    tmp             = pin - 8;
    GPIO->EXTIPSELH = (GPIO->EXTIPSELH & ~(0xF << (4 * tmp))) |
                      (port << (4 * tmp));
  }

  /* Enable/disable rising edge */
  BITBAND_Peripheral(&(GPIO->EXTIRISE), pin, (unsigned int)risingEdge);

  /* Enable/disable falling edge */
  BITBAND_Peripheral(&(GPIO->EXTIFALL), pin, (unsigned int)fallingEdge);

  /* Clear any pending interrupt */
  GPIO->IFC = 1 << pin;

  /* Finally enable/disable interrupt */
  BITBAND_Peripheral(&(GPIO->IEN), pin, (unsigned int)enable);
}


/***************************************************************************//**
 * @brief
 *   Set the mode for a GPIO pin.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] pin
 *   The pin number in the port.
 *
 * @param[in] mode
 *   The desired pin mode.
 *
 * @param[in] out
 *   Value to set for pin in DOUT register. The DOUT setting is important for
 *   even some input mode configurations, determining pull-up/down direction.
 ******************************************************************************/
void GPIO_PinModeSet(GPIO_Port_TypeDef port,
                     unsigned int pin,
                     GPIO_Mode_TypeDef mode,
                     unsigned int out)
{
  EFM_ASSERT(GPIO_PORT_VALID(port) && GPIO_PIN_VALID(pin));

  /* If disabling pin, do not modify DOUT in order to reduce chance for */
  /* glitch/spike (may not be sufficient precaution in all use cases) */
  if (mode != gpioModeDisabled)
  {
    if (out)
    {
      GPIO->P[port].DOUTSET = 1 << pin;
    }
    else
    {
      GPIO->P[port].DOUTCLR = 1 << pin;
    }
  }

  /* There are two registers controlling the pins for each port. The MODEL
   * register controls pins 0-7 and MODEH controls pins 8-15. */
  if (pin < 8)
  {
    GPIO->P[port].MODEL = (GPIO->P[port].MODEL & ~(0xF << (pin * 4))) |
                          (mode << (pin * 4));
  }
  else
  {
    GPIO->P[port].MODEH = (GPIO->P[port].MODEH & ~(0xF << ((pin - 8) * 4))) |
                          (mode << ((pin - 8) * 4));
  }

  if (mode == gpioModeDisabled)
  {
    if (out)
    {
      GPIO->P[port].DOUTSET = 1 << pin;
    }
    else
    {
      GPIO->P[port].DOUTCLR = 1 << pin;
    }
  }
}

/** @} (end addtogroup GPIO) */
/** @} (end addtogroup EM_Library) */

#endif /* defined(GPIO_COUNT) && (GPIO_COUNT > 0) */
