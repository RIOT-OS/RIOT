#include "net/gnrc/ipv6/ipsec/ipsec_db.h"
#include "net/ipv6/addr.h"
#include <errno.h>

typedef struct __attribute__((__packed__)) {
    uint8_t set;
    ipsec_sa_t sa_ext;
} internal_ipsec_sa_t;

typedef struct __attribute__((__packed__)) {
    uint8_t set;
    ipsec_sp_t sp_ext;
} internal_ipsec_sp_t;


/**
 * @todo
 * Add mutex and dynamic listsSS
 */
static internal_ipsec_sa_t sadb[IPSEC_MAX_SA_NUM];
static internal_ipsec_sp_t spdb[IPSEC_MAX_SP_NUM];

int sadb_init(void)
{
    for (int i = 0; i < IPSEC_MAX_SA_NUM; ++i) {
        sadb[i] = (internal_ipsec_sa_t){ 0 };
    }
    return 0;
}

int spdb_init(void)
{
    for (int i = 0; i < IPSEC_MAX_SP_NUM; ++i) {
        spdb[i] = (internal_ipsec_sp_t){ 0 };
    }
    return 0;
}

int sasp_tmp_init(void)
{
    ipsec_sp_t sp1 = {
        .rule = IPSEC_SP_RULE_PROTECT,
        .tun_dst_mask = 128,
        .tun_src_mask = 128,
        .proto = 0,
        .prio = 10,
    };
    ipsec_sp_t sp2 = {
        .rule = IPSEC_SP_RULE_PROTECT,
        .tun_dst_mask = 128,
        .tun_src_mask = 128,
        .proto = 0,
        .prio = 10,
    };
    ipsec_sp_t sp3 = {
        .rule = IPSEC_SP_RULE_BYPASS,
        .tun_dst_mask = 0,
        .tun_src_mask = 0,
        .proto = 0,
        .prio = 20,
    };
    ipsec_sp_t sp4 = {
        .rule = IPSEC_SP_RULE_BYPASS,
        .tun_dst_mask = 0,
        .tun_src_mask = 0,
        .dst_port = 500,
        .proto = 0,
        .prio = 5,
    };
    ipsec_sp_t sp5 = {
        .rule = IPSEC_SP_RULE_BYPASS,
        .tun_dst_mask = 0,
        .tun_src_mask = 0,
        .dst_port = 4500,
        .proto = 0,
        .prio = 5,
    };

    ipv6_addr_from_str(&sp1.src, "aa::02");
    ipv6_addr_from_str(&sp1.dst, "aa::01");
    ipv6_addr_from_str(&sp1.tun_src, "aa::02");
    ipv6_addr_from_str(&sp1.tun_dst, "aa::01");
    ipv6_addr_from_str(&sp2.src, "aa::01");
    ipv6_addr_from_str(&sp2.dst, "aa::02");
    ipv6_addr_from_str(&sp2.tun_src, "aa::01");
    ipv6_addr_from_str(&sp2.tun_dst, "aa::02");

    ipsec_sa_t sa1 = {
        .spi = 0x12345678,
        .mode = IPSEC_MODE_TRANSPORT,
        .c_mode = IPSEC_CIPHER_MODE_ENC_AUTH,
        .tun_src_mask = 128,
        .tun_dst_mask = 128,
        .crypt_info = {
            .cipher = IPSEC_CIPHER_AES128_CBC,
            .hash = IPSEC_HASH_SHA1,
            .key = { 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34,
                     0x56, 0x78, 0x12, 0x34, 0x56, 0x78 },
            .hash_key = { 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12,
                          0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34,
                          0x56, 0x78 },
        },
    };

    ipv6_addr_from_str(&sa1.src, "aa::01");
    ipv6_addr_from_str(&sa1.dst, "aa::02");
    ipv6_addr_from_str(&sa1.tun_src, "aa::01");
    ipv6_addr_from_str(&sa1.tun_dst, "aa::02");

    ipsec_sa_t sa2 = {
        .spi = 0x12345679,
        .mode = IPSEC_MODE_TRANSPORT,
        .c_mode = IPSEC_CIPHER_MODE_ENC_AUTH,
        .tun_src_mask = 128,
        .tun_dst_mask = 128,
        .crypt_info = {
            .cipher = IPSEC_CIPHER_AES128_CBC,
            .hash = IPSEC_HASH_SHA1,
            .key = { 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34,
                     0x56, 0x78, 0x12, 0x34, 0x56, 0x78 },
            .hash_key = { 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12,
                          0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34,
                          0x56, 0x78 },
        },
    };

    ipv6_addr_from_str(&sa2.src, "aa::02");
    ipv6_addr_from_str(&sa2.dst, "aa::01");
    ipv6_addr_from_str(&sa2.tun_src, "aa::02");
    ipv6_addr_from_str(&sa2.tun_dst, "aa::01");

    //add_sp(&sp1);
    //add_sp(&sp2);
    add_sp(&sp3);
    add_sp(&sp4);
    add_sp(&sp5);
    //add_sa(&sa1);
    //add_sa(&sa2);
    return 0;
}

uint32_t get_spi(void)
{
    static uint32_t spi = 256;

    return spi++;
}

int add_sa(ipsec_sa_t *sa)
{
    internal_ipsec_sa_t *entry;

    for (int i = 0; i < IPSEC_MAX_SA_NUM; ++i) {
        entry = &sadb[i];
        if (entry->set == 0) {
            entry->sa_ext = *sa;
            entry->set = 1;
            return 0;
        }
    }
    return -ENOMEM;
}

int add_sp(ipsec_sp_t *sp)
{
    internal_ipsec_sp_t *entry;

    for (int i = 0; i < IPSEC_MAX_SP_NUM; ++i) {
        entry = &spdb[i];
        if (entry->set == 0) {
            entry->sp_ext = *sp;
            entry->set = 1;
            return i;
        }
    }
    return -ENOMEM;
}

int del_sa(uint32_t spi)
{
    internal_ipsec_sa_t *entry;

    for (int i = 0; i < IPSEC_MAX_SA_NUM; ++i) {
        entry = &sadb[i];
        if (entry->sa_ext.spi == spi) {
            entry->sa_ext = (ipsec_sa_t){ 0 };
            entry->set = 0;
            return 0;
        }
    }
    return -ENOENT;
}

int del_sp(uint32_t sp_idx)
{
    internal_ipsec_sp_t *entry;

    if (sp_idx >= IPSEC_MAX_SP_NUM) {
        return -ENOENT;
    }
    entry = &spdb[sp_idx];
    if (entry->set == 0) {
        return -ENOENT;
    }
    entry->set = 0;
    entry->sp_ext = (ipsec_sp_t){ 0 };
    return 0;
}

int ipsec_get_sp_by_ts(ipsec_ts_t *ts, ipsec_sp_t *sp)
{
    internal_ipsec_sp_t *entry, *best = NULL;
    uint16_t best_prio = 256;

    for (int i = 0; i < IPSEC_MAX_SP_NUM; ++i) {
        entry = &spdb[i];
        if (entry->set == 0) {
            continue;
        }
        if (entry->sp_ext.src_port && entry->sp_ext.src_port != ts->src_port) {
            continue;
        }
        if (entry->sp_ext.dst_port && entry->sp_ext.dst_port != ts->dst_port) {
            continue;
        }
        if (entry->sp_ext.proto && entry->sp_ext.proto != ts->proto) {
            continue;
        }
        if (ipv6_addr_match_prefix(&entry->sp_ext.tun_src, &ts->src)
            < entry->sp_ext.tun_src_mask) {
            continue;
        }
        if (ipv6_addr_match_prefix(&entry->sp_ext.tun_dst, &ts->dst)
            < entry->sp_ext.tun_dst_mask) {
            continue;
        }
        if (entry->sp_ext.prio < best_prio) {
            best_prio = entry->sp_ext.prio;
            best = entry;
        }
    }
    if (best) {
        *sp = best->sp_ext;
        return 0;
    }
    return -ENOENT;
}

int ipsec_get_sa_by_spi(uint32_t spi, ipsec_sa_t *sa)
{
    internal_ipsec_sa_t *entry;

    for (int i = 0; i < IPSEC_MAX_SA_NUM; ++i) {
        entry = &sadb[i];
        if (entry->set && entry->sa_ext.spi == spi) {
            memcpy(sa, &entry->sa_ext, sizeof(*sa));
            return 0;
        }
    }
    return -ENOENT;
}


uint64_t ipsec_inc_sn(uint32_t spi)
{
    internal_ipsec_sa_t *entry;

    for (int i = 0; i < IPSEC_MAX_SA_NUM; ++i) {
        entry = &sadb[i];
        if (entry->set && entry->sa_ext.spi == spi) {
            return ++(entry->sa_ext.sn);
        }
    }
    return 0;

}


int ipsec_get_sa_by_ts(ipsec_ts_t *ts, ipsec_sa_t *sa)
{
    internal_ipsec_sa_t *entry;

    for (int i = 0; i < IPSEC_MAX_SA_NUM; ++i) {
        entry = &sadb[i];
        if (ipv6_addr_match_prefix(&entry->sa_ext.tun_src, &ts->src)
            < entry->sa_ext.tun_src_mask) {
            continue;
        }
        if (ipv6_addr_match_prefix(&entry->sa_ext.tun_dst, &ts->dst)
            < entry->sa_ext.tun_dst_mask) {
            continue;
        }
        *sa = entry->sa_ext;
        return 0;
    }
    return -ENOENT;
}
