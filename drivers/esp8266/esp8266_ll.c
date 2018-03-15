/**
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
 */
#include "include/esp8266_ll.h"
#include "periph/uart.h"

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/*    Edit file name to esp8266_ll.c and edit values for your platform    */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

/* USART receive interrupt handler */
void USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME(void *arg, uint8_t c)
{
    uint8_t ch;

    /* Get character from USART */
    ch = c;

    /* Send received character to ESP stack */
    ESP8266_DataReceived(&ch, 1);
}


uint8_t ESP8266_LL_USARTInit(uint32_t baudrate)
{
    /* Init USART */
    if (uart_init(1, baudrate, USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME, NULL) < 0) {
        return -1;
    }
#if ESP8266_USE_CTS
    /* Enable RTS pin on MCU as output and set it low */
#endif

    /* Return 0 = Successful */
    return 0;
}

uint8_t ESP8266_LL_USARTSend(uint8_t *data, uint16_t count)
{
    /* Send data via USART */
    uart_write(1, (uint8_t *)data, count);
    /* Return 0 = Successful */
    return 0;
}


void ESP8266_LL_SetRTS(uint8_t dir)
{
    /* Set RTS direction we require */
    if (dir == ESP_RTS_HIGH) {
        //Set RTS pin high
    }
    else {
        //Set RTS pin low
    }
}
