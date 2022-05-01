/*
 * Copyright (C) 2022 Juliusz Neuman
 */


#ifndef NET_GNRC_IPV6_IPSEC_DB_H
#define NET_GNRC_IPV6_IPSEC_DB_H


#include "net/gnrc/pkt.h"
#include "net/gnrc/ipv6/ipsec/ipsec_ts.h"
#include "net/gnrc/ipv6/ipsec/ipsec_enums.h"
#include "net/gnrc/ipv6/ipsec/config.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((__packed__)) {
    ipsec_cipher_t cipher;
    ipsec_hash_t hash;
    uint8_t key[IPSEC_MAX_KEY_SIZE];
    uint8_t hash_key[IPSEC_MAX_HASH_SIZE];
} ipsec_crypto_info_t;

typedef struct __attribute__((__packed__)) {
    uint32_t spi;                   /**< security parameter index */
    uint64_t sn;                    /**< sequence number */
    ipsec_cipher_mode_t c_mode;     /**< cryptographic mode */
    ipsec_crypto_info_t crypt_info; /**< cypher key, mode, etc. */
    ipsec_mode_t mode;              /**< transport / tunnel */
    ipv6_addr_t dst;                /**< destination ipv6 address */
    ipv6_addr_t src;                /**< source ipv6 address */
    ipv6_addr_t tun_dst;            /**< tunnel selector destination ipv6 address */
    uint8_t tun_dst_mask;           /**< tunnel selector destination prefix >*/
    ipv6_addr_t tun_src;            /**< tunnel selector source ipv6 address */
    uint8_t tun_src_mask;           /**< tunnel selector destination prefix >*/
} ipsec_sa_t;

typedef struct __attribute__((__packed__)) {
    ipv6_addr_t dst;            /**< destination ipv6 address */
    ipv6_addr_t src;            /**< source ipv6 address */
    ipv6_addr_t tun_dst;        /**< tunnel selector destination ipv6 address */
    uint8_t tun_dst_mask;       /**< tunnel selector destination prefix >*/
    ipv6_addr_t tun_src;        /**< tunnel selector source ipv6 address */
    uint8_t tun_src_mask;       /**< tunnel selector destination prefix >*/
    uint8_t proto;              /**< payloads IP protocoll number */
    uint16_t dst_port;          /**< Traffic Selector(TS) destination port
                                   number for UDP/TCP */
    uint16_t src_port;          /**< Traffic Selector(TS) source port number
                                   for UDP/TCP */
    ipsec_sp_rule_t rule;       /**< firewall filter rule */
    ipsec_mode_t tun_mode;      /**< transport / tunnel */
    ipsec_cipher_mode_t c_mode; /**< cipher mode >*/
    uint8_t prio;               /* priority */
} ipsec_sp_t;


uint32_t get_spi(void);

int sadb_init(void);

int spdb_init(void);

/**
 * @brief Initialize temporary test databases
 */
int sasp_tmp_init(void);

int add_sa(ipsec_sa_t *sa);

int add_sp(ipsec_sp_t *sp);

int del_sa(uint32_t spi);

int del_sp(uint32_t sp_idx);

int ipsec_get_sp_by_ts(ipsec_ts_t *ts, ipsec_sp_t *sp);

int ipsec_get_sa_by_spi(uint32_t spi, ipsec_sa_t *sa);

int ipsec_get_sa_by_ts(ipsec_ts_t *ts, ipsec_sa_t *sa);

uint64_t ipsec_inc_sn(uint32_t spi);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_IPSEC_DB_H */
