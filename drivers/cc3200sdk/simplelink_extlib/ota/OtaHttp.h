/*
 * OtaHttp.h - Header file for OtaHttp.c
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
#ifndef __OTA_HTTP_H__
#define __OTA_HTTP_H__

#ifdef    __cplusplus
extern "C" {
#endif

#include "OtaClient.h"

#define HTTP_MAX_RETRIES          4000
#define HTTP_HEADER_STR           "HTTP/1.1 200"
#define HTTP_HEADER_SIZE          12     /* HTTP/x.x YYY */
#define HTTP_HEADER_OK            "200"  /* OK Code      */

#define SOCKET_SECURED            1
#define SOCKET_NON_SECURED        0
#define SOCKET_BLOCKING           0
#define SOCKET_NON_BLOCKING       1
#define SOCKET_PORT_DEFAULT       0
#define SOCKET_PORT_HTTP          80
#define SOCKET_PORT_HTTPS         443

/* service buffers */
#define HTTP_SEND_BUF_LEN   512  
#define HTTP_RECV_BUF_LEN  1440  
_u8 *http_send_buf(void);
_u8 *http_recv_buf(void);

/* HTTP */
_i32  http_extract_domain_by_url(_u8 *url_name, _u8 *domain_name, _u8 *req_uri);
_i32  http_skip_headers(_i16 sockId);
_i32  http_build_request(_u8 *pHttpReq, _u8 *method, _u8 *host_name, _u8 *req_uri_prefix, _u8 *req_uri, _u8 *hdr_name, _u8 *hdr_val);
_i16  http_connect_server(_u8 *ServerName, _i32 IpAddr, _i32 port, _i32 secured, _i32 nonBlocking);
/* JSON */
_i32  json_parse_rsrc_metadata_url(_u8 *media_response_buf, _u8 *media_url);
_i32  json_parse_update_check_resp(_i16 sockId, RsrcData_t *pRsrcData, _u8 *read_buf, _i32 size);

/* Services */
_i32  sl_Recv_eagain(_i16 sockId, void *pBuf, _i32 Len, _i32 flags, _i32 max_eagain);

/* Dropbox only */
_i32  json_parse_dropbox_metadata(_i16 sockId, RsrcData_t *pRsrcData, _u8 *read_buf, _i32 size);
_i32  json_parse_dropbox_media_url(_u8 *media_response_buf, _u8 *media_url);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __OTA_HTTP_H__ */
