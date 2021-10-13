/*
 * Copyright (C) 2016 Mathias Tausig, FH Campus Wien
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "tiny-asn1.h"

void print_hex(const uint8_t *data, unsigned int len)
{
    unsigned int count = 0;
    unsigned int blockCount = 0;

    while (count < len) {
        printf("%02x ", data[count]);
        ++count;
        ++blockCount;
        if (blockCount == 4) {
            printf("  ");
        }
        if (blockCount == 8) {
            printf("\n");
            blockCount = 0;
        }
    }
    printf("\n");
}

void print_asn1(asn1_tree *list, int depth)
{
    printf("d=%d, Tag: %02x, len=%" PRIu32 "\n", depth, list->type, list->length);
    if (list->child == NULL) {
        printf("Value:\n");
        print_hex(list->data, list->length);
    }
    else {
        print_asn1(list->child, depth + 1);
    }

    if (list->next != NULL) {
        print_asn1(list->next, depth);
    }
}

int main(void)
{
    /* Created with echo "12345678901234567890"|openssl cms -EncryptedData_encrypt -aes128 -secretkey 39904F36D98779D00F9A2B8139D2957F -outform der|xxd --i */
    uint8_t cms_data[] = {
        0x30, 0x60, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07,
        0x06, 0xa0, 0x53, 0x30, 0x51, 0x02, 0x01, 0x00, 0x30, 0x4c, 0x06, 0x09,
        0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x01, 0x30, 0x1d, 0x06,
        0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x02, 0x04, 0x10,
        0x4d, 0xaa, 0x7b, 0x74, 0xfb, 0xf7, 0x7a, 0xfa, 0xb0, 0x23, 0xa2, 0x9c,
        0x1a, 0xb6, 0x51, 0xe4, 0x80, 0x20, 0x6b, 0x8f, 0x64, 0xd7, 0x3a, 0x81,
        0x01, 0x0c, 0x7b, 0xab, 0xfd, 0x26, 0x6d, 0x46, 0x2e, 0x3e, 0xc4, 0xf6,
        0x01, 0x78, 0x8a, 0xc0, 0xc0, 0x52, 0x0a, 0x66, 0x17, 0x71, 0x4f, 0xf6,
        0xd9, 0x95
    };

    int32_t asn1_object_count = der_object_count(cms_data, sizeof(cms_data));

    if (asn1_object_count < 0) {
        printf("ERROR: Could not calculate the number of Elements within the data.\n");
        return 1;
    }

    asn1_tree *asn1_objects = (asn1_tree *)(malloc(sizeof(asn1_tree) * asn1_object_count));
    if (asn1_objects == NULL) {
        printf("ERROR: Could not allocate the memory for the ASN.1 objects.\n");
        return 1;
    }

    asn1_tree cms;

    if (der_decode(cms_data, sizeof(cms_data), &cms, asn1_objects, asn1_object_count) < 0) {
        printf("ERROR: Could not parse the data.\n");
        return 1;
    }

    /* Dump the data */
    print_asn1(&cms, 0);

    /* Since we know this is CMS data, we can try to interpret it. */
    if (cms.type != 0x30) {
        printf("ERROR: The outer type is not a SEQUENCE.\n");
        return 1;
    }

    asn1_tree *content_type = cms.child;
    if ((content_type == NULL) || (content_type->type != 0x06)) {
        printf("ERROR: No ContentType information available.\n");
        return 1;
    }
    printf("ContentType: ");
    if (memcmp(content_type->data, (uint8_t[]){ 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x06, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x06 }, content_type->length) != 0) {
        printf("ERROR: ContentType is not of type encryptedData.\n");
        return 1;
    }
    printf("encryptedData\n");

    asn1_tree *encrypted_data = content_type->next->child;
    if ((encrypted_data == NULL) || (encrypted_data->type != 0x30)) {
        printf("ERROR: EncryptedData not available.\n");
        return 1;
    }

    asn1_tree *cms_version = encrypted_data->child;
    if ((cms_version == NULL) || (cms_version->type != 0x02) || (cms_version->length != 1)) {
        printf("ERROR: CMSVersion not available.\n");
        return 1;
    }
    uint8_t version = cms_version->data[0];
    printf("CMSVersion: %d\n", version);

    asn1_tree *encrypted_content_info = cms_version->next;
    if ((encrypted_content_info == NULL) || (encrypted_content_info->type != 0x30)) {
        printf("ERROR: EncryptedContentInfo not available.\n");
        return 1;
    }

    asn1_tree *encrypted_content_info_content_type = encrypted_content_info->child;
    if ((encrypted_content_info_content_type == NULL) || (encrypted_content_info_content_type->type != 0x06)) {
        printf("ERROR: ContentType of EncryptedContentInfo not available.\n");
        return 1;
    }
    if (memcmp(encrypted_content_info_content_type->data,
               (uint8_t[]){ 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x01 }, encrypted_content_info_content_type->length) != 0) {
        printf("ERROR: ContentType of EncrytptedContentInfo is not of type pkcs#7.\n");
        return 1;
    }
    printf("ContentType of EncryptedContentInfo: pkcs#7\n");

    asn1_tree *content_encryption_algorithm = encrypted_content_info_content_type->next;
    if (content_encryption_algorithm == NULL) {
        printf("ERROR: ContentEncryptionAlgorithm not available.\n");
        return 1;
    }

    if (content_encryption_algorithm->type == 0x30) {
        /* extended definition of ContentEncryptionAlgorithmIdentifier by RFC5911 */
        asn1_tree *encryption_algorithm_identifier = content_encryption_algorithm->child;
        if ((encryption_algorithm_identifier == NULL) || (encryption_algorithm_identifier->type != 0x06)) {
            printf("ERROR: No encryption algorithm identifier available.\n");
            return 1;
        }
        if (memcmp(encryption_algorithm_identifier->data,
                   (uint8_t[]){ 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x02 }, encryption_algorithm_identifier->length) == 0) {
            printf("Content encryption algorithm: AES-128-CBC.\n");
            asn1_tree *aes_iv = encryption_algorithm_identifier->next;
            if ((aes_iv == NULL) || (aes_iv->type != 0x04)) {
                printf("ERROR: IV not available.\n");
                return 1;
            }
            printf("IV:\n");
            print_hex(aes_iv->data, aes_iv->length);
        }
        else {
            printf("Unknown encryption algorithm.\n");
        }
        asn1_tree *encrypted_content = content_encryption_algorithm->next;
        if ((encrypted_content == NULL) || (encrypted_content->type != 0x80)) {
            printf("ERROR: encrypted content not available.\n");
            return 1;
        }
        printf("Encrypted content:\n");
        print_hex(encrypted_content->data, encrypted_content->length);

    }

    asn1_tree *unprotected_attrs = encrypted_content_info->next;
    if (unprotected_attrs != NULL) {
        printf("Unprotected attributes are available.\n");
    }
    else {
        printf("Unprotected attributes are not available.\n");
    }

    /* try to encode the asn1_tree and check if it has not been altered */
    uint8_t encoded[98];
    if (der_encode(&cms, encoded, sizeof(encoded)) < 0) {
        printf("ERROR: Could not re-encode the decoded data.\n");
        return 1;
    }
    if (memcmp(cms_data, encoded, sizeof(cms_data)) != 0) {
        printf("ERROR: Data has changes while reencoding it.\n");
        return 1;
    }
    printf("DER encoded data:\n");
    print_hex(encoded, sizeof(encoded));

    free(asn1_objects);

    /* Everything worked fine */
    printf("Decoding finished successfully\n");
    return 0;
}
