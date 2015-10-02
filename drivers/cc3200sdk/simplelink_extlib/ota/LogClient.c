/*
 * LogClient.c - Logger Implementation for OTA
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
#include "simplelink.h"
#include "OtaCommon.h"
#include "ota_api.h"
#include "OtaApp.h"
#include "OtaHttp.h"
#include "LogClient.h"

/* Internals */
LogClient_t g_LogClient;

void *LogClient_Init(void)
{
    LogClient_t *pLogClient = &g_LogClient;

    memset(pLogClient, 0, sizeof(LogClient_t));
    pLogClient->serverSockId = -1;

    return (void *)pLogClient;
}

_i32 LogClient_ConnectServer(void *pvLogClient, OtaOptServerInfo_t *pOtaServerInfo)
{
    LogClient_t *pLogClient = (LogClient_t *)pvLogClient;

    pLogClient->pOtaServerInfo = pOtaServerInfo;
    /* Connect to the OTA server */
    Report("LogClient_ConnectServer: http_connect_server %s\r\n", pOtaServerInfo->log_server_name);
    pLogClient->serverSockId = http_connect_server(pOtaServerInfo->log_server_name, 0, SOCKET_PORT_DEFAULT, SOCKET_SECURED, SOCKET_BLOCKING);
    if (pLogClient->serverSockId < 0)
    {
        Report("LogClient_ConnectServer: ERROR http_connect_server, status=%d\r\n", pLogClient->serverSockId);
        if (pLogClient->serverSockId == OTA_STATUS_ERROR_CONTINUOUS_ACCESS)
        {
            return OTA_STATUS_ERROR_CONTINUOUS_ACCESS;
        }
        return OTA_STATUS_ERROR;
    }

    return OTA_STATUS_OK;
}

_i32 LogClient_Print(void *pvLogClient, _u8 *pFilename, OtaApp_statistics_t *pStatistics)
{
    LogClient_t *pLogClient = (LogClient_t *)pvLogClient;
#ifdef DEBUG_PRINT
    _i32 status=0;
#endif
    _i32 len;
    _u8 *send_buf = http_send_buf();
    _u8 *response_buf = http_recv_buf();
    _u8 local_buf[96];
    _u8 *log_filename = local_buf;
    _u8 *log_buf = http_recv_buf();

    strcpy((char *)log_filename, "/Log/Stat_");
    strcat((char *)log_filename, (const char *)pFilename);
    strcat((char *)log_filename, ".txt");
    /*strcat(log_filename, "?overwrite=false"); */


    http_build_request (send_buf, "PUT ", pLogClient->pOtaServerInfo->log_server_name, pLogClient->pOtaServerInfo->rest_files_put, log_filename, pLogClient->pOtaServerInfo->rest_hdr, pLogClient->pOtaServerInfo->rest_hdr_val);

    /* prepare log buffer */
    sprintf((char *)log_buf, "start counter=%ld, access errors=%ld ---------------\n", pStatistics->startCount, pStatistics->continuousAccessErrorCount);
    sprintf((char *)&log_buf[strlen((const char *)log_buf)], "connect server done=%ld, error=%ld, update=%ld, metadata=%ld\n", pStatistics->ota_connect_done, pStatistics->ota_connect_error, pStatistics->ota_error_updatecheck, pStatistics->ota_error_metadata);
    sprintf((char *)&log_buf[strlen((const char *)log_buf)], "connect cdn    done=%ld, error=%ld\n", pStatistics->cdn_connect_done, pStatistics->cdn_connect_error);
    sprintf((char *)&log_buf[strlen((const char *)log_buf)], "download file  done=%ld, error=%ld, save_chunk=%ld, max_eagain=%ld\n", pStatistics->download_done, pStatistics->download_error, pStatistics->download_error_save_chunk, pStatistics->download_error_max_eagain);

    /* Add file length header */
    send_buf[strlen((const char *)send_buf)-2]=0;
    sprintf((char *)&send_buf[strlen((const char *)send_buf)], "Content-Length: %d", strlen((const char *)log_buf));
    /*strcat(send_buf, "Content-Length: 76"); */
    strcat((char *)send_buf, "\r\n");
    strcat((char *)send_buf, "\r\n");

    len = sl_Send(pLogClient->serverSockId, send_buf, (_i16)strlen((const char *)send_buf), 0);
    if (len <= 0)
    {
        Report("LogClient_UpdateCheck: ERROR metadata sl_Send status=%ld\r\n", len);
        return OTA_STATUS_ERROR;
    }

    len = sl_Send(pLogClient->serverSockId, log_buf, (_i16)strlen((const char *)log_buf), 0);
    if (len <= 0)
    {
        Report("LogClient_UpdateCheck: ERROR metadata sl_Send status=%ld\r\n", len);
        return OTA_STATUS_ERROR;
    }

    /* recv json */
    len = sl_Recv_eagain(pLogClient->serverSockId, response_buf, HTTP_RECV_BUF_LEN, 0, MAX_EAGAIN_RETRIES);
    if (len <= 0)
    {
            Report("LogClient_UpdateCheck: ERROR metadata sl_Recv status=%ld\r\n", status);
            return OTA_STATUS_ERROR;
    }

    return OTA_STATUS_OK;
}

void LogClient_Close(void *pvLogClient)
{
    LogClient_t *pLogClient = (LogClient_t *)pvLogClient;

    if (pLogClient->serverSockId >= 0)
    {
        sl_Close(pLogClient->serverSockId);
        pLogClient->serverSockId = -1;
    }
}

_i32 LogClient_ConnectAndPrint(void *pvLogClient, OtaOptServerInfo_t *pOtaServerInfo, _u8 *vendorStr, OtaApp_statistics_t *pStatistics)
{
    _i32 status;
    _u8 filename[40];
    _u8 *pMac = pOtaServerInfo->log_mac_address;


    status = LogClient_ConnectServer(pvLogClient, pOtaServerInfo);
    if( status < 0)
    {
        Report("LogClient_ConnectAndPrint: LogClient_ConnectServer, status=%ld\r\n", status);
        return status;
    }

    sprintf((char *)filename,"%s_Mac%02X%02X%02X%02X%02X%02X", vendorStr, pMac[0], pMac[1], pMac[2], pMac[3], pMac[4], pMac[5]);
    LogClient_Print(pvLogClient, filename, pStatistics);
    LogClient_Close(pvLogClient);
    return OTA_STATUS_OK;
}
