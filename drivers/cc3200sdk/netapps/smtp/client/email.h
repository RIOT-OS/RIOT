//*****************************************************************************
// Copyright (C) 2014 Texas Instruments Incorporated
//
// All rights reserved. Property of Texas Instruments Incorporated.
// Restricted rights to use, duplicate or disclose this code are
// granted through contract.
// The program may not be used without the written permission of
// Texas Instruments Incorporated or against the terms and conditions
// stipulated in the agreement under which this program has been supplied,
// and under no circumstances can it be used with non-TI connectivity device.
//
//*****************************************************************************


#include "simplelink.h"

#ifndef __EMAIL_H__
#define __EMAIL_H__


#ifdef	__cplusplus
extern "C" {
#endif

// SMTP defines
#define SMTP_BUF_LEN 		100
  

/* NetApp Email IDs */
#define SL_NET_APP_EMAIL_ID   		(3)

/* NetApp Email set/get options */
#define NETAPP_ADVANCED_OPT		(1)
#define NETAPP_SOURCE_EMAIL		(2)
#define NETAPP_PASSWORD			(3)
#define NETAPP_DEST_EMAIL 		(4)
#define NETAPP_SUBJECT			(5)

//Initialize error handling flags for smtp state machine
typedef enum
{
  SL_EMAIL_ERROR_FAILED = -1,
  SL_EMAIL_ERROR_NONE = 0,
  SL_EMAIL_ERROR_INIT,
  SL_EMAIL_ERROR_HELO,
  SL_EMAIL_ERROR_AUTH,
  SL_EMAIL_ERROR_FROM,
  SL_EMAIL_ERROR_RCPT,
  SL_EMAIL_ERROR_DATA,
  SL_EMAIL_ERROR_MESSAGE,
  SL_EMAIL_ERROR_QUIT
}SlNetAppEmailERROR_e;  

/*NetApp Email protocol types */
#define SL_NET_APP_SMTP_PROTOCOL	(1)
typedef struct
{
    unsigned long  ProtocolSubType;
    unsigned long  Port;
    unsigned long  Family;
    unsigned long  SecurityMethod;
    unsigned long  SecurityCypher;
    unsigned long  Ip;                     /* IPv4 address or IPv6 first 4 bytes  */
    unsigned long  Ip1OrPaadding;
    unsigned long  Ip2OrPaadding;
    unsigned long  Ip3OrPaadding;
}SlNetAppEmailOpt_t;

typedef struct
{
     unsigned char Username[254];
	 unsigned char Length;
}SlNetAppSourceEmail_t;

typedef struct
{
     unsigned char Password[30];
	 unsigned char Length;
}SlNetAppSourcePassword_t;

typedef struct
{
	unsigned char Email[254];
	unsigned char Length;
}SlNetAppDestination_t;

typedef struct
{
	unsigned char Value[60];
	unsigned char Length;
}SlNetAppEmailSubject_t;

typedef struct
{
	unsigned char Value[100];
	unsigned char Length;
}SlNetAppServerError_t;

/*!
    \brief Configures Email Parameters
     
    \param[in]      AppId: SL_NET_APP_EMAIL_ID
    \param[in]      Option, NETAPP_ADVANCED_OPT, NETAPP_SOURCE_EMAIL etc                  
    \param[in]      OptionLen, Length of the Option Value 
    \param[in]      pOptionValue, Depends on Option
              For Ex. NETAPP_ADVANCED_OPT, Email Server Info(SlNetAppEmailOpt_t)
    \return         On success, zero is returned. On error, -1 is 
                    returned   
    \sa             sl_NetAppEmailGet
*/
long sl_NetAppEmailSet(unsigned char AppId ,unsigned char Option ,
                       unsigned char OptionLen, unsigned char *pOptionValue);

/*!
    \brief Configures Email Parameters
     
    \param[in]      AppId: SL_NET_APP_EMAIL_ID
    \param[in]      Option, NETAPP_ADVANCED_OPT, NETAPP_SOURCE_EMAIL etc                  
    \param[out]     OptionLen, Length of the Option Value 
    \param[out]      pOptionValue, Depends on Option
              For Ex. NETAPP_ADVANCED_OPT, Email Server Info(SlNetAppEmailOpt_t)
    \return         On success, zero is returned. On error, -1 is 
                    returned   
    \sa             sl_NetAppEmailSet
*/
long sl_NetAppEmailGet(unsigned char AppId, unsigned char Option, 
                       unsigned char *pOptionLen, unsigned char *pOptionValue);


/*!
    \brief connect to Email Server    
 
    \return         On success, zero is returned. On error, negative is 

*/
long sl_NetAppEmailConnect();

/*!
    \brief Configures Email Parameters
     
    \param[in]      Destination: Recipient Email Address (ex. abc@xyz.com)
    \param[in]      Subject - Email Subject                  
    \param[in]      Message - Content of Email 
    \param[out]      out_server_err, Server Error Message
              For Ex. Authentication Failed etc
              
    \return         SlNetAppEmailERROR_e(On success, zero is returned)   
*/
long sl_NetAppEmailSend(char* Destination, char* Subject, char* Message,
                        SlNetAppServerError_t* out_server_err);


#ifdef  __cplusplus
}
#endif // __cplusplus

#endif	// __NETAPP_H__

