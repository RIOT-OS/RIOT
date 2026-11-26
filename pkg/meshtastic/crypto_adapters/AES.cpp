
#include "AES.h"

#include "crypto/aes.h"

AESCommon::~AESCommon() {
}

size_t AESCommon::blockSize() const {
    return cipher_get_block_size(&_cipher);
}

void AESCommon::encryptBlock(uint8_t *output, const uint8_t *input) {
    cipher_encrypt(&_cipher, input, output);
}

void AESCommon::decryptBlock(uint8_t *output, const uint8_t *input) {
    cipher_decrypt(&_cipher, input, output);
}

void AESCommon::clear() {
}

AESCommon::AESCommon() {
}


AES128::AES128() {
    
}

AES128::~AES128() {
}

size_t AES128::keySize() const {
    return _key_size;
}

bool AES128::setKey(const uint8_t *key, size_t len) {
    if(len != AES_KEY_SIZE_128) {
        return false;
    }
    _key_size = len;
    return cipher_init(&_cipher, CIPHER_AES, key, len) == CIPHER_INIT_SUCCESS;
}

AES256::AES256() {
}

AES256::~AES256() {
}

size_t AES256::keySize() const {
    return _key_size;
}

bool AES256::setKey(const uint8_t *key, size_t len) {
    if(len != AES_KEY_SIZE_256) {
        return false;
    }
    _key_size = len;
    return cipher_init(&_cipher, CIPHER_AES, key, len) == CIPHER_INIT_SUCCESS;
}

AESTiny256::AESTiny256() {
}

AESTiny256::~AESTiny256() {
}

AESSmall256::AESSmall256() {
}

AESSmall256::~AESSmall256() {
}

