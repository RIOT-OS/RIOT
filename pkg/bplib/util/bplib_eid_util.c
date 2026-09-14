#include "bplib_eid_util.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

bool bplib_util_eid2str(BPLib_EID_t* eid, char* str)
{
    if (BPLib_EID_IsValid(eid)) {
        if (eid->Scheme == BPLIB_EID_SCHEME_DTN) {
            /* bplib currently only supports IPN, however the none dtn endpoint
             * is still valid. Everything that is valid & dtn scheme is dtn:none. */
            snprintf(str, DTN_EID_IPN_MAX_SIZE, "dtn:none");
            return true;
        }
        else if (eid->Scheme == BPLIB_EID_SCHEME_IPN) {
            if (eid->IpnSspFormat == BPLIB_EID_IPN_SSP_FORMAT_THREE_DIGIT) {
                snprintf(str, DTN_EID_IPN_MAX_SIZE, "dtn:%"PRIu64".%"PRIu64".%"PRIu64,
                    eid->Allocator, eid->Node, eid->Service);
                return true;
            }
            else if (eid->IpnSspFormat == BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT) {
                snprintf(str, DTN_EID_IPN_MAX_SIZE, "dtn:%"PRIu64".%"PRIu64,
                    eid->Node, eid->Service);
                return true;
            }
        }
    }

    snprintf(str, DTN_EID_IPN_MAX_SIZE, "[invalid]");
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
