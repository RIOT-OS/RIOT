/**
 * Copyright (c) 2016 Tilen Majerle
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @ingroup     driver
 * @{
 *
 * @file
 * @brief       Platform independent ESP8266 AT commands parser for ESP8266 802.11b/g/n
 *
 * @author      Tilen Majerle
 * @author      Markus Blechschmidt <markus.blechschmidt@haw-hamburg.de>
 *
 * @}
 */

#include "esp8266.h"

/* Commands list */
#define ESP8266_COMMAND_IDLE            0
#define ESP8266_COMMAND_CWQAP           1
#define ESP8266_COMMAND_CWLAP           2
#define ESP8266_COMMAND_CWJAP           3
#define ESP8266_COMMAND_CIUPDATE        4
#define ESP8266_COMMAND_CWMODE          5
#define ESP8266_COMMAND_CIPSERVER       6
#define ESP8266_COMMAND_CIPDINFO        7
#define ESP8266_COMMAND_SEND            8
#define ESP8266_COMMAND_CLOSE           9
#define ESP8266_COMMAND_CIPSTART        10
#define ESP8266_COMMAND_CIPMUX          11
#define ESP8266_COMMAND_CWSAP           12
#define ESP8266_COMMAND_ATE             13
#define ESP8266_COMMAND_AT              14
#define ESP8266_COMMAND_RST             15
#define ESP8266_COMMAND_RESTORE         16
#define ESP8266_COMMAND_UART            17
#define ESP8266_COMMAND_PING            18
#define ESP8266_COMMAND_CWJAP_GET       19
#define ESP8266_COMMAND_SLEEP           20
#define ESP8266_COMMAND_GSLP            21
#define ESP8266_COMMAND_CIPSTA          22
#define ESP8266_COMMAND_CIPAP           23
#define ESP8266_COMMAND_CIPSTAMAC       24
#define ESP8266_COMMAND_CIPAPMAC        25
#define ESP8266_COMMAND_CIPSTO          26
#define ESP8266_COMMAND_CWLIF           27
#define ESP8266_COMMAND_CIPSTATUS       28
#define ESP8266_COMMAND_SENDDATA        29
#define ESP8266_COMMAND_WPS             30
#define ESP8266_COMMAND_AUTOCONN        31
#define ESP8266_COMMAND_SSLBUFFERSIZE   32
#define ESP8266_COMMAND_RFPOWER         33

#if ESP8266_USE_SNTP == 1                                   /* User custom commands */
#define ESP8266_COMMAND_SNTP_SET        100
#define ESP8266_COMMAND_SNTP            101
#endif

#define ESP8266_DEFAULT_BAUDRATE        115200              /* Default ESP8266 baudrate */
#define ESP8266_TIMEOUT                 30000               /* Timeout value in milliseconds */

/* Debug */
#define ESP8266_DEBUG(x)                printf("%s", x)

/* Wrapper for sending strings */
#define ESP8266_USARTSENDSTRING(str)    ESP8266_LL_USARTSend((uint8_t *)(str), strlen((char *)(str)))
#define ESP8266_USARTSENDCHAR(c)        ESP8266_LL_USARTSend((uint8_t *)(c), 1)


#define ESP8255_MAX_BUFF_SIZE           5842                /* Maximum number of return data size in one +IPD from ESP8266 module */

/* Delay milliseconds */
#define ESP8266_DELAYMS(ESP, x)         do { volatile uint32_t t = (ESP)->Time; while (((ESP)->Time - t) < (x)) ; } while (0);

/* Constants */
#define ESP8266_MAX_RFPWR               82

/* ESP responses */
#define ESP8266_RESPONSE_OK             "OK\r\n"
#define ESP8266_RESPONSE_ERROR          "ERROR\r\n"
#define ESP8266_CRLF                    "\r\n"

#define CHARISNUM(x)                    ((x) >= '0' && (x) <= '9')
#define CHARISHEXNUM(x)                 (((x) >= '0' && (x) <= '9') || ((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F'))
#define CHAR2NUM(x)                     ((x) - '0')

#if ESP8266_USE_CTS
static uint8_t RTSStatus;                                   /* RTS pin status */
#endif

#if ESP8266_USE_APSEARCH
static ESP8266_APs_t ESP8266_APs;                           /* AP list */
#endif

#if ESP8266_USE_SINGLE_CONNECTION_BUFFER == 1
/* Create data array for connections */
static char ConnectionData[ESP8266_CONNECTION_BUFFER_SIZE + 1]; /* Data array */
#endif

/* Buffers */
static BUFFER_t TMP_Buffer;
static BUFFER_t USART_Buffer;
static uint8_t TMPBuffer[ESP8266_TMPBUFFER_SIZE];
static uint8_t USARTBuffer[ESP8266_USARTBUFFER_SIZE];

/* List of possible ESP8266 baudrates */
static uint32_t ESP8266_Baudrate[] = {
    9600, 57600, 115200, 921600
};

/* Check IDLE */
#define ESP8266_CHECK_IDLE(ESP8266)                         \
    do {                                                        \
        if ((ESP8266)->ActiveCommand != ESP8266_COMMAND_IDLE) { \
            ESP8266_Update(ESP8266);                            \
            ESP8266_RETURNWITHSTATUS(ESP8266, ESP_BUSY);        \
        }                                                       \
    } while (0)

/* Check if device is connected to WIFI */
#define ESP8266_CHECK_WIFICONNECTED(ESP8266)                \
    do {                                                        \
        if (!(ESP8266)->Flags.F.WifiConnected) { ESP8266_RETURNWITHSTATUS(ESP8266, ESP_WIFINOTCONNECTED); } \
    } while (0)

/* Return from function with desired status */
#define ESP8266_RETURNWITHSTATUS(ESP8266, status)           \
    do {                                                        \
        (ESP8266)->Result = status; return status;              \
    } while (0)

/* Reset ESP connection */
#define ESP8266_RESET_CONNECTION(ESP8266, conn)             \
    do {                                                        \
        (conn)->Flags.F.Active = 0;                             \
        (conn)->Flags.F.Client = 0;                             \
        (conn)->Flags.F.FirstPacket = 0;                        \
        (conn)->Flags.F.HeadersDone = 0;                        \
    } while (0)

/* Wait and return from function with operation status */
#define ESP8266_RETURNWITHOPERATIONSTATUS(ESP8266)          \
    do {                                                        \
        ESP8266_WaitReady(ESP8266);                             \
        if (ESP8266->Flags.F.LastOperationStatus) {             \
            ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);          \
        } else {                                                \
            ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);       \
        }                                                       \
    } while (0)

/* Reset all connections */
#define ESP8266_RESET_CONNECTIONS(ESP8266)                  \
    do {                                                        \
        uint8_t i = 0;                                          \
        for (i = 0; i < ESP8266_MAX_CONNECTIONS; i ++) {         \
            ESP8266_RESET_CONNECTION(ESP8266, & (ESP8266)->Connection[i]);   \
        }                                                       \
    } while (0)


/******************************************/
/*           PRIVATE FUNCTIONS            */
/******************************************/
static                                                      /* Check if needle exists in haystack memory */
void *mem_mem(void *haystack, size_t haystacksize, void *needle, size_t needlesize)
{
    unsigned char *hptr = (unsigned char *)haystack;
    unsigned char *nptr = (unsigned char *)needle;
    unsigned int i;

    if (needlesize > haystacksize) {                        /* Check sizes */
        return 0;                                           /* Needle is greater than haystack = nothing in memory */
    }
    if (haystacksize == needlesize) {                       /* Check if same length */
        if (memcmp(hptr, nptr, needlesize) == 0) {
            return hptr;
        }
    }
    haystacksize -= needlesize;                             /* Set haystack size pointers */
    for (i = 0; i < haystacksize; i++) {                    /* Go through entire memory */
        if (memcmp(&hptr[i], nptr, needlesize) == 0) {      /* Check memory match */
            return &hptr[i];
        }
    }
    return 0;                                               /* Not found */
}

static                                                      /* Converts integer number to string format */
void Int2String(char *ptr, long int num)
{
    sprintf(ptr, "%ld", num);
}

static                                                      /* Returns number from hex value */
uint8_t Hex2Num(char a)
{
    if (a >= '0' && a <= '9') {                             /* Char is num */
        return a - '0';
    }
    else if (a >= 'a' && a <= 'f') {                        /* Char is lowercase character A - Z (hex) */
        return (a - 'a') + 10;
    }
    else if (a >= 'A' && a <= 'F') {                        /* Char is uppercase character A - Z (hex) */
        return (a - 'A') + 10;
    }

    return 0;
}

static                                                      /* Parses and returns number from string */
int32_t ParseNumber(char *ptr, uint8_t *cnt)
{
    uint8_t minus = 0, i = 0;
    int32_t sum = 0;

    if (*ptr == '-') {                                      /* Check for minus character */
        minus = 1;
        ptr++;
        i++;
    }
    while (CHARISNUM(*ptr)) {                               /* Parse number */
        sum = 10 * sum + CHAR2NUM(*ptr);
        ptr++;
        i++;
    }
    if (cnt != NULL) {                                      /* Save number of characters used for number */
        *cnt = i;
    }
    if (minus) {                                            /* Minus detected */
        return 0 - sum;
    }
    return sum;                                             /* Return number */
}

static                                                      /* Parses and returns HEX number from string */
uint32_t ParseHexNumber(char *ptr, uint8_t *cnt)
{
    uint32_t sum = 0;
    uint8_t i = 0;

    while (CHARISHEXNUM(*ptr)) {                            /* Parse number */
        sum <<= 4;
        sum += Hex2Num(*ptr);
        ptr++;
        i++;
    }

    if (cnt != NULL) {                                      /* Save number of characters used for number */
        *cnt = i;
    }
    return sum;                                             /* Return number */
}

static                                                      /* Parses IP string */
void ParseIP(char *ip_str, uint8_t *arr, uint8_t *cnt)
{
    char *token;
    uint8_t i = 0, x = 0, c;
    char Data[16];

    memcpy(Data, ip_str, sizeof(Data) - 1);                 /* Make a string copy first */
    Data[sizeof(Data) - 1] = 0;
    token = strtok(Data, ".");                              /* Parse numbers, skip :" */
    while (token != NULL) {
        arr[x++] = ParseNumber(token, &c);
        i += c;
        if (x >= 4) {
            break;
        }
        i++;
        token = strtok(NULL, ".");
    }

    if (cnt != NULL) {
        *cnt = i;
    }
}

static                                                      /* Parses MAC string */
void ParseMAC(char *ptr, uint8_t *arr, uint8_t *cnt)
{
    char *hexptr;
    uint8_t hexnum = 0, tmpcnt = 0, sum = 0;

    hexptr = strtok(ptr, ":");                              /* Get token */
    while (hexptr != NULL) {                                /* Do it till NULL */
        arr[hexnum++] = ParseHexNumber(hexptr, &tmpcnt);
        sum += tmpcnt;
        if (hexnum >= 6) {
            break;
        }
        sum++;
        hexptr = strtok(NULL, ":");
    }

    if (cnt) {
        *cnt = sum;
    }
}

static                                                      /* Parses +CWSAP statement */
void ParseCWSAP(ESP8266_t *ESP8266, char *Buffer)
{
    char *ptr;
    uint8_t i, cnt;

    ptr = Buffer;
    while (*ptr) {                                          /* Find : in string */
        if (*ptr == ':') {
            break;
        }
        ptr++;
    }

    if (*ptr == 0) {                                        /* Check if exists */
        return;
    }
    ptr++;                                                  /* Go to '"' character */

    /**** NEEDS IMPROVEMENT ****/
    /* If '"' character is inside SSID or password part, parser will fail */

    /***** SSID ****/
    ESP8266->AP.SSID[0] = 0;
    if (*ptr == '"') {
        ptr++;
    }

    i = 0;                                                  /* Copy till "," which indicates end of SSID string and start of password part */
    while (*ptr && (*ptr != '"' || *(ptr + 1) != ',' || *(ptr + 2) != '"')) {
        ESP8266->AP.SSID[i++] = *ptr++;
    }
    ESP8266->AP.SSID[i++] = 0;
    ptr += 3;                                               /* Increase pointer by 3, ignore "," part */
    i = 0;                                                  /* Copy till ", which indicates end of password string and start of number */
    while (*ptr && (*ptr != '"' || *(ptr + 1) != ',')) {
        ESP8266->AP.Pass[i++] = *ptr++;
    }
    ESP8266->AP.Pass[i++] = 0;
    ptr += 2;                                                   /* Increase pointer by 2 */
    ESP8266->AP.Channel = ParseNumber(ptr, &cnt);               /* Get channel number */
    ptr += cnt + 1;                                             /* Increase pointer and comma */
    ESP8266->AP.Ecn = (ESP8266_Ecn_t)ParseNumber(ptr, &cnt);    /* Get ECN value */
    ptr += cnt + 1;                                             /* Increase pointer and comma */
    ESP8266->AP.MaxConnections = ParseNumber(ptr, &cnt);        /* Get max connections value */
    ptr += cnt + 1;                                             /* Increase pointer and comma */
    ESP8266->AP.Hidden = ParseNumber(ptr, &cnt);                /* Get hidden value */
}

#if ESP8266_USE_APSEARCH
static
void ParseCWLAP(ESP8266_t *ESP8266, char *Buffer)
{
    uint8_t pos = 7, num = 0;
    char *ptr;

    if (ESP8266_APs.Count >= ESP8266_MAX_CONNECTIONS) {     /* Check if we have memory available first */
        return;
    }

    if (Buffer[pos] == '(') {                               /* Get start pointer */
        pos++;
    }
    ptr = strtok(&Buffer[pos], ",");                        /* Get token */
    while (ptr != NULL) {                                   /* Do it until token != NULL */
        switch (num++) {                                    /* Get positions */
            case 0:
                ESP8266_APs.AP[ESP8266_APs.Count].Ecn = ParseNumber(ptr, NULL);
                break;
            case 1:
                ptr++;                                      /* Ignore first and last " */
                ptr[strlen(ptr) - 1] = 0;
                strncpy(ESP8266_APs.AP[ESP8266_APs.Count].SSID, ptr, ESP8266_MAX_SSID_NAME);
                break;
            case 2:
                ESP8266_APs.AP[ESP8266_APs.Count].RSSI = ParseNumber(ptr, NULL);
                break;
            case 3:
                ptr++;                                                      /* Ignore first and last " */
                ptr[strlen(ptr) - 1] = 0;
                ParseMAC(ptr, ESP8266_APs.AP[ESP8266_APs.Count].MAC, NULL); /* Parse MAC address */
                break;
            case 4:
                ESP8266_APs.AP[ESP8266_APs.Count].Channel = ParseNumber(ptr, NULL);
                break;
            case 5:
                ESP8266_APs.AP[ESP8266_APs.Count].Offset = ParseNumber(ptr, NULL);
                break;
            case 6:
                ESP8266_APs.AP[ESP8266_APs.Count].Calibration = ParseNumber(ptr, NULL);
                break;
            default: break;
        }
        ptr = strtok(NULL, ",");                            /* Get new token */
    }
    ESP8266_APs.Count++;                                    /* Increase count */
}
#endif

static
void ParseCIPSTA(ESP8266_t *ESP8266, char *Buffer)
{
    uint8_t pos, s;
    uint8_t command = 0;

    /* Get positions */
    if (strncmp("+CIPSTA_CUR:ip", Buffer, 14) == 0) {
        pos = 14;
        s = 1;
        command = ESP8266_COMMAND_CIPSTA;
    }
    else if (strncmp("+CIPSTA_CUR:netmask", Buffer, 19) == 0) {
        pos = 19;
        s = 2;
        command = ESP8266_COMMAND_CIPSTA;
    }
    else if (strncmp("+CIPSTA_CUR:gateway", Buffer, 19) == 0) {
        pos = 19;
        s = 3;
        command = ESP8266_COMMAND_CIPSTA;
    }
    else if (strncmp("+CIPSTA:ip", Buffer, 10) == 0) {
        pos = 10;
        s = 1;
        command = ESP8266_COMMAND_CIPSTA;
    }
    else if (strncmp("+CIPSTA:netmask", Buffer, 15) == 0) {
        pos = 15;
        s = 2;
        command = ESP8266_COMMAND_CIPSTA;
    }
    else if (strncmp("+CIPSTA:gateway", Buffer, 15) == 0) {
        pos = 15;
        s = 3;
        command = ESP8266_COMMAND_CIPSTA;
    }
    else if (strncmp("+CIPAP_CUR:ip", Buffer, 13) == 0) {
        pos = 13;
        s = 1;
    }
    else if (strncmp("+CIPAP_CUR:netmask", Buffer, 18) == 0) {
        pos = 18;
        s = 2;
    }
    else if (strncmp("+CIPAP_CUR:gateway", Buffer, 18) == 0) {
        pos = 18;
        s = 3;
    }
    else if (strncmp("+CIPAP:ip", Buffer, 9) == 0) {
        pos = 9;
        s = 1;
    }
    else if (strncmp("+CIPAP:netmask", Buffer, 14) == 0) {
        pos = 14;
        s = 2;
    }
    else if (strncmp("+CIPAP:gateway", Buffer, 14) == 0) {
        pos = 14;
        s = 3;
    }
    else {
        return;                                             /* This should never happen */
    }

    if (command == ESP8266_COMMAND_CIPSTA) {                /* Copy content */
        switch (s) {
            case 1:
                ParseIP(&Buffer[pos + 2], ESP8266->STAIP, NULL);    /* Parse IP string */
                ESP8266->Flags.F.STAIPIsSet = 1;
                break;
            case 2:
                ParseIP(&Buffer[pos + 2], ESP8266->STANetmask, NULL);   /* Parse netmask string */
                ESP8266->Flags.F.STANetmaskIsSet = 1;
                break;
            case 3:
                ParseIP(&Buffer[pos + 2], ESP8266->STAGateway, NULL);   /* Parse gateway string */
                ESP8266->Flags.F.STAGatewayIsSet = 1;
                break;
            default: break;
        }
    }
    else {
        switch (s) {
            case 1:
                ParseIP(&Buffer[pos + 2], ESP8266->APIP, NULL); /* Parse IP string */
                ESP8266->Flags.F.APIPIsSet = 1;
                break;
            case 2:
                ParseIP(&Buffer[pos + 2], ESP8266->APNetmask, NULL);    /* Parse netmask string */
                ESP8266->Flags.F.APNetmaskIsSet = 1;
                break;
            case 3:
                ParseIP(&Buffer[pos + 2], ESP8266->APGateway, NULL);    /* Parse gateway string */
                ESP8266->Flags.F.APGatewayIsSet = 1;
                break;
            default: break;
        }
    }
}

#if ESP8266_USE_CONNECTED_STATIONS == 1
static
void ParseCWLIF(ESP8266_t *ESP8266, char *Buffer)
{
    uint8_t cnt;

    if (ESP8266->ConnectedStations.Count >= ESP8266_MAX_CONNECTEDSTATIONS) {    /* Check if memory available */
        return;
    }

    ParseIP(Buffer, ESP8266->ConnectedStations.Stations[ESP8266->ConnectedStations.Count].IP, &cnt);                /* Parse IP */
    ParseMAC(&Buffer[cnt + 1], ESP8266->ConnectedStations.Stations[ESP8266->ConnectedStations.Count].MAC, NULL);    /* Parse MAC */
    ESP8266->ConnectedStations.Count++;                                                                             /* Increase counter */
}
#endif

static
void ParseCWJAP(ESP8266_t *ESP8266, char *Buffer)
{
    char *ptr = Buffer;
    uint8_t i, cnt;

    if (!strstr(Buffer, "+CWJAP_")) {                       /* Check for existance */
        return;
    }
    while (*ptr && *ptr != '"') {                           /* Find first " character */
        ptr++;
    }
    if (!*ptr) {                                            /* Check if zero detected */
        return;
    }
    ptr++;                                                  /* Remove first " for SSID */
    i = 0;                                                  /* Parse SSID part */
    while (*ptr && (*ptr != '"' || *(ptr + 1) != ',' || *(ptr + 2) != '"')) {
        ESP8266->ConnectedWifi.SSID[i++] = *ptr++;
    }
    ESP8266->ConnectedWifi.SSID[i++] = 0;
    ptr += 3;                                                   /* Increase pointer by 3, ignore "," part */
    ParseMAC(ptr, ESP8266->ConnectedWifi.MAC, &cnt);            /* Get MAC */
    ptr += cnt + 2;                                             /* Increase counter by elements in MAC address and ", part */
    ESP8266->ConnectedWifi.Channel = ParseNumber(ptr, &cnt);    /* Get channel */
    ptr += cnt + 1;                                             /* Increase position */
    ESP8266->ConnectedWifi.RSSI = ParseNumber(ptr, &cnt);       /* Get RSSI */
}

static
ESP8266_Result_t SendCommand(ESP8266_t *ESP8266, uint8_t Command, char *CommandStr, const char *StartRespond)
{
    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle mode */
    ESP8266->ActiveCommand = Command;                       /* Save current active command */
    ESP8266->ActiveCommandResponse = (char *)StartRespond;  /* Save current active command response */

    if (Command == ESP8266_COMMAND_UART) {                  /* Clear buffer */
        BUFFER_Reset(&USART_Buffer);                        /* Reset USART buffer */
    }

    if (CommandStr != NULL) {                               /* Send command if valid pointer */
        ESP8266_USARTSENDSTRING(CommandStr);                /* Send command to ESP8266 device */
    }
    ESP8266->StartTime = ESP8266->Time;                     /* Set command start time */
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

char *EscapeString(char *str, char *buff)
{
    char *str_ptr = buff;

    while (*str) {                                          /* Go through string */
        if (*str == ',' || *str == '"' || *str == '\\') {   /* Check for special character */
            *str_ptr++ = '/';
        }
        *str_ptr++ = *str++;                                /* Copy and increase pointers */
    }
    *str_ptr = 0;                                           /* Add zero to the end */
    return buff;                                            /* Return buffer */
}

static
void EscapeStringAndSend(char *str)
{
    char special = '\\';

    while (*str) {                                          /* Go through string */
        if (*str == ',' || *str == '"' || *str == '\\') {   /* Check for special character */
            ESP8266_USARTSENDCHAR(&special);                /* Send special character */
        }
        ESP8266_USARTSENDCHAR(str++);                       /* Send character */
    }
}

char *ReverseEscapeString(char *str, char *buff)
{
    char *str_ptr = buff;

    while (*str) {                                                              /* Go through string */
        if (*str == '/') {                                                      /* Check for special character */
            if (*(str + 1) == ',' || *(str + 1) == '"' || *(str + 1) == '\\') { /* Check for next string after '/' */
                str++;                                                          /* Ignore '/' */
            }
        }
        *str_ptr++ = *str++;                                /* Copy and increase pointers */
    }
    *str_ptr = 0;                                           /* Add zero to the end */
    return buff;                                            /* Return buffer */
}

static
void CallConnectionCallbacks(ESP8266_t *ESP8266)
{
    uint8_t conn_number;

    for (conn_number = 0; conn_number < ESP8266_MAX_CONNECTIONS; conn_number++) {                                           /* Check if there are any pending data to be sent to connection */
        if (ESP8266->Connection[conn_number].Flags.F.Active && ESP8266->Connection[conn_number].Flags.F.CallDataReceived) { /* We must call function for received data */
            if (!ESP8266->Connection[conn_number].Flags.F.Client && ESP8266->ActiveCommand != ESP8266_COMMAND_IDLE) {       /* In case we are server, we must be idle to call functions */
                continue;
            }
            ESP8266->Connection[conn_number].Flags.F.CallDataReceived = 0;    /* Clear flag */
            ESP8266->Connection[conn_number].Data[ESP8266->Connection[conn_number].DataSize] = 0;

            if (ESP8266->Connection[conn_number].Flags.F.Client) {                                                                                  /* Call user function according to connection type */
                ESP8266_Callback_ClientConnectionDataReceived(ESP8266, &ESP8266->Connection[conn_number], ESP8266->Connection[conn_number].Data);   /* Client mode */
            }
            else {
                ESP8266_Callback_ServerConnectionDataReceived(ESP8266, &ESP8266->Connection[conn_number], ESP8266->Connection[conn_number].Data);   /* Server mode */
            }
        }
    }
}

static
void ProcessSendData(ESP8266_t *ESP8266)
{
    uint16_t len, max_buff = 2046;
    ESP8266_Connection_t *Connection = ESP8266->LastConnection;

    ESP8266->Flags.F.WaitForWrapper = 0;                    /* Wrapper was found */
    ESP8266->ActiveCommand = ESP8266_COMMAND_SENDDATA;      /* Go to SENDDATA command as active */

    if (ESP8266_CONNECTION_BUFFER_SIZE < 2046) {            /* Calculate maximal buffer size */
        max_buff = ESP8266_CONNECTION_BUFFER_SIZE;          /* Use maximal possible buffer size */
    }

    if (Connection->Flags.F.Client) {                                                                       /* Get data from user */
        len = ESP8266_Callback_ClientConnectionSendData(ESP8266, Connection, Connection->Data, max_buff);   /* Get data as client */
    }
    else {
        len = ESP8266_Callback_ServerConnectionSendData(ESP8266, Connection, Connection->Data, max_buff);   /* Get data as server */
    }

    if (len > max_buff) {                                   /* Check for input data */
        len = max_buff;
    }
    if (len > 0) {                                              /* If data valid */
        ESP8266_LL_USARTSend((uint8_t *)Connection->Data, len); /* Send data */
        ESP8266->TotalBytesSent += len;                         /* Increase number of bytes sent */
    }
    ESP8266_LL_USARTSend((uint8_t *)"\\0", 2);                  /* Send zero at the end even if data are not valid = stop sending data to module */
    Connection->Flags.F.WaitingSentRespond = 1;                 /* Set flag as data sent we are now waiting for response */
}

static
ESP8266_Result_t ESP8266_SetMux(ESP8266_t *ESP8266, uint8_t mux)
{
    char m = (char) mux + '0';

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle */

    ESP8266_USARTSENDSTRING("AT+CIPMUX=");                  /* Format command and send */
    ESP8266_USARTSENDCHAR(&m);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);
    if (SendCommand(ESP8266, ESP8266_COMMAND_CIPMUX, NULL, NULL) != ESP_OK) {   /* Send command */
        return ESP8266->Result;
    }
    ESP8266_WaitReady(ESP8266);                             /* Wait till command end */
    if (!ESP8266->Flags.F.LastOperationStatus) {            /* Check last status */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);       /* Return error */
    }
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

static
ESP8266_Result_t ESP8266_Setdinfo(ESP8266_t *ESP8266, uint8_t info)
{
    char i = (char) info + '0';

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle */

    ESP8266_USARTSENDSTRING("AT+CIPDINFO=");                /* Format command and send */
    ESP8266_USARTSENDCHAR(&i);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);

    if (SendCommand(ESP8266, ESP8266_COMMAND_CIPDINFO, NULL, NULL) != ESP_OK) { /* Send command and wait */
        return ESP8266->Result;
    }

    ESP8266_WaitReady(ESP8266);                             /* Wait till command end */
    if (!ESP8266->Flags.F.LastOperationStatus) {            /* Check last status */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);       /* Return error */
    }
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

static
ESP8266_Result_t StartClientConnection(
    ESP8266_t *ESP8266,
    ESP8266_ConnectionType_t type,
    const char *conn_type,
    const char *name,
    const char *location,
    uint16_t port,
    uint16_t udp_local_port,
    void *user_parameters
    )
{
    int8_t conn = -1;
    uint8_t i = 0;

    ESP8266_CHECK_IDLE(ESP8266);                                /* Check IDLE state */
    ESP8266_CHECK_WIFICONNECTED(ESP8266);                       /* Check if connected to network */

    if (type == ESP8266_ConnectionType_SSL) {                   /* Only 1 SSL connection can be active at a time */
        for (i = 0; i < ESP8266_MAX_CONNECTIONS; i++) {         /* Check if any SSL connection exists and is active */
            if (ESP8266->Connection[i].Flags.F.Active && ESP8266->Connection[i].Type == ESP8266_ConnectionType_SSL) {
                ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);   /* Return error, SSL connection already exists */
            }
        }
    }

    for (i = 0; i < ESP8266_MAX_CONNECTIONS; i++) {         /* Find available connection */
        if (!ESP8266->Connection[i].Flags.F.Active) {
            conn = i;                                       /* Save connection and start execution*/
            break;
        }
    }
    if (conn != -1) {                                       /* Try to assign */
        char tmp[6];

        conn += '0';                                        /* Go to ASCII number */
        ESP8266_USARTSENDSTRING("AT+CIPSTART=");            /* Send separate */
        ESP8266_USARTSENDCHAR(&conn);
        ESP8266_USARTSENDSTRING(",\"");
        ESP8266_USARTSENDSTRING(conn_type);
        ESP8266_USARTSENDSTRING("\",\"");
        ESP8266_USARTSENDSTRING(location);
        ESP8266_USARTSENDSTRING("\",");
        Int2String(tmp, port);                              /* Format port */
        ESP8266_USARTSENDSTRING(tmp);
        if (type == ESP8266_ConnectionType_UDP && udp_local_port > 0) {
            Int2String(tmp, udp_local_port);                /* Format local port for UDP */
            ESP8266_USARTSENDSTRING(",");
            ESP8266_USARTSENDSTRING(tmp);
        }
        ESP8266_USARTSENDSTRING(ESP8266_CRLF);

        if (SendCommand(ESP8266, ESP8266_COMMAND_CIPSTART, NULL, NULL) != ESP_OK) { /* Send command */
            return ESP8266->Result;
        }

        conn -= '0';                                        /* Go back from ASCII number to real number */
        ESP8266->Connection[i].Flags.F.Active = 1;          /* We are active */
        ESP8266->Connection[i].Flags.F.Client = 1;          /* We are in client mode */
        ESP8266->Connection[i].Type = type;                 /* Set connection type */
        ESP8266->Connection[i].TotalBytesReceived = 0;
        ESP8266->Connection[i].Number = conn;
#if ESP8266_USE_SINGLE_CONNECTION_BUFFER == 1
        ESP8266->Connection[i].Data = ConnectionData;
#endif
        ESP8266->StartConnectionSent = i;                           /* Save connection number we have started */

        ESP8266->Connection[i].Name = (char *)name;                 /* Save name */
        ESP8266->Connection[i].UserParameters = user_parameters;    /* Save user parameters pointer */

        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);                  /* Return OK */
    }
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);                   /* Return error */
}

static                                                      /* Returns number of remaining (not used) characters in buffer */
uint32_t ParseReceived(ESP8266_t *ESP8266, char *Received, uint16_t bufflen, uint8_t from_usart_buffer)
{
    char *ch_ptr;
    uint8_t bytes_cnt;
    uint32_t ipd_ptr = 0;
    ESP8266_Connection_t *Conn;

    ESP8266->LastReceivedTime = ESP8266->Time;                          /* Update last activity */
    if (bufflen == 2 && Received[0] == '\r' && Received[1] == '\n') {   /* Check for empty new line */
        return 0;
    }

    if (ESP8266->ActiveCommand != ESP8266_COMMAND_IDLE && from_usart_buffer) {  /* First check, if any command is active */
        if (                                                                    /* Check if string does not belong to this command */
            strcmp(Received, ESP8266_RESPONSE_OK) != 0 &&
            strcmp(Received, "SEND OK\r\n") != 0 &&
            strcmp(Received, "SEND FAIL\r\n") != 0 &&
            strcmp(&Received[1], ",CONNECT\r\n") != 0 &&
            //strcmp(&Received[1], ",CLOSED\r\n") != 0 &&
            strcmp(Received, ESP8266_RESPONSE_ERROR) != 0 &&
            strcmp(Received, "ready\r\n") != 0 &&
            strcmp(Received, "busy p...\r\n") != 0 &&
            strncmp(Received, "+IPD,", 5) != 0 &&
            strncmp(Received, ESP8266->ActiveCommandResponse, strlen(ESP8266->ActiveCommandResponse)) != 0
            ) {
            BUFFER_WriteString(&TMP_Buffer, Received);      /* Save string to temporary buffer, because we received a string which does not belong to this command */
            return 0;                                       /* Return from function, no remaining bytes to process */
        }
    }

    if (strcmp(Received, "ready\r\n") == 0) {               /* Device is ready */
        ESP8266_Callback_DeviceReady(ESP8266);
    }
    if (strcmp(Received, "wdt reset\r\n") == 0) {           /* Device WDT reset */
        ESP8266_Callback_WatchdogReset(ESP8266);
    }

    CallConnectionCallbacks(ESP8266);                       /* Call user callback functions for connections */

    if (strcmp(Received, "WIFI CONNECTED\r\n") == 0) {      /* We are connected to Wi-Fi */
        ESP8266->Flags.F.WifiConnected = 1;                 /* Set flag */
        ESP8266_Callback_WifiConnected(ESP8266);            /* Call user callback function */
    }
    else if (strcmp(Received, "WIFI DISCONNECT\r\n") == 0) {
        ESP8266->Flags.F.WifiConnected = 0;                                             /* Clear flags */
        ESP8266->Flags.F.WifiGotIP = 0;
        memset((uint8_t *)&ESP8266->ConnectedWifi, 0, sizeof(ESP8266->ConnectedWifi));  /* Reset connected wifi structure */
        ESP8266_RESET_CONNECTIONS(ESP8266);                                             /* Reset all connections */
        ESP8266_Callback_WifiDisconnected(ESP8266);                                     /* Call user callback function */
    }
    else if (strcmp(Received, "WIFI GOT IP\r\n") == 0) {
        ESP8266->Flags.F.WifiGotIP = 1;                     /* Wifi got IP address */
        ESP8266_Callback_WifiGotIP(ESP8266);                /* Call user callback function */
    }
    else if (strcmp(Received, "DHCP TIMEOUT") == 0) {
        ESP8266_Callback_DHCPTimeout(ESP8266);              /* Call user function */
    }
    if (strstr(Received, "SEND OK\r\n") != NULL) {          /* In case data were send */
        uint8_t cnt;

        ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;                      /* Reset active command so user will be able to call new command in callback function */
        for (cnt = 0; cnt < ESP8266_MAX_CONNECTIONS; cnt++) {
            if (ESP8266->Connection[cnt].Flags.F.WaitingSentRespond) {      /* Check for data sent */
                ESP8266->Connection[cnt].Flags.F.WaitingSentRespond = 0;    /* Reset flag */
                if (ESP8266->Connection[cnt].Flags.F.Client) {              /* Call user function according to connection type */
                    ESP8266_Callback_ClientConnectionDataSent(ESP8266, &ESP8266->Connection[cnt]);
                }
                else {
                    ESP8266_Callback_ServerConnectionDataSent(ESP8266, &ESP8266->Connection[cnt]);
                }
            }
        }
        return 0;
    }
    if (strstr(Received, "SEND FAIL\r\n") != NULL) {        /* In case data were not send */
        uint8_t cnt;

        ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;                      /* Reset active command so user will be able to call new command in callback function */
        for (cnt = 0; cnt < ESP8266_MAX_CONNECTIONS; cnt++) {
            if (ESP8266->Connection[cnt].Flags.F.WaitingSentRespond) {      /* Check for data sent */
                ESP8266->Connection[cnt].Flags.F.WaitingSentRespond = 0;    /* Reset flag */
                if (ESP8266->Connection[cnt].Flags.F.Client) {              /* Call user function according to connection type */
                    ESP8266_Callback_ClientConnectionDataSentError(ESP8266, &ESP8266->Connection[cnt]);
                }
                else {
                    ESP8266_Callback_ServerConnectionDataSentError(ESP8266, &ESP8266->Connection[cnt]);
                }
            }
        }
        return 0;
    }

    if (bufflen > 10 && (ch_ptr = (char *)mem_mem(&Received[bufflen - 10], 10, ",CONNECT\r\n", 10)) != NULL) {  /* Check if we have a new connection */
        Conn = &ESP8266->Connection[CHAR2NUM(*(ch_ptr - 1))];                                                   /* New connection has been made */
        Conn->Flags.F.Active = 1;
        Conn->Number = CHAR2NUM(*(ch_ptr - 1));
        ESP8266->LastConnection = Conn;                             /* Save last connection we operate */

        if (!Conn->Flags.F.Client) {                                /* Check server status */
            ESP8266_Callback_ServerConnectionActive(ESP8266, Conn); /* Connection started as server */
        }
        else {
            /* Process other data in CIPSTART OK response for client */
        }
    }
    else if (strstr(Received, "ALREADY CONNECTED\r\n") != NULL) {   /* Check if already connected */


    }
    else if (bufflen > 9 && (ch_ptr = (char *)mem_mem(&Received[bufflen - 9], 9, ",CLOSED\r\n", 9)) != NULL && Received != ch_ptr) {    /* Check if we have a closed connection, check the end of string */
        uint8_t client, active;
        Conn = &ESP8266->Connection[CHAR2NUM(*(ch_ptr - 1))];

        /* Check if CLOSED statement is on beginning, if not, write it to temporary buffer and leave there */
        /* If not on beginning of string, probably ,CLOSED was returned after +IPD statement */
        /* Make string standalone */
        if (ch_ptr == (Received + 1)) {
            client = Conn->Flags.F.Client;                                  /* Save values */
            active = Conn->Flags.F.Active;
            ESP8266_RESET_CONNECTION(ESP8266, Conn);                        /* Connection closed, reset flags now */
            if (active) {                                                   /* Call user function */
                if (client) {
                    ESP8266_Callback_ClientConnectionClosed(ESP8266, Conn); /* Client connection closed */
                }
                else {
                    ESP8266_Callback_ServerConnectionClosed(ESP8266, Conn); /* Server connection closed */
                }
            }
        }
        else {
            BUFFER_Write(&TMP_Buffer, (uint8_t *)(ch_ptr - 1), 10); /* Write to temporary buffer */
        }
    }
    else if (bufflen > 16 && (ch_ptr = strstr(&Received[bufflen - 15], ",CONNECT FAIL\r\n")) != NULL) { /* Check if we have a new connection, analyze only last part */
        Conn = &ESP8266->Connection[CHAR2NUM(*(ch_ptr - 1))];                                           /* New connection has been made */
        ESP8266_RESET_CONNECTION(ESP8266, Conn);
        Conn->Number = CHAR2NUM(*(ch_ptr - 1));

        if (Conn->Flags.F.Client) {                                 /* Call user function according to connection type (client, server) */
            if (ESP8266->ActiveCommand == ESP8266_COMMAND_CIPSTART) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;      /* Reset current connection */
            }
            ESP8266_Callback_ClientConnectionError(ESP8266, Conn);  /* Connection failed */
        }
    }

    if (strncmp(Received, "+IPD,", 5) == 0) {               /* Check if +IPD was received with incoming data */
        uint16_t blength = bufflen;
        uint16_t blength2 = bufflen;
        uint16_t ipd_ptr_org;

        ESP8266->IPD.InIPD = 1;                                             /* If we are not in IPD mode already, go to IPD mode */
        ESP8266->IPD.USART_Buffer = from_usart_buffer;                      /* Save IPD buffer location */

        ipd_ptr = 5;                                                        /* Reset pointer */
        ESP8266->IPD.ConnNumber = CHAR2NUM(Received[ipd_ptr]);              /* Get connection number from IPD statement */
        Conn = &ESP8266->Connection[ESP8266->IPD.ConnNumber];               /* Save connection pointer */
#if ESP8266_USE_SINGLE_CONNECTION_BUFFER == 1
        Conn->Data = ConnectionData;                                        /* Set working buffer for this connection */
#endif
        Conn->Number = ESP8266->IPD.ConnNumber;                             /* Save connection number */
        ipd_ptr += 2;                                                       /* Increase pointer by 2 */
        Conn->BytesReceived = ParseNumber(&Received[ipd_ptr], &bytes_cnt);  /* Save number of received bytes */

        if (Conn->TotalBytesReceived == 0) {                                /* First time */
            Conn->Flags.F.HeadersDone = 0;                                  /* Reset flag */
            Conn->Flags.F.FirstPacket = 1;                                  /* This is first packet of data */
        }
        else {
            Conn->Flags.F.FirstPacket = 0;                  /* This is not first packet */
        }

        Conn->TotalBytesReceived += Conn->BytesReceived;                /* Save total number of bytes */
        ESP8266->TotalBytesReceived += Conn->BytesReceived;             /* Increase global number of bytes received from ESP8266 module to stack */
        ipd_ptr += bytes_cnt + 1;                                       /* Increase pointer for number of characters for number and for comma */
        ParseIP(&Received[ipd_ptr], Conn->RemoteIP, &bytes_cnt);        /* Save IP */
        ipd_ptr += bytes_cnt + 1;                                       /* Increase pointer for number of characters for IP string and for comma */
        Conn->RemotePort = ParseNumber(&Received[ipd_ptr], &bytes_cnt); /* Save PORT */

        while (Received[ipd_ptr] != ':' && ipd_ptr < blength) {         /* Find data starts position */
            ipd_ptr++;
        }
        ipd_ptr++;
        ipd_ptr_org = ipd_ptr;
        if ((blength - ipd_ptr) > Conn->BytesReceived) {    /* Calculate size of buffer */
            blength = Conn->BytesReceived + ipd_ptr;
        }

        memcpy((uint8_t *)Conn->Data, (uint8_t *)&Received[ipd_ptr], blength - ipd_ptr);    /* Copy content to beginning of buffer */
        ipd_ptr_org += blength - ipd_ptr;

        if ((blength - ipd_ptr) > Conn->BytesReceived) {                /* Check for length */
            Conn->Data[Conn->BytesReceived] = 0;                        /* Add zero at the end of string */
        }
        ESP8266->IPD.InPtr = ESP8266->IPD.PtrTotal = blength - ipd_ptr; /* Calculate remaining bytes */
        if (ESP8266->IPD.PtrTotal >= Conn->BytesReceived) {             /* Check remaining data */
            ESP8266->IPD.InIPD = 0;                                     /* Not in IPD anymore */
            Conn->DataSize = ipd_ptr;                                   /* Set package data size */
            Conn->Flags.F.LastPart = 1;
            Conn->Flags.F.CallDataReceived = 1;                         /* Enable flag to call received data callback */
        }
        return blength2 - (ipd_ptr_org);                                /* Return number of bytes we didn't process in this request */
    }

    switch (ESP8266->ActiveCommand) {                                                           /* Check commands we have sent */
        case ESP8266_COMMAND_CWJAP:
            if (strncmp(Received, "+CWJAP:", 7) == 0) {                                         /* We send command and we have error response */
                ESP8266->ActiveCommandResponse = "FAIL\r\n";                                    /* We received an error, wait for "FAIL" string for next time */
                ESP8266->WifiConnectError = (ESP8266_WifiConnectError_t)CHAR2NUM(Received[7]);  /* Check reason */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
            }
            if (strcmp(Received, "FAIL\r\n") == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
                ESP8266_Callback_WifiConnectFailed(ESP8266);    /* Call user function */
            }
            break;
        case ESP8266_COMMAND_CWJAP_GET:
            if (strncmp(Received, "+CWJAP_CUR:", 11) == 0) {    /* We sent command to get current connected AP */
                ParseCWJAP(ESP8266, Received);                  /* Parse string */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
            }
            break;
#if ESP8266_USE_APSEARCH
        case ESP8266_COMMAND_CWLAP:
            if (strncmp(Received, "+CWLAP:", 7) == 0) {     /* CWLAP received, parse it */
                ParseCWLAP(ESP8266, Received);              /* Parse CWLAP */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;          /* Reset active command */
                ESP8266_Callback_WifiDetected(ESP8266, &ESP8266_APs);   /* Call user function */
            }
            break;
#endif
        case ESP8266_COMMAND_CWSAP:
            if (strncmp(Received, "+CWSAP", 6) == 0) {      /* CWLAP received, parse it */
                ParseCWSAP(ESP8266, Received);              /* Parse CWLAP */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
            }
            break;
        case ESP8266_COMMAND_CIPSTA:
            if (strncmp(Received, "+CIPSTA", 7) == 0) {     /* CIPSTA detected */
                ParseCIPSTA(ESP8266, Received);             /* Parse CIPSTA */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
                ESP8266_Callback_WifiIPSet(ESP8266);            /* Callback function */
            }
            break;
        case ESP8266_COMMAND_CIPAP:
            if (strncmp(Received, "+CIPAP", 6) == 0) {          /* CIPAP detected */
                ParseCIPSTA(ESP8266, Received);                 /* Parse CIPAP (or CIPSTA) */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {   /* Reset active command */
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;
            }
            break;
        case ESP8266_COMMAND_CWMODE:
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {   /* Reset active command */
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;
                ESP8266->Mode = ESP8266->SentMode;              /* Save mode */
            }
            break;
        case ESP8266_COMMAND_CIPSERVER:
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
            }
            break;
        case ESP8266_COMMAND_SEND:
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_SENDDATA;  /* Go to send data command */
                ESP8266->Flags.F.WaitForWrapper = 1;                /* Do not reset command, instead, wait for wrapper command! */
                ESP8266->ActiveCommandResponse = "SEND OK";         /* We are now waiting for SEND OK */
            }
            break;
        case ESP8266_COMMAND_SENDDATA:
            break;
        case ESP8266_COMMAND_CIPSTART:
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;                                  /* Reset active command */
                ESP8266_Callback_ClientConnectionConnected(ESP8266, ESP8266->LastConnection);   /* Connection started as client */
            }
            if (strcmp(Received, ESP8266_RESPONSE_ERROR) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;                                                          /* Reset active command */
                ESP8266_RESET_CONNECTION(ESP8266, &ESP8266->Connection[ESP8266->StartConnectionSent]);                  /* Reset connection */
                ESP8266_Callback_ClientConnectionError(ESP8266, &ESP8266->Connection[ESP8266->StartConnectionSent]);    /* Call user function */
            }
            break;
        case ESP8266_COMMAND_CIPMUX:
        case ESP8266_COMMAND_CIPDINFO:
        case ESP8266_COMMAND_AT:
        case ESP8266_COMMAND_UART:
        case ESP8266_COMMAND_CLOSE:
        case ESP8266_COMMAND_SLEEP:
        case ESP8266_COMMAND_GSLP:
        case ESP8266_COMMAND_CIPSTO:
        case ESP8266_COMMAND_RESTORE:
        case ESP8266_COMMAND_AUTOCONN:
        case ESP8266_COMMAND_CWQAP:
        case ESP8266_COMMAND_SSLBUFFERSIZE:
        case ESP8266_COMMAND_RFPOWER:
#if ESP8266_USE_SNTP
        case ESP8266_COMMAND_SNTP_SET:
#endif
#if ESP8266_USE_WPS
        case ESP8266_COMMAND_WPS:
#endif
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
            }
            break;
        case ESP8266_COMMAND_RST:
            if (strcmp(Received, "ready\r\n") == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
                ESP8266->Flags.F.LastOperationStatus = 1;       /* Set flag */
            }
            break;
#if ESP8266_USE_PING
        case ESP8266_COMMAND_PING:
            if (Received[0] == '+') {                       /* Check if data about ping milliseconds are received */
                /* Parse number for pinging */
                ESP8266->Pinging.Time = ParseNumber(&Received[1], NULL);
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;              /* Reset active command */
                ESP8266->Pinging.Success = 1;                               /* Set status */
                ESP8266_Callback_PingFinished(ESP8266, &ESP8266->Pinging);  /* Call user function */
            }
            else if (strcmp(Received, ESP8266_RESPONSE_ERROR) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;              /* Reset active command */
                ESP8266->Pinging.Success = 0;                               /* Set status */
                ESP8266_Callback_PingFinished(ESP8266, &ESP8266->Pinging);  /* Call user function */
            }
            break;
#endif
        case ESP8266_COMMAND_CIPSTAMAC:
            if (strncmp(Received, "+CIPSTAMAC", 10) == 0) {     /* CIPSTA detected */
                ParseMAC(&Received[12], ESP8266->STAMAC, NULL); /* Parse CIPSTA */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
            }
            break;
        case ESP8266_COMMAND_CIPAPMAC:
            if (strncmp(Received, "+CIPAPMAC", 9) == 0) {       /* CIPSTA detected */
                ParseMAC(&Received[11], ESP8266->APMAC, NULL);  /* Parse CIPSTA */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
            }
            break;
#if ESP8266_USE_FIRMWAREUPDATE
        case ESP8266_COMMAND_CIUPDATE:
            if (strncmp(Received, "+CIPUPDATE:", 11) == 0) {                                    /* Check for strings for update */
                uint8_t num = CHAR2NUM(Received[11]);                                           /* Get current number */
                if (num == 4) {                                                                 /* Check step */
                    ESP8266->Timeout = (unsigned long)10 * ESP8266_TIMEOUT;                     /* We are waiting last step, increase timeout */
                }
                ESP8266_Callback_FirmwareUpdateStatus(ESP8266, (ESP8266_FirmwareUpdate_t) num); /* Call user function */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0 || strcmp(Received, "ready\r\n") == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;      /* Reset active command */
                ESP8266->Timeout = ESP8266_TIMEOUT;                 /* Reset timeout */
                ESP8266_Callback_FirmwareUpdateSuccess(ESP8266);    /* Call user function */
            }
            if (strcmp(Received, ESP8266_RESPONSE_ERROR) == 0) {
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
                ESP8266->Timeout = ESP8266_TIMEOUT;             /* Reset timeout */
                ESP8266_Callback_FirmwareUpdateError(ESP8266);  /* Call user function */
            }
            break;
#endif
#if ESP8266_USE_CONNECTED_STATIONS
        case ESP8266_COMMAND_CWLIF:
            if (CHARISNUM(Received[0])) {                   /* Check if first character is number */
                ParseCWLIF(ESP8266, Received);              /* Parse response */
            }

            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {                                       /* Reset active command */
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;
                ESP8266_Callback_ConnectedStationsDetected(ESP8266, &ESP8266->ConnectedStations);   /* Call user function */
            }
            break;
#endif
#if ESP8266_USE_SNTP
        case ESP8266_COMMAND_SNTP:
            if (strncmp(Received, "+SNTP_UNIX:", 11) == 0) {
                ESP8266->SNTP.Time = ParseNumber(&Received[11], NULL);  /* Parse time */
            }
            if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {           /* Check for OK */
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;          /* Reset active command */
                ESP8266_Callback_SNTPOk(ESP8266, &ESP8266->SNTP);       /* Call user function */
            }

            if (strcmp(Received, ESP8266_RESPONSE_ERROR) == 0) {    /* Check for OK */
                ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;      /* Reset active command */
                ESP8266_Callback_SNTPError(ESP8266);                /* Call user function */
            }
#endif
        default:                                            /* No command was used to send, data received without command */
            break;
    }

    if (strcmp(Received, ESP8266_RESPONSE_OK) == 0) {       /* Set flag for last operation status */
        ESP8266->Flags.F.LastOperationStatus = 1;

        if (ESP8266->ActiveCommand != ESP8266_COMMAND_SEND && ESP8266->ActiveCommand != ESP8266_COMMAND_SENDDATA) {
            ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;  /* Reset active command */
        }
    }
    if (strcmp(Received, ESP8266_RESPONSE_ERROR) == 0 || strcmp(Received, "busy p...\r\n") == 0) {
        ESP8266->Flags.F.LastOperationStatus = 0;
        ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;      /* Reset active command */

        /* TODO: Check if ERROR here */
    }
    return 0;                                               /* No remaining bytes to process */
}

static
ESP8266_Result_t SendUARTCommand(ESP8266_t *ESP8266, uint32_t baudrate, const char *cmd)
{
    char baud[8];

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check IDLE state */

    Int2String(baud, baudrate);                             /* Format baudrate */

    ESP8266_USARTSENDSTRING((char *)cmd);                   /* Format and send command */
    ESP8266_USARTSENDSTRING("=");
    ESP8266_USARTSENDSTRING(baud);
#if ESP8266_USE_CTS
    ESP8266_USARTSENDSTRING(",8,1,0,2\r\n");
#else
    ESP8266_USARTSENDSTRING(",8,1,0,0\r\n");
#endif
    if (SendCommand(ESP8266, ESP8266_COMMAND_UART, NULL, "AT+UART") != ESP_OK) {    /* Send command */
        return ESP8266->Result;
    }
    ESP8266_WaitReady(ESP8266);                             /* Wait till command end */
    if (!ESP8266->Flags.F.LastOperationStatus) {            /* Check for success */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);
    }
    ESP8266->Baudrate = baudrate;                           /* Save baudrate */
    ESP8266_DELAYMS(ESP8266, 5);                            /* Delay a little, wait for all bytes from ESP are received before we delete them from buffer */
    ESP8266_LL_USARTInit(ESP8266->Baudrate);                /* Set new UART baudrate */
    BUFFER_Reset(&USART_Buffer);                            /* Clear buffer */
    ESP8266_DELAYMS(ESP8266, 5);                            /* Delay a little */
    ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;          /* Reset command */
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

static
ESP8266_Result_t SendMACCommand(ESP8266_t *ESP8266, uint8_t *addr, const char *cmd, uint8_t command)
{
    char tmp[3];
    uint8_t i = 0;

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle */

    ESP8266_USARTSENDSTRING((char *)cmd);                   /* Format command and send */
    ESP8266_USARTSENDSTRING("=\"");
    for (i = 0; i < 6; i++) {
        sprintf(tmp, "%02x", addr[i]);                      /* Format string */
        ESP8266_USARTSENDSTRING(tmp);
        if (i < 5) {
            ESP8266_USARTSENDSTRING(":");                   /* Send separator */
        }
    }
    ESP8266_USARTSENDSTRING("\""ESP8266_CRLF);
    SendCommand(ESP8266, command, NULL, NULL);                                                      /* Send command with rest data */

    ESP8266_WaitReady(ESP8266);                                                                     /* Wait ready */
    if (ESP8266->Flags.F.LastOperationStatus) {                                                     /* Check last operation */
        memcpy(command == ESP8266_COMMAND_CIPSTAMAC ? &ESP8266->STAMAC : &ESP8266->APMAC, addr, 6); /* MAC set was OK, copy MAC address */
    }
    else {
        if (ESP8266->Result == ESP_OK) {                    /* Check status */
            if (command == ESP8266_COMMAND_CIPSTAMAC) {     /* Reset flags */
                ESP8266->Flags.F.STAMACIsSet = 0;
            }
            else {
                ESP8266->Flags.F.APMACIsSet = 0;
            }
        }
    }
    return ESP8266->Result;                                 /* Return status */
}

/******************************************/
/*           Public API functions         */
/******************************************/

/******************************************/
/*          Basic AT commands Set         */
/******************************************/
ESP8266_Result_t ESP8266_Init(ESP8266_t *ESP8266, uint32_t baudrate)
{
    uint8_t i;

    ESP8266->Timeout = 0;                                               /* Save settings */
    if (BUFFER_Init(&TMP_Buffer, ESP8266_TMPBUFFER_SIZE, TMPBuffer)) {  /* Init temporary buffer */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_NOHEAP);
    }
    if (BUFFER_Init(&USART_Buffer, ESP8266_USARTBUFFER_SIZE, USARTBuffer)) {    /* Init USART working */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_NOHEAP);
    }

    ESP8266_RESET_INIT;                                     /* Init RESET pin */
    ESP8266_RESET_LOW;                                      /* Set pin low */
    ESP8266_DELAYMS(ESP8266, 100);                          /* Delay for while */
    ESP8266_RESET_HIGH;                                     /* Set pin high */
    ESP8266_DELAYMS(ESP8266, 100);                          /* Delay for while */
    ESP8266->Baudrate = baudrate;                           /* Save current baudrate */
    ESP8266_LL_USARTInit(ESP8266->Baudrate);                /* Init USART */
#if ESP8266_USE_CTS
    ESP8266_LL_SetRTS(ESP_RTS_LOW);                         /* Set RTS low */
    RTSStatus = ESP_RTS_LOW;
#endif
    ESP8266->Timeout = 1000;                                                            /* Set allowed timeout */

    SendCommand(ESP8266, ESP8266_COMMAND_RST, "AT+RST\r\n", "ready\r\n");               /* Reset device */
    ESP8266_DELAYMS(ESP8266, 500);                                                      /* Delay for while */
    ESP8266_WaitReady(ESP8266);                                                         /* Wait till idle */

    if (!ESP8266->Flags.F.LastOperationStatus) {                                        /* Check status */
        for (i = 0; i < sizeof(ESP8266_Baudrate) / sizeof(ESP8266_Baudrate[0]); i++) {  /* Check for baudrate, try with predefined baudrates */
            ESP8266_LL_USARTInit(ESP8266_Baudrate[i]);                                  /* Init USART */
            ESP8266->Timeout = 1000;                                                    /* Set allowed timeout */
            SendCommand(ESP8266, ESP8266_COMMAND_RST, "AT+RST\r\n", "ready\r\n");       /* Reset device */
            ESP8266_WaitReady(ESP8266);                                                 /* Wait till idle */
            if (ESP8266->Flags.F.LastOperationStatus) {                                 /* Check status */
                ESP8266->Baudrate = ESP8266_Baudrate[i];                                /* Save current baudrate */
                break;
            }
        }
    }

    if (!ESP8266->Flags.F.LastOperationStatus) {                                /* Check status */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_DEVICENOTCONNECTED);              /* Device is not connected */
    }
    ESP8266->Timeout = ESP8266_TIMEOUT;                                         /* Set allowed timeout to 30sec */
    SendCommand(ESP8266, ESP8266_COMMAND_AT, "AT\r\n", ESP8266_RESPONSE_OK);    /* Test device */
    ESP8266_WaitReady(ESP8266);                                                 /* Wait till idle */

    if (!ESP8266->Flags.F.LastOperationStatus) {                                /* Check status */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_DEVICENOTCONNECTED);
    }

#if ESP8266_ECHO
    SendCommand(ESP8266, ESP8266_COMMAND_ATE, "ATE1\r\n", "ATE1");      /* Enable echo if not already */
#else
    SendCommand(ESP8266, ESP8266_COMMAND_ATE, "ATE0\r\n", "ATE0");      /* Disable echo if not already */
#endif
    ESP8266_WaitReady(ESP8266);                                         /* Wait till idle */

    while (ESP8266_SetMux(ESP8266, 1) != ESP_OK) ;                      /* Enable multiple connections */
    while (ESP8266_Setdinfo(ESP8266, 1) != ESP_OK) ;                    /* Enable IP and PORT to be shown on +IPD statement */
    while (ESP8266_SetMode(ESP8266, ESP8266_Mode_STA_AP) != ESP_OK) ;   /* Set mode to STA+AP by default */
    while (ESP8266_GetSTAMAC(ESP8266) != ESP_OK) ;                      /* Get station MAC */
    while (ESP8266_GetAPMAC(ESP8266) != ESP_OK) ;                       /* Get softAP MAC */
    while (ESP8266_GetAPIP(ESP8266) != ESP_OK) ;                        /* Get softAP IP */

#if ESP8266_USE_CTS
    ESP8266_WaitReady(ESP8266);                             /* Wait ready state */
    while (ESP8266_SetUART(ESP8266, baudrate) != ESP_OK) ;  /* Set UART and enable CTS pin on ESP8266 */
#endif

    return ESP8266_WaitReady(ESP8266);                      /* Return wait ready status for last operation */
}

ESP8266_Result_t ESP8266_DeInit(ESP8266_t *ESP8266)
{
    BUFFER_Free(&TMP_Buffer);                               /* Clear temporary buffer */
    BUFFER_Free(&USART_Buffer);                             /* Clear USART buffer */

    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK from function */
}

/******************************************/
/*          UPDATE STACK PROCESS          */
/******************************************/
ESP8266_Result_t ESP8266_Update(ESP8266_t *ESP8266)
{
    char ch, Received[128];
    uint8_t lastcmd;
    uint16_t stringlength, receivedleft;

    if (ESP8266->Timeout == 0) {                            /* If timeout is set to 0 */
        ESP8266->Timeout = 30000;
    }

    if ((ESP8266->Time - ESP8266->StartTime) > ESP8266->Timeout) {                                                  /* Check timeout */
        lastcmd = ESP8266->ActiveCommand;                                                                           /* Save temporary active command */
        ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;                                                              /* Timeout reached, reset command */
        if (lastcmd == ESP8266_COMMAND_CIPSTART) {                                                                  /* Timeout reached */
            ESP8266_RESET_CONNECTION(ESP8266, &ESP8266->Connection[ESP8266->StartConnectionSent]);                  /* We get timeout on cipstart */
            ESP8266_Callback_ClientConnectionTimeout(ESP8266, &ESP8266->Connection[ESP8266->StartConnectionSent]);  /* Call user function */
        }
    }

    if (ESP8266->ActiveCommand == ESP8266_COMMAND_SENDDATA) {   /* We are waiting to send data */
        if (ESP8266->Flags.F.WaitForWrapper) {                  /* Check what we are searching for */
            int16_t found;
            uint8_t dummy[2];

            if ((found = BUFFER_Find(&USART_Buffer, (uint8_t *)"> ", 2)) >= 0) {    /* Wait for character */
                if (found == 0) {
                    BUFFER_Read(&USART_Buffer, dummy, 2);                           /* Make a dummy read */
#if ESP8266_USE_CTS
                    /* Set CTS low */
                    if (RTSStatus != ESP_RTS_LOW) {
                        ESP8266_LL_SetRTS(ESP_RTS_LOW);
                        RTSStatus = ESP_RTS_LOW;
                    }
#endif
                }
            }
            else if ((found = BUFFER_Find(&TMP_Buffer, (uint8_t *)"> ", 2)) >= 0) {
                if (found == 0) {
                    BUFFER_Read(&TMP_Buffer, dummy, 2);     /* Make 2 dummy reads */
                }
            }
            if (found >= 0) {
                ProcessSendData(ESP8266);                   /* Send data */
            }
        }
    }

    if (                                                                            /* If AT+UART command was used, only check if "OK" exists in buffer */
        ESP8266->ActiveCommand == ESP8266_COMMAND_UART &&                           /* Active command is UART change */
        !ESP8266->IPD.InIPD                                                         /* We are not in IPD mode */
        ) {
        if (BUFFER_Find(&USART_Buffer, (uint8_t *)ESP8266_RESPONSE_OK, 4) >= 0) {   /* Check for "OK\r" */
            BUFFER_Reset(&USART_Buffer);                                            /* Clear buffer, invalid all data */
            ESP8266->ActiveCommand = ESP8266_COMMAND_IDLE;                          /* We are OK here */
            ESP8266->Flags.F.LastOperationStatus = 1;                               /* Last command is OK */
        }
    }

    while (                                                                                 /* Get string from USART buffer if we are not in IPD mode */
        !ESP8266->IPD.InIPD &&                                                              /* Not in IPD mode */
        (stringlength = BUFFER_ReadString(&USART_Buffer, Received, sizeof(Received))) > 0   /* Something in USART buffer */
        ) {
#if ESP8266_USE_CTS
        if (RTSStatus != ESP_RTS_LOW) {
            RTSStatus = ESP_RTS_LOW;                        /* Set CTS low */
            ESP8266_LL_SetRTS(ESP_RTS_LOW);
        }
#endif
        receivedleft = ParseReceived(ESP8266, Received, stringlength, 1);                                       /* Parse received string */
        if (receivedleft > 0) {                                                                                 /* Copy bytes we didn't proceed to beginning of buffer, this should never happen but just in case. If should be handled in if statement below */
#if ESP8266_USE_CTS
            ESP8266_LL_SetRTS(ESP_RTS_HIGH);                                                                    /* Disable receive operations */
#endif
            BUFFER_WriteToTop(&USART_Buffer, (uint8_t *)&Received[stringlength - receivedleft], receivedleft);  /* Write remaining data to top of memory */
#if ESP8266_USE_CTS
            ESP8266_LL_SetRTS(ESP_RTS_LOW);                                                                     /* Enable receive operations */
#endif
        }
    }

    while (                                                                                                     /* Get string from TMP buffer when no command active */
        !ESP8266->IPD.InIPD &&                                                                                  /* Not in IPD mode */
        ESP8266->ActiveCommand == ESP8266_COMMAND_IDLE &&                                                       /* We are in IDLE mode */
        (stringlength = BUFFER_ReadString(&TMP_Buffer, Received, sizeof(Received))) > 0                         /* Something in TMP buffer */
        ) {
        receivedleft = ParseReceived(ESP8266, Received, stringlength, 0);                                       /* Parse received string */
        if (receivedleft > 0) {
            BUFFER_WriteToTop(&TMP_Buffer, (uint8_t *)&Received[stringlength - receivedleft], receivedleft);    /* Write data to top of buffer */
        }
    }

    if (!ESP8266->IPD.InIPD) {                              /* Manually check for IPD statements, +IPD format: +IPD,a,bbbb,ccc.ccc.ccc.ccc,ddddd: */
        int32_t ipd_pos;

        if (USART_Buffer.Buffer[USART_Buffer.Out] == '+') {                                         /* Check if first character to read is plus sign, small optimization to prevent buffer search all the time */
            ipd_pos = BUFFER_Find(&USART_Buffer, (uint8_t *)"+IPD,", 5);                            /* Try to find +IPD statement in buffer */
            if (ipd_pos == 0) {                                                                     /* +IPD is on start of buffer read operations */
                ipd_pos = BUFFER_Find(&USART_Buffer, (uint8_t *)":", 1);
                if (ipd_pos > 0 &&  ipd_pos < 35) {                                                 /* Check if colon exists and is in valid range */
                    stringlength = BUFFER_Read(&USART_Buffer, (uint8_t *)Received, ipd_pos + 1);    /* Read from buffer until colon is received */
                    Received[stringlength] = 0;                                                     /* Add zero to the end of read string */
                    ParseReceived(ESP8266, Received, stringlength, 1);                              /* Start parsing +IPD statement */
                }
            }
        }
    }

    if (ESP8266->IPD.InIPD) {                               /* If we are in IPD mode */
        BUFFER_t *buff;
        if (ESP8266->IPD.USART_Buffer) {                    /* Get proper buffer for operation */
            buff = &USART_Buffer;                           /* Use USART buffer as working buffer */
        }
        else {
            buff = &TMP_Buffer;                             /* Use TMP buffer as working buffer */
        }

        while (                                                                                     /* If anything received */
            ESP8266->IPD.PtrTotal < ESP8266->Connection[ESP8266->IPD.ConnNumber].BytesReceived &&   /* Until everything received*/
            BUFFER_GetFull(buff) > 0                                                                /* Data are available in buffer */
            ) {
            BUFFER_Read(buff, (uint8_t *)&ch, 1);                                                   /* Read from buffer */

#if ESP8266_USE_CTS
            if (RTSStatus != ESP_RTS_LOW) {                 /* Set CTS low again */
                RTSStatus = ESP_RTS_LOW;
                ESP8266_LL_SetRTS(ESP_RTS_LOW);
            }
#endif
            ESP8266->Connection[ESP8266->IPD.ConnNumber].Data[ESP8266->IPD.InPtr] = ch; /* Add from buffer */
            ESP8266->IPD.InPtr++;                                                       /* Increase current packet pointer */
            ESP8266->IPD.PtrTotal++;                                                    /* Increase global packet pointer */

#if ESP8266_CONNECTION_BUFFER_SIZE < ESP8255_MAX_BUFF_SIZE
            if ((ESP8266->IPD.InPtr >= ESP8266_CONNECTION_BUFFER_SIZE && ESP8266->IPD.InPtr != ESP8266->Connection[ESP8266->IPD.ConnNumber].BytesReceived)) {   /* Check for pointer */
                ESP8266->Connection[ESP8266->IPD.ConnNumber].DataSize = ESP8266->IPD.InPtr;                                                                     /* Set connection data size */
                ESP8266->Connection[ESP8266->IPD.ConnNumber].Data[ESP8266->Connection[ESP8266->IPD.ConnNumber].DataSize] = 0;                                   /* Add zero to the end (in case of string) */
                ESP8266->Connection[ESP8266->IPD.ConnNumber].Flags.F.LastPart = 0;                                                                              /* Reset last part flag */

                if (ESP8266->Connection[ESP8266->IPD.ConnNumber].Flags.F.Client) {                                                                              /* Buffer is full, call user function */
                    ESP8266_Callback_ClientConnectionDataReceived(ESP8266, &ESP8266->Connection[ESP8266->IPD.ConnNumber], ESP8266->Connection[ESP8266->IPD.ConnNumber].Data);
                }
                else {
                    ESP8266_Callback_ServerConnectionDataReceived(ESP8266, &ESP8266->Connection[ESP8266->IPD.ConnNumber], ESP8266->Connection[ESP8266->IPD.ConnNumber].Data);
                }

                ESP8266->IPD.InPtr = 0;                     /* Reset input pointer for new small packet */
                ESP8266->Connection[ESP8266->IPD.ConnNumber].Flags.F.FirstPacket = 0;
            }
#endif
        }

        if (ESP8266->IPD.PtrTotal >= ESP8266->Connection[ESP8266->IPD.ConnNumber].BytesReceived) {  /* Check if everything received */
            char *ptr;
            ESP8266->IPD.InIPD = 0;                                                                 /* Not in IPD anymore */

            ESP8266->Connection[ESP8266->IPD.ConnNumber].DataSize = ESP8266->IPD.InPtr;             /* Set package data size */
            ESP8266->Connection[ESP8266->IPD.ConnNumber].Flags.F.LastPart = 1;                      /* Last part of package */

            if (                                                                                    /* We have data, lets see if Content-Length exists and save it */
                ESP8266->Connection[ESP8266->IPD.ConnNumber].Flags.F.FirstPacket &&
                (
                    (ptr = strstr(ESP8266->Connection[ESP8266->IPD.ConnNumber].Data, "Content-Length:")) != NULL ||
                    (ptr = strstr(ESP8266->Connection[ESP8266->IPD.ConnNumber].Data, "content-length:")) != NULL
                )
                ) {
                ptr += 15;                                  /* Increase pointer and parse number */
                if (*ptr == ' ') {
                    ptr++;
                }
                ESP8266->Connection[ESP8266->IPD.ConnNumber].ContentLength = ParseNumber(ptr, NULL);    /* Parse content length */
            }
            ESP8266->Connection[ESP8266->IPD.ConnNumber].Flags.F.CallDataReceived = 1;                  /* Set flag to trigger callback for data received */
        }
    }

    CallConnectionCallbacks(ESP8266);                       /* Call user functions on connections */
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

/******************************************/
/*           STACK TIME UPDATE            */
/******************************************/
void ESP8266_TimeUpdate(ESP8266_t *ESP8266, uint32_t time_increase)
{
    ESP8266->Time += time_increase;                         /* Increase time */
}

/******************************************/
/*          DEVICE READY STATUS           */
/******************************************/
ESP8266_Result_t ESP8266_WaitReady(ESP8266_t *ESP8266)
{
    do {                                                                /* Do job */
        if (ESP8266->Flags.F.WaitForWrapper) {                          /* Check for wrapper */
            if (BUFFER_Find(&USART_Buffer, (uint8_t *)"> ", 2) >= 0) {  /* We have found it, stop execution here */
                //ESP8266->Flags.F.WaitForWrapper = 0;
                break;
            }
        }
        ESP8266_Update(ESP8266);                            /* Update device */
    } while (ESP8266->ActiveCommand != ESP8266_COMMAND_IDLE);
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

ESP8266_Result_t ESP8266_IsReady(ESP8266_t *ESP8266)
{
    ESP8266_CHECK_IDLE(ESP8266);                            /* Check IDLE */
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

/******************************************/
/*            WIFI MANAGEMENT             */
/******************************************/
ESP8266_Result_t ESP8266_WifiDisconnect(ESP8266_t *ESP8266)
{
    return SendCommand(ESP8266, ESP8266_COMMAND_CWQAP, "AT+CWQAP\r\n", "AT+CWQAP"); /* Send command */
}

ESP8266_Result_t ESP8266_WifiConnect(ESP8266_t *ESP8266, const char *ssid, const char *pass)
{
    ESP8266_CHECK_IDLE(ESP8266);                        /* Check idle */

    ESP8266_USARTSENDSTRING("AT+CWJAP=\"");             /* Send commands separate with escape strings */
    EscapeStringAndSend((char *)ssid);
    ESP8266_USARTSENDSTRING("\",\"");
    EscapeStringAndSend((char *)pass);
    ESP8266_USARTSENDSTRING("\""ESP8266_CRLF);

    return SendCommand(ESP8266, ESP8266_COMMAND_CWJAP, NULL, "+CWJAP:");    /* Send command */
}

ESP8266_Result_t ESP8266_WifiConnectDefault(ESP8266_t *ESP8266, const char *ssid, const char *pass)
{
    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle */

    ESP8266_USARTSENDSTRING("AT+CWJAP_DEF=\"");             /* Send commands separate with escape strings */
    EscapeStringAndSend((char *)ssid);
    ESP8266_USARTSENDSTRING("\",\"");
    EscapeStringAndSend((char *)pass);
    ESP8266_USARTSENDSTRING("\""ESP8266_CRLF);

    return SendCommand(ESP8266, ESP8266_COMMAND_CWJAP, NULL, "+CWJAP:");    /* Send command */
}

ESP8266_Result_t ESP8266_WifiGetConnected(ESP8266_t *ESP8266)
{
    return SendCommand(ESP8266, ESP8266_COMMAND_CWJAP_GET, "AT+CWJAP_CUR?\r\n", "+CWJAP_CUR:");    /* Send command */
}

/******************************************/
/*              RESTORE DEVICE            */
/******************************************/
ESP8266_Result_t ESP8266_RestoreDefault(ESP8266_t *ESP8266)
{
    if (SendCommand(ESP8266, ESP8266_COMMAND_RESTORE, "AT+RESTORE\r\n", "ready\r\n") != ESP_OK) {   /* Send command */
        return ESP8266->Result;
    }

    ESP8266_DELAYMS(ESP8266, 2);                            /* Little delay */
    ESP8266_LL_USARTInit(ESP8266_DEFAULT_BAUDRATE);         /* Reset USART to default ESP baudrate */
    ESP8266_WaitReady(ESP8266);                             /* Wait till ready, ESP will send data in default baudrate after reset */
    BUFFER_Reset(&USART_Buffer);                            /* Reset USART buffer */
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

/******************************************/
/*            TX POWER SETTINGS           */
/******************************************/
ESP8266_Result_t ESP8266_SetRFPower(ESP8266_t *ESP8266, float pwr)
{
    uint32_t intval = (int)((float)pwr * (float)4.0);
    char num[3];

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check IDLE status */

    if (intval > ESP8266_MAX_RFPWR) {                       /* Check parameters first */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_INVALIDPARAMETERS);
    }

    Int2String(num, intval);                                /* Format number to string */
    ESP8266_USARTSENDSTRING("AT+RFPOWER=");                 /* Send command */
    ESP8266_USARTSENDSTRING(num);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);

    if (SendCommand(ESP8266, ESP8266_COMMAND_RFPOWER, NULL, NULL) != ESP_OK) {  /* Send command */
        return ESP8266->Result;
    }

    return ESP8266_Update(ESP8266);                         /* Return status */
}

/******************************************/
/*             FIRMWARE UPDATE            */
/******************************************/
#if ESP8266_USE_FIRMWAREUPDATE
ESP8266_Result_t ESP8266_FirmwareUpdate(ESP8266_t *ESP8266)
{
    ESP8266_CHECK_WIFICONNECTED(ESP8266);                                                       /* Check connected */
    return SendCommand(ESP8266, ESP8266_COMMAND_CIUPDATE, "AT+CIUPDATE\r\n", "+CIPUPDATE:");    /* Send command if possible */
}
#endif

/******************************************/
/*              UART SETTINGS             */
/******************************************/
ESP8266_Result_t ESP8266_SetUART(ESP8266_t *ESP8266, uint32_t baudrate)
{
    return SendUARTCommand(ESP8266, baudrate, "AT+UART_CUR");   /* Set current baudrate */
}

ESP8266_Result_t ESP8266_SetUARTDefault(ESP8266_t *ESP8266, uint32_t baudrate)
{
    return SendUARTCommand(ESP8266, baudrate, "AT+UART_DEF");   /* Set default baudrate */
}

/******************************************/
/*            SLEEP MANAGEMENT            */
/******************************************/
ESP8266_Result_t ESP8266_SetSleepMode(ESP8266_t *ESP8266, ESP8266_SleepMode_t SleepMode)
{
    uint8_t sleep = (uint8_t) SleepMode;

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle mode */
    sleep += '0';                                           /* Go to ASCII */
    ESP8266_USARTSENDSTRING("AT+SLEEP=");                   /* Send command */
    ESP8266_USARTSENDCHAR(&sleep);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);
    SendCommand(ESP8266, ESP8266_COMMAND_SLEEP, NULL, "+SLEEP");    /* Send command */

    return ESP8266_WaitReady(ESP8266);                              /* Wait ready */
}

ESP8266_Result_t ESP8266_Sleep(ESP8266_t *ESP8266, uint32_t Milliseconds)
{
    char tmp[10];

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle mode */

    Int2String(tmp, Milliseconds);                          /* Format string from milliseconds */
    ESP8266_USARTSENDSTRING("AT+GSLP=");                    /* Send sleep values to module */
    ESP8266_USARTSENDSTRING(tmp);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);
    SendCommand(ESP8266, ESP8266_COMMAND_GSLP, NULL, NULL); /* Send command */

    return ESP8266_WaitReady(ESP8266);                      /* Wait ready */
}

/******************************************/
/*             MODE MANAGEMENT            */
/******************************************/
ESP8266_Result_t ESP8266_SetMode(ESP8266_t *ESP8266, ESP8266_Mode_t Mode)
{
    uint8_t m = (uint8_t) Mode;

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check IDLE mode */

    m += '0';                                               /* Go to ASCII number */
    ESP8266_USARTSENDSTRING("AT+CWMODE_CUR=");              /* Send command */
    ESP8266_USARTSENDCHAR(&m);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);
    if (SendCommand(ESP8266, ESP8266_COMMAND_CWMODE, NULL, "AT+CWMODE") != ESP_OK) {    /* Send command */
        return ESP8266->Result;
    }

    ESP8266->SentMode = Mode;                               /* Save mode we sent */
    ESP8266_WaitReady(ESP8266);                             /* Wait till command end */

    if (ESP8266->Mode != Mode) {                            /* Check status */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);       /* Return error */
    }
    return ESP8266_GetAP(ESP8266);                          /* Now get settings for current AP mode */
}

/******************************************/
/*            SEND NETWORK DATA           */
/******************************************/
ESP8266_Result_t ESP8266_RequestSendData(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    uint8_t num;

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle state */

    num = Connection->Number + '0';                         /* Go to ASCII */

    ESP8266_USARTSENDSTRING("AT+CIPSENDEX=");               /* Format and send command */
    ESP8266_USARTSENDCHAR(&num);
    ESP8266_USARTSENDSTRING(",2048\r\n");

    if (SendCommand(ESP8266, ESP8266_COMMAND_SEND, NULL, NULL) != ESP_OK) { /* Send command */
        return ESP8266->Result;
    }
    ESP8266->LastConnection = Connection;                   /* Save connection pointer */

    return ESP8266->Result;                                 /* Return from function */
}

/******************************************/
/*         CONNECTIONS MANAGEMENT         */
/******************************************/
ESP8266_Result_t ESP8266_CloseConnection(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    uint8_t num;

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle state */
    num = Connection->Number + '0';                         /* Go to ASCII */

    ESP8266_USARTSENDSTRING("AT+CIPCLOSE=");                /* Format command and send */
    ESP8266_USARTSENDCHAR(&num);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);

    return SendCommand(ESP8266, ESP8266_COMMAND_CLOSE, NULL, NULL); /* Send command */
}

ESP8266_Result_t ESP8266_CloseAllConnections(ESP8266_t *ESP8266)
{
    return SendCommand(ESP8266, ESP8266_COMMAND_CLOSE, "AT+CIPCLOSE=5\r\n", NULL);  /* Send command */
}

ESP8266_Result_t ESP8266_AllConnectionsClosed(ESP8266_t *ESP8266)
{
    uint8_t i;

    for (i = 0; i < ESP8266_MAX_CONNECTIONS; i++) {         /* Go through all connections */
        if (ESP8266->Connection[i].Flags.F.Active) {        /* Check if active */
            ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);   /* Oops, connection is active */
        }
    }

    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

/******************************************/
/*            WPS MANAGEMENT              */
/******************************************/
#if ESP8266_USE_WPS == 1
ESP8266_Result_t ESP8266_SetWPS(ESP8266_t *ESP8266, ESP8266_WPS_t wps)
{
    char w = (char) wps + '0';

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle */

    ESP8266_USARTSENDSTRING("AT+WPS=");                     /* Format command and send */
    ESP8266_USARTSENDCHAR(&w);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);

    if (SendCommand(ESP8266, ESP8266_COMMAND_WPS, NULL, NULL) != ESP_OK) {  /* Send command and wait */
        return ESP8266->Result;
    }
    return ESP8266_WaitReady(ESP8266);                      /* Wait till command end */
}
#endif

/******************************************/
/*           SERVER MANAGEMENT            */
/******************************************/
ESP8266_Result_t ESP8266_ServerEnable(ESP8266_t *ESP8266, uint16_t port)
{
    char port_str[7];

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle */

    Int2String(port_str, port);                             /* Convert to string */
    ESP8266_USARTSENDSTRING("AT+CIPSERVER=1,");             /* Format string and send */
    ESP8266_USARTSENDSTRING(port_str);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);

    if (SendCommand(ESP8266, ESP8266_COMMAND_CIPSERVER, NULL, NULL) != ESP_OK) {    /* Send command and wait */
        return ESP8266->Result;
    }

    ESP8266_WaitReady(ESP8266);                             /* Wait till command end */
    if (!ESP8266->Flags.F.LastOperationStatus) {            /* Check last status */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);       /* Return error */
    }
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

ESP8266_Result_t ESP8266_ServerDisable(ESP8266_t *ESP8266)
{
    if (SendCommand(ESP8266, ESP8266_COMMAND_CIPSERVER, "AT+CIPSERVER=0\r\n", NULL) != ESP_OK) {    /* Send command and wait */
        return ESP8266->Result;
    }
    ESP8266_WaitReady(ESP8266);                             /* Wait till command end */
    if (!ESP8266->Flags.F.LastOperationStatus) {            /* Check last status */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);       /* Return error */
    }
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

ESP8266_Result_t ESP8266_SetServerTimeout(ESP8266_t *ESP8266, uint16_t timeout)
{
    char timeout_str[7];

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle */

    Int2String(timeout_str, timeout);                       /* Convert to string */

    ESP8266_USARTSENDSTRING("AT+CIPSTO=");                  /* Format string and send */
    ESP8266_USARTSENDSTRING(timeout_str);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);

    if (SendCommand(ESP8266, ESP8266_COMMAND_CIPSTO, NULL, NULL) != ESP_OK) {   /* Send command and wait */
        return ESP8266->Result;
    }

    ESP8266_WaitReady(ESP8266);                             /* Wait till command end */
    if (!ESP8266->Flags.F.LastOperationStatus) {            /* Check last status */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);       /* Return error */
    }
    ESP8266_RETURNWITHSTATUS(ESP8266, ESP_OK);              /* Return OK */
}

ESP8266_Result_t ESP8266_GetSTAIPBlocking(ESP8266_t *ESP8266)
{
    if (ESP8266_GetSTAIP(ESP8266) != ESP_OK) {              /* Send command */

    }
    return ESP8266_WaitReady(ESP8266);                      /* Wait till command end */
}

ESP8266_Result_t ESP8266_GetAPIPBlocking(ESP8266_t *ESP8266)
{
    if (ESP8266_GetAPIP(ESP8266) != ESP_OK) {               /* Send command */

    }
    return ESP8266_WaitReady(ESP8266);                      /* Wait till command end */
}

ESP8266_Result_t ESP8266_GetSTAIP(ESP8266_t *ESP8266)
{
    SendCommand(ESP8266, ESP8266_COMMAND_CIPSTA, "AT+CIPSTA_CUR?\r\n", "+CIPSTA_CUR");  /* Send command */
    if (ESP8266->Result == ESP_OK) {                                                    /* Check status */
        ESP8266->Flags.F.STAIPIsSet = 0;                                                /* Reset flags */
        ESP8266->Flags.F.STANetmaskIsSet = 0;
        ESP8266->Flags.F.STAGatewayIsSet = 0;
    }
    return ESP8266->Result;                                 /* Return status */
}

ESP8266_Result_t ESP8266_GetAPIP(ESP8266_t *ESP8266)
{
    SendCommand(ESP8266, ESP8266_COMMAND_CIPAP, "AT+CIPAP_CUR?\r\n", "+CIPAP_CUR"); /* Send command */
    if (ESP8266->Result == ESP_OK) {                                                /* Check status */
        ESP8266->Flags.F.APIPIsSet = 0;                                             /* Reset flags */
        ESP8266->Flags.F.APNetmaskIsSet = 0;
        ESP8266->Flags.F.APGatewayIsSet = 0;
    }
    return ESP8266->Result;                                 /* Return status */
}

/******************************************/
/*            MAC MANIPULATION            */
/******************************************/
ESP8266_Result_t ESP8266_GetSTAMAC(ESP8266_t *ESP8266)
{
    SendCommand(ESP8266, ESP8266_COMMAND_CIPSTAMAC, "AT+CIPSTAMAC?\r\n", "+CIPSTAMAC"); /* Send command */
    if (ESP8266->Result == ESP_OK) {                                                    /* Check status */
        ESP8266->Flags.F.STAMACIsSet = 0;                                               /* Reset flags */
    }
    return ESP8266->Result;                                                             /* Return stats */
}

ESP8266_Result_t ESP8266_SetSTAMAC(ESP8266_t *ESP8266, uint8_t *addr)
{

    return SendMACCommand(ESP8266, addr, "AT+CIPSTAMAC_CUR", ESP8266_COMMAND_CIPSTAMAC);    /* Send current MAC command */
}

ESP8266_Result_t ESP8266_SetSTAMACDefault(ESP8266_t *ESP8266, uint8_t *addr)
{

    return SendMACCommand(ESP8266, addr, "AT+CIPSTAMAC_DEF", ESP8266_COMMAND_CIPSTAMAC);    /* Send current MAC command */
}

ESP8266_Result_t ESP8266_GetAPMAC(ESP8266_t *ESP8266)
{
    SendCommand(ESP8266, ESP8266_COMMAND_CIPAPMAC, "AT+CIPAPMAC?\r\n", "+CIPAPMAC");    /* Send command */
    if (ESP8266->Result == ESP_OK) {                                                    /* Check status */
        ESP8266->Flags.F.APMACIsSet = 0;                                                /* Reset flags */
    }
    return ESP8266->Result;                                                             /* Return status */
}

ESP8266_Result_t ESP8266_SetAPMAC(ESP8266_t *ESP8266, uint8_t *addr)
{
    return SendMACCommand(ESP8266, addr, "AT+CIPAPMAC_CUR", ESP8266_COMMAND_CIPAPMAC);  /* Send current MAC command */
}

ESP8266_Result_t ESP8266_SetAPMACDefault(ESP8266_t *ESP8266, uint8_t *addr)
{
    return SendMACCommand(ESP8266, addr, "AT+CIPAPMAC_DEF", ESP8266_COMMAND_CIPAPMAC);  /* Send current MAC command */
}

/******************************************/
/*                AP + STA                */
/******************************************/
#if ESP8266_USE_APSEARCH
ESP8266_Result_t ESP8266_ListWifiStations(ESP8266_t *ESP8266)
{
    ESP8266_APs.Count = 0;                                                          /* Reset pointer */
    return SendCommand(ESP8266, ESP8266_COMMAND_CWLAP, "AT+CWLAP\r\n", "+CWLAP");   /* Send list command */
}
#endif

ESP8266_Result_t ESP8266_GetAP(ESP8266_t *ESP8266)
{
    if (SendCommand(ESP8266, ESP8266_COMMAND_CWSAP, "AT+CWSAP?\r\n", "+CWSAP") != ESP_OK) { /* Send command to read current AP settings */
        return ESP8266->Result;
    }
    return ESP8266_WaitReady(ESP8266);                      /* Wait till command end */
}

ESP8266_Result_t ESP8266_SetAP(ESP8266_t *ESP8266, ESP8266_APConfig_t *ESP8266_Config)
{
    uint8_t ecn, maxc, hid, sep = ',';
    char ch[4];

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check IDLE state */
    if (                                                    /* Check input values */
        strlen(ESP8266_Config->SSID) > 64 ||
        strlen(ESP8266_Config->Pass) > 64 ||
        (ESP8266_Config->Ecn != ESP8266_Ecn_OPEN && strlen(ESP8266_Config->Pass) < 8) ||
        ESP8266_Config->Ecn == ESP8266_Ecn_WEP ||
        ESP8266_Config->MaxConnections < 1 || ESP8266_Config->MaxConnections > 4
        ) {
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);       /* Error */
    }

    Int2String(ch, ESP8266_Config->Channel);                /* Format number to string */
    ecn = (uint8_t)ESP8266_Config->Ecn + '0';
    maxc = (uint8_t)ESP8266_Config->MaxConnections + '0';
    hid = (uint8_t)ESP8266_Config->Hidden + '0';


    ESP8266_USARTSENDSTRING("AT+CWSAP_CUR=\"");             /* Send separate values */
    EscapeStringAndSend(ESP8266_Config->SSID);
    ESP8266_USARTSENDSTRING("\",\"");
    EscapeStringAndSend(ESP8266_Config->Pass);
    ESP8266_USARTSENDSTRING("\",");
    EscapeStringAndSend(ch);
    ESP8266_USARTSENDCHAR(&sep);
    ESP8266_USARTSENDCHAR(&ecn);
    ESP8266_USARTSENDCHAR(&sep);
    ESP8266_USARTSENDCHAR(&maxc);
    ESP8266_USARTSENDCHAR(&sep);
    ESP8266_USARTSENDCHAR(&hid);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);

    SendCommand(ESP8266, ESP8266_COMMAND_CWSAP, NULL, NULL);    /* Send command */
    return ESP8266_Update(ESP8266);                             /* Return status */
}

ESP8266_Result_t ESP8266_SetAPDefault(ESP8266_t *ESP8266, ESP8266_APConfig_t *ESP8266_Config)
{
    uint8_t ecn, maxc, hid, sep = ',';
    char ch[4];

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check IDLE state */
    if (                                                    /* Check input values */
        strlen(ESP8266_Config->SSID) > 64 ||
        strlen(ESP8266_Config->Pass) > 64 ||
        (ESP8266_Config->Ecn != ESP8266_Ecn_OPEN && strlen(ESP8266_Config->Pass) < 8) ||
        ESP8266_Config->Ecn == ESP8266_Ecn_WEP ||
        ESP8266_Config->MaxConnections < 1 || ESP8266_Config->MaxConnections > 4
        ) {
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_ERROR);       /* Error */
    }

    Int2String(ch, ESP8266_Config->Channel);                /* Go to ASCII */
    ecn = (uint8_t)ESP8266_Config->Ecn + '0';
    maxc = (uint8_t)ESP8266_Config->MaxConnections + '0';
    hid = (uint8_t)ESP8266_Config->Hidden + '0';

    ESP8266_USARTSENDSTRING("AT+CWSAP_DEF=\"");             /* Send separate */
    EscapeStringAndSend(ESP8266_Config->SSID);
    ESP8266_USARTSENDSTRING("\",\"");
    EscapeStringAndSend(ESP8266_Config->Pass);
    ESP8266_USARTSENDSTRING("\",");
    EscapeStringAndSend(ch);
    ESP8266_USARTSENDCHAR(&sep);
    ESP8266_USARTSENDCHAR(&ecn);
    ESP8266_USARTSENDCHAR(&sep);
    ESP8266_USARTSENDCHAR(&maxc);
    ESP8266_USARTSENDCHAR(&sep);
    ESP8266_USARTSENDCHAR(&hid);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);
    SendCommand(ESP8266, ESP8266_COMMAND_CWSAP, NULL, NULL);    /* Send command */

    return ESP8266_Update(ESP8266);                             /* Return status */
}
#if ESP8266_USE_CONNECTED_STATIONS == 1
static char resp[4];
ESP8266_Result_t ESP8266_GetConnectedStations(ESP8266_t *ESP8266)
{
    Int2String(resp, ESP8266->APIP[0]);                                                                 /* Format response, use first byte of IP as first string */
    if (SendCommand(ESP8266, ESP8266_COMMAND_CWLIF, "AT+CWLIF\r\n", (const char *)resp) != ESP_OK) {    /* Try to send command */
        return ESP8266->Result;
    }
    ESP8266->ConnectedStations.Count = 0;                   /* Reset counters in structure */
    return ESP8266->Result;                                 /* Return result */
}
#endif

ESP8266_Result_t ESP8266_SetAutoConnect(ESP8266_t *ESP8266, ESP8266_AutoConnect_t Autoconn)
{
    char c = (uint8_t)Autoconn + '0';

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check IDLE state */

    ESP8266_USARTSENDSTRING("AT+CWAUTOCONN=");              /* Format command and send */
    ESP8266_USARTSENDCHAR(&c);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);
    SendCommand(ESP8266, ESP8266_COMMAND_AUTOCONN, NULL, NULL); /* Send command */

    return ESP8266_WaitReady(ESP8266);                          /* Wait till end */
}

/******************************************/
/*               TCP CLIENT               */
/******************************************/
ESP8266_Result_t ESP8266_StartClientConnectionTCP(ESP8266_t *ESP8266, const char *name, char *location, uint16_t port, void *user_parameters)
{
    return StartClientConnection(ESP8266, ESP8266_ConnectionType_TCP, "TCP", name, location, port, 0, user_parameters);
}

/******************************************/
/*               UDP CLIENT               */
/******************************************/
ESP8266_Result_t ESP8266_StartClientConnectionUDP(ESP8266_t *ESP8266, const char *name, char *location, uint16_t port, uint16_t local_port, void *user_parameters)
{
    return StartClientConnection(ESP8266, ESP8266_ConnectionType_UDP, "UDP", name, location, port, local_port, user_parameters);
}

/******************************************/
/*               SSL CLIENT               */
/******************************************/
ESP8266_Result_t ESP8266_StartClientConnectionSSL(ESP8266_t *ESP8266, const char *name, const char *location, uint16_t port, void *user_parameters)
{
    return StartClientConnection(ESP8266, ESP8266_ConnectionType_SSL, "SSL", name, location, port, 0, user_parameters);
}

ESP8266_Result_t ESP8266_SetSSLBufferSize(ESP8266_t *ESP8266, uint16_t buffersize)
{
    char buff[6];

    ESP8266_CHECK_IDLE(ESP8266);                            /* Check if IDLE */

    Int2String(buff, buffersize);                           /* Format to string */

    ESP8266_USARTSENDSTRING("AT+CIPSSLSIZE=");              /* Format and send command */
    ESP8266_USARTSENDSTRING(buff);
    ESP8266_USARTSENDSTRING(ESP8266_CRLF);
    SendCommand(ESP8266, ESP8266_COMMAND_SSLBUFFERSIZE, NULL, NULL);    /* Send command */

    return ESP8266_WaitReady(ESP8266);                                  /* Wait response */
}

/******************************************/
/*              PING SUPPORT              */
/******************************************/
#if ESP8266_USE_PING == 1
ESP8266_Result_t ESP8266_Ping(ESP8266_t *ESP8266, const char *addr)
{
    ESP8266_CHECK_IDLE(ESP8266);                            /* Check idle */
    ESP8266_CHECK_WIFICONNECTED(ESP8266);                   /* Device must be connected to wifi */

    ESP8266->Pinging.Address = (char *)addr;                /* Save ping address information */
    ESP8266->Pinging.Success = 0;                           /* Reset flag */

    ESP8266_USARTSENDSTRING("AT+PING=\"");                  /* Format and send command */
    ESP8266_USARTSENDSTRING((char *)addr);
    ESP8266_USARTSENDSTRING("\""ESP8266_CRLF);

    if (SendCommand(ESP8266, ESP8266_COMMAND_PING, NULL, "+") == ESP_OK) {  /* Send command */
        ESP8266_Callback_PingStarted(ESP8266, addr);                        /* Call user function */
    }
    return ESP8266->Result;                                                 /* Return status */
}
#endif

/******************************************/
/*              SNTP SUPPORT              */
/******************************************/
#if ESP8266_USE_SNTP == 1
ESP8266_Result_t ESP8266_SNTPSetServer(ESP8266_t *ESP8266, uint8_t num, const char *servername)
{
    ESP8266_CHECK_IDLE(ESP8266);                            /* Check if IDLE */
    if (num > 2 || servername == NULL) {                    /* Check input parameters */
        ESP8266_RETURNWITHSTATUS(ESP8266, ESP_INVALIDPARAMETERS);
    }
    num += '0';                                             /* Go to ASCII */
    ESP8266_USARTSENDSTRING("AT+SNTP=");                    /* Format and send command */
    ESP8266_USARTSENDCHAR(&num);
    ESP8266_USARTSENDSTRING(",\"");
    ESP8266_USARTSENDSTRING(servername);
    ESP8266_USARTSENDSTRING("\""ESP8266_CRLF);
    SendCommand(ESP8266, ESP8266_COMMAND_SNTP_SET, NULL, NULL); /* Send command */

    ESP8266_WaitReady(ESP8266);                                 /* Wait till end */
    ESP8266_RETURNWITHOPERATIONSTATUS(ESP8266);                 /* Check last operation */
}

ESP8266_Result_t ESP8266_SNTPGetDateTime(ESP8266_t *ESP8266)
{
    ESP8266_CHECK_IDLE(ESP8266);                                                /* Check if IDLE */
    ESP8266_CHECK_WIFICONNECTED(ESP8266);                                       /* Check if connected to network */
    return SendCommand(ESP8266, ESP8266_COMMAND_SNTP, "AT+SNTP\r\n", "+SNTP");  /* Send command */
}
#endif

/******************************************/
/*             DATA RECEIVED              */
/******************************************/
uint16_t ESP8266_DataReceived(uint8_t *ch, uint16_t count)
{
    uint32_t r;

    r = BUFFER_Write(&USART_Buffer, ch, count);             /* Writes data to USART buffer */
#if ESP8266_USE_CTS
    if (BUFFER_GetFree(&USART_Buffer) <= 3) {
        if (RTSStatus != ESP_RTS_HIGH) {                    /* Set RTS high */
            ESP8266_LL_SetRTS(ESP_RTS_HIGH);
            RTSStatus = ESP_RTS_HIGH;
        }
    }
#endif
    return r;
}

/******************************************/
/*                CALLBACKS               */
/******************************************/
/* Called when ready string detected */
__weak void ESP8266_Callback_DeviceReady(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_DeviceReady could be implemented in the user file
     */
}

/* Called when watchdog reset on ESP8266 is detected */
__weak void ESP8266_Callback_WatchdogReset(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_WatchdogReset could be implemented in the user file
     */
}

/* Called when module disconnects from wifi network */
__weak void ESP8266_Callback_WifiDisconnected(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_WifiDisconnected could be implemented in the user file
     */
}

/* Called when module connects to wifi network */
__weak void ESP8266_Callback_WifiConnected(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_WifiConnected could be implemented in the user file
     */
}

/* Called when connection to wifi network fails */
__weak void ESP8266_Callback_WifiConnectFailed(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_WifiConnectFailed could be implemented in the user file
     */
}

/* Called when module gets IP address */
__weak void ESP8266_Callback_WifiGotIP(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_WifiGotIP could be implemented in the user file
     */
}

/* Called when IP is set */
__weak void ESP8266_Callback_WifiIPSet(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_WifiIPSet could be implemented in the user file
     */
}

/* Called when wifi spot is detected */
__weak void ESP8266_Callback_WifiDetected(ESP8266_t *ESP8266, ESP8266_APs_t *ESP8266_AP)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_WifiDetected could be implemented in the user file
     */
}

/* Called on DHCP timeout */
__weak void ESP8266_Callback_DHCPTimeout(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_DHCPTimeout could be implemented in the user file
     */
}

/* Called when "x,CONNECT" is detected */
__weak void ESP8266_Callback_ServerConnectionActive(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ServerConnectionActive could be implemented in the user file
     */
}

/* Called when "x,CLOSED" is detected */
__weak void ESP8266_Callback_ServerConnectionClosed(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ServerConnectionClosed could be implemented in the user file
     */
}

/* Called when "+IPD..." is detected */
__weak void ESP8266_Callback_ServerConnectionDataReceived(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection, char *Buffer)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ServerConnectionDataReceived could be implemented in the user file
     */
}

/* Called when user should fill data buffer to be sent with connection */
__weak uint16_t ESP8266_Callback_ServerConnectionSendData(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection, char *Buffer, uint16_t max_buffer_size)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ServerConnectionSendData could be implemented in the user file
     */

    /* Return number of bytes in array */
    return 0;
}

/* Called when data are send successfully */
__weak void ESP8266_Callback_ServerConnectionDataSent(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ServerConnectionDataSent could be implemented in the user file
     */
}

/* Called when error returned trying to sent data */
__weak void ESP8266_Callback_ServerConnectionDataSentError(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ServerConnectionDataSentError could be implemented in the user file
     */
}

/* Called when user is connected to server as client */
__weak void ESP8266_Callback_ClientConnectionConnected(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ClientConnectionConnected could be implemented in the user file
     */
}

/* Called when user should fill data buffer to be sent with connection */
__weak uint16_t ESP8266_Callback_ClientConnectionSendData(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection, char *Buffer, uint16_t max_buffer_size)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ClientConnectionSendData could be implemented in the user file
     */

    /* Return number of bytes in array */
    return 0;
}

/* Called when data are send successfully */
__weak void ESP8266_Callback_ClientConnectionDataSent(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ClientConnectionDataSent could be implemented in the user file
     */
}

/* Called when error returned trying to sent data */
__weak void ESP8266_Callback_ClientConnectionDataSentError(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ClientConnectionDataSentError could be implemented in the user file
     */
}

/* Called when server returns data back to client */
__weak void ESP8266_Callback_ClientConnectionDataReceived(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection, char *Buffer)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ClientConnectionDataReceived could be implemented in the user file
     */
}

/* Called when ERROR is returned on AT+CIPSTART command */
__weak void ESP8266_Callback_ClientConnectionError(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ClientConnectionError could be implemented in the user file
     */
}

/* Called when timeout is reached on AT+CIPSTART command */
__weak void ESP8266_Callback_ClientConnectionTimeout(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ClientConnectionTimeout could be implemented in the user file
     */
}

/* Called when "x,CLOSED" is detected when connection was made as client */
__weak void ESP8266_Callback_ClientConnectionClosed(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ClientConnectionClosed could be implemented in the user file
     */
}

#if ESP8266_USE_PING == 1
/* Called when pinging started */
__weak void ESP8266_Callback_PingStarted(ESP8266_t *ESP8266, const char *address)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_PingStarted could be implemented in the user file
     */
}

/* Called when PING command ends */
__weak void ESP8266_Callback_PingFinished(ESP8266_t *ESP8266, ESP8266_Ping_t *Pinging)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_PingFinished could be implemented in the user file
     */
}
#endif

#if ESP8266_USE_FIRMWAREUPDATE == 1
/* Called on status messages for network firmware update */
__weak void ESP8266_Callback_FirmwareUpdateStatus(ESP8266_t *ESP8266, ESP8266_FirmwareUpdate_t status)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_FirmwareUpdateStatus could be implemented in the user file
     */
}

/* Called when firmware network update was successful */
__weak void ESP8266_Callback_FirmwareUpdateSuccess(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_FirmwareUpdateSuccess could be implemented in the user file
     */
}

/* Called when firmware network error occurred */
__weak void ESP8266_Callback_FirmwareUpdateError(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_FirmwareUpdateError could be implemented in the user file
     */
}
#endif

/* Called when AT+CWLIF returns OK */
#if ESP8266_USE_CONNECTED_STATIONS == 1
__weak void ESP8266_Callback_ConnectedStationsDetected(ESP8266_t *ESP8266, ESP8266_ConnectedStations_t *Stations)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_ConnectedStationsDetected could be implemented in the user file
     */
}
#endif

#if ESP8266_USE_SNTP == 1
/* Called on OK received from ESP for SNTP command */
__weak void ESP8266_Callback_SNTPOk(ESP8266_t *ESP8266, ESP8266_SNTP_t *SNTP)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_SNTPOk could be implemented in the user file
     */
}

/* Called when Error is detected on SNTP command */
__weak void ESP8266_Callback_SNTPError(ESP8266_t *ESP8266)
{
    /* NOTE: This function Should not be modified, when the callback is needed,
           the ESP8266_Callback_SNTPError could be implemented in the user file
     */
}
#endif
