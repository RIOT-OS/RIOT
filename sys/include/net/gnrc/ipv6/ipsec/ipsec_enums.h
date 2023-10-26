#ifndef IPSECNET_GNRC_IPV6_IPSEC_ENUMS_H
#define IPSECNET_GNRC_IPV6_IPSEC_ENUMS_H


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    IPSEC_SP_RULE_PROTECT,
    IPSEC_SP_RULE_BYPASS,
    IPSEC_SP_RULE_DROP,
    IPSEC_SP_RULE_ERROR,
} ipsec_sp_rule_t;

typedef enum {
    TRAFFIC_DIR_OUT,
    TRAFFIC_DIR_IN,
} ipsec_traffic_dir_t;

typedef enum {
    IPSEC_CIPHER_MODE_NONE,
    IPSEC_CIPHER_MODE_AUTH_ONLY,
    IPSEC_CIPHER_MODE_ENC_AUTH,
    IPSEC_CIPHER_MODE_COMB,
} ipsec_cipher_mode_t;

typedef enum {
    IPSEC_CIPHER_NONE,
    IPSEC_CIPHER_AES128_CBC,
} ipsec_cipher_t;

typedef enum {
    IPSEC_HASH_NONE,
    IPSEC_HASH_SHA1,
} ipsec_hash_t;

typedef enum {
    IPSEC_MODE_TUNNEL,
    IPSEC_MODE_TRANSPORT,
} ipsec_mode_t;

#ifdef __cplusplus
}
#endif

#endif /* IPSECNET_GNRC_IPV6_IPSEC_ENUMS_H */
