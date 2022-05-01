#include "net/gnrc/ipv6.h"
#include "net/gnrc/ipv6/ipsec/ipsec.h"
#include "net/gnrc/ipv6/ipsec/ipsec_db.h"
#include "net/gnrc/ipv6/ipsec/esp.h"
#include "crypto/aes.h"
#include "hashes/sha1.h"
#include "crypto/modes/cbc.h"
#include "random.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define ENCRYPTION_BUFFER_SIZE 2048


static int _calc_fields(const ipsec_sa_t *sa, uint8_t *iv_size, uint8_t *icv_size,
                        uint8_t *block_size)
{

    /* On using DietESP: If Implicit IV is used, that has to be minded here */

    switch (sa->crypt_info.cipher) {
    case IPSEC_CIPHER_AES128_CBC:
        *block_size = 16;
        *iv_size = 16;
        break;
    default:
        DEBUG("ipsec_esp: ERROR unsupported cipher\n");
        return -1;
    }
    switch (sa->crypt_info.hash) {
    case IPSEC_HASH_SHA1:
        *icv_size = 12;
        break;
    default:
        DEBUG("ipsec_esp: ERROR unsupported hash\n");
        return -1;
    }
    return 0;
}

static void _calc_padding(uint8_t *padding_size, int plaintext_size,
                          uint8_t block_size)
{

    uint8_t mod_payload = (uint8_t)((plaintext_size) % (int)block_size);

    if (mod_payload == 0) {
        *padding_size = 0;
    }
    else {
        *padding_size = block_size - mod_payload;
    }
}

static int _decrypt(gnrc_pktsnip_t *esp, const ipsec_sa_t *sa)
{

    /* On using DietESP: On en- and decryption some negotiated DietESP rules
       need to be checked and minded for all ciphers, like e.g. Implicit IV */
    uint8_t icv_size, iv_size, block_size;
    size_t enc_len;
    uint8_t *pld, *iv;
    cipher_t cipher;
    uint8_t data[ENCRYPTION_BUFFER_SIZE];

    if (_calc_fields(sa, &iv_size, &icv_size, &block_size)) {
        return -1;
    }
    iv = (uint8_t *)esp->data + sizeof(ipv6_esp_hdr_t);
    pld = iv + iv_size;
    enc_len = esp->size - sizeof(ipv6_esp_hdr_t) - iv_size - icv_size;
    if (enc_len > ENCRYPTION_BUFFER_SIZE) {

        DEBUG("ipv6_esp: Too much data for decryption\n");
        return -1;
    }
    switch (sa->crypt_info.cipher) {
    case IPSEC_CIPHER_AES128_CBC:
        if (cipher_init(&cipher, CIPHER_AES, sa->crypt_info.key, 16) != CIPHER_INIT_SUCCESS) {
            DEBUG("ipv6_esp: Failed to initialize cipher\n");
            return -1;
        }

        if (cipher_decrypt_cbc(&cipher, iv, pld, enc_len, data) < 0) {
            DEBUG("ipv6_esp: Failed to decrypt\n");
            return -1;
        }
        break;
    default:
        DEBUG("ipv6_esp: Unsupported cipher decryption\n");
        return -1;
    }
    memcpy((uint8_t *)esp->data + sizeof(ipv6_esp_hdr_t) + iv_size, data, enc_len);
    return 0;
}


static int _encrypt(gnrc_pktsnip_t *esp, const ipsec_sa_t *sa)
{
    uint8_t *iv, *icv, *pld;
    uint8_t iv_size, icv_size, block_size;
    size_t pld_len;
    cipher_t cipher;
    sha1_context hash;
    uint8_t data[ENCRYPTION_BUFFER_SIZE];

    _calc_fields(sa, &iv_size, &icv_size, &block_size);
    iv = (uint8_t *)esp->data + sizeof(ipv6_esp_hdr_t);
    random_bytes(iv, iv_size);
    pld = iv + iv_size;
    icv = (uint8_t *)esp->data + esp->size - icv_size;
    pld_len = esp->size - sizeof(ipv6_esp_hdr_t) - iv_size - icv_size;
    switch (sa->crypt_info.cipher) {
    case IPSEC_CIPHER_AES128_CBC:
        if (cipher_init(&cipher, CIPHER_AES, sa->crypt_info.key, 16) != CIPHER_INIT_SUCCESS) {
            DEBUG("ipv6_esp: Failed to initialize cipher\n");
            return -1;
        }

        if (cipher_encrypt_cbc(&cipher, iv, pld, pld_len, data) < 0) {
            DEBUG("ipv6_esp: Failed to encrypt\n");
            return -1;
        }
        memcpy(pld, data, pld_len);
        /* code */
        break;

    default:
        DEBUG("ipv6_esp: Unsupported cipher encryption\n");
        return 0;
    }
    switch (sa->crypt_info.hash) {
    case IPSEC_HASH_SHA1:
        sha1_init_hmac(&hash, sa->crypt_info.hash_key, 32);
        sha1_update(&hash, esp->data, esp->size - icv_size);
        sha1_final_hmac(&hash, data);
        memcpy(icv, data, icv_size);
        /* code */
        break;

    default:
        DEBUG("ipv6_esp: Unsupported hmac encryption\n");
        return 0;
    }
    return 0;
}

gnrc_pktsnip_t *esp_header_process(gnrc_pktsnip_t *esp, uint8_t protnum)
{
    gnrc_pktsnip_t *data_snip;
    gnrc_pktsnip_t *new_ipv6;
    ipsec_sa_t sa;
    ipv6_esp_trl_t trl;

    uint8_t iv_size;
    uint8_t icv_size;
    uint32_t spi;
    uint32_t sn;
    size_t data_size;
    uint8_t blocksize;

    assert(protnum == PROTNUM_IPV6_EXT_ESP);
    DEBUG("ipv6_esp: Rx ESP packet\n");


    spi = byteorder_ntohl(*(network_uint32_t *)esp->data);
    sn = byteorder_ntohl(*(network_uint32_t *)((uint8_t *)esp->data + 4));

    DEBUG("ipv6_esp: Rx pkt spi: %i  sn: %i\n", (int)spi, (int)sn);

    if (ipsec_get_sa_by_spi(spi, &sa)) {
        DEBUG("ipv6_esp: Rx sa by spi not found. spi: %i\n", (int)spi);
        /* pkt will be released by caller */
        return NULL;
    }

    /* TODO: Send SN to 'Anti Replay Window' processing
     * pkt = _check_arpw() /@return NULL if out of range
     */

    /* Authenticate and Decrypt ESP packet */
    switch (sa.c_mode) {
    case IPSEC_CIPHER_MODE_ENC_AUTH:
        //_verify(esp, sa);
        _decrypt(esp, &sa);
        break;
    default:
        DEBUG("ipv6_esp: ERROR Cipher mode not supported\n");
        return NULL;
    }

    /* TODO: Check against SPD database.
     *
     * -> After the packet is decrypted, we need to check it against the SDP
     * rule set, since we where not able to determine its content before
     * decryption. This stems from the fact, that an SA can be shared by
     * multiple SPD rules. Imagine a scenario where a single SA is used for all
     * comunication between two systems, but where the SPD rules states to
     * DISCARD all TCP traffic. */

    /** On using DietESP: At this stange we send the decrypted packet to the
     * EHC routines to decompress it */

    /* we do not need blocksize here, but else we'd need two methods */
    _calc_fields(&sa, &iv_size, &icv_size, &blocksize);
    trl = *(ipv6_esp_trl_t *)((uint8_t *)esp->data + esp->size - icv_size - sizeof(ipv6_esp_trl_t));
    data_size = esp->size -
                (sizeof(ipv6_esp_hdr_t) + sizeof(ipv6_esp_trl_t) + trl.pl
                 + icv_size + iv_size);
    data_snip = gnrc_pktbuf_add(NULL, NULL, data_size, gnrc_nettype_from_protnum(trl.nh));
    memcpy(data_snip->data,
           (((uint8_t *)esp->data) + sizeof(ipv6_esp_hdr_t) + iv_size), data_size);
    esp = gnrc_pktbuf_remove_snip(esp, esp);
    esp = gnrc_pkt_prepend(esp, data_snip);

    if (sa.mode != IPSEC_MODE_TRANSPORT) {
        DEBUG("ipv6_esp: Only transport mode supported");
        gnrc_pktbuf_release(data_snip);
        return NULL;
    }

    /* adjusting original ipv6 header fields*/
    LL_SEARCH_SCALAR(esp, new_ipv6, type, GNRC_NETTYPE_IPV6);
    /* TODO: consider intermediate ext headers for len */
    ((ipv6_hdr_t *)new_ipv6->data)->len = byteorder_htons((uint16_t)esp->size);
    if (esp->next->type == GNRC_NETTYPE_IPV6) {
        ((ipv6_hdr_t *)esp->next->data)->nh = gnrc_nettype_to_protnum(esp->type);
    }
    else {      /* prev header is ext header */
        ((ipv6_ext_t *)esp->next->data)->nh = gnrc_nettype_to_protnum(esp->type);
    }

    /* TODO: add original? pktsize to SA bytecount limiters */

    return esp;
}

gnrc_pktsnip_t *esp_header_build(gnrc_pktsnip_t *pkt,
                                 const ipsec_sa_t *sa, ipsec_ts_t *ts)
{
    gnrc_pktsnip_t *prev = NULL;
    gnrc_pktsnip_t *next = NULL;
    gnrc_pktsnip_t *ipv6 = NULL;
    gnrc_pktsnip_t *esp;
    ipv6_hdr_t     *ipv6_h;
    ipv6_esp_hdr_t *esp_h;
    ipv6_esp_trl_t *esp_trl;
    uint64_t sn;
    uint16_t data_size;
    uint16_t esp_size;
    uint16_t itm_size;      //size of intermediate headers
    uint8_t block_size;
    uint8_t iv_size;
    uint8_t icv_size;
    uint8_t padding_size;
    uint8_t nh;
    void *payload;

    LL_SEARCH_SCALAR(pkt, ipv6, type, GNRC_NETTYPE_IPV6);
    if (ipv6 == NULL) {
        DEBUG("ipsec_esp: ERROR No IPv6 header found\n");
        return NULL;
    }
    ipv6_h = ipv6->data;
    nh = ts->proto;

    if (sa->mode == IPSEC_MODE_TRANSPORT) {
        /* in transport mode we should have all snips marked to iterate over */
        LL_SEARCH_SCALAR(pkt, next, type, gnrc_nettype_from_protnum(nh));
        if (next == NULL) {
            DEBUG("ipsec_esp: Payload snip not found. Protnum:%i\n", nh);
            return NULL;
        }
    }
    else {
        return NULL;
    }

    /* Get preceeding pktsnip */
    /* Note: The first 'next' argument isn't a variable in this call */
    LL_SEARCH_SCALAR(pkt, prev, next, next);
    if (prev == NULL) {
        DEBUG("ipsec_esp: Couldn't get leading pktsnip\n");
        return NULL;
    }

    data_size = gnrc_pkt_len_upto(next, 255);
    _calc_fields(sa, &iv_size, &icv_size, &block_size);
    _calc_padding(&padding_size, (data_size + 2), block_size);

    esp_size = sizeof(ipv6_esp_hdr_t) + iv_size + data_size + padding_size
               + sizeof(ipv6_esp_trl_t) + icv_size;
    esp = gnrc_pktbuf_add(NULL, NULL, esp_size, GNRC_NETTYPE_IPV6_EXT_ESP);
    if (esp == NULL) {
        DEBUG("ipsec_esp: could not add pkt tp buffer\n");
        return NULL;
    }
    esp_h = esp->data;
    sn = ipsec_inc_sn(sa->spi);
    if (!sn) {
        DEBUG("ipsec_esp: sequence number incrementation rejected\n");
        gnrc_pktbuf_release(esp);
        return NULL;
    }
    esp_h->sn = byteorder_htonl(sn);
    esp_h->spi = byteorder_htonl(sa->spi);
    payload = (uint8_t *)esp_h + sizeof(ipv6_esp_hdr_t);
    /* nulling the bits in padding*/
    memset(((uint8_t *)payload + iv_size + data_size), 0, padding_size);
    esp_trl = (ipv6_esp_trl_t *)(((uint8_t *)esp_h) + esp->size
                                 - icv_size - sizeof(ipv6_esp_trl_t));
    esp_trl->nh = nh;
    esp_trl->pl = padding_size;


    /* On using DietESP: We can't simply merge and copy the data since we need
     * the fields for payload compression. Instead we will send the two packets
     * to a subroutine, where the payloads header andalso the esp header fields
     * will be compressed and copied to a smaller pktsnip, one after another,
     * according to the given EHC rules. The new diet_esp packet is returned to
     * be encrypted */

    gnrc_pktbuf_start_write(next);
    gnrc_pktbuf_merge(next);
    memcpy((uint8_t *)payload + iv_size, next->data, next->size);
    gnrc_pktbuf_release(next);
    prev->next = esp;

    /* All relevant space for the encryption should be available at this point,
     * so we can work directly on the packet while encrypting. ICV and IV are
     * filled inside the ecryption*/

    switch (sa->c_mode) {
    case IPSEC_CIPHER_MODE_ENC_AUTH:
        if (_encrypt(esp, sa)) {
            DEBUG("ipsec_esp: ERROR encrytping packet\n");
            return NULL;
        }
        break;
    default:
        DEBUG("ipsec_esp: ERROR Cypher mode not supported\n");
        return NULL;
    }

    /* calculate intermediate headers between ipv6 and original data */
    itm_size = byteorder_ntohs(ipv6_h->len) - data_size;


    /* adjusting original ipv6 header fields */
    ((ipv6_hdr_t *)ipv6->data)->len = byteorder_htons((uint16_t)(esp->size + itm_size));
    if (prev->type == GNRC_NETTYPE_IPV6) {
        ((ipv6_hdr_t *)prev->data)->nh = PROTNUM_IPV6_EXT_ESP;
    }
    else {
        /* prev header is ext header */
        ((ipv6_ext_t *)prev->data)->nh = PROTNUM_IPV6_EXT_ESP;
    }

    /* TODO: sending a merged pkt so the interface results in jibberish beein
     * sent out. I couldn't quickly figure why. *
     * check regarding pmtu:
       gnrc_pktbuf_merge(pkt);
       if( (sizeof(ethernet_hdr_t) + pkt->size) > sa->pmtu ) {
        DEBUG("ipsec_esp: finished ESP packet exceeded PMTU\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
       }
     */

    /* TODO: add pktsize to SA bytecount limiters. payload or final? */

    return pkt;
}
