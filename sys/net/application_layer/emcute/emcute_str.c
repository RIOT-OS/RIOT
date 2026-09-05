/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     net_emcute
 * @{
 *
 * @file
 * @brief       emCute string functions (for debugging purposes)
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "net/emcute.h"
#include "emcute_internal.h"

const char *emcute_type_str(uint8_t type)
{
    switch (type) {
        case ADVERTISE:     return "ADVERTISE";
        case SEARCHGW:      return "SEARCHGW";
        case GWINFO:        return "GWINFO";
        case CONNECT:       return "CONNECT";
        case CONNACK:       return "CONNACK";
        case WILLTOPICREQ:  return "WILLTOPICREQ";
        case WILLTOPIC:     return "WILLTOPIC";
        case WILLMSGREQ:    return "WILLMSGREQ";
        case WILLMSG:       return "WILLMSG";
        case REGISTER:      return "REGISTER";
        case REGACK:        return "REGACK";
        case PUBLISH:       return "PUBLISH";
        case PUBACK:        return "PUBACK";
        case PUBCOMP:       return "PUBCOMP";
        case PUBREC:        return "PUBREC";
        case PUBREL:        return "PUBREL";
        case SUBSCRIBE:     return "SUBSCRIBE";
        case SUBACK:        return "SUBACK";
        case UNSUBSCRIBE:   return "UNSUBSCRIBE";
        case UNSUBACK:      return "UNSUBACK";
        case PINGREQ:       return "PINGREQ";
        case PINGRESP:      return "PINGRESP";
        case DISCONNECT:    return "DISCONNECT";
        case WILLTOPICUPD:  return "WILLTOPICUPD";
        case WILLTOPICRESP: return "WILLTOPICRESP";
        case WILLMSGUPD:    return "WILLMSGUPD";
        case WILLMSGRESP:   return "WILLMSGRESP";
        default:            return "UNKNOWN";
    }
}
