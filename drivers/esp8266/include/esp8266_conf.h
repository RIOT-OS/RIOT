/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website http://esp8266at.com
 * \license MIT
 * \brief   Configuration part for ESP8266 library
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
#ifndef ESP8266_CONF_H
#define ESP8266_CONF_H 100

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/*   Edit file name to esp8266_conf.h and edit values for your platform   */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

/**
 * \defgroup ESP8266_CONF
 * \brief    Configuration parameters for ESP8266 library
 * \{
 */

/**
 * \brief   USART input buffer size. This buffer stores data received from USART RX interrupt 
 *          before stack use them.
 *
 *          Size of buffer depends on speed of calling \ref ESP8266_Update function, ESP baudrate and
 *          max ESP receive string size.
 *
 * \note    When possible, buffer should be at least 1024 bytes.
 */
#define ESP8266_USARTBUFFER_SIZE                  1024

/**
 * \brief   Temporary buffer size. 
 *
 *          When stack sends command, and it waits for "OK" or "ERROR" or something else and if there are data received,
 *          which are not part of this command, then all that data are stored in temporary buffer before they are processed.
 *
 *          Size of buffer depends on speed of calling \ref ESP8266_Update function, ESP baudrate and
 *          max ESP receive string size.
 *
 * \note    When possible, buffer should be at least 512 bytes for safety reasons.
 */
#define ESP8266_TMPBUFFER_SIZE                    512

/**
 * \brief   This options allows you to specify if you will use single buffer which will be shared between
 *          all connections together. You can use this option on small embedded systems where you have limited RAM resource.
 *
 *          If option is set to 0, then each connection will have separate data buffer which decreases option for data loss.
 */
#define ESP8266_USE_SINGLE_CONNECTION_BUFFER      1

/**
 * \brief   Buffer size for connection data.
 *
 *          Data packs from ESP module (+IPD statements) have normally up to 1460 bytes on one packet.
 *
 *          However, it may happen, tnat ESP will send you a pack of data with 4x normal size (4 * 1460) bytes.
 *
 *          If you want all that data, you should have at least 4 * 1460 bytes of data available, otherwise,
 *          data will be ignored if buffer will have overflow
 *
 * \note    Buffer must be at least 256 bytes or there might be unexpected results
 */
#define ESP8266_CONNECTION_BUFFER_SIZE            5842

/**
 * \brief   Enables (1) or disables (0) pinging functionality to other servers
 *
 *          When enabled, additional structure for pinging is added to main structure,
 *          but it is removed if feature is disabled
 */
#define ESP8266_USE_PING                          1

/**
 * \brief   Enables (1) or disables (0) update ESP8288 AT software via network using AT commands.
 *
 *          When using ESP8266 with at least 8Mbit flash memory and proper AT software flashed on it (AT with boot support),
 *          ESP8266 can self update firmware if proper AT command is used.
 *
 *          This macro enables/disables functions for firmware updating
 */
#define ESP8266_USE_FIRMWAREUPDATE                1

/**
 * \brief   Enables (1) or disables (0) feature to search network for Acces points
 *
 *          You can disable this feature if you know exact network where you will connect to save some memory
 */
#define ESP8266_USE_APSEARCH                      1

/**
 * \brief   Sets how many AP stations stack will parse which are received from ESP8266 module
 *
 *          Set to 10 by default.
 *
 * \note    \ref ESP8266_USE_APSEARCH must be enabled for this feature
 */
#define ESP8266_MAX_DETECTED_AP                   10

/**
 * \brief   Number of characters SSID can have when we are connected to it.
 *
 *          If it has more than used here, unexpected behavior can occur.
 */
#define ESP8266_MAX_CONNECTED_SSID_NAME_LENGTH    16

/**
 * \brief   Maximal SSID name in any case used in library except on connected SSID name
 */
#define ESP8266_MAX_SSID_NAME                     20

/**
 * \brief   Enables (1) or disables (0) functions for setting WPS
 */
#define ESP8266_USE_WPS                            1

/** 
 * \brief   Enables (1) or disables (0) functions for getting a list
 *          of connected stations to our software AP
 */
#define ESP8266_USE_CONNECTED_STATIONS            1

/**
 * \brief   Enables (1) or disables (0) CTS pin on ESP8266 and software RTS pin on microcontroller
 *          
 *          When option is enabled, then MCU must implement RTS software output pin to disable ESP
 *          module to send data before they are read from buffer.
 *          This prevents loosing data if you have low RAM memory and you need to do hard work with data,
 *          like saving data to SDCARD.
 * 
 *          Function to control software RTS output on MCU is called from function
 *          which adds data to internal buffer of ESP stack.
 *         
 *          When buffer is read from ESP8266_Update function, RTS should be set low again.
 *
 * \note    Software RTS pin from microcontroller (any pin selected by user)
 *           must be connected to CTS pin on ESP8266.
 */
#define ESP8266_USE_CTS                           0

/**
 * \brief   Enables (1) or disables (0) SNTP feature for ESP8266.
 *
 *          This command does not work with official AT commands build yet.
 *           Maybe Espresif will add this command.
 */
#define ESP8266_USE_SNTP                          0

/**
 * \}
 */

#endif
