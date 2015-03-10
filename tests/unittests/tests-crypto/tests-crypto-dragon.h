/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file        tests-crypto-dragon.h
 * @brief       Test vectors for the Dragon cipher unittests
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 */

#ifndef __TESTS_CRYPTO_DRAGON_H_
#define __TESTS_CRYPTO_DRAGON_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 128

/* Dragon test vectors */

#define TV1_KEY \
"\x00\x00\x11\x11\x22\x22\x33\x33\x44\x44\x55\x55\x66\x66\x77\x77"
#define TV1_IV \
"\x00\x00\x11\x11\x22\x22\x33\x33\x44\x44\x55\x55\x66\x66\x77\x77"
#define TV1_EXPECT \
"\x99\xB3\xAA\x14\xB6\x3B\xD0\x2F\xE1\x43\x58\xA4\x54\x95\x04\x25" \
"\xF4\xB0\xD3\xFD\x8B\xA6\x91\x78\xE0\x39\x29\x38\xA7\x18\xC1\x65" \
"\x2E\x3B\xEB\x1E\x11\x61\x3D\x58\x9E\xAB\xB9\xF5\x43\xA1\xC5\x1C" \
"\x73\xC1\xF2\x27\x9D\x1C\xAE\xA8\x5C\x55\xF5\x39\xBA\xFD\x3C\x59" \
"\xEC\xAC\x88\xBD\x17\xEB\x1C\x9D\xA2\x8D\xD6\x3E\x90\x93\xC9\x13" \
"\x30\x32\xD9\x18\x3A\x9B\x33\xBC\x29\x33\xA7\x9D\x75\x66\x98\x27" \
"\x20\xEF\x30\x04\xC5\x3B\x02\x53\x7A\x1B\xE7\x96\x29\xF8\xD9\xA3" \
"\x8D\xC1\xFD\x31\xED\x9D\x11\x00\xB0\x7D\xFF\xB1\xAC\x75\xEB\x31"

#define TV2_KEY \
"\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF"
#define TV2_IV \
"\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF"
#define TV2_EXPECT \
"\x98\x82\x15\x06\x0E\x87\xE6\x95\xEB\x7A\xEF\x36\x31\x3F\xF9\x10" \
"\xE6\xC7\x31\x2F\x30\x35\x74\x24\x49\x22\x04\x3D\x98\x14\x6E\xE2" \
"\x20\x2D\x4D\x49\x6C\x60\x2E\xCC\x93\x7D\xD3\xF4\xE3\x9B\xE2\x6C" \
"\x84\x9D\xB4\x15\xF0\x4C\x54\x0E\x88\x58\x8C\x7A\xA3\xC6\x5A\x31" \
"\xE2\x15\x62\x29\x1E\x86\x02\x8B\x3F\x5A\x21\xB9\x4A\x94\xC1\x35" \
"\xB3\xA0\x15\x27\x74\x7E\x65\x21\xFF\xEE\x14\xF0\xFA\x1F\xCC\x73" \
"\x74\xC8\xB2\x04\x40\x09\xF5\x7D\x1D\x63\x00\x7E\xF1\xD8\xD2\x21" \
"\xE4\x29\xEB\xA8\x60\xF5\x60\x98\x45\x89\x1D\x74\x71\x66\x94\xB2"

#define TV3_KEY \
"\x00\x00\x11\x11\x22\x22\x33\x33\x44\x44\x55\x55\x66\x66\x77\x77" \
"\x88\x88\x99\x99\xAA\xAA\xBB\xBB\xCC\xCC\xDD\xDD\xEE\xEE\xFF\xFF"
#define TV3_IV \
"\x00\x00\x11\x11\x22\x22\x33\x33\x44\x44\x55\x55\x66\x66\x77\x77" \
"\x88\x88\x99\x99\xAA\xAA\xBB\xBB\xCC\xCC\xDD\xDD\xEE\xEE\xFF\xFF"
#define TV3_EXPECT \
"\xBC\x02\x07\x67\xDC\x48\xDA\xE3\x14\x77\x8D\x8C\x92\x7E\x8B\x32" \
"\xE0\x86\xC6\xCD\xE5\x93\xC0\x08\x60\x0C\x9D\x47\xA4\x88\xF6\x22" \
"\x3A\x2B\x94\xD6\xB8\x53\xD6\x44\x27\xE9\x33\x62\xAB\xB8\xBA\x21" \
"\x75\x1C\xAA\xF7\xBD\x31\x65\x95\x2A\x37\xFC\x1E\xA3\xF1\x2F\xE2" \
"\x5C\x13\x3B\xA7\x4C\x15\xCE\x4B\x35\x42\xFD\xF8\x93\xDA\xA7\x51" \
"\xF5\x71\x02\x56\x49\x79\x5D\x54\x31\x91\x4E\xBA\x0D\xE2\xC2\xA7" \
"\x80\x13\xD2\x9B\x56\xD4\xA0\x28\x3E\xB6\xF3\x12\x76\x44\xEC\xFE" \
"\x38\xB9\xCA\x11\x19\x24\xFB\xC9\x4A\x0A\x30\xF2\xAF\xFF\x5F\xE0"

#define TV4_KEY \
"\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF" \
"\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF"
#define TV4_IV \
"\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF" \
"\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF"
#define TV4_EXPECT \
"\x8D\x3A\xB9\xBA\x01\xDA\xA3\xEB\x5C\xBD\x0F\x6D\xE3\xEC\xFC\xAB" \
"\x61\x9A\xF8\x08\xCF\x9C\x4A\x42\xE2\x87\x77\x66\x6D\x2D\x70\x37" \
"\xEE\x6F\x94\xAC\x29\xD1\xEE\xE5\x34\x0D\xB0\x47\x8E\x91\xA6\x79" \
"\x48\x0D\x8D\x88\x23\x67\xCE\x2A\x31\xC9\x6A\xD4\x49\xE7\x07\x56" \
"\x81\x5E\xBE\xB2\x29\x0D\xBA\x7A\x3C\xCB\x76\xA2\x25\x7B\xD1\x22" \
"\x2B\x0B\x7A\xED\x91\x7F\xAF\xFF\x6B\x58\xB2\xB2\xB0\x5F\x24\xF6" \
"\xE2\x71\xA0\x16\x9E\x89\x7B\xEF\xF5\xC2\x24\x51\xDA\x6F\x9E\x40" \
"\x52\xB7\x8B\xE5\x6C\x97\xC1\xA5\xC6\xF8\xE7\x91\x0F\x7B\x9C\x98"

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_CRYPTO_DRAGON_H_ */
/** @} */
