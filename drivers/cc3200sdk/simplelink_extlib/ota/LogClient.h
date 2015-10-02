/*
 * CdnClient.h - Header file for logger APIs
 *
 * Copyright (C) 2014 Texas Instruments Incorporated
 *
 * All rights reserved. Property of Texas Instruments Incorporated.
 * Restricted rights to use, duplicate or disclose this code are
 * granted through contract.
 * The program may not be used without the written permission of
 * Texas Instruments Incorporated or against the terms and conditions
 * stipulated in the agreement under which this program has been supplied,
 * and under no circumstances can it be used with non-TI connectivity device.
 *
*/
#ifndef __LOG_CLIENT_H__
#define __LOG_CLIENT_H__

#ifdef    __cplusplus
extern "C" {
#endif

/* locals - move to object control block */
typedef struct 
{
    OtaOptServerInfo_t       *pOtaServerInfo;
    _i16 serverSockId;
} LogClient_t;

void *LogClient_Init(void);
void LogClient_Close(void *pvLogClient);
_i32  LogClient_ConnectServer(void *pvLogClient, OtaOptServerInfo_t *pOtaServerInfo);
_i32  LogClient_Print(void *pvLogClient, _u8 *vendorStr, OtaApp_statistics_t *pStatistics);
_i32  LogClient_ConnectAndPrint(void *pvLogClient, OtaOptServerInfo_t *pOtaServerInfo, _u8 *vendorStr, OtaApp_statistics_t *pStatistics);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __LOG_CLIENT_H__ */
