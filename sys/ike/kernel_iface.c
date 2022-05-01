#include "ike/ike_kernel_iface.h"
#include "net/gnrc/ipv6/ipsec/ipsec_db.h"

int install_esp(chunk_t our_ska, chunk_t other_ska, chunk_t our_ske, chunk_t other_ske,
                ipv6_addr_t our_addr, ipv6_addr_t other_addr, uint32_t our_spi,
                uint32_t other_spi, int *policy_id_in, int *policy_id_out)
{
    ipsec_sa_t sa_in, sa_out;
    ipsec_sp_t sp_in, sp_out;
    int sp_in_idx = -1, sp_out_idx = -1;

    sa_in = (ipsec_sa_t){
        .c_mode = IPSEC_CIPHER_MODE_ENC_AUTH,
        .spi = our_spi,
        .mode = IPSEC_MODE_TRANSPORT,
        .crypt_info = {
            .cipher = IPSEC_CIPHER_AES128_CBC,
            .hash = IPSEC_HASH_SHA1,
        },
        .src = other_addr,
        .tun_src = other_addr,
        .tun_src_mask = 128,
        .dst = our_addr,
        .tun_dst = our_addr,
        .tun_dst_mask = 128,
    };
    memcpy(sa_in.crypt_info.key, other_ske.ptr, other_ske.len);
    memcpy(sa_in.crypt_info.hash_key, other_ska.ptr, other_ska.len);

    sa_out = (ipsec_sa_t){
        .c_mode = IPSEC_CIPHER_MODE_ENC_AUTH,
        .spi = other_spi,
        .mode = IPSEC_MODE_TRANSPORT,
        .crypt_info = {
            .cipher = IPSEC_CIPHER_AES128_CBC,
            .hash = IPSEC_HASH_SHA1,
        },
        .src = our_addr,
        .tun_src = our_addr,
        .tun_src_mask = 128,
        .dst = other_addr,
        .tun_dst = other_addr,
        .tun_dst_mask = 128,
    };
    memcpy(sa_out.crypt_info.key, our_ske.ptr, our_ske.len);
    memcpy(sa_out.crypt_info.hash_key, our_ska.ptr, our_ska.len);

    sp_in = (ipsec_sp_t){
        .c_mode = IPSEC_CIPHER_MODE_ENC_AUTH,
        .prio = 10,
        .rule = IPSEC_SP_RULE_PROTECT,
        .tun_mode = IPSEC_MODE_TRANSPORT,
        .src = other_addr,
        .tun_src = other_addr,
        .tun_src_mask = 128,
        .dst = our_addr,
        .tun_dst = our_addr,
        .tun_dst_mask = 128,
    };

    sp_out = (ipsec_sp_t){
        .c_mode = IPSEC_CIPHER_MODE_ENC_AUTH,
        .prio = 10,
        .rule = IPSEC_SP_RULE_PROTECT,
        .tun_mode = IPSEC_MODE_TRANSPORT,
        .src = our_addr,
        .tun_src = our_addr,
        .tun_src_mask = 128,
        .dst = other_addr,
        .tun_dst = other_addr,
        .tun_dst_mask = 128,
    };

    if (add_sa(&sa_in) < 0) {
        goto error;
    }
    if (add_sa(&sa_out) < 0) {
        goto error;
    }
    if ((*policy_id_in = add_sp(&sp_in)) < 0) {
        goto error;
    }
    if ((*policy_id_out = add_sp(&sp_out)) < 0) {
        goto error;
    }

    return 0;
error:
    clear_esp(sa_in.spi, sa_out.spi, sp_in_idx, sp_out_idx);
    return -1;
}


int clear_esp(uint32_t our_spi, uint32_t other_spi, int policy_in, int policy_out)
{
    del_sa(our_spi);
    del_sa(other_spi);
    if (policy_in >= 0) {
        del_sp(policy_in);
    }
    if (policy_out >= 0) {
        del_sp(policy_out);
    }
    return 0;
}
