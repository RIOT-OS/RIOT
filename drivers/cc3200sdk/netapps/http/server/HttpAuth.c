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

#include "HttpAuth.h"

#ifdef HTTP_CORE_ENABLE_AUTH

#include <string.h>
#include "HttpString.h"
/**
 * @addtogroup HttpAuth
 * @{
 */

#define DIGEST_AUTHENTICATION_BUFFER_SIZE (32)

/**
 * This structure holds the HTTP digest-access authentication state
 */
struct HttpAuthState
{
	/// Last-generated nonce
    UINT8 nonce[DIGEST_AUTHENTICATION_BUFFER_SIZE];
    /// Last-generated opaque
    UINT8 opaque[DIGEST_AUTHENTICATION_BUFFER_SIZE];
    /// The hash of the username, realm, and password
    UINT8 ha1[DIGEST_AUTHENTICATION_BUFFER_SIZE];
};



/// The global state for digest-access authentication

static struct HttpAuthState g_authState; // = { {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

///
MD5_CTX MD5state;

char    HTTP_AUTH_REALM[]                     =  "cc3200 web server";

/// Header strings to be used for response headers
char    HTTP_AUTHENTICATE_RESPONSE_REALM[]    =  "WWW-Authenticate: Digest realm=\"";
char    HTTP_AUTHENTICATE_RESPONSE_NONCE[]    =  "\",qop=\"auth\",nonce=\"";
char    HTTP_AUTHENTICATE_RESPONSE_OPAQUE[]   =  "\",opaque=\"";
char    HTTP_AUTHENTICATE_RESPONSE_EOH[]      =  "\"\r\n";

/// Authenticate header tokens
char    HTTP_AUTHENTICATE_REALM[]      =  "realm";
char    HTTP_AUTHENTICATE_QOP[]        =  "qop";
char    HTTP_AUTHENTICATE_AUTH[]       =  "auth";
char    HTTP_AUTHENTICATE_NONCE[]      =  "nonce";
char    HTTP_AUTHENTICATE_OPAQUE[]     =  "opaque";
char    HTTP_AUTHENTICATE_DIGEST[]     =  "digest";
char    HTTP_AUTHENTICATE_URI[]        =  "uri";
char    HTTP_AUTHENTICATE_NC[]         =  "nc=";
char    HTTP_AUTHENTICATE_CNONCE[]     =  "cnonce";
char    HTTP_AUTHENTICATE_RESPONSE[]   =  "response";
char    HTTP_AUTHENTICATE_USERNAME[]   =  "username";
char    HTTP_DELIMITER_QUOTE[]         =  "\"";
char    HTTP_DELIMITER_COMMA[]         =  ",";


/// The length of the response authentication header
/// The two 32 numbers represent nonce and opaque strings and the -5 is to compensate for the sizeof() calls
UINT16  HTTP_AUTHENTICATE_RESPONSE_HEADER_LENGTH =
                sizeof(HTTP_AUTHENTICATE_RESPONSE_REALM) +
                sizeof(HTTP_AUTHENTICATE_RESPONSE_NONCE) +
                sizeof(HTTP_AUTHENTICATE_RESPONSE_OPAQUE) +
                sizeof(HTTP_AUTHENTICATE_RESPONSE_EOH) +
                sizeof(HTTP_AUTH_REALM) +
                DIGEST_AUTHENTICATION_BUFFER_SIZE + DIGEST_AUTHENTICATION_BUFFER_SIZE - 5;


/**
 * Simple random generator
 * To improve randomness the initial seed has to be dynamic
 */
static UINT32 GetRandomUint()
{
    // set static seed
    static UINT32 m_z = 1234;
    static UINT32 m_w = 98877;

    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;
}

static void MD5_FinalToString(UINT8* str, MD5_CTX *md5stat)
{
    UINT8 tmp[16];
    UINT8 i;
    struct HttpBlob location;
    location.uLength = 2;
    location.pData = str;
    MD5_Final(tmp, md5stat);
    for (i=0; i<16; i++, location.pData += 2)
        HttpString_htoa(tmp[i], &location, 1);
}


/**
 * This function will generate random 16 bytes to be used for Nonce and opaque strings
 */
static void Generate32BytesRandomString(UINT8 *str)
{
    struct HttpBlob str1;
    str1.uLength = 8;
    str1.pData = str;
    HttpString_htoa(GetRandomUint(), &str1, 1);
    str1.pData = str + 8;
    HttpString_htoa(GetRandomUint(), &str1, 1);
    str1.pData = str + 16;
    HttpString_htoa(GetRandomUint(), &str1, 1);
    str1.pData = str + 24;
    HttpString_htoa(GetRandomUint(), &str1, 1);
}

void HttpAuth_Init(struct HttpBlob username, struct HttpBlob password)
{
    MD5_Init(&MD5state);
    MD5_Update(&MD5state, username.pData, username.uLength);
    MD5_Update(&MD5state, ":", 1);
    MD5_Update(&MD5state, HTTP_AUTH_REALM, sizeof(HTTP_AUTH_REALM)-1);
    MD5_Update(&MD5state, ":", 1);
    MD5_Update(&MD5state, password.pData, password.uLength);
    MD5_FinalToString(g_authState.ha1, &MD5state);
}

static void AddStringToBlob(struct HttpBlob * trgt, char *str, UINT16 length)
{
    memcpy(trgt->pData + trgt->uLength, str, length);
    trgt->uLength += length;
}

void HttpAuth_ResponseAuthenticate(struct HttpRequest* pRequest, struct HttpBlob* pWWWAuthenticate)
{
    struct HttpBlob headerBlob;

    if (pWWWAuthenticate->uLength < HTTP_AUTHENTICATE_RESPONSE_HEADER_LENGTH)
    {
        pWWWAuthenticate->uLength = 0;
        return;
    }
    // There is enough space to add the authenticate header

    headerBlob.pData = pWWWAuthenticate->pData;
    headerBlob.uLength = 0;

    // Generate new Nonce and opaque
    Generate32BytesRandomString(g_authState.nonce);
    Generate32BytesRandomString(g_authState.opaque);

    // Build response header
    AddStringToBlob(&headerBlob, HTTP_AUTHENTICATE_RESPONSE_REALM, sizeof(HTTP_AUTHENTICATE_RESPONSE_REALM)-1);
    AddStringToBlob(&headerBlob, HTTP_AUTH_REALM, sizeof(HTTP_AUTH_REALM) -1);
    AddStringToBlob(&headerBlob, HTTP_AUTHENTICATE_RESPONSE_NONCE, sizeof(HTTP_AUTHENTICATE_RESPONSE_NONCE) -1);
    AddStringToBlob(&headerBlob, (char*)g_authState.nonce, sizeof(g_authState.nonce));
    AddStringToBlob(&headerBlob, HTTP_AUTHENTICATE_RESPONSE_OPAQUE, sizeof(HTTP_AUTHENTICATE_RESPONSE_OPAQUE) -1);
    AddStringToBlob(&headerBlob, (char*)g_authState.opaque, sizeof(g_authState.opaque));
    AddStringToBlob(&headerBlob, HTTP_AUTHENTICATE_RESPONSE_EOH, sizeof(HTTP_AUTHENTICATE_RESPONSE_EOH) - 1);

    pWWWAuthenticate->uLength = headerBlob.uLength;
}

/**
 * Find/verify name value pair in the input blob
 *
 * After return the location is stays in the same place since the order of the name value pairs in not constant
 * Returns:     if header not found return 0
 *              if value token is NULL - return pointer to start of value length of value till the \" delimiter
 *              if value token is not NULL - return 1 if values match
 */
static UINT16 HttpAuth_VerifyHeaderNameValue(struct HttpBlob *location, char* nameToken, UINT8 tokenlenLen, char *value, UINT8 valuelen, char** outValue)
{
    UINT8 * found;
    struct HttpBlob originalLocation;
    originalLocation.uLength = location->uLength;
    originalLocation.pData = location->pData;

    found = HttpString_nextToken(nameToken, tokenlenLen, *location);

    // Missing header name
    if (found == 0)
        return 0;
    else
    {
        location->uLength = originalLocation.uLength - (UINT16)(found - originalLocation.pData) - (tokenlenLen + 2);
        location->pData = found + tokenlenLen + 2;
        // Return the value pointer and size
        if (value == NULL)
        {
            *outValue= (char*)location->pData;
            nameToken = (char *)location->pData;
            found = HttpString_nextToken(HTTP_DELIMITER_QUOTE, sizeof(HTTP_DELIMITER_QUOTE) - 1, *location);
            if (found==0)
                found = HttpString_nextToken(HTTP_DELIMITER_COMMA, sizeof(HTTP_DELIMITER_COMMA) - 1, *location);

            // Restore current location
            location->uLength = originalLocation.uLength;
            location->pData = originalLocation.pData;
            return (UINT16)((char *)found - nameToken);
        }

        found = HttpString_nextToken(value, valuelen, *location);
        // Value does not match - restore location
        if (found == 0)
        {
            location->uLength = originalLocation.uLength;
            location->pData = originalLocation.pData;
            return 0;
        }

        // Restore location
        location->uLength = originalLocation.uLength;
        location->pData = originalLocation.pData;
        return 1;
    }
}


void HttpAuth_RequestAuthenticate(struct HttpRequest* pRequest, struct HttpBlob authorization)
{
    UINT8 ha2[DIGEST_AUTHENTICATION_BUFFER_SIZE], correctResponse[DIGEST_AUTHENTICATION_BUFFER_SIZE];
    struct HttpBlob currentLocation, blob;
    // HA1 was not copmuted
    if ((UINT32)*g_authState.ha1 == 0)
    {
        return;
    }

    // Parse the header - find relevant tokens and handle
    currentLocation.pData = authorization.pData;
    currentLocation.uLength = authorization.uLength;

    // Verify the mandatory tokens, whose content we ignore are present

    // verify we are in degest authentication method, any other is not supported
    if (HttpString_nextToken(HTTP_AUTHENTICATE_DIGEST, sizeof(HTTP_AUTHENTICATE_DIGEST)-1, currentLocation) == 0)
        return;

    // verify username exists
    if (HttpString_nextToken(HTTP_AUTHENTICATE_USERNAME, sizeof(HTTP_AUTHENTICATE_USERNAME)-1, currentLocation) == 0)
        return;

    // Verify realm
    if (HttpAuth_VerifyHeaderNameValue(&currentLocation, HTTP_AUTHENTICATE_REALM, sizeof(HTTP_AUTHENTICATE_REALM)-1, HTTP_AUTH_REALM, sizeof(HTTP_AUTH_REALM)-1, 0) != 1)
        return;

    // Verify correct nonce
    if (HttpAuth_VerifyHeaderNameValue(&currentLocation, HTTP_AUTHENTICATE_NONCE, sizeof(HTTP_AUTHENTICATE_NONCE)-1, (char *)g_authState.nonce, DIGEST_AUTHENTICATION_BUFFER_SIZE, 0) != 1)
        return;

    // Verify correct opaque
    if (HttpAuth_VerifyHeaderNameValue(&currentLocation, HTTP_AUTHENTICATE_OPAQUE, sizeof(HTTP_AUTHENTICATE_OPAQUE)-1, (char *)g_authState.opaque, DIGEST_AUTHENTICATION_BUFFER_SIZE, 0) != 1)
        return;

    // Find neccessary tokents and compute HA2 if some tokens are not found - return
    blob.pData = NULL;
    blob.uLength = HttpAuth_VerifyHeaderNameValue(&currentLocation, HTTP_AUTHENTICATE_URI, sizeof(HTTP_AUTHENTICATE_URI)-1, 0, 0, (char**)&blob.pData);
    // Uri is missing
    if (blob.uLength == 0)
        return;

    MD5_Init(&MD5state);
    if ((pRequest->uFlags & HTTP_REQUEST_FLAG_METHOD_POST) != 0)
        MD5_Update(&MD5state, "POST", 4);
    else
        MD5_Update(&MD5state, "GET", 3);
    MD5_Update(&MD5state, ":", 1);
    MD5_Update(&MD5state, blob.pData, blob.uLength);
    MD5_FinalToString(ha2, &MD5state);

    // Find tokens to compute correct response
    blob.pData = HttpString_nextToken(HTTP_AUTHENTICATE_NC, sizeof(HTTP_AUTHENTICATE_NC)-1, currentLocation);
    // Ncount is missing
    if (blob.pData == 0)
        return;
    blob.pData += sizeof(HTTP_AUTHENTICATE_NC) - 1;
    blob.uLength = 8;

    MD5_Init(&MD5state);
    MD5_Update(&MD5state, g_authState.ha1, DIGEST_AUTHENTICATION_BUFFER_SIZE);
    MD5_Update(&MD5state, ":", 1);
    MD5_Update(&MD5state, g_authState.nonce, DIGEST_AUTHENTICATION_BUFFER_SIZE);
    MD5_Update(&MD5state, ":", 1);
    MD5_Update(&MD5state, blob.pData, blob.uLength);

    blob.pData = NULL;
    blob.uLength = HttpAuth_VerifyHeaderNameValue(&currentLocation, HTTP_AUTHENTICATE_CNONCE, sizeof(HTTP_AUTHENTICATE_CNONCE)-1, 0, 0, (char **)&blob.pData);
    // Cnonce is missing
    if (blob.uLength == 0)
        return;
    MD5_Update(&MD5state, ":", 1);
    MD5_Update(&MD5state, blob.pData, blob.uLength);
    MD5_Update(&MD5state, ":auth:", 6);
    MD5_Update(&MD5state, ha2, DIGEST_AUTHENTICATION_BUFFER_SIZE);
    MD5_FinalToString(correctResponse, &MD5state);

    // Compare received response to the one computed locally - if equal then authorize
    blob.pData = NULL;
    blob.uLength = HttpAuth_VerifyHeaderNameValue(&currentLocation, HTTP_AUTHENTICATE_RESPONSE, sizeof(HTTP_AUTHENTICATE_RESPONSE)-1, 0, 0, (char **)&blob.pData);
    // Response is missing
    if (blob.uLength != DIGEST_AUTHENTICATION_BUFFER_SIZE)
        return;

    currentLocation.pData = correctResponse;
    currentLocation.uLength = DIGEST_AUTHENTICATION_BUFFER_SIZE;
    // if the responses are equal
    if (HttpString_strcmp(blob, currentLocation) == 0)
        pRequest->uFlags |= HTTP_REQUEST_FLAG_AUTHENTICATED;

}
#endif

/// @}

