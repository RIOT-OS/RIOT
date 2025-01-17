#ifndef NDEF_URI_PAYLOAD_H
#define NDEF_URI_PAYLOAD_H

#include "net/nfc/ndef/ndef.h"

typedef enum {
    NDEF_URI_NA = 0x00,
    NDEF_URI_HTTP_WWW = 0x01,
    NDEF_URI_HTTPS_WWW = 0x02,
    NDEF_URI_HTTP = 0x03,
    NDEF_URI_HTTPS = 0x04,
    NDEF_URI_TEL = 0x05,
    NDEF_URI_MAILTO = 0x06,
    NDEF_URI_FTP_ANONYMOUS = 0x07,
    NDEF_URI_FTP_FTP = 0x08,
    NDEF_URI_FTPS = 0x09,
    NDEF_URI_SFTP = 0x0A,
    NDEF_URI_SMB = 0x0B,
    NDEF_URI_NFS = 0x0C,
    NDEF_URI_FTP = 0x0D,
    NDEF_URI_DAV = 0x0E,
    NDEF_URI_NEWS = 0x0F,
    NDEF_URI_TELNET = 0x10,
    NDEF_URI_IMAP = 0x11,
    NDEF_URI_RTSP = 0x12,
    NDEF_URI_URN = 0x13,
    NDEF_URI_POP = 0x14,
    NDEF_URI_SIP = 0x15,
    NDEF_URI_SIPS = 0x16,
    NDEF_URI_TFTP = 0x17,
    NDEF_URI_BTSPP = 0x18,
    NDEF_URI_BTL2CAP = 0x19,
    NDEF_URI_BTGOEP = 0x1A,
    NDEF_URI_TCPOBEX = 0x1B,
    NDEF_URI_IRDAOBEX = 0x1C,
    NDEF_URI_FILE = 0x1D,
    NDEF_URI_URN_EPC_ID = 0x1E,
    NDEF_URI_URN_EPC_TAG = 0x1F,
    NDEF_URI_URN_EPC_PAT = 0x20,
    NDEF_URI_URN_EPC_RAW = 0x21,
    NDEF_URI_URN_EPC = 0x22,
    NDEF_URI_URN_NFC = 0x23
} ndef_uri_identifier_code_t;

int ndef_add_uri_record(ndef_t *message, ndef_uri_identifier_code_t identifier_code, const char *uri, uint32_t uri_length);

#endif 