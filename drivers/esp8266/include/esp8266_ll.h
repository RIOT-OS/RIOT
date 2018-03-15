/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website http://esp8266at.com
 * \license MIT
 * \brief   Low level, platform dependant, part for communicate with ESP module and platform.
 *
   \verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
   \endverbatim
 */
#ifndef ESP8266_LL_H
#define ESP8266_LL_H 110

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/*    Edit file name to esp8266_ll.h and edit values for your platform    */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

/**
 * \defgroup ESP8266_LL
 * \brief    Low level, platform dependent, part for communicate with ESP module and platform.
 * \{
 *
 * This is a low-level part of ESP module library.
 *
 * It provides communication between ESP module and platform. There are some function, which needs to be implemented by user and with care.
 *
 * \par U(S)ART configuration
 *
 * ESP8266 module works with U(S)ART communication with device. For this purpose, library supposes 2, USART based, functions, which are called from ESP module stack when needed:
 *
 * - \ref ESP8266_LL_USARTInit: Function, which is called when USART should be initialized
 * - \ref ESP8266_LL_USARTSend: Function, which is called when data should be sent to ESP8266 device
 *
 * ESP stack module does not check for any incoming data from ESP8266 module to USART of your device.
 *
 * Most microcontrollers have USART RX capability, so when USART RX interrupt happens,
 * you should send this received byte to ESP8266 module using \ref ESP8266_DataReceived function to notify new incoming data.
 * Use interrupt handler routine to notify new data using previous mentioned function.
 *
   \code
   //Code below show example procedure and does need to be directly for your platform.
   //You have to make sure to properly configure UART and RX interrupt for it.

   //USART Initialization function, which is called from ESP stack
   uint8_t ESP8266_LL_USARTInit(uint32_t baudrate) {
    //Init USART peripheral and GPIO clock for it.
    //Use baudate as passed in function.

    //Enable RX Data Ready interrupt (RXNE = Received Not Empty)

 #if ESP8266_USE_CTS
    //Enable custom RTS pin as output and connect it to ESP8266 CTS pin
 #endif

    //Return 0 = Successful
    return 0;
   }

   //USART send function, which is called from ESP stack
   uint8_t ESP8266_LL_USARTSend(uint8_t* data, uint16_t count) {
    //Send data via USART

    //Return 0 = Successful
    return 0;
   }

   //USART receive interrupt handler,
   //Function names depends on your platform
   void USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME(void) {
    uint8_t ch;
    //Get character from USART, example code
    uart_read_character(&ch);

    //Send received character to ESP stack
    ESP8266_DataReceived(&ch, 1);
   }

 #if ESP8266_USE_CTS
   void ESP8266_LL_SetRTS(uint8_t dir) {
    if (dir) {
        //Set RTS pin high
    } else {
        //Set RTS pin low
    }
   }
 #endif

   \endcode
 *
 * \par Reset configuration
 *
 * ESP8266 module can be reset using AT commands. However, it may happen that ESP module ignores AT commands for some reasons.
 *
 * You need to implement 3 macros or functions for GPIO capability for reset purpose.
 *
 * Take a look for \ref ESP8266_RESET_INIT, \ref ESP8266_RESET_LOW and \ref ESP8266_RESET_HIGH macros.
 *
   \code
   //Examples for 3 defines
   //Implementation must be made according to your platform
   //This is just a technical pseudo code and does not need to work on any platform

   //Init reset pin as output
   //Use do-while if you must specify more than 1 statement in define
 #define ESP8266_RESET_INIT        do {  \
    GPIOA->MODER |= GPIO_Pin_5;         \
    GPIOA->OTYPER |= GPIO_Pin_5;        \
   } while (0)

   //Set reset pin high
 #define ESP8266_RESET_HIGH        GPIOA->ODR |= GPIO_Pin_5

   //Set reset pin low
 #define ESP8266_RESET_LOW         GPIOA->ODR &= ~GPIO_Pin_5
   \endcode
 *
 * \par Time configuration
 *
 * ESP module part needs current time in milliseconds to take care of any possible timeouts on connections and similar things.
 *
 * You need to implement your own time source (systick interrupt, normal timer with interrupt capability, etc) to tell ESP stack current time.
 *
 * Use \ref ESP8266_TimeUpdate to notify ESP stack with new time.
 *
   \code
   //Example of configuring timer for 1ms interrupts

   //Function for timer init
   void timer_init() {
    //Enable timer clock if needed

    //Set timer prescaler and period according to selected platform and MCU speed
    //Set values for 1ms overflow

    //Enable timer interrupt overflow
   }

   int main(void) {
    //Init timer
    timer_init();

    //Do other job
    ...
   }

   //Handle timer interrupts
   void TIMER_INTERRUPT_FUNCTION_NAME(void) {
    //Call stack function and notify time update
    //Notify that time has been updated for 1 ms according to last time
    ESP8266_TimeUpdate(&ESP8266, 1);
   }
   \endcode
 */

/* Include ESP layer */
#include "esp8266.h"

/* ESP defines */
#define ESP_RTS_HIGH         1
#define ESP_RTS_LOW          0

/**
 * \brief  Initializes U(S)ART peripheral for ESP8266 communication
 * \note   This function is called from ESP stack
 * \param  baudrate: baudrate for USART you have to use when initializing USART peripheral
 * \retval Initialization status:
 *           - 0: Initialization OK
 *           - > 0: Initialization failed
 */
uint8_t ESP8266_LL_USARTInit(uint32_t baudrate);

/**
 * \brief  Sends data from ESP stack to ESP8266 module using USART
 * \param  *data: Pointer to data array which should be sent
 * \param  count: Number of data bytes to sent to module
 * \retval Sent status:
 *           - 0: Sent OK
 *           - > 0: Sent error
 */
uint8_t ESP8266_LL_USARTSend(uint8_t *data, uint16_t count);

/**
 * \brief  Sets software RTS pin on microcontroller
 * \param  dir: Pin direction
 * \retval None
 */
void ESP8266_LL_SetRTS(uint8_t dir);

/**
 * \brief  Initializes reset pin on platform
 * \note   Function is called from ESP stack module when needed
 * \note   Declared as macro
 */
#define ESP8266_RESET_INIT    (void)0

/**
 * \brief  Sets reset pin LOW
 * \note   Function is called from ESP stack module when needed
 * \note   Declared as macro
 */
#define ESP8266_RESET_LOW     (void)0

/**
 * \brief  Sets reset pin HIGH
 * \note   Function is called from ESP stack module when needed
 * \note   Declared as macro
 */
#define ESP8266_RESET_HIGH    (void)0

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
