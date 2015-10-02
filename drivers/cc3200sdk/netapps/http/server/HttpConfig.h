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

#ifndef _HTTP_CONFIG_H_
#define _HTTP_CONFIG_H_

/**
 * @defgroup HttpConfig HTTP Server compile-time configuration options
 * This header file contains various compile-time options for the server
 *
 * @{
 */

/// Use HttpStatic module for static content
#define HTTP_CORE_ENABLE_STATIC
/// Use HttpDynamic module for dynamic content
#define HTTP_CORE_ENABLE_DYNAMIC
/// Use HttpAuth for HTTP digest-access authentication
//#define HTTP_CORE_ENABLE_AUTH
/// Use HttpDebug module to generate debug traces
#define HTTP_CORE_ENABLE_DEBUG
/// Use WebSocket module for using ws connection
#define HTTP_CORE_ENABLE_WS


/// @}

#endif //_HTTP_CONFIG_H_
