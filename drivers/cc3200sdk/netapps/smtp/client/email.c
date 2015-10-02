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

//*****************************************************************************
//
// Protocol Name     -  Simple Mail Transfer Protocol
// Protocol Overview -  The objective of the Simple Mail Transfer Protocol (SMTP) is to
//					    transfer mail reliably and efficiently.SMTP is independent of the
//						particular transmission subsystem and requires only a reliable
//    					ordered data stream channel.
//						Refer: https://www.ietf.org/rfc/rfc2821.txt
//*****************************************************************************

// simplelink includes
#include "simplelink.h"

//Apps include
#include "email.h"


//****************************************************************************
//                          LOCAL DEFINES                                   
//****************************************************************************
#define EMAIL_APP_VERSION "v1.1.0"
#define SUCCESS 	(0)
/*NetApp Email protocol types */
#define SL_NET_APP_SMTP_PROTOCOL		(1)

// check the error code and handle it
#define ASSERT_ON_ERROR(error_code)\
            {\
                 if(error_code < 0) \
                   {\
                        return error_code;\
                 }\
            }

//Initialize states for smtp state machine
typedef enum
{
    smtpINACTIVE = 0,
    smtpINIT,
    smtpHELO,
    smtpAUTH,
    smtpFROM,
    smtpRCPT,
    smtpDATA,
    smtpMESSAGE,
    smtpQUIT,
    smtpERROR
}_SlsmtpStatus_e;


//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
//////////////////////////////////////////////////////////////////////////////
// Moved definitions from smtp_gen.h to c file
const char cSmtpHelo[] = {'H','E','L','O','\0'};
const char cSmtpMailFrom[] = {'M','A','I','L',' ','F','R','O','M',':',' ',
							  '\0'};
const char cSmtpRcpt[] = {'R','C','P','T',' ','T','O',':',' ','\0'};
const char cSmtpData[] = "DATA";
const char cSmtpCrlf[] = "\r\n";
const char cSmtpDcrlf[] = "\r\n\r\n";
const char cSmtpSubject[] = "Subject: ";
const char cSmtpTo[] = "To: ";
const char cSmtpFrom[] = "From: ";
const char cSmtpDataEnd[] = {'\r','\n','.','\r','\n','\0'};
const char cSmtpQuit[] = {'Q','U','I','T','\r','\n','\0'};
// Return Codes
const char cSmtpCodeReady[] = {'2','2','0','\0'};
const char cSmtpCodeOkReply[] = {'2','5','0','\0'};
const char cSmtpCodeIntermedReply[] = {'3','5','4','\0'};
const char cSmtpCodeAuthReply[] = {'3','3','4','\0'};
const char cSmtpCodeAuthSuccess[] = {'2','3','5','\0'};

/////////SMTP INFORMATION ////
unsigned int g_uiSmtpStatus = smtpINIT;
int g_iConnectStatus = -1;
long g_lSmtpSocket = -1;
unsigned int g_uiEmailSetStatus = 0;
signed char g_uiEmailErrorInfo = SL_EMAIL_ERROR_NONE;


char g_cCmdBuf[SMTP_BUF_LEN];
char g_cBuf[SMTP_BUF_LEN];
char g_cBasekey1[128];
char g_cBasekey2[128];
char g_cMessage[64];
char g_cUsernameRfc[254];
char g_cDestinationEmailRfc[256];


SlNetAppEmailOpt_t g_sEmailOpt;
SlNetAppSourceEmail_t g_sEmail;
SlNetAppSourcePassword_t g_sSourcePass;
SlNetAppDestination_t g_sDestination;
SlNetAppEmailSubject_t g_sSubject;

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

extern long ConvertToBase64(char *pcOutStr, const char *pccInStr, int iLen);

//****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES                           
//****************************************************************************
signed long _smtpConnect(void);
signed long _smtpStateMachine(void);
long _sendSMTPCommand(long socket, char * cmd, char * cmdparam,
							  char * respBuf);
long _generateBase64Key(char * basekey1, char * basekey2);



//****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//***************************************************************************


//****************************************************************************
//
//! Function to set following option/params
//!		1. Source email address
//!		2. Destination email address
//!		3. password
//!		4. Email subject
//!		5. Advance options like security method, port		
//!
//! \param 	AppId - Application ID
//!			Option - Option that need to set
//!			OptionLen - Optio data length
//!			pOptionValue - Pointer to option data
//!
//! return 0 on success or error code
//
//****************************************************************************
long sl_NetAppEmailSet(unsigned char AppId, unsigned char Option,
						   unsigned char OptionLen,
						   unsigned char *pOptionValue)
{

    SlNetAppEmailOpt_t* pEmailOpt = 0;
    SlNetAppSourceEmail_t* pSourceEmail;
    SlNetAppSourcePassword_t* pSourcePassword;
    SlNetAppDestination_t* pDestinationEmail;
    SlNetAppEmailSubject_t* pSubject;

    if (AppId != SL_NET_APP_EMAIL_ID)
    {
        return -1;
    }

    switch (Option)
    {
        case NETAPP_ADVANCED_OPT:
			/* Set advance options */
            pEmailOpt = (SlNetAppEmailOpt_t*)pOptionValue;
            g_sEmailOpt.Port = pEmailOpt->Port;
            g_sEmailOpt.Family = pEmailOpt->Family;
            g_sEmailOpt.SecurityMethod = pEmailOpt->SecurityMethod;
            g_sEmailOpt.SecurityCypher = pEmailOpt->SecurityCypher;
            g_sEmailOpt.Ip = pEmailOpt->Ip;
            
            g_uiEmailSetStatus+=1;
        break;

        case NETAPP_SOURCE_EMAIL:
			/* Set source email address */
            pSourceEmail = (SlNetAppSourceEmail_t*)pOptionValue; 
            memcpy(g_sEmail.Username, pSourceEmail->Username, OptionLen);
            g_sEmail.Length = OptionLen;

            g_uiEmailSetStatus+=2;

        break;

        case NETAPP_PASSWORD:
			/* Set source password */
            pSourcePassword = (SlNetAppSourcePassword_t*)pOptionValue;
            memcpy(g_sSourcePass.Password, pSourcePassword->Password,
                       OptionLen);
            g_sSourcePass.Length = OptionLen;

            g_uiEmailSetStatus+=4;

        break;

        case NETAPP_DEST_EMAIL:
			/* Set destination address */
            pDestinationEmail=(SlNetAppDestination_t*)pOptionValue;
            memcpy(g_sDestination.Email, pDestinationEmail->Email, OptionLen);
            g_sDestination.Length=OptionLen;

            g_uiEmailSetStatus+=8;

        break;

        case NETAPP_SUBJECT:
			/* Set email subject */
            pSubject=(SlNetAppEmailSubject_t*)pOptionValue;
            memcpy(g_sSubject.Value, pSubject->Value, OptionLen);
            g_sSubject.Length=OptionLen;

            g_uiEmailSetStatus+=16;

        break;

        default:
                return -1;
  }  
  return SUCCESS;
}


//****************************************************************************
//
//! Function to get following option/params value
//!		1. Source email address
//!		2. Destination email address
//!		3. password
//!		4. Email subject
//!		5. Advance options like security method, port		
//!
//! \param 	AppId - Application ID
//!			Option - Option that want to get
//!			pOptionLen - Poninter to option data length container
//!			pOptionValue - Pointer to option data
//!
//! return 0 on success or error code
//
//****************************************************************************
long sl_NetAppEmailGet(unsigned char AppId, unsigned char Option, 
						   unsigned char *pOptionLen, 
						   unsigned char *pOptionValue)
{
   
    if (AppId != SL_NET_APP_EMAIL_ID)
    {
            return -1;
    }

    switch (Option)
    {     
        case NETAPP_ADVANCED_OPT:
        {
			/* Get advanve option value */
			SlNetAppEmailOpt_t* pEmailOpt = 0;
            pEmailOpt = (SlNetAppEmailOpt_t*)pOptionValue;

            if(pOptionLen)
                  *pOptionLen = 0;
            
            pEmailOpt->Port = g_sEmailOpt.Port;
            pEmailOpt->Family = g_sEmailOpt.Family;
            pEmailOpt->SecurityMethod = g_sEmailOpt.SecurityMethod;
            pEmailOpt->SecurityCypher = g_sEmailOpt.SecurityCypher;
            pEmailOpt->Ip = g_sEmailOpt.Ip;
        }
        break;

        case NETAPP_SOURCE_EMAIL:
			/* Get source email address */
			pOptionValue = g_sEmail.Username; 
			pOptionLen = (unsigned char*)g_sEmail.Length;
        break;

        case NETAPP_PASSWORD:
			/* Get source email password */
			pOptionValue = g_sSourcePass.Password;
			pOptionLen = (unsigned char*)g_sSourcePass.Length;
        break;

        case NETAPP_DEST_EMAIL:
			/* Get destination email address */
			pOptionValue = g_sDestination.Email;
			pOptionLen = (unsigned char*)g_sDestination.Length;
        break;

        default:
                return -1;
        } 
	
    return SUCCESS;
}


//****************************************************************************
//
//! Function to connect smtp server.
//!
//! \param none
//!
//! return smtp server connection status
//
//****************************************************************************
long sl_NetAppEmailConnect()
{
  
    if (!(g_uiEmailSetStatus >= 0x07))
    {
        return -1;
    }

    g_iConnectStatus = _smtpConnect();
    return g_iConnectStatus;
}

//****************************************************************************
//
//! Function to send email to destination email address.
//!
//! \param 	Destination - Destination email address
//!			Subject - Email subject
//!			Message - email body
//!			out_server_err - Pointer to container to collec
//!							smtp error mesages
//!
//! return status
//
//****************************************************************************
long sl_NetAppEmailSend(char* Destination, char* Subject, char* Message,
							 SlNetAppServerError_t* out_server_err)
{
    long ret =0;

    if(g_iConnectStatus!=0)
            return SL_EMAIL_ERROR_FAILED;

    strcpy(g_cMessage,Message);

    ret = _smtpStateMachine();

    strncpy((char*)out_server_err->Value,g_cBuf,100);
    out_server_err->Length = strlen((char*)out_server_err->Value);

    return ret;

  
}

//////////////////////////////////////////////////////////////////////////////
//****************************************************************************
//
//!	smtpConnect
//!
//! @brief	Creates and connects to an unsecure socket to be used for SMTP
//!
//! @param	None
//!
//! @return	0 if success, -1 if socket create/connect error, -2 if socket option error
//
//****************************************************************************
signed long _smtpConnect(void)
{
    SlSockAddrIn_t LocalAddr;
    int LocalAddrSize;
    //sockaddr LocalAddr;
    //unsigned long smtpServerIP;
    int Status;

    //TODO: Implement gethostbyname

    LocalAddr.sin_family = g_sEmailOpt.Family;
    LocalAddr.sin_port = sl_Htons(g_sEmailOpt.Port);
    LocalAddr.sin_addr.s_addr = sl_Htonl(g_sEmailOpt.Ip);
    LocalAddrSize = sizeof(SlSockAddrIn_t);



    //If TLS is required
    if(g_sEmailOpt.SecurityMethod<=5)
    {
        // Create secure socket
        g_lSmtpSocket = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_SEC_SOCKET);
        // Check if socket was created successfully
        if (g_lSmtpSocket == -1)
        {
            sl_Stop(90);
            return -1;
        }
        SlTimeval_t tTimeout;
        tTimeout.tv_sec=10;
        tTimeout.tv_usec=90000;
        Status = sl_SetSockOpt(g_lSmtpSocket, SL_SO_RCVTIMEO, 0, &tTimeout,
                                                   sizeof(SlTimeval_t));

        char method = g_sEmailOpt.SecurityMethod;

        long cipher = g_sEmailOpt.SecurityCypher;

        //Set Socket Options that were just defined
        Status = sl_SetSockOpt(g_lSmtpSocket, SL_SOL_SOCKET, 25, &method,
                                                   sizeof(method));
        if( Status < 0 )
        {
            sl_Close(g_lSmtpSocket);
            return -2;
          
        }
        Status = sl_SetSockOpt(g_lSmtpSocket, SL_SOL_SOCKET, 26, &cipher,
                                                        sizeof(cipher));
        if( Status < 0 )
        {	
            sl_Close(g_lSmtpSocket);
            return -2;
          
        }
    }
    // If no TLS required
    else
    {
        // Create socket
        g_lSmtpSocket = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_IPPROTO_TCP);
        // Check if socket was created successfully
        if (g_lSmtpSocket == -1)
        {
            sl_Stop(90);
            return -1;
        }	
    }
    // If success, connect to socket
    Status = sl_Connect(g_lSmtpSocket, (SlSockAddr_t *)&LocalAddr, 
                                              LocalAddrSize);
    if((Status < 0) &&(SL_ESECSNOVERIFY !=Status))
    {
        return -1;
    }
    // Success
    return SUCCESS;
}



//****************************************************************************
//
//! smtpStateMachine
//!
//!     @brief  This function contains the smtp state machine
//!
//!     @param  None
//!
//!     @return 0 if success, -1 if error
//!  
//
//****************************************************************************
signed long _smtpStateMachine(void)
{
    int exit = 0;
	long lRetVal = -1;
    g_uiEmailErrorInfo = SL_EMAIL_ERROR_NONE;

    // If socket has been opened, check for acknowledge from SMTP server
    while(exit == 0)
    {      
        switch(g_uiSmtpStatus)
        {
			//An SMTP session is initiated when a client opens a connection to a
			//server and the server responds with an opening message.
            case smtpINIT:		  
                // Create buffer, Read so we can chek for 220 'OK' from server
                memset(g_cBuf,0,100);
                //TODO: verify this use
                lRetVal = sl_Recv(g_lSmtpSocket,g_cBuf,sizeof(g_cBuf),0);
                if(lRetVal < 0)
                {
                    g_uiSmtpStatus= smtpERROR;
                    g_uiEmailErrorInfo = SL_EMAIL_ERROR_INIT;  
                    break;
                }
                

                // If buffer has 220, set state to HELO
                if(g_cBuf[0] == cSmtpCodeReady[0] &&
                   g_cBuf[1] == cSmtpCodeReady[1] && 
                   g_cBuf[2] == cSmtpCodeReady[2])
                {
                        g_uiSmtpStatus= smtpHELO;
                }
                // Else error, set state to ERROR
                else
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_INIT;
                }
          break;
      
			//   The client normally sends the EHLO command to the
			//   server, indicating the client's identity.  In addition to opening the
			//   session, use of EHLO indicates that the client is able to process
			//   service extensions and requests that the server provide a list of the
			//   extensions it supports.
            case smtpHELO:
                lRetVal = _sendSMTPCommand(g_lSmtpSocket, "HELO localhost", NULL, g_cBuf);
                if(lRetVal < 0)
                {
                    g_uiSmtpStatus= smtpERROR;
                    g_uiEmailErrorInfo = SL_EMAIL_ERROR_HELO;  
                    break;
                }                

                // If buffer has 250, set state to AUTH
                if(g_cBuf[0] == cSmtpCodeOkReply[0] &&
                         g_cBuf[1] == cSmtpCodeOkReply[1] &&
                         g_cBuf[2] == cSmtpCodeOkReply[2])
                {
                        g_uiSmtpStatus= smtpAUTH;
                }
                // Else error, set state to ERROR
                else
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_HELO;
                }
            break;
      

			// The client sends the AUTH command with SASL mechanism to use with
            case smtpAUTH:
    			// SASL PLain - Authentication with server username and password
            	//Refer - http://www.ietf.org/rfc/rfc4616.txt
                lRetVal = _generateBase64Key((char*)g_sEmail.Username, g_cBasekey1);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_AUTH;
                        break;
                }                
                lRetVal = _generateBase64Key((char*)g_sSourcePass.Password, g_cBasekey2);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_AUTH;
                        break;
                }                

                // Send request to server for authentication
                lRetVal = _sendSMTPCommand(g_lSmtpSocket, "AUTH LOGIN", NULL, g_cBuf);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_AUTH;
                        break;
                }                
                // If buffer is 334, give username in base64
                if(g_cBuf[0] == cSmtpCodeAuthReply[0] &&
                         g_cBuf[1] == cSmtpCodeAuthReply[1] && 
                         g_cBuf[2] == cSmtpCodeAuthReply[2])
                {
                        lRetVal = _sendSMTPCommand(g_lSmtpSocket, g_cBasekey1, NULL, g_cBuf);
                        if(lRetVal < 0)
                        {
                                g_uiSmtpStatus= smtpERROR;
                                g_uiEmailErrorInfo = SL_EMAIL_ERROR_AUTH;
                                break;
                        }
                        // If buffer is 334, give password in base64
                        if(g_cBuf[0] == cSmtpCodeAuthReply[0] && 
                           g_cBuf[1] == cSmtpCodeAuthReply[1] && 
                           g_cBuf[2] == cSmtpCodeAuthReply[2])
                        {
                                lRetVal = _sendSMTPCommand(g_lSmtpSocket, g_cBasekey2, NULL,
                                                     g_cBuf);
                                if(lRetVal < 0)
                                {
                                        g_uiSmtpStatus= smtpERROR;
                                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_AUTH;
                                        break;
                                }
                        }
                }
                if(g_cBuf[0] == cSmtpCodeAuthSuccess[0] &&
                         g_cBuf[1] == cSmtpCodeAuthSuccess[1] &&
                         g_cBuf[2] == cSmtpCodeAuthSuccess[2])
                {
                        // Authentication was successful, set state to FROM
                        g_uiSmtpStatus= smtpFROM;
                }
                // Else error, set state to ERROR
                else
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_AUTH;
                }							
          
              break;	
      
             // The SMTP transaction starts with a MAIL command which includes the
             // sender information
             // MAIL FROM:<reverse-path> [SP <mail-parameters> ] <CRLF>
            case smtpFROM:     

                lRetVal = _sendSMTPCommand(g_lSmtpSocket, (char *)cSmtpMailFrom, 
                                        (char*)g_sEmail.Username, g_cBuf); 
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_FROM;
                        break;
                }                

                // If buffer has 250, set state to RCPT
                if(g_cBuf[0] == cSmtpCodeOkReply[0] &&
                         g_cBuf[1] == cSmtpCodeOkReply[1] && 
                         g_cBuf[2] == cSmtpCodeOkReply[2])
                {
                        g_uiSmtpStatus= smtpRCPT;
                }
                else
                {
                        memcpy(g_cUsernameRfc,"<",1);
                        memcpy(&g_cUsernameRfc[1], g_sEmail.Username,
                                    strlen((char*)g_sEmail.Username));
                        memcpy(&g_cUsernameRfc[1+strlen((char*)g_sEmail.Username)],
                                   ">",1);
                        lRetVal = _sendSMTPCommand(g_lSmtpSocket, (char *)cSmtpMailFrom,
                                                         g_cUsernameRfc, g_cBuf);  
                        if(lRetVal < 0)
                        {
                                g_uiSmtpStatus= smtpERROR;
                                g_uiEmailErrorInfo = SL_EMAIL_ERROR_FROM;
                                break;
                        }                        
                        if(g_cBuf[0] == cSmtpCodeOkReply[0] &&
                           g_cBuf[1] == cSmtpCodeOkReply[1] &&
                           g_cBuf[2] == cSmtpCodeOkReply[2])
                        {
                                g_uiSmtpStatus= smtpRCPT;
                        }
                        else
                        {
                            g_uiSmtpStatus= smtpERROR;
                            g_uiEmailErrorInfo = SL_EMAIL_ERROR_FROM;	
                        }
                }
              break;
      
            // Send the destination email to the smtp server
            //RCPT TO:<forward-path> [ SP <rcpt-parameters> ] <CRLF>
            case smtpRCPT:      

                lRetVal = _sendSMTPCommand(g_lSmtpSocket, (char *)cSmtpRcpt, 
                             (char*)g_sDestination.Email, g_cBuf);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_RCPT;
                        break;
                }                

                // If buffer has 250, set state to DATA
                if(g_cBuf[0] == cSmtpCodeOkReply[0] && 
                         g_cBuf[1] == cSmtpCodeOkReply[1] &&
                         g_cBuf[2] == cSmtpCodeOkReply[2])
                {
                    g_uiSmtpStatus= smtpDATA;
                }
                else
            {
                    memcpy(g_cDestinationEmailRfc,"<",1);
                    memcpy(&g_cDestinationEmailRfc[1], g_sDestination.Email,
                                    strlen((char*)g_sDestination.Email));
                    
                    memcpy(&g_cDestinationEmailRfc
                            [1+strlen((char*)g_sDestination.Email)],">",1);
                    
                    lRetVal = _sendSMTPCommand(g_lSmtpSocket, (char *)cSmtpRcpt,
                                                      g_cDestinationEmailRfc, g_cBuf);
                    if(lRetVal < 0)
                    {
                            g_uiSmtpStatus= smtpERROR;
                            g_uiEmailErrorInfo = SL_EMAIL_ERROR_RCPT;
                            break;
                    }                    
                    // If buffer has 250, set state to DATA
                    if(g_cBuf[0] == cSmtpCodeOkReply[0] &&
                       g_cBuf[1] == cSmtpCodeOkReply[1] &&
                       g_cBuf[2] == cSmtpCodeOkReply[2])
                    {
                        g_uiSmtpStatus= smtpDATA;
                    }
                    else
                    {		 
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_RCPT;
                    }
                }
              break;	
      
            // Send the "DATA" message to the server, indicates client is ready to 
            // construct  the body of the email
            // DATA <CRLF>
            case smtpDATA:
                lRetVal = _sendSMTPCommand(g_lSmtpSocket, (char *)cSmtpData, NULL,
                                                 g_cBuf);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_DATA;
                        break;
                }                
                // If buffer has 250, set state to MESSAGE
                if(g_cBuf[0] == cSmtpCodeIntermedReply[0] &&
                         g_cBuf[1] == cSmtpCodeIntermedReply[1] &&
                         g_cBuf[2] == cSmtpCodeIntermedReply[2])
                {
                        g_uiSmtpStatus= smtpMESSAGE;
                }
                else
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_DATA;
                }
              break;
      
            case smtpMESSAGE:
                // Send actual Message, preceded by FROM, TO and Subject

                // Start with E-Mail's "Subject:" field
                lRetVal = _sendSMTPCommand(g_lSmtpSocket, (char *)cSmtpSubject,
                                                   (char*)g_sSubject.Value, NULL);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_MESSAGE;
                        break;
                }                

                // Add E-mail's "To:" field
                lRetVal = _sendSMTPCommand(g_lSmtpSocket, (char *)cSmtpTo,
                                                  (char*)g_sDestination.Email, NULL);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_MESSAGE;
                        break;
                }                 

                // Add E-mail's "From:" field
                lRetVal = _sendSMTPCommand(g_lSmtpSocket, (char *)cSmtpFrom,
                                                   (char*)g_sEmail.Username, NULL);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_MESSAGE;
                        break;
                }                 

                // Send CRLF
                lRetVal = sl_Send(g_lSmtpSocket,cSmtpCrlf,strlen(cSmtpCrlf),0);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_MESSAGE;
                        break;
                }                 

                // Send body of message
                lRetVal = _sendSMTPCommand(g_lSmtpSocket, g_cMessage, NULL, NULL);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_MESSAGE;
                        break;
                }                 

                // End Message
                lRetVal = _sendSMTPCommand(g_lSmtpSocket,(char *)cSmtpDataEnd,NULL,
                                                 g_cBuf);
                if(lRetVal < 0)
                {
                        g_uiSmtpStatus= smtpERROR;
                        g_uiEmailErrorInfo = SL_EMAIL_ERROR_MESSAGE;
                        break;
                }                 

                // Server will send 250 is successful. Move into QUIT state.
                if(g_cBuf[0] == cSmtpCodeOkReply[0] &&
                         g_cBuf[1] == cSmtpCodeOkReply[1] &&
                         g_cBuf[2] == cSmtpCodeOkReply[2])
                {
                        // Disconnect from server by sending QUIT command
                    lRetVal = sl_Send(g_lSmtpSocket,cSmtpQuit,strlen(cSmtpQuit),0);
                    if(lRetVal < 0)
                    {
                            g_uiSmtpStatus= smtpERROR;
                            g_uiEmailErrorInfo = SL_EMAIL_ERROR_MESSAGE;
                            break;
                    }                                         
                    // Close socket and reset
                    sl_Close(g_lSmtpSocket);					
                    g_lSmtpSocket = 0xFFFFFFFF;
                    
                    //Reset the state machine
                    g_uiSmtpStatus= smtpINIT;
                    
                    exit = 1;
                    return g_uiEmailErrorInfo;
                }
                else
                {
                    g_uiSmtpStatus= smtpERROR;
                    g_uiEmailErrorInfo = SL_EMAIL_ERROR_MESSAGE;
                }	
                
                break;
                    
            // Error Handling for SMTP
            case smtpERROR:
                // Close socket and reset
                sl_Close(g_lSmtpSocket);
                
                //Reset the state machine
                g_uiSmtpStatus= smtpINIT;
                
                exit = 1;
                return g_uiEmailErrorInfo;
              
            default:
                    // 	TODO: Remove
            break;
    }
    }
    return SUCCESS;
}


//****************************************************************************
//
//! GenerateBase64Key
//!
//! @brief  Generates Base64 Key needed for authentication
//! 
//! @param  input is the string to be converted to base64
//! @param  basekey1 is the base64 converted output
//!
//! \return 0 on success else error code
//!
//
//****************************************************************************
long _generateBase64Key(char* input, char* basekey1)
{
    // In case the input string needs to be modified before conversion, define
    //  new string to pass-through Use InputStr and *pIn
    int InputLen = strlen(input);
    char InputStr[128];
    char *pIn = (char *)InputStr;
	long lRetVal = -1;

	if(!input || !basekey1)
	{
		return -1;
	}

    //Generate Base64 string, right now is only the function input parameter
    memcpy(pIn, input, InputLen);	
    pIn += InputLen;

    // to64frombits function
    lRetVal = ConvertToBase64(basekey1, (void *)InputStr, InputLen);
	ASSERT_ON_ERROR(lRetVal);
	return SUCCESS;
  
}

//****************************************************************************
//
//!	sendSMTPCommand 
//!
//! @brief  Sends an SMTP command and provides the server response.
//! 		If cmd or cmd param are NULL, it only ends up sending a <CR><LF>
//! 		If respBuf is NULL, it will not attempt to receive response
//!
//! @param 	socket is the socket descriptor to be used for sending
//! @param 	cmd is the string of the command
//! @param 	cmdpara is the command parameter
//! @param 	respBuf is a pointer to the SMTP response from the server
//!
//! @return 0 on success else error code
//
//****************************************************************************
long _sendSMTPCommand(long socket, char* cmd, char* cmdparam, char* respBuf)
{
	
	long lRetVal = -1;  
    int sendLen = 0;
    memset(g_cCmdBuf,0,sizeof(g_cCmdBuf));

    if(cmd != NULL)
    {        
        sendLen = strlen(cmd);
        memcpy(g_cCmdBuf,cmd,strlen(cmd));
    }

    if(cmdparam != NULL)
    {        
        memcpy(&g_cCmdBuf[sendLen],cmdparam, strlen(cmdparam));
        sendLen += strlen(cmdparam);
    }

    memcpy(&g_cCmdBuf[sendLen],cSmtpCrlf,strlen(cSmtpCrlf));
    sendLen += strlen(cSmtpCrlf);
    
    lRetVal = sl_Send(socket, g_cCmdBuf,sendLen,0);
    ASSERT_ON_ERROR(lRetVal);


    if(respBuf != NULL)
    {
        memset(respBuf,0,SMTP_BUF_LEN);
        lRetVal = sl_Recv(socket, respBuf,SMTP_BUF_LEN,0);
        ASSERT_ON_ERROR(lRetVal);
    }
	return SUCCESS;
}
    


#ifdef  __cplusplus
}
#endif // __cplusplus




