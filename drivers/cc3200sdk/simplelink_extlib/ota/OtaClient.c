/*
 * OtaClient.c - OTA Client implementation
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
#include "OtaHttp.h"
#include "OtaClient.h"

OtaClient_t g_OtaClient;
OtaFileMetadata_t        g_OtaFileMetadata;
OtaCheckUpdateResponse_t g_OtaCheckUpdateResponse;
_i32 OtaClient_ResourceNameConvert(void *pvOtaClient, _u8 *resource_file_name, OtaFileMetadata_t *pMetadata);

void *OtaClient_Init(FlcCb_t *pFlcSflashCb, FlcCb_t *pFlcHostCb)
{
    OtaClient_t *pOtaClient = (OtaClient_t *)&g_OtaClient;

    memset(pOtaClient, 0, sizeof(OtaClient_t));
    pOtaClient->serverSockId = -1;
    pOtaClient->pFlcSflashCb = pFlcSflashCb;
    pOtaClient->pFlcHostCb = pFlcHostCb;
    pOtaClient->pOtaMetadataResponse = &g_OtaFileMetadata;
    pOtaClient->pOtaCheckUpdateResponse = &g_OtaCheckUpdateResponse;

    return (void *)pOtaClient;
}

_i32 OtaClient_ConnectServer(void *pvOtaClient, OtaOptServerInfo_t *pOtaServerInfo)
{
    OtaClient_t *pOtaClient = (OtaClient_t *)pvOtaClient;

    pOtaClient->currUpdateIndex = 0;
    pOtaClient->pOtaServerInfo = pOtaServerInfo;

    /* Connect to the OTA server */
    Report("OtaClient_ConnectServer: http_connect_server %s\r\n", pOtaServerInfo->server_domain);
    pOtaClient->serverSockId = http_connect_server(pOtaServerInfo->server_domain, pOtaServerInfo->ip_address, SOCKET_PORT_DEFAULT, pOtaServerInfo->secured_connection, SOCKET_BLOCKING);
    if (pOtaClient->serverSockId < 0)
    {
        Report("OtaClient_ConnectServer: ERROR http_connect_server, status=%d\r\n", pOtaClient->serverSockId);
        if (pOtaClient->serverSockId == OTA_STATUS_ERROR_CONTINUOUS_ACCESS)
        {
            return OTA_STATUS_ERROR_CONTINUOUS_ACCESS;
        }
        return OTA_STATUS_ERROR;
    }

    return OTA_STATUS_OK;
}

_i32 OtaClient_UpdateCheck(void *pvOtaClient, _u8 *pVendorStr)
{
    OtaClient_t *pOtaClient = (OtaClient_t *)pvOtaClient;
    OtaOptServerInfo_t *pOtaServerInfo = pOtaClient->pOtaServerInfo;
    _i32 status=0;
    _i32 numUpdates;
    _i32 len;
    _u8 *send_buf = http_send_buf();
    _u8 *response_buf = http_recv_buf();

    pOtaClient->pVendorStr = pVendorStr;
    Report("OtaClient_UpdateCheck: call http_build_request %s\r\n", pOtaServerInfo->rest_update_chk);

#ifdef TI_OTA_SERVER
    http_build_request (send_buf, "GET ", pOtaServerInfo->server_domain, pOtaServerInfo->rest_update_chk, NULL, NULL, NULL);
#else
    http_build_request (send_buf, "GET ", pOtaServerInfo->server_domain, pOtaServerInfo->rest_update_chk , pOtaClient->pVendorStr, pOtaServerInfo->rest_hdr, pOtaServerInfo->rest_hdr_val);
#endif

    len = sl_Send(pOtaClient->serverSockId, send_buf, (_i16)strlen((const char *)send_buf), 0);
    if (len <= 0)
    {
        Report("OtaClient_UpdateCheck: ERROR metadata sl_Send status=%ld\r\n", len);
        return OTA_STATUS_ERROR;
    }

    len = sl_Recv_eagain(pOtaClient->serverSockId, response_buf, HTTP_RECV_BUF_LEN, 0, MAX_EAGAIN_RETRIES);
    if (len <= 0)
    {
        Report("OtaClient_UpdateCheck: ERROR metadata sl_Recv status=%ld\r\n", len);
        return OTA_STATUS_ERROR;
    }

    while (len < HTTP_HEADER_SIZE)
    {
        status = sl_Recv_eagain(pOtaClient->serverSockId, &response_buf[len], HTTP_RECV_BUF_LEN, 0, MAX_EAGAIN_RETRIES);
        if (status <= 0)
        {
            Report("OtaClient_UpdateCheck: ERROR metadata sl_Recv status=%ld\r\n", status);
            return OTA_STATUS_ERROR;
        }
        len += status;
    }

#ifdef TI_OTA_SERVER
    numUpdates = json_parse_update_check_resp(pOtaClient->serverSockId, pOtaClient->pOtaCheckUpdateResponse->rsrcList, response_buf, len);
#else
    numUpdates = json_parse_dropbox_metadata(pOtaClient->serverSockId, pOtaClient->pOtaCheckUpdateResponse->rsrcList, response_buf, len);
#endif

    pOtaClient->currUpdateIndex = 0;
    pOtaClient->numUpdates = numUpdates;

    return numUpdates;
}

_u8 *OtaClient_GetNextUpdate(void *pvOtaClient, _i32 *size)
{
    OtaClient_t *pOtaClient = (OtaClient_t *)pvOtaClient;
    if (pOtaClient->currUpdateIndex >= pOtaClient->numUpdates)
    {
        /* no more files to update */
        return NULL;
    }
    *size = pOtaClient->pOtaCheckUpdateResponse->rsrcList[pOtaClient->currUpdateIndex].size;
    return pOtaClient->pOtaCheckUpdateResponse->rsrcList[pOtaClient->currUpdateIndex++].filename;
}

/*
    file name format
    ----------------
    faa_sys_filename.ext
        f     file prefix
        aa    file flags bitmap
                01 - the file is secured - METADATA_FLAGS_SECURED
                02 - the file is secured with signature   - METADATA_FLAGS_SIGNATURE, file name must be filename.sig !!!
                04 - the file is secured with certificate - METADATA_FLAGS_CERTIFICATE, file name must be filename.cer !!!
                08 - don't convert _sys_ into /sys/ for SFLASH file name - METADATA_FLAGS_NOT_CONVERT_SYS_DIR
                10 - use external storage instead of SFLASH - METADATA_FLAGS_NOT_SFLASH_STORAGE
                20 - MCU bootloader should use user1img.bin file - METADATA_FLAGS_USE_MCU_1
                40 - MCU bootloader should use user2img.bin file - METADATA_FLAGS_USE_MCU_2
                80 - MCU should be reset after this download - METADATA_FLAGS_RESET_MCU
        sys   optional for /sys/ directory
        ext
              signature file - must be filename.sig, filename is the name of the secured file
              certificate file  - must be filename.cer, filename is the name of the secured file
*/
_i32 OtaClient_ResourceMetadata(void *pvOtaClient, _u8 *resource_file_name, OtaFileMetadata_t **paramResourceMetadata)
{
    OtaClient_t *pOtaClient = (OtaClient_t *)pvOtaClient;
    OtaOptServerInfo_t *pOtaServerInfo = pOtaClient->pOtaServerInfo;
    OtaFileMetadata_t *pMetadata = pOtaClient->pOtaMetadataResponse;
    _i32 status=0;
    _i32 len;
    _u8 *send_buf = http_send_buf();
    _u8 *response_buf = http_recv_buf();

    memset(pMetadata, 0, sizeof(OtaFileMetadata_t));
    *paramResourceMetadata = pMetadata;
    Report("OtaClient_ResourceMetadata: call http_build_request %s\r\n", pOtaServerInfo->rest_rsrc_metadata);
    memset(response_buf,0,HTTP_RECV_BUF_LEN);

    /* first check and covert file name: faa_sys_filename.ext */
    status = OtaClient_ResourceNameConvert(pvOtaClient, resource_file_name, pMetadata);
    if (status < 0)
    {
        Report("OtaClient_ResourceMetadata: Error on OtaClient_ResourceNameConvert, status=%ld\r\n", status);
        return OTA_STATUS_ERROR;
    }

#ifdef TI_OTA_SERVER
    http_build_request (send_buf, "GET ",  pOtaServerInfo->server_domain, pOtaServerInfo->rest_rsrc_metadata, NULL,             , pOtaServerInfo->rest_hdr, pOtaServerInfo->rest_hdr_val);
#else
    http_build_request (send_buf, "POST ", pOtaServerInfo->server_domain, pOtaServerInfo->rest_rsrc_metadata, resource_file_name, pOtaServerInfo->rest_hdr, pOtaServerInfo->rest_hdr_val);
#endif

    len = sl_Send(pOtaClient->serverSockId, send_buf, (_i16)strlen((const char *)send_buf), 0);
    if (len <= 0)
    {
        Report("OtaClient_ResourceMetadata: Error media sl_Send status=%ld\r\n", len);
        return OTA_STATUS_ERROR;
    }

    len = sl_Recv_eagain(pOtaClient->serverSockId, response_buf, HTTP_RECV_BUF_LEN, 0, MAX_EAGAIN_RETRIES);
    if (len <= 0)
    {
        Report("OtaClient_ResourceMetadata: Error media sl_Recv_eagain status=%ld\r\n", len);
        return OTA_STATUS_ERROR;
    }

    while (len < HTTP_HEADER_SIZE)
    {
        status = sl_Recv_eagain(pOtaClient->serverSockId, &response_buf[len], HTTP_RECV_BUF_LEN, 0, MAX_EAGAIN_RETRIES);
        if (status <= 0)
        {
            Report("OtaClient_ResourceMetadata: ERROR metadata sl_Recv status=%ld\r\n", status);
            return OTA_STATUS_ERROR;
        }
        len += status;
    }

#ifdef TI_OTA_SERVER
    status = json_parse_rsrc_metadata_url(response_buf, pMetadata->cdn_url);
#else
    status = json_parse_dropbox_media_url(response_buf, pMetadata->cdn_url);
#endif
    if (status)
    {
        Report("OtaClient_ResourceMetadata: Error media json_parse_media status=%ld\r\n", status);
        return OTA_STATUS_ERROR;
    }

    return OTA_STATUS_OK;
}

_i32 OtaClient_ResourceNameConvert(void *pvOtaClient, _u8 *resource_file_name, OtaFileMetadata_t *pMetadata)
{
    OtaClient_t *pOtaClient = (OtaClient_t *)pvOtaClient;
    _i32 file_flags;

    pMetadata->p_file_name = &pMetadata->rsrc_file_name[10]; /* space for prefix */
    strcpy((char *)pMetadata->p_file_name, (const char *)resource_file_name);

    /* Add metadata info (remove it after server complete) */
    pMetadata->p_cert_filename = NULL;
    pMetadata->p_signature = NULL;
    pMetadata->flags = 0;

    /* skip vendor string - as directory in the file name */
    if (pOtaClient->pVendorStr)
    {
        pMetadata->p_file_name = &pMetadata->p_file_name[strlen((const char *)pOtaClient->pVendorStr)+1];  /* skip "/Vid00_Pid00_Ver00" */
    }

    if (pMetadata->p_file_name[1] != 'f') /* must start with 'f' flags */
    {
        Report("OtaClient_ResourceMetadata: ignore file name: %s, without f prefix\r\n", pMetadata->p_file_name);
        return OTA_STATUS_ERROR;
    }

    /* extract file flags */
    file_flags = 0;
    if ((pMetadata->p_file_name[2] >= 'a') && (pMetadata->p_file_name[2] <= 'f'))
        file_flags |= (0xa + (pMetadata->p_file_name[2] - 'a')) << 4;
    if ((pMetadata->p_file_name[2] >= '0') && (pMetadata->p_file_name[2] <= '9'))
        file_flags |= (0x0 + (pMetadata->p_file_name[2] - '0')) << 4;
    if ((pMetadata->p_file_name[3] >= 'a') && (pMetadata->p_file_name[2] <= 'f'))
        file_flags |= 0xa + (pMetadata->p_file_name[3] - 'a');
    if ((pMetadata->p_file_name[3] >= '0') && (pMetadata->p_file_name[2] <= '9'))
        file_flags |= 0x0 + (pMetadata->p_file_name[3] - '0');
    /*file_flags = ((pMetadata->p_file_name[2] - '0') << 4) + (pMetadata->p_file_name[3] - '0'); */
    if (file_flags & 0x01) pMetadata->flags |= METADATA_FLAGS_SECURED;
    if (file_flags & 0x02) pMetadata->flags |= METADATA_FLAGS_SIGNATURE;
    if (file_flags & 0x04) pMetadata->flags |= METADATA_FLAGS_CERTIFICATE;
    if (file_flags & 0x08) pMetadata->flags |= METADATA_FLAGS_NOT_CONVERT_SYS_DIR;
    if (file_flags & 0x10) pMetadata->flags |= METADATA_FLAGS_NOT_SFLASH_STORAGE;
    if (file_flags & 0x20) pMetadata->flags |= METADATA_FLAGS_RESEREVED_1;
    if (file_flags & 0x40) pMetadata->flags |= METADATA_FLAGS_RESET_NWP;
    if (file_flags & 0x80) pMetadata->flags |= METADATA_FLAGS_RESET_MCU;
    Report("OtaClient_ResourceMetadata: file flags=%x,", file_flags);
	Report("metadata flags=%x\r\n", pMetadata->flags);
    /* skip file flags */
    pMetadata->p_file_name += 4;        /* skip "/f00" of /f00_sys_file.ext" */
    pMetadata->p_file_name[0] = '/';    /* convert "_sys" to "/sys_file.ext" */

    /* convert _sys_ to /sys/ */
    if ((pMetadata->flags & METADATA_FLAGS_NOT_CONVERT_SYS_DIR) == 0)
    {
        pMetadata->p_file_name[4] = '/';    /* convert "/sys_file.ext" to "/sys/file.ext" */
    }
    else
    {
        pMetadata->p_file_name += 1;        // skip "/"
    }

    if (pMetadata->flags & METADATA_FLAGS_NOT_SFLASH_STORAGE)
    {
        if (pOtaClient->pFlcHostCb == NULL)
        {
            Report("OtaClient_ResourceMetadata: METADATA_FLAGS_NOT_SFLASH_STORAGE is set but no host storage function installed!!!!\r\n");
            return OTA_STATUS_ERROR;
        }
    }

    /* set signature file */
    if (pMetadata->flags & METADATA_FLAGS_SIGNATURE)
    {
        Report("OtaClient_ResourceMetadata: file=%s is secured with signature\r\n", pMetadata->p_file_name);
        /* signature will be extracted from "file.sig" file */
        pMetadata->flags |= METADATA_FLAGS_SIGNATURE;
        strcpy((char *)pMetadata->signature_filename, (const char *)pMetadata->p_file_name);
        strcpy((char *)&pMetadata->signature_filename[strlen((const char *)pMetadata->signature_filename)-4], ".sig");
    }

    /* set certificate file */
    if (pMetadata->flags & METADATA_FLAGS_CERTIFICATE)
    {
        Report("OtaClient_ResourceMetadata: file=%s is secured with certificate\r\n", pMetadata->p_file_name);
        /* signature will be extracted from "file.cer" file */
        strcpy((char *)pMetadata->cert2_filename, (const char *)pMetadata->p_file_name);
        strcpy((char *)&pMetadata->cert2_filename[strlen((const char *)pMetadata->signature_filename)-4], ".cer");
        pMetadata->p_cert_filename = pMetadata->cert2_filename;
    }

    if (strstr((const char *)pMetadata->p_file_name, ".sig") != NULL)
    {
        Report("OtaClient_ResourceMetadata: remove old signature file %s\r\n", pMetadata->p_file_name);
        /* remove old sig file, must download new file */
        sl_FsDel((_u8 *)pMetadata->p_file_name, (_u32)0);
    }

    return OTA_STATUS_OK;
}

void OtaClient_CloseServer(void *pvOtaClient)
{
    OtaClient_t *pOtaClient = (OtaClient_t *)pvOtaClient;

    if (pOtaClient->serverSockId >= 0)
    {
        sl_Close(pOtaClient->serverSockId);
        pOtaClient->serverSockId = -1;
    }
}
