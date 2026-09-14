/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "bplib_eid_util.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

bool bplib_util_eid2str(BPLib_EID_t* eid, char* str)
{
    // TODO replace with 7.0.5 functions when rebased 
    if (BPLib_EID_IsValid(eid)) {
        if (eid->Scheme == BPLIB_EID_SCHEME_DTN) {
            /* bplib currently only supports IPN, however the none dtn endpoint
             * is still valid. Everything that is valid & dtn scheme is dtn:none. */
            snprintf(str, DTN_EID_IPN_MAX_SIZE, "dtn:none");
            return true;
        }
        else if (eid->Scheme == BPLIB_EID_SCHEME_IPN) {
            if (eid->IpnSspFormat == BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT) {
                snprintf(str, DTN_EID_IPN_MAX_SIZE, "ipn:%"PRIu64".%"PRIu64".%"PRIu64,
                    eid->Allocator, eid->Node, eid->Service);
                return true;
            }
            else if (eid->IpnSspFormat == BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT) {
                snprintf(str, DTN_EID_IPN_MAX_SIZE, "ipn:%"PRIu64".%"PRIu64,
                    eid->Node, eid->Service);
                return true;
            }
        }
    }

    snprintf(str, DTN_EID_IPN_MAX_SIZE, "[invalid]");
    return false;
}

static bool _strtou32(const char* str, uint32_t* res)
{
    unsigned long temp = strtoul(str, NULL, 10);
    if ((errno == ERANGE) || (temp > UINT32_MAX) || (str[0] == '\0')) {
        errno = 0;
        return false;
    }

    *res = temp;
    return true;
}

static bool _strtou64(const char* str, uint64_t* res)
{
    unsigned long long temp = strtoull(str, NULL, 10);
    if ((errno == ERANGE) || (temp > UINT64_MAX) || (str[0] == '\0')) {
        errno = 0;
        return false;
    }

    *res = temp;
    return true;
}

bool bplib_util_str2eid(const char* str, BPLib_EID_t* eid)
{
    char buf[DTN_EID_IPN_MAX_SIZE];
    strncpy(buf, str, DTN_EID_IPN_MAX_SIZE);
    buf[DTN_EID_IPN_MAX_SIZE - 1] = '\0';

    if ((str == NULL) || (eid == NULL)) {
        return false;
    }

    if (strcmp(buf, "dtn:none") == 0) {
        *eid = BPLIB_EID_DTN_NONE;
        return true;
    }

    /* Since bplib currently cannot deal with DTN EIDs, only accept ipn */
    if ((buf[0] != 'i') || (buf[1] != 'p') ||
        (buf[2] != 'n') || (buf[3] != ':')) {
        return false;
    }

    eid->Scheme = BPLIB_EID_SCHEME_IPN;

    /* Pointers to the sub segments of a 3 (or 2) digit IPN string.
     * Until we know if there are 3 or 2 segments, the first one could be
     * allocator (in 3 digit) or node (in 2 digit) */
    char* segments[3];
    segments[0] = buf + 4;

    int num_dots = 0;
    for (int i = 4; i < DTN_EID_IPN_MAX_SIZE - 1; i++) {
        if (buf[i] == '.') {
            if (++num_dots > 2) {
                /* Not a valid IPN string */
                return false;
            }
            /* At i+1 there is at least the \0, which would not be a problem */
            segments[num_dots] = buf + i + 1;
            buf[i] = '\0';
        }
        else if (buf[i] == '\0') {
            break;
        }
    }

    if (num_dots == 1) {
        eid->IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT;
        eid->Allocator = 0;
        if (_strtou64(segments[0], &eid->Node) &&
            _strtou64(segments[1], &eid->Service)) {
            return true;
        }
    }
    else if (num_dots == 2) {
        uint32_t temp1, temp2;
        eid->IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT;
        if (_strtou32(segments[0], &temp1) &&
            _strtou32(segments[1], &temp2) &&
            _strtou64(segments[2], &eid->Service)) {
            eid->Allocator = temp1;
            eid->Node = temp2;
            return true;
        }
    }

    return false;
}

void bplib_util_eids_from_pattern(BPLib_EID_Pattern_t* pat, BPLib_EID_t* eid1, BPLib_EID_t* eid2)
{
    if (eid1) {
        eid1->Scheme = pat->Scheme;
        eid1->IpnSspFormat = pat->IpnSspFormat;
        eid1->Allocator = pat->MinAllocator;
        eid1->Node = pat->MinNode;
        eid1->Service = pat->MinService;
    }
    if (eid2) {
        eid2->Scheme = pat->Scheme;
        eid2->IpnSspFormat = pat->IpnSspFormat;
        eid2->Allocator = pat->MaxAllocator;
        eid2->Node = pat->MaxNode;
        eid2->Service = pat->MaxService;
    }
}
