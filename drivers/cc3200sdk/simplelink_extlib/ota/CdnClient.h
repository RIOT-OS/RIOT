/*
 * CdnClient.h - Header file for CDN Client APIs and MACROS
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
#ifndef __CDN_CLIENT_H__
#define __CDN_CLIENT_H__

#ifdef    __cplusplus
extern "C" {
#endif

#define ALIGNMENT_READ_MASK 0x0F

/* RunStatus */
#define CDN_STATUS_DOWNLOAD_DONE        1
#define CDN_STATUS_OK                   0
#define CDN_STATUS_CONTINUE             0
#define CDN_STATUS_ERROR                -1
#define CDN_STATUS_ERROR_OPEN_FILE      -2
#define CDN_STATUS_ERROR_RECV_CHUNK     -3
#define CDN_STATUS_ERROR_SAVE_CHUNK     -4
#define CDN_STATUS_ERROR_MAX_EAGAIN     -5
#define CDN_STATUS_ERROR_CONNECT_CDN    -6
#define CDN_STATUS_ERROR_READ_HDRS      -7

typedef enum
{
    CDN_STATE_IDLE = 0,
    CDN_STATE_CDN_SERVER_CONNECTED,
    CDN_STATE_FILE_DOWNLOAD_AND_SAVE,
    CDN_STATE_CLOSED
} CdnState_e;

/* locals - move to object control block */
typedef struct
{
    _u8 cdn_server_name[64]; /* save server name in order not to connect every file (if same server) */
    _i32  port_num;

    _u8 *p_file_name;
    _i32 file_size;
    _u32 ulToken;
    _i32 lFileHandle;
    _i16 fileSockId;
    _u8 *recv_buf;

    /* states */
    CdnState_e  state;
    _i32 totalBytesReceived;
    OtaFileMetadata_t *pResourceMetadata;
    FlcCb_t    *pFlcSflashCb;
    FlcCb_t    *pFlcHostCb;
    FlcCb_t    *pFlcCb;
} CdnClient_t;

void *CdnClient_Init(FlcCb_t *pFlcSflashCb, FlcCb_t *pFlcHostCb);
_i32  CdnClient_ConnectByUrl(void *pvCdnClient, OtaFileMetadata_t *pResourceMetadata, _i32 secured_connection);
_i32  CdnClient_SetFileInfo(void *pvCdnClient, _i32 file_size, _u8 *file_name);
_i32  CdnClient_Run(void *pvCdnClient);
void CdnClient_CloseServer(void *pvCdnClient);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __CDN_CLIENT_H__ */
