/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website http://esp8266at.com
 * \version v1.1
 * \license MIT
 * \brief   Library for ESP8266 module using AT commands for embedded systems
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
#ifndef ESP8266_H
#define ESP8266_H 110

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
 
/**
 * \defgroup ESP8266_API
 * \brief    High level, application part of module
 * \{
 *
 * \par Platform dependant implementation
 *
 * Library itself is platform independent, however, USART and GPIO things must be implemented by user.
 * 
 * Please follow instructions provided in \ref ESP8266_LL page.
 *
 * \par Dependencies
 *
\verbatim
 - string.h
 - stdio.h
 - stdint.h
 - buffer.h
 - esp8266_ll.h
 - esp8266_conf.h
\endverbatim
 */

/* Standard C libraries */
#include "string.h"
#include "stdio.h"
#include "stdint.h"

/* Low level based implementation */
#include "esp8266_ll.h"

/* Include configuration */
#include "esp8266_conf.h"

/* Buffer implementation */
#include "buffer.h"

/* Check values */
#if !defined(ESP8266_CONF_H) || ESP8266_CONF_H != ESP8266_H
//#error Wrong configuration file!
#endif

/**
 * \defgroup ESP8266_Macros
 * \brief    Library defines
 * \{
 */

/* This settings should not be modified */
#define ESP8266_MAX_CONNECTIONS        5  /*!< Number of maximum active connections on ESP */
#define ESP8266_MAX_CONNECTEDSTATIONS  10 /*!< Number of AP stations saved to received data array */

/* Check for GNUC */
#ifndef __weak  
#if defined (__GNUC__)  
#define __weak      __attribute__((weak))
#endif
#endif  /* Weak attribute */

/* Check debug */
#ifndef ESP8266_ECHO
#define ESP8266_ECHO           1
#endif

/**
 * \}
 */
 
/**
 * \defgroup ESP8266_Typedefs
 * \brief    Library Typedefs
 * \{
 */

/**
 * \brief  ESP8266 library possible return statements on function calls
 */
typedef enum {
    ESP_OK = 0x00,          /*!< Everything is OK */
    ESP_ERROR,              /*!< An error occurred */
    ESP_DEVICENOTCONNECTED, /*!< Device is not connected to UART */
    ESP_TIMEOUT,            /*!< Timeout was detected when sending command to ESP module */
    ESP_NOHEAP,             /*!< Heap memory is not available */
    ESP_WIFINOTCONNECTED,   /*!< Wifi is not connected to network */
    ESP_BUSY,               /*!< Device is busy, new command is not possible */
    ESP_INVALIDPARAMETERS   /*!< Parameters for functions are invalid */
} ESP8266_Result_t;

/**
 * \brief  ESP8266 modes of operation enumeration
 */
typedef enum {
    ESP8266_Mode_STA = 0x01,   /*!< ESP in station mode */
    ESP8266_Mode_AP = 0x02,    /*!< ESP as software Access Point mode */
    ESP8266_Mode_STA_AP = 0x03 /*!< ESP in both modes */
} ESP8266_Mode_t;

/**
 * \brief  Security settings for wifi network
 */
typedef enum {
    ESP8266_Ecn_OPEN = 0x00,         /*!< Wifi is open */
    ESP8266_Ecn_WEP = 0x01,          /*!< Wired Equivalent Privacy option for wifi security. \note This mode can't be used when setting up ESP8266 wifi */
    ESP8266_Ecn_WPA_PSK = 0x02,      /*!< Wi-Fi Protected Access */
    ESP8266_Ecn_WPA2_PSK = 0x03,     /*!< Wi-Fi Protected Access 2 */
    ESP8266_Ecn_WPA_WPA2_PSK = 0x04, /*!< Wi-Fi Protected Access with both modes */
} ESP8266_Ecn_t;

/**
 * \brief  Wifi connection error enumeration
 */
typedef enum {
    ESP8266_WifiConnectError_Timeout = 0x01,       /*!< Connection timeout */
    ESP8266_WifiConnectError_WrongPassword = 0x02, /*!< Wrong password for AP */
    ESP8266_WifiConnectError_APNotFound = 0x03,    /*!< AP was not found */
    ESP8266_WifiConnectError_Fail = 0x04           /*!< Connection failed with unknown cause */
} ESP8266_WifiConnectError_t;

/**
 * \brief  Firmware update statuses
 */
typedef enum {
    ESP8266_FirmwareUpdate_ServerFound = 0x01, /*!< Server for update has been found */
    ESP8266_FirmwareUpdate_Connected = 0x02,   /*!< We are connected to server for firmware */
    ESP8266_FirmwareUpdate_GotEdition = 0x03,  /*!< We have firmware edition to download */
    ESP8266_FirmwareUpdate_StartUpdate = 0x04, /*!< Update has started */
} ESP8266_FirmwareUpdate_t;

/**
 * \brief  Sleep mode enumeration
 */
typedef enum {
    ESP8266_SleepMode_Disable = 0x00, /*!< Sleep mode disabled */
    ESP8266_SleepMode_Light = 0x01,   /*!< Light sleep mode */
    ESP8266_SleepMode_Modem = 0x02    /*!< Model sleep mode */
} ESP8266_SleepMode_t;

/**
 * \brief  IPD network data structure
 */
typedef struct {
    uint8_t InIPD;        /*!< Set to 1 when ESP is in IPD mode with data */
    uint16_t InPtr;       /*!< Input pointer to save data to buffer */
    uint16_t PtrTotal;    /*!< Total pointer to count all received data */
    uint8_t ConnNumber;   /*!< Connection number where IPD is active */
    uint8_t USART_Buffer; /*!< Set to 1 when data are read from USART buffer or 0 if from temporary buffer */
} ESP8266_IPD_t;

/**
 * \brief  Connection type
 */
typedef enum {
    ESP8266_ConnectionType_TCP = 0x00, /*!< Connection type is TCP */
    ESP8266_ConnectionType_UDP = 0x01, /*!< Connection type is UDP */
    ESP8266_ConnectionType_SSL = 0x02  /*!< Connection type is SSL */
} ESP8266_ConnectionType_t;

/**
 * \brief  Connection structure
 */
typedef struct {
    uint8_t Number;              /*!< Connection number */
    uint16_t RemotePort;         /*!< Remote PORT number */
    uint8_t RemoteIP[4];         /*!< IP address of device */
    ESP8266_ConnectionType_t Type; /*!< Connection type. Parameter is valid only if connection is made as client */
    uint32_t BytesReceived;      /*!< Number of bytes received in current +IPD data package. U
                                        Use @arg DataSize to detect how many data bytes are in current package when callback function is called for received data */
    uint32_t TotalBytesReceived; /*!< Number of bytes received in entire connection lifecycle */
#if ESP8266_USE_SINGLE_CONNECTION_BUFFER == 1
    char* Data;                  /*<! Use pointer to data array */
#else
    char Data[ESP8266_CONNECTION_BUFFER_SIZE + 1]; /*!< Data array for connection */
#endif
    uint16_t DataSize;           /*!< Number of bytes in current data package.
                                        Becomes useful, when we have buffer size for data less than ESP8266 IPD statement has data for us.
                                        In this case, big packet from ESP8266 is split into several packages and this argument represent package size */
    uint32_t ContentLength;      /*!< Value of "Content-Length" header if it exists in +IPD data packet */
    char* Name;                  /*!< Pointer to connection name, useful when using as client */
    void* UserParameters;        /*!< User parameters pointer. Useful when user wants to pass custom data which can later be used in callbacks */
    union {
        struct {
            uint8_t Active:1;              /*!< Status if connection is active */
            uint8_t Client:1;              /*!< Set to 1 if connection was made as client */
            uint8_t WaitingSentRespond:1;  /*!< Set to 1 when we have sent data and we are waiting respond */
            uint8_t CallDataReceived:1;    /*!< Set to 1 when we are waiting for commands to be inactive before we call callback function */
            uint8_t LastPart:1;            /*!< When connection buffer is less than ESP8266 max +IPD possible data length,
                                                this parameter can be used if received part of data is last on one +IPD packet.
                                                When data buffer is bigger, this parameter is always set to 1 */
            uint8_t HeadersDone:1;         /*!< User option flag to set when headers has been found in response */
            uint8_t FirstPacket:1;         /*!< Set to 1 when if first packet in connection received */
        } F;
        uint8_t Value;
    } Flags;
} ESP8266_Connection_t;

/**
 * \brief  Connected network structure
 */
typedef struct {
    char SSID[ESP8266_MAX_CONNECTED_SSID_NAME_LENGTH]; /*!< SSID network name */
    uint8_t MAC[6];                                    /*!< MAC address of network */
    uint8_t Channel;                                   /*!< Network channel */
    int16_t RSSI;                                      /*!< Signal strength */
} ESP8266_ConnectedWifi_t;

/**
 * \brief  AP station structure to use when searching for network
 */
typedef struct {
    uint8_t Ecn;                          /*!< Security of Wi-Fi spot. This parameter has a value of \ref ESP8266_Ecn_t enumeration */
    char SSID[ESP8266_MAX_SSID_NAME + 1]; /*!< Service Set Identifier value. Wi-Fi spot name */
    int16_t RSSI;                         /*!< Signal strength of Wi-Fi spot */
    uint8_t MAC[6];                       /*!< MAC address of spot */
    uint8_t Channel;                      /*!< Wi-Fi channel */
    uint8_t Offset;                       /*!< Frequency offset from base 2.4GHz in kHz */
    uint8_t Calibration;                  /*!< Frequency offset calibration */
} ESP8266_AP_t;

/**
 * \brief  List of AP stations found on network search
 */
typedef struct {
    ESP8266_AP_t AP[ESP8266_MAX_DETECTED_AP]; /*!< Each AP point data */
    uint8_t Count;                            /*!< Number of valid AP stations */
} ESP8266_APs_t;

/**
 * \brief  Structure for connected station to softAP to ESP module
 */
typedef struct {
    uint8_t IP[4];  /*!< IP address of connected station */
    uint8_t MAC[6]; /*!< MAC address of connected station */
} ESP8266_ConnectedStation_t;

/**
 * \brief  List of connected stations to softAP
 */
typedef struct {
    ESP8266_ConnectedStation_t Stations[ESP8266_MAX_CONNECTEDSTATIONS]; /*!< Array of connected stations to AP. Valid number of stations is in \ref Count variable */
    uint8_t Count;                                                      /*!< Number of connected stations to AP */
} ESP8266_ConnectedStations_t;

/**
 * \brief  Access point configuration
 */
typedef struct {
    char SSID[20];          /*!< Network public name for ESP AP mode */
    char Pass[20];          /*!< Network password for ESP AP mode */
    ESP8266_Ecn_t Ecn;      /*!< Security of Wi-Fi spot. This parameter can be a value of \ref ESP8266_Ecn_t enumeration */
    uint8_t Channel;        /*!< Channel Wi-Fi is operating at */
    uint8_t MaxConnections; /*!< Max number of stations that are allowed to connect to ESP AP, between 1 and 4 */
    uint8_t Hidden;         /*!< Set to 1 if network is hidden (not broadcast) or zero if noz */
} ESP8266_APConfig_t;

/**
 * \brief  Ping structure
 */
typedef struct {
    char* Address;   /*!< Pointer to domain or IP to ping */
    uint32_t Time;   /*!< Time in milliseconds needed for pinging */
    uint8_t Success; /*!< Status indicates if ping was successful */
} ESP8266_Ping_t;

/**
 * \brief  WPS functionality
 */
typedef enum {
    ESP8266_WPS_Off = 0x00, /*!< Disables WPS functionality */
    ESP8266_WPS_On = 0x01   /*!< Enables WPS functionality */
} ESP8266_WPS_t;

/**
 * \brief  Automatic connection to saved AP on power up
 */
typedef enum {
    ESP8266_AutoConnect_Off = 0x00, /*!< Disables automatic connection to AP on power up */
    ESP8266_AutoConnect_On = 0x01   /*!< Enables automatic connection to AP on power up */
} ESP8266_AutoConnect_t;

/**
 * \brief  SNTP structure for current time
 */
typedef struct {
    uint32_t Time;
} ESP8266_SNTP_t;

/**
 * \brief  Main ESP8266 working structure
 */
typedef struct {
    uint32_t Baudrate;                                        /*!< Currently used baudrate for ESP module */
    volatile uint32_t ActiveCommand;                          /*!< Currently active AT command for module */
    char* ActiveCommandResponse;                              /*!< List of responses we expect with AT command */
    volatile uint32_t StartTime;                              /*!< Time when command was sent */
    volatile uint32_t Time;                                   /*!< Curent time in milliseconds */
    volatile uint32_t LastReceivedTime;                       /*!< Time when last string was received from ESP module */
    uint32_t Timeout;                                         /*!< Timeout in milliseconds for command to return response */
    ESP8266_Connection_t Connection[ESP8266_MAX_CONNECTIONS]; /*!< Array of connections */
    uint8_t STAIP[4];                                         /*!< Assigned IP address for station for ESP module */
    uint8_t STAGateway[4];                                    /*!< Gateway address for station ESP is using */
    uint8_t STANetmask[4];                                    /*!< Netmask address for station ESP is using */
    uint8_t STAMAC[6];                                        /*!< MAC address for station of ESP module */
    uint8_t APIP[4];                                          /*!< Assigned IP address for softAP for ESP module */
    uint8_t APGateway[4];                                     /*!< Gateway address ESP for softAP is using */
    uint8_t APNetmask[4];                                     /*!< Netmask address ESP for softAP is using */
    uint8_t APMAC[6];                                         /*!< MAC address for softAP of ESP module */
    ESP8266_Mode_t SentMode;                                  /*!< AP/STA mode we sent to module. This parameter can be a value of \ref ESP8266_Mode_t enumeration */
    ESP8266_Mode_t Mode;                                      /*!< AT/STA mode which is currently active. This parameter can be a value of \ref ESP8266_Mode_t enumeration */
    ESP8266_APConfig_t AP;                                    /*!< Configuration settings for ESP when using as Access point mode */
    ESP8266_IPD_t IPD;                                        /*!< IPD status structure. Used when new data are available from module */
#if ESP8266_USE_PING == 1
    ESP8266_Ping_t Pinging;                                   /*!< Pinging structure */
#endif
    ESP8266_ConnectedWifi_t ConnectedWifi;                    /*!< Informations about currently connected wifi network */
    ESP8266_WifiConnectError_t WifiConnectError;              /*!< Error code for connection to wifi network. This parameter can be a value of \ref ESP8266_WifiConnectError_t enumeration */
    int8_t StartConnectionSent;                               /*!< Connection number which has active CIPSTART command and waits response */
#if ESP8266_USE_CONNECTED_STATIONS == 1
    ESP8266_ConnectedStations_t ConnectedStations;            /*!< Connected stations to ESP8266 module softAP */
#endif
#if ESP8266_USE_SNTP == 1
    ESP8266_SNTP_t SNTP;                                      /*!< SNTP structure for current time */
#endif
    uint32_t TotalBytesReceived;                              /*!< Total number of bytes ESP8266 module has received from network and sent to our stack */
    uint32_t TotalBytesSent;                                  /*!< Total number of network data bytes we have sent to ESP8266 module for transmission */
    ESP8266_Connection_t* LastConnection    ;                 /*!< Pointer to currently active connection to operate */
    union {
        struct {
            uint8_t STAIPIsSet:1;                             /*!< IP is set */
            uint8_t STANetmaskIsSet:1;                        /*!< Netmask address is set */
            uint8_t STAGatewayIsSet:1;                        /*!< Gateway address is set */
            uint8_t STAMACIsSet:1;                            /*!< MAC address is set */
            uint8_t APIPIsSet:1;                              /*!< IP is set */
            uint8_t APNetmaskIsSet:1;                         /*!< Netmask address is set */
            uint8_t APGatewayIsSet:1;                         /*!< Gateway address is set */
            uint8_t APMACIsSet:1;                             /*!< MAC address is set */
            uint8_t WaitForWrapper:1;                         /*!< We are waiting for wrapper */
            uint8_t LastOperationStatus:1;                    /*!< Last operations status was OK */
            uint8_t WifiConnected:1;                          /*!< Wifi is connected to network */
            uint8_t WifiGotIP:1;                              /*!< Wifi got IP address from network */
        } F;
        uint32_t Value;
    } Flags;
    ESP8266_Result_t Result;                                  /*!< Result status as returned from last function call. This parameter can be a value of \ref ESP8266_Result_t enumeration */
} ESP8266_t;

/**
 * \}
 */

/**
 * \defgroup ESP8266_Functions
 * \brief    Library Functions
 * \{
 */

/**
 * \brief  Initializes ESP8266 module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  baudrate: USART baudrate for ESP8266 module
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_Init(ESP8266_t* ESP8266, uint32_t baudrate);

/**
 * \brief  Deinitializes ESP8266 module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_DeInit(ESP8266_t* ESP8266);

/**
 * \brief  Waits for ESP8266 to be ready to accept new command
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_WaitReady(ESP8266_t* ESP8266);

/**
 * \brief  Checks if ESP module can accept new AT command
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_IsReady(ESP8266_t* ESP8266);

/**
 * \brief  Update function which does entire work.
 * \note   This function must be called periodically inside main loop to process all events
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_Update(ESP8266_t* ESP8266);

/**
 * \brief  Updates current time
 * \note   This function must be called periodically, best if from interrupt handler, like Systick or other timer based irq
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  time_increase: Number of milliseconds timer will be increased
 * \retval None
 */
void ESP8266_TimeUpdate(ESP8266_t* ESP8266, uint32_t time_increase);

/**
 * \brief  Restores default values from ESP8266 module flash memory
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_RestoreDefault(ESP8266_t* ESP8266);

/**
 * \brief  Sets radio frequency power output in steps by 0.25 dBm
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  pwr: Power in units of dBm. This parameter can be a value between 0 and 20.5 dBm
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetRFPower(ESP8266_t* ESP8266, float pwr);

/**
 * \brief  Starts firmware module update over the air (OTA)
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_FirmwareUpdate(ESP8266_t* ESP8266);

/**
 * \brief  Sets baudrate for ESP8266 module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  baudrate: Baudrate to use with module
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_SetUART(ESP8266_t* ESP8266, uint32_t baudrate);

/**
 * \brief  Sets baudrate for ESP8266 module and stores it to ESP flash for future use
 * \note   I don't recommend to use this feature. Use \ref ESP8266_SetUART to set new baudrate until ESP8266 will reset again.
 *             
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  baudrate: Baudrate to use with module
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetUARTDefault(ESP8266_t* ESP8266, uint32_t baudrate);

/**
 * \brief  Sets sleep mode for ESP8266 module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  SleepMode: Sleep mode type. This parameter can be a value of \ref ESP8266_SleepMode_t enumeration
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetSleepMode(ESP8266_t* ESP8266, ESP8266_SleepMode_t SleepMode);

/**
 * \brief  Puts ESP8266 to sleep for specific amount of milliseconds
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  Milliseconds: Number of milliseconds ESP will be in sleep mode
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_Sleep(ESP8266_t* ESP8266, uint32_t Milliseconds);

/**
 * \brief  Connects to wifi network
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *ssid: SSID name to connect to
 * \param  *pass: Password for SSID. Set to "" if there is no password required
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_WifiConnect(ESP8266_t* ESP8266, const char* ssid, const char* pass);

/**
 * \brief  Connects to wifi network and saves setting to internal flash of ESP for auto connect to network
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *ssid: SSID name to connect to
 * \param  *pass: Password for SSID. Set to "" if there is no password required
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_WifiConnectDefault(ESP8266_t* ESP8266, const char* ssid, const char* pass);

/**
 * \brief  Gets AP settings of connected network
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure where data about AP will be stored
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_WifiGetConnected(ESP8266_t* ESP8266);

/**
 * \brief  Disconnects from connected AP if any
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_WifiDisconnect(ESP8266_t* ESP8266);

/**
 * \brief  Sets mode for ESP8266, either STA, AP or both
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  Mode: ESP8266 working mode. This parameter can be a value of \ref ESP8266_Mode_t enumeration
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetMode(ESP8266_t* ESP8266, ESP8266_Mode_t Mode);

/**
 * \brief  Sets WPS mode for ESP8266
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  wps: WPS status. This parameter can be a value of \ref ESP8266_WPS_t enumeration
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetWPS(ESP8266_t* ESP8266, ESP8266_WPS_t wps);

/**
 * \brief  Enables server mode on ESP8266 module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  port: Port number ESP will be visible on
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_ServerEnable(ESP8266_t* ESP8266, uint16_t port);

/**
 * \brief  Disables server mode on ESP8266 module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_ServerDisable(ESP8266_t* ESP8266);

/**
 * \brief  Sets server timeout value for connections waiting ESP to respond. This applies for all clients which connects to ESP module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  timeout: Timeout value in unit of seconds
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetServerTimeout(ESP8266_t* ESP8266, uint16_t timeout);

/**
 * \brief  Gets current IP of ESP module connected to other wifi network as station
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_GetSTAIP(ESP8266_t* ESP8266);

/**
 * \brief  Gets current IP of ESP module connected to other wifi network as station and waits for response from module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_GetSTAIPBlocking(ESP8266_t* ESP8266);

/**
 * \brief  Gets ESP MAC address when acting like station
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_GetSTAMAC(ESP8266_t* ESP8266);

/**
 * \brief  Sets ESP MAC address when acting like station
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *addr: 6-byts long MAX address
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetSTAMAC(ESP8266_t* ESP8266, uint8_t* addr);

/**
 * \brief  Sets ESP MAC address when acting like station and stores value to ESP flash memory
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *addr: 6-byts long MAX address
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetSTAMACDefault(ESP8266_t* ESP8266, uint8_t* addr);

/**
 * \brief  Gets current IP of ESP module acting like softAP
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_GetAPIP(ESP8266_t* ESP8266);

/**
 * \brief  Gets current IP of ESP module acting like softAP and waits for response from module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_GetAPIPBlocking(ESP8266_t* ESP8266);

/**
 * \brief  Gets ESP MAC address when acting like softAP
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_GetAPMAC(ESP8266_t* ESP8266);

/**
 * \brief  Sets ESP MAC address when acting like softAP
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *addr: 6-bytes long MAC address
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetAPMAC(ESP8266_t* ESP8266, uint8_t* addr);

/**
 * \brief  Sets ESP MAC address when acting like softAP and stores value to flash memory
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *addr: 6-bytes long MAC address
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetAPMACDefault(ESP8266_t* ESP8266, uint8_t* addr);

/**
 * \brief  Lists for all available AP stations ESP can connect to
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_ListWifiStations(ESP8266_t* ESP8266);

/**
 * \brief  Gets current AP settings of ESP module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_GetAP(ESP8266_t* ESP8266);

/**
 * \brief  Sets automatic connection to AP on power up.
 * \note   For connection, saved AP is used, which can be saved to flash using \ref ESP8266_WifiConnectDefault
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  Autoconn: Autoconnection status. This parameter can be a value of \ref ESP8266_AutoConnect_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetAutoConnect(ESP8266_t* ESP8266, ESP8266_AutoConnect_t Autoconn);

/**
 * \brief  Sets AP config values for ESP module
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *ESP8266_Config: Pointer to \ref ESP8266_APConfig_t structure with settings
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetAP(ESP8266_t* ESP8266, ESP8266_APConfig_t* ESP8266_Config);

/**
 * \brief  Sets AP config values for ESP module and stores it into ESP flash
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *ESP8266_Config: Pointer to \ref ESP8266_APConfig_t structure with settings
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetAPDefault(ESP8266_t* ESP8266, ESP8266_APConfig_t* ESP8266_Config);

/**
 * \brief  Starts ping operation to another server
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *addr: Address to ping. Can be either domain name or IP address as string
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_Ping(ESP8266_t* ESP8266, const char* addr);

/**
 * \brief  Wrapper for TCP connection.
 * \note   For more informations, take a look at \ref ESP8266_StartClientConnectionTCP
 */
#define ESP8266_StartClientConnection ESP8266_StartClientConnectionTCP

/**
 * \brief  Starts new TCP connection as ESP client and connects to given address and port
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *name: Identification connection name for callback functions to detect proper connection
 * \param  *location: Domain name or IP address to connect to as string
 * \param  port: Port to connect to
 * \param  *user_parameters: Pointer to custom user parameters (if needed) which will later be passed to callback functions for client connection
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_StartClientConnectionTCP(ESP8266_t* ESP8266, const char* name, char* location, uint16_t port, void* user_parameters);

/**
 * \brief  Starts new UDP connection as ESP client and connects to given address and port
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *name: Identification connection name for callback functions to detect proper connection
 * \param  *location: Domain name or IP address to connect to as string
 * \param  port: Port to connect to
 * \param  local_port: ESP local port. This is optional and if not needed, set parameter to 0
 * \param  *user_parameters: Pointer to custom user parameters (if needed) which will later be passed to callback functions for client connection
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_StartClientConnectionUDP(ESP8266_t* ESP8266, const char* name, char* location, uint16_t port, uint16_t local_port, void* user_parameters);

/**
 * \brief  Starts new SSL connection as ESP client and connects to given address and port
 * \note   Only one connection can be made as SSL at a time
 * \note   Use \ref ESP8266_SetSSLBufferSize first before you start connection
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *name: Identification connection name for callback functions to detect proper connection
 * \param  *location: Domain name or IP address to connect to as string
 * \param  port: Port to connect to
 * \param  *user_parameters: Pointer to custom user parameters (if needed) which will later be passed to callback functions for client connection
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_StartClientConnectionSSL(ESP8266_t* ESP8266, const char* name, const char* location, uint16_t port, void* user_parameters);

/**
 * \brief  Sets SSL buffer size for connections
 * \note   This function must be called before you can make a new connection using \ref ESP8266_StartClientConnectionSSL
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  buffersize: Size of SSL buffer. Recommended value is 4096 or more.
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SetSSLBufferSize(ESP8266_t* ESP8266, uint16_t buffersize);

/**
 * \brief  Closes all opened connections
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_CloseAllConnections(ESP8266_t* ESP8266);

/**
 * \brief  Closes specific previously opened connection
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t structure to close it
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_CloseConnection(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  Checks if all connections are closed
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_AllConnectionsClosed(ESP8266_t* ESP8266);

/**
 * \brief  Makes a request to send data to specific open connection
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t structure to close it
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_RequestSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  Gets a list of connected station devices to softAP on ESP module
 * \note   If function succedded, \ref ESP8266_Callback_ConnectedStationsDetected will be called when data are available
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 */
ESP8266_Result_t ESP8266_GetConnectedStations(ESP8266_t* ESP8266);

/**
 * \brief  Sets server for SNTP datetime retrieving
 * \note   This feature is not implemented by default in ESP8266 AT commands software.
 *            Instead of, custom implementation was made for this and may not work on your software for ESP8266 module.
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  num: Number (between 0 and 2) indicating which of 3 possible servers you will set
 * \param  servername: Server name as hostname or IP in string format
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SNTPSetServer(ESP8266_t* ESP8266, uint8_t num, const char* servername);

/**
 * \brief  Gets date and time from previously set servers with \ref ESP8266_SNTPSetServer function.
 *            For status about function success of fail, callback functions are used.
 * \note   This feature is not implemented by default in ESP8266 AT commands software.
 *            Instead of, custom implementation was made for this and may not work on your software for ESP8266 module.
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval Member of \ref ESP8266_Result_t enumeration
 * \note   This function is blocking function and will wait till ESP8266 sends result
 */
ESP8266_Result_t ESP8266_SNTPGetDateTime(ESP8266_t* ESP8266);

/**
 * \brief  Writes data from user defined USART RX interrupt handler to module stack
 * \note   This function should be called from USART RX interrupt handler to write new data
 * \param  *ch: Pointer to data to be written to module buffer
 * \param  count: Number of data bytes to write to module buffer
 * \retval Number of bytes written to buffer
 */
uint16_t ESP8266_DataReceived(uint8_t* ch, uint16_t count);

/**
 * \}
 */

/**
 * \defgroup ESP8266_Callback_Functions
 * \brief    Library callback functions
 *           
 *           Callback functions are called from ESP stack to user which should implement them when necessary.
 * \{
 */
 
/**
 * \brief  Device is ready callback
 *         
 *         Function is called when device has ready string sent to stack
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_DeviceReady(ESP8266_t* ESP8266);
 
/**
 * \brief  Watchdog reset detected on device
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WatchdogReset(ESP8266_t* ESP8266);
 
/**
 * \brief  Device has disconnected from wifi network
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiDisconnected(ESP8266_t* ESP8266);

/**
 * \brief  Device has connected to wifi network
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiConnected(ESP8266_t* ESP8266);

/**
 * \brief  Device did not succeed with connection to wifi network
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiConnectFailed(ESP8266_t* ESP8266);

/**
 * \brief  Device has received IP address as station (when connected to another access point) from connected access point (AP)
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiGotIP(ESP8266_t* ESP8266);

 
/**
 * \brief  Device has received station IP.
 * \note   Function is called in case you try to get IP with \ref ESP8266_GetSTAIP function
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiIPSet(ESP8266_t* ESP8266);

/**
 * \brief  Device failed to retrieve IP address via DHCP
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_DHCPTimeout(ESP8266_t* ESP8266);

/**
 * \brief  Device has detected wifi access point where we can connect to.
 * \note   Function is called when you use \ref ESP8266_ListWifiStations function
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *ESP8266_AP: Pointer to \ref ESP8266_APs_t structure with data
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiDetected(ESP8266_t* ESP8266, ESP8266_APs_t* ESP8266_AP);

/**
 * \brief  ESP8266 has a new connection active, acting like server
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionActive(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  ESP8266 connection closed, acting like server
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionClosed(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  ESP8266 has a data received on active connection when acting like server
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \param  *Buffer: Buffer with data
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionDataReceived(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer);

/**
 * \brief  ESP8266 is ready to accept data to be sent when connection is active as server
 * \note   This function is called in case \ref ESP8266_RequestSendData is called by user
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \param  *Buffer: Pointer to buffer where data should be stored
 * \param  max_buffer_size: Buffer size in units of bytes
 * \retval Number of bytes written into buffer
 * \note   With weak parameter to prevent link errors if not defined by user
 */
uint16_t ESP8266_Callback_ServerConnectionSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size);

/**
 * \brief  ESP8266 has successfully sent data for active connection
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionDataSent(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  ESP8266 has not sent data for active connection
 * \note   When this happen, you can use \ref ESP8266_RequestSendData again to request new data sent
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionDataSentError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  Connection is active when ESP8266 starts new connection using \ref ESP8266_StartClientConnection
 * \note   When this function is called, use \ref ESP8266_RequestSendData if you want to send any data to connection
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionConnected(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  ESP8266 returns error when trying to connect to external server as client
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  ESP826 has not return any response in defined amount of time when connection to external server as client
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionTimeout(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  Connection as client has been successfully closed
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionClosed(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  ESP8266 is ready to accept data to be sent when connection is active as client
 * \note   This function is called in case \ref ESP8266_RequestSendData is called by user
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \param  *Buffer: Pointer to buffer where data should be stored
 * \param  max_buffer_size: Buffer size in units of bytes
 * \retval Number of bytes written into buffer
 * \note   With weak parameter to prevent link errors if not defined by user
 */
uint16_t ESP8266_Callback_ClientConnectionSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size);

/**
 * \brief  ESP8266 has successfully sent data for active connection as client
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionDataSent(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  ESP8266 has not sent data for active connection as client
 * \note   When this happen, you can use \ref ESP8266_RequestSendData again to request new data sent
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionDataSentError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection);

/**
 * \brief  ESP8266 received network data and sends it to microcontroller. Function is called when when entire package of data is parsed
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \param  *Buffer: Buffer with data
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionDataReceived(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer);

/**
 * \brief  Pinging to external server has started
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *address: Pointer to address string where ping started
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_PingStarted(ESP8266_t* ESP8266, const char* address);

/**
 * \brief  Pinging to external server has started
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Pinging: Pointer to \ref ESP8266_Ping_t structure with information
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_PingFinished(ESP8266_t* ESP8266, ESP8266_Ping_t* Pinging);

/**
 * \brief  Firmware update status checking
 * \note   You must use \ref ESP8266_FirmwareUpdate function to start updating
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  status: Called with current status about updating. This parameter can be a value of \ref ESP8266_FirmwareUpdate_t enumeration
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_FirmwareUpdateStatus(ESP8266_t* ESP8266, ESP8266_FirmwareUpdate_t status);

/**
 * \brief  Firmware update has been successful
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_FirmwareUpdateSuccess(ESP8266_t* ESP8266);

/**
 * \brief  Firmware update has failed
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_FirmwareUpdateError(ESP8266_t* ESP8266);

/**
 * \brief  ESP8266 returns new data about connected stations to our softAP
 * \note   This function is called in case \ref ESP8266_GetConnectedStations is used for detection connected stations
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Stations: Pointer to \ref ESP8266_ConnectedStations_t structure with data
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ConnectedStationsDetected(ESP8266_t* ESP8266, ESP8266_ConnectedStations_t* Stations);

/**
 * \brief  ESP8266 returns new data about connected stations to our softAP
 * \note   This function is called in case \ref ESP8266_SNTPGetDateTime is used for current date time detection and error is returned
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *SNTP: Pointer to \ref ESP8266_SNTP_t structure with data from response
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_SNTPOk(ESP8266_t* ESP8266, ESP8266_SNTP_t* SNTP);

/**
 * \brief  ESP8266 returns new data about connected stations to our softAP
 * \note   This function is called in case \ref ESP8266_SNTPGetDateTime is used for current date time detection and error is returned
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_SNTPError(ESP8266_t* ESP8266);

/**
 * \}
 */
 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
