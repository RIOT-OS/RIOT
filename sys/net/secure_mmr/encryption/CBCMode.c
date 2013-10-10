/*
 * CBCMode.c - The CBC Mode of Operation.
 *
 * Copyright (C) 2013 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */


/**
 * @file        CBCMode.c
 * @brief       implementation of the CBC Mode of Operation with Ciphertext-
 *              Stealing for encryption.
 *              Implements CBC Mode using Cipher Text Stealing (CBC-CTS) as
 *              described in Schneir's Applied Cryptography (195-6) and
 *              RFC-2040.
 *              <br>
 *              Plain CBC mode is pretty simple; using CTS complicates things.
 *              CTS allows the ciphertext to be the same size as the plaintext,
 *              even for plaintexts which aren't a multiple of the block size.
 *
 *              C_0     == E[IV]
 *              C_i     == E[ C_{i-1} ^ P_i ]  0 < i < n-2
 *              C_{n-1} == E[ C_{n-2} ^ P_{n-1} ]
 *                      == E[ C_{n-2} ^ (P_{n-1} || 0/-L) ]
 *                      == C_{n-1}/+L || C_{n-1}/-L (naming of ciphertext
 *                                                   block into left and right
 *                                                   pieces)
 *              C_n     == E[ C_{n-1}/+L ^ P_n || C_{n-1}/-L ]
 *
 *              Where 0 = the zero block
 *              n = number of blocks. the last block may be length 1..blockSize
 *              bytes
 *              L = |P_n|, the length of the last block and
 *              /+L refers to the first L bytes of a block and
 *              /-L refers to the last (blockSize - L) bytes of a block
 *              We then output C_0 || ... || C_{n-2} || C_n || C_{n-1}/+L so
 *              that the ciphertext is the same size as the input.
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Naveen Sastry
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        18.09.2013 18:35:21
 *
 */

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Ciphers.h"
#include "CBCMode.h"
#include "SkipJack.h"
#include "RC5.h"
#include  "packet_encryption.h"

//activate the define to printout debug-output
// #define CBCMODE_DEBUG

#ifndef IDENTITY

extern BlockCipher_Archive_t archive;


int block_cipher_mode_init(CipherModeContext *context, uint8_t keySize,
                           uint8_t *key)
{
    //check for AES or TWOFISH as MAC algorithm
    char aesString[] = "AES\0";
    char twofishString[] = "TWOFISH\0";
    bool bigBlocks = !(strcmp(archive.ciphers[0].name, aesString));

    if (!bigBlocks) {
        bigBlocks = !(strcmp(archive.ciphers[0].name, twofishString));
    }

    uint8_t blockSize = block_cipher_info_get_preferred_block_size(0);

    if (bigBlocks) {
        if (blockSize > CBCMODE_MAX_BLOCK_SIZE_AES) {
            return FAIL;
        }
    }
    else {
        if (blockSize > CBCMODE_MAX_BLOCK_SIZE) {
            return FAIL;
        }
    }

    ((CBCModeContext *)(context->context))->bsize = blockSize;
    return block_cipher_init(&context->cc, blockSize, keySize, key, 0);
}

int block_cipher_mode_init0(CipherModeContext *context, uint8_t keySize,
                            uint8_t *key, uint8_t cipher_index)
{
    //check for AES or TWOFISH as MAC algorithm
    char aesString[] = "AES\0";
    char twofishString[] = "TWOFISH\0";
    bool bigBlocks = !(strcmp(archive.ciphers[cipher_index].name, aesString));

    if (!bigBlocks) {
        bigBlocks = !(strcmp(archive.ciphers[cipher_index].name,
                             twofishString));
    }

    uint8_t blockSize = block_cipher_info_get_preferred_block_size(
                            cipher_index);

    if (bigBlocks) {
        if (blockSize > CBCMODE_MAX_BLOCK_SIZE_AES) {
            printf("%-40s: blockSize > CBCMODE_MAX_BLOCK_SIZE_AES...\r\n",
                   __FUNCTION__);
            return FAIL;
        }
    }
    else {
        if (blockSize > CBCMODE_MAX_BLOCK_SIZE) {
            printf("%-40s: blockSize > CBCMODE_MAX_BLOCK_SIZE...\r\n",
                   __FUNCTION__);
            return FAIL;
        }
    }

    ((CBCModeContext *)context->context)->bsize = blockSize;
    return block_cipher_init(&context->cc, blockSize, keySize, key,
                             cipher_index);
}

void dbg(uint8_t mode, const char *format, ...)
{
#ifdef CBCMODE_DEBUG

    va_list args;
    va_start(args, format);
    fprintf(stdout, "%i: ", net_get_interface_address(0));
    vfprintf(stdout, format, args);
    va_end(args);

#endif
    return;
}


void dump_buffer(char *bufName, uint8_t *buf, uint8_t size)
{
#ifdef CBCMODE_DEBUG

    if (size > 512) {
        printf("%-40s: size (%i) is too big!\r\n", __FUNCTION__, size);
        return;
    }

    uint8_t i = 0;
    char *tmp = malloc(512);

    for (; i < size; i++) {
        sprintf(tmp + i * 3, "%2x ", (char)buf[i] & 0xff);
    }

    dbg(DBG_CRYPTO, "%-34s: {%s}\n", bufName, tmp);
    free(tmp);
#endif
}


int block_cipher_mode_encrypt(CipherModeContext *context, uint8_t *plain_blocks,
                              uint8_t *cipher_blocks, uint16_t num_bytes,
                              uint8_t *IV)
{
    if (archive.NoCiphers <= 0) {
        return -1;
    }

    int (*fp_encrypt)(CipherContext * context, uint8_t * plainBlock,
                      uint8_t * cipherBlock) = NULL;
    uint8_t *plainBackup;

    //array for IV with 16-Byte BlockSize
    uint8_t bigIV[CBCMODE_MAX_BLOCK_SIZE_AES];

    //backup context struct in case of more than one cipher
    CipherModeContext cmc;

    uint8_t *IVptr = NULL;
    uint8_t bkIV[8];
    uint8_t e;
    int res = 0;

    /*
     * if we take more than 1 round of encryption we need to reuse the
     * ciphertext so copy plainBlocks to cipherBlocks and operate only
     * on cipherBlocks
     **/
    if (archive.NoCiphers > 1) {
        int n;

        for (n = 0; n < num_bytes; n++) {
            cipher_blocks[n] = plain_blocks[n];
        }

        plainBackup = plain_blocks;
        plain_blocks = cipher_blocks;

        //backup context
        memcpy(&cmc, context, sizeof(cmc));

        //backup IV
        for (n = 0; n < 8; n++) {
            bkIV[n] = IV[n];
        }
    }

    //backup pointer to 8-Byte IV
    IVptr = IV;

    /*
     * for-loop to go through all encryption algorithms in defined order
     * (see CipherManager.c)
     **/
    for (e = 0; e < archive.NoCiphers; e++) {

        //restore 8-Byte IV
        IV = IVptr;

        //restore context and IV - in case of chained ciphers
        uint8_t x;

        if (archive.NoCiphers > 1) {
            //restore context
            memcpy(context, &cmc, sizeof(cmc));

            //restore IV
            for (x = 0; x < 8; x++) {
                IV[x] = bkIV[x];
            }
        }

        //for the first cipher no init is needed
        if (!e) {
            fp_encrypt = archive.ciphers[e].BlockCipher_encrypt;
        }
        else {
            res = block_cipher_mode_init0(context, CIPHERS_KEYSIZE,
                                          (uint8_t *)context->cc.context, e);

            if (res <= 0) {
                printf("%-40s: BlockCipherMode_init0 failed!Code %i\r\n",
                       __FUNCTION__, res);
                return res;
            }

            fp_encrypt = archive.ciphers[e].BlockCipher_encrypt;
        }

        //starting to encrypt with algorithm i
#ifdef CBCMODE_DEBUG
        printf("%-40s: [INFO] Algorithm %i - Encrypting with %s\r\n"
               , __FUNCTION__, e + 1, archive.ciphers[e].name); // key
#endif

        //if BlockSize is 16-Byte, we need to expand the IV to 16 bytes
        char aesString[] = "AES\0";
        char twofishString[] = "TWOFISH\0";

        // check for AES as algorithm
        if (!(strcmp(archive.ciphers[e].name, aesString))) {
            memcpy(bigIV, IV, CBCMODE_MAX_BLOCK_SIZE);
            //fill up Bytes 9-16 with IV again
            memcpy(&bigIV[CBCMODE_MAX_BLOCK_SIZE], IV, CBCMODE_MAX_BLOCK_SIZE);
            IV = bigIV;
        }
        // check for TWOFISH as algorithm
        else if (!(strcmp(archive.ciphers[e].name, twofishString))) {
            memcpy(bigIV, IV, CBCMODE_MAX_BLOCK_SIZE);
            //fill up Bytes 9-16 with IV again
            memcpy(&bigIV[CBCMODE_MAX_BLOCK_SIZE], IV, CBCMODE_MAX_BLOCK_SIZE);
            IV = bigIV;
        }

        /******** original BlockCipherMode_encrypt starts here ********/
        uint8_t i, j, t, bsize, bsize2;
        uint16_t bc = 0;
        uint8_t spillblock[archive.ciphers[e].
                           BlockCipherInfo_getPreferredBlockSize()];
        uint8_t eIV[archive.ciphers[e].
                    BlockCipherInfo_getPreferredBlockSize()];

        bsize = ((CBCModeContext *)(context->context))->bsize;
        bsize2 = bsize + bsize;

        if (num_bytes == 0) {
            continue;//return SUCCESS;
        }

        // we can only encrypt 256 blocks (since our block counter is a byte
        // [quicker to maintain an 8 bit counter than a 16 bit counter]_
        if ((num_bytes / 256) > bsize) {
            printf("%-40s: [ERROR] numBytes / 256 >  bsize!\r\n",
                   __FUNCTION__);
            return FAIL;
        }

        // we need at least 1 block size to work with.
        if (num_bytes < bsize) {
            printf("%-40s: [ERROR] numBytes < bsize!\r\n", __FUNCTION__);
            return FAIL;
        }

#ifdef CBCMODE_DEBUG
        dump_buffer("CBC.encrypt orig", plain_blocks, num_bytes);
#endif

        if (fp_encrypt(&(context->cc), IV, eIV) == FAIL) {
            printf("%-40s: [ERROR] fp_encrypt(&(context->cc), IV, eIV) == \
                   FAIL (1) \r\n", __FUNCTION__);
            return FAIL;
        }

        IV = eIV;

        // special case for the 1 block encryption
        if (num_bytes == bsize) {
            // FIXME UNROLL:
            // xor the iv and plaintext and encrypt
            for (j = 0; j < bsize; j++) {
                cipher_blocks[bc + j] = plain_blocks[bc + j] ^ IV[j];
            }

            if (fp_encrypt(&(context->cc), cipher_blocks + bc,
                           cipher_blocks + bc) == FAIL) {
                printf("%-40s: [ERROR] fp_encrypt (&(context->cc), \
                           cipherBlocks + bc, cipherBlocks + bc) == FAIL (1) \r\n",
                       __FUNCTION__);
                return FAIL;
            }

#ifdef CBCMODE_DEBUG
            dump_buffer("CBC.encrypt cipher:", cipher_blocks, num_bytes);
#endif
            continue;
        }

        // this loop deals with all but the last two blocks
        // it xors the prev encr (stored in iv) and encrypts
        if (num_bytes > bsize2) {
            for (bc = 0; bc < num_bytes - bsize2; bc += bsize) {
                // FIXME UNROLL:
                for (j = 0; j < bsize; j++) {
                    cipher_blocks[bc + j] = plain_blocks[bc + j] ^ IV[j];
                }

                if (fp_encrypt(&context->cc, cipher_blocks + bc,
                               cipher_blocks + bc) == FAIL) {
                    printf("%-40s: [ERROR] fp_encrypt(&context->cc, \
                           cipherBlocks + bc, cipherBlocks + bc) == \
                           FAIL (2) \r\n", __FUNCTION__);
                    return FAIL;
                }

                IV = cipher_blocks + bc;
            }
        }

        /*
         * dbg (DBG_CRYPTO, "bc: %d\n", bc);
         * Now we deal with the last two blocks. The very last block may not be
         * full, so we use a technique called ciphertext stealing to deal with
         * it.
         * We encrypt the second to last block as normal. Call the ciphertext
         * C_{n-1}. We do not output this ciphertext.  We then xor the last
         * partial block with C_{n-1} (of size m) and encrypt it to obtain
         * C_n. The ciphertext C_n is output in place of C_{n-1}. And we then
         * only need to output the first m bytes of C_{n-1}.
         *
         * How does this work? Well to decrypt, it is easy to obtain
         * C_{n-1} ^ p_n (the decryption of C_{n-1} yields this). xoring the
         * first m bytes of C_{n-1} yields p_n, and hence the complete C_{n-1}.
         * start by generating C_{n-1} -- xor with the prev IV and encrypt.
         **/
        for (j = 0; j < bsize; j++) {
            spillblock[j] = plain_blocks[bc + j] ^ IV[j];
        }

        if (fp_encrypt(&context->cc, spillblock, spillblock) == FAIL) {
            printf("%-40s: [ERROR] fp_encrypt(&context->cc, spillblock, \
                   spillblock) == FAIL \r\n", __FUNCTION__);
            return FAIL;
        }

        // dumpBuffer ("CBC.encrypt spill:", spillblock, bsize);

        j = num_bytes - bc - bsize;

        // dbg(DBG_CRYPTO, "CBC.encrypt j: %d; bc: %d\n", j, bc);
        // xor and output the first m bytes of C_{n-1}
        for (i = 0 ; i < j; i++) {
            /* we do this in a convoluted manner to avoid alias problems:
             *  if cipherBlock = plainblock
             **/
            t = plain_blocks[bc + bsize + i];
            cipher_blocks[bc + bsize + i] = spillblock[i];
            spillblock[i] ^= t;
        }

        // and encrypt -- note that the output of the encryption places the
        // last ciphertext in the correct position -- where the C_{n-1} would
        // ordinarily go.
        if (fp_encrypt(&context->cc, spillblock, cipher_blocks + bc) == FAIL) {
            printf("%-40s: [ERROR] fp_encrypt(&context->cc, spillblock, \
                   cipherBlocks + bc) == FAIL \r\n", __FUNCTION__);
            return FAIL;
        }

#ifdef CBCMODE_DEBUG
        dump_buffer("CBC.encrypt cipher:", cipher_blocks, num_bytes);
#endif
        //continue;//return SUCCESS;

        /************ original BlockCipherMode_encrypt ends here ************/
    }   // end of for-loop going through encryption-algorithms

    //if we took more than 1 round restore backuped plainBlocks-Pointer
    if (archive.NoCiphers > 1) {
        //restore original plainBlocks-Pointer
        plain_blocks = plainBackup;

        //restore context
        memcpy(context, &cmc, sizeof(cmc));
    }

    return SUCCESS;
}


int block_cipher_mode_decrypt(CipherModeContext *context, uint8_t *cipherBlock,
                              uint8_t *plainBlock, uint16_t numBytes,
                              uint8_t *IV)
{
    if (archive.NoCiphers <= 0) {
        return -1;
    }

    int (*fp_decrypt)(CipherContext * context, uint8_t * cipherBlock,
                      uint8_t * plainBlock) = NULL;
    int d;
    int res = 0;

    //backup plainBlock
    uint8_t *plainBackup = plainBlock;
    plainBlock = cipherBlock;

    //array for IV with 16-Byte BlockSize
    uint8_t bigIV[CBCMODE_MAX_BLOCK_SIZE_AES];

    //backup IV
    uint8_t *IVptr = IV;
    uint8_t bkIV[8];

    for (d = 0; d < 8; d++) {
        bkIV[d] = IV[d];
    }

    //backup context
    CipherModeContext cmc;
    memcpy(&cmc, context, sizeof(CipherModeContext));

    for (d = (archive.NoCiphers - 1); d >= 0; d--) {
        //restore context and IV - in case of chained ciphers
        uint8_t x;

        if (archive.NoCiphers > 1) {
            //restore context
            memcpy(context, &cmc, sizeof(CipherModeContext));

            //restore original IV
            IV = IVptr;

            for (x = 0; x < 8; x++) {
                IV[x] = bkIV[x];
            }
        }

        /*
         * for the last cipher (i.e. at i=0) plainBlock is output - before
         *  that cipherBlock is input and output
         */
        if (!d) {
            res = block_cipher_mode_init(context, CIPHERS_KEYSIZE,
                                         (uint8_t *)context->cc.context);

            if (res < 0) {
                return res;
            }

            fp_decrypt = archive.ciphers[d].BlockCipher_decrypt;
            plainBlock = plainBackup;

        }
        else {
            res = block_cipher_mode_init0(context, CIPHERS_KEYSIZE,
                                          (uint8_t *)context->cc.context, d);

            if (res < 0) {
                return res;
            }

            fp_decrypt = archive.ciphers[d].BlockCipher_decrypt;
        }

        //starting to encrypt with algorithm i
#ifdef CBCMODE_DEBUG
        printf("%-40s: [INFO] Reversing Algorithm %i - Decrypting with %s\r\n",
               __FUNCTION__, d + 1, archive.ciphers[d].name);
#endif

        //if BlockSize is 16-Byte, we need to expand the IV to 16 bytes
        char aesString[] = "AES\0";
        char twofishString[] = "TWOFISH\0";

        if (!(strcmp(archive.ciphers[d].name, aesString))) { //check for AES
            memcpy(bigIV, IV, CBCMODE_MAX_BLOCK_SIZE);
            //fill up Bytes 9-16 with IV again
            memcpy(&bigIV[CBCMODE_MAX_BLOCK_SIZE], IV, CBCMODE_MAX_BLOCK_SIZE);
            IV = bigIV;
        }
        //check for TWOFISH
        else if (!(strcmp(archive.ciphers[d].name, twofishString))) {
            memcpy(bigIV, IV, CBCMODE_MAX_BLOCK_SIZE);
            //fill up Bytes 9-16 with IV again
            memcpy(&bigIV[CBCMODE_MAX_BLOCK_SIZE], IV, CBCMODE_MAX_BLOCK_SIZE);
            IV = bigIV;
        }

        /*********** original BlockCipherMode_decrypt starts here ***********/
        uint8_t i = 0, partialSize = 0,
                bsize = ((CBCModeContext *)context->context)->bsize;
        uint16_t bc = 0;
        uint8_t spillblock[archive.ciphers[d].
                           BlockCipherInfo_getPreferredBlockSize()];
        uint8_t spillblock2[archive.ciphers[d].
                            BlockCipherInfo_getPreferredBlockSize()];
        uint8_t eIV[archive.ciphers[d].
                    BlockCipherInfo_getPreferredBlockSize()];

        if (numBytes == 0) {
            continue;
        }

        // we need at least one block size to deal with.
        if (numBytes < bsize) {
            return FAIL;
        }

        if (archive.ciphers[d].BlockCipher_encrypt(&context->cc,
                IV, eIV) != SUCCESS) {
            return FAIL;
        }

        IV = eIV;

        // deal with the single block case a bit specially: encrypt and xor it
        // and move on.
        if (numBytes == bsize) {
            if (fp_decrypt(&context->cc, cipherBlock, plainBlock) != SUCCESS) {
                return FAIL;
            }

            for (i = 0; i < bsize; i++) {
                plainBlock[i] ^= IV[i];
            }

            continue;
        }

        // else we know there are 2 or more blocks (though the last one may be
        // a partial).

#ifdef CBCMODE_DEBUG
        dump_buffer("CBC.decrypt cipher:", cipherBlock, numBytes);
#endif

        // find the start of the last whole block:
        bc = bsize;

        while (bc  < numBytes) {
            bc += bsize;
        }

        // and the block before it
        bc -= (bsize << 1);
        partialSize = numBytes - bc - bsize;

        /*
         * dbg (DBG_CRYPTO, "CBC.decrypt bc: %d; partial %d ;
         * cipherblock = %p\r\n", bc, partialSize, cipherBlock);
         * split up the computation: depending on whether  the last block
         * is full or not.
         **/

        if (partialSize) {
            // decrypt C_n
            if (fp_decrypt(&context->cc, cipherBlock + bc,
                           spillblock) != SUCCESS) {
                return FAIL;
            }

            // recover C_{n-1}, [in spillblock2] and P_{n} [in spillblock]
            for (i = 0; i < partialSize; i++) {
                // bit convoluted for the case where we
                // alias plainBlock and CipherBlock
                spillblock[i] ^= cipherBlock [bc + bsize + i];
                spillblock2[i] = cipherBlock[bc + bsize + i];
                plainBlock[bc + bsize + i] = spillblock[i];
            }

            // copy over the remaining portion of the spillblock
            for (i = partialSize; i < bsize; i++) {
                spillblock2[i] = spillblock[i];
            }

            // and decrypt the spillblock (C_{n-1}) into position.
            if (fp_decrypt(&context->cc,
                           spillblock2, plainBlock + bc) != SUCCESS) {
                return FAIL;
            }

            // NOW xor pref [iv or prev block]. We work from the end forward.
            // that happens after the if / else
            //dumpBuffer ("CBC.decrypt partial", plainBlock, numBytes);
        }
        else {
            // bit simpler - just decrypt C_{n-1} into the spillblock
            if (fp_decrypt(&context->cc, cipherBlock + bc,
                           spillblock) != SUCCESS) {
                return FAIL;
            }

            // xor to recover C_{n} and decrypt into place
            for (i = 0; i < bsize; i++) {
                spillblock[i] ^= cipherBlock[bc + i + bsize];
            }

            if (fp_decrypt(&context->cc, cipherBlock + bc + bsize,
                           plainBlock + bc) != SUCCESS) {
                return FAIL;
            }

            // copy P_{n-1} into place
            for (i = 0; i < bsize; i++) {
                plainBlock[bc + bsize + i] = spillblock[i];
            }

            // NOW xor prev [iv or prev block in]
        }

        // handle blocks 0.. n-2
        // by xoring in the n-1 ciphertext and decrypting.

        while (bc) {
            bc -= bsize;

            for (i = 0 ; i < bsize; i++) {
                plainBlock [ bc + bsize + i] ^= cipherBlock [bc + i];
            }

            if (fp_decrypt(&context->cc, cipherBlock + bc,
                           plainBlock + bc) != SUCCESS) {
                return FAIL;
            }
        }

        // xor the iv to recover P_0
        for (i = 0 ; i < bsize; i++) {
            plainBlock[i] ^= IV[i];
        }

#ifdef CBCMODE_DEBUG
        dump_buffer("CBC.decrypt plain:", plainBlock, numBytes);
#endif
        /************ original BlockCipherMode_decrypt ends here ************/
    }

    //restore context
    memcpy(context, &cmc, sizeof(cmc));

    return SUCCESS;
}

#endif //IDENTITY
