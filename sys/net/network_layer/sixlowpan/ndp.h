#include <stdint.h>

#include "sixlowpan/ndp.h"
#include "sixlowpan/types.h"

int ndp_state_change_on_ipv6_packet_send(ndp_nce_type_t *nce, int if_id,
                                         ipv6_addr_t *src, ipv6_addr_t *dest,
                                         uint8_t aro);
int ndp_state_change_on_ndp_message_receive(ndp_nce_type_t *nce, int if_id,
                                            icmpv6_hdr_t *ndp_message);
int ndp_get_ll_addr_from_sllao(uint8_t *ll_addr, icmpv6_ndp_opt_stllao_t *sllao);

static inline uint8_t ndp_get_link_layer_address_len_from_llao(icmpv6_ndp_opt_stllao_t *sllao)
{
#ifdef MODULE_SIXLOWPAN
    if (sllao->length == 1) {
        return 2;
    }
    else if (sllao->length == 2) {
        return 8;
    }
#else
    if (sllao->length == 1) {
        return 6;
    }
    else if (sllao->length == 2) {
        return 8;
    }
#endif

    return 0;
}
